#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

#include "stdlib.h"
#include "TGraph.h"
#include "TFile.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TMinuit.h"

#include <iomanip>

const int beam_1 = 1 ;
const int beam_2 = 2 ;

const int DX_position_in_twiss_line = 10 ;

class TObservationsPerDetector
{
	public:

	string detector_name ;

	double Dx_m ;
	double Dx_m_rel_error ;
	bool   valid_Dx ;

	double beam_position_x_m ;
	double beam_position_x_m_rel_error ;
	bool   valid_x ;

	TObservationsPerDetector() ;

	double CalculateChi2Contribution(double, double, bool, bool) ;
};

TObservationsPerDetector::TObservationsPerDetector() : detector_name(""), Dx_m(0), Dx_m_rel_error(0), beam_position_x_m(0), beam_position_x_m_rel_error(0)
{} ;

double TObservationsPerDetector::CalculateChi2Contribution(double measured_Dx_m, double measured_beam_position_x_m, bool skip_Dx, bool skip_x)
{
	double chi2 = 0 ;

	if(valid_Dx && !skip_Dx)
	{
		double delta_Dx = (measured_Dx_m - Dx_m) / (Dx_m * Dx_m_rel_error) ;
		chi2 += (delta_Dx * delta_Dx) ;
	}


	if(valid_x && !skip_x)
	{
		double delta_beam_position_x_m = (measured_beam_position_x_m - beam_position_x_m) / (beam_position_x_m * beam_position_x_m_rel_error) ;
		chi2 += (delta_beam_position_x_m * delta_beam_position_x_m) ;
	}

	return chi2 ;
}

const int horizontal_crossing_angle_B1 = 1 ;
const int horizontal_crossing_angle_B2 = 2 ;
const int quadrupole_position = 3 ;
const int kicker_strength = 4 ;	
const int delta_x = 5 ;

class TPerturbation
{
	public:

	string name ;
	int type ;

	double value ;
	double error ;
	double absolute_value ;
	double upper_limit, lower_limit, step_size ;
	
	TPerturbation() ;
} ;

TPerturbation::TPerturbation() : name(""), type(0), value(0), error(0), absolute_value(0), upper_limit(0), lower_limit(0), step_size(0)
{
}

map<string, TObservationsPerDetector> map_from_detector_name_to_ObservationsPerDetector ;
vector<TPerturbation> vector_of_Perturbation ;

const double Half_crossing_angle_B1			= 185e-6 ;
const double Half_crossing_angle_B1_rel_error		= 0.1 ;
const double Half_crossing_angle_B1_limits		= 0.2 ;
const double Quadrupole_x_position_m_error		= 5e-4 ;
const double kicker_strength_error			= 3e-2 ;
const double delta_x_error				= 1e-3 ;

void WriteMADXPerturbation(ofstream &perturbations, string MADX_element_name, int perturbation, double absolute_value, double value)
{
	if(perturbation == horizontal_crossing_angle_B1)
	{
		perturbations << "DELTA_PX_B1:=" << (absolute_value * (1.0 + value)) << " ; "  << endl ;
	}
	else if(perturbation == horizontal_crossing_angle_B2)
	{
		perturbations << "DELTA_PX_B2:=" << (absolute_value * (1.0 + value)) << " ; "  << endl ;
	}
	else if(perturbation == quadrupole_position)
	{
		perturbations << "EOPTION,SEED=123923,ADD=TRUE;" << endl ;
		perturbations << "select, flag=error, clear;" << endl ;
		perturbations << "SELECT,FLAG=ERROR,pattern=\"" + MADX_element_name + "\";" << endl ;
		perturbations << "ealign, dx=" << value << ";" << endl ;

	}
	else if(perturbation == kicker_strength)
	{
		perturbations << "DELTA_K_" + MADX_element_name << ":=" << value << ";" << endl ;
	}
	else if(perturbation == delta_x)
	{
		perturbations << "DELTA_X:=" << value << ";" << endl ;
	}
	else
	{
		cout << "Error: fcn: the perturbation type does not exist!" << endl ;
		exit(1) ;
	}

}

double ProcessTFS(bool skip_Dx, bool skip_x)
{
	double chi2 = 0 ;

	ifstream tfsfile ;

	tfsfile.open("result/twiss_lhcb2.tfs") ;
	string line ;

	if(tfsfile.is_open())
	{
		while ( tfsfile.good() )
		{
		    getline (tfsfile,line);
		    if(line.substr(0,1).compare("$") == 0) break ;
		}
		if(tfsfile.good())
		{
			string word = "" ;

			while(tfsfile >> word)
			{
				map<string, TObservationsPerDetector>::iterator it ;

				for(it = map_from_detector_name_to_ObservationsPerDetector.begin() ; it != map_from_detector_name_to_ObservationsPerDetector.end() ; ++it)
				{
					if(word.compare(it->first) == 0)
					{
						cout << it->first << " : " ;

						double Dx_value_m = 0 ;
						double x_position_m = 0 ;

						bool valid_Dx_value_m = false ;
						bool valid_x_position_m = false ;

						for(int i = 0 ; i < DX_position_in_twiss_line ; ++i)
						{
							tfsfile >> Dx_value_m ;
	
							if(i == 1)
							{
								x_position_m = Dx_value_m ;
								valid_x_position_m = true ;
							}

							if(i == (DX_position_in_twiss_line - 1))
							{
								valid_Dx_value_m = true ;
							}

						}

						if(!(valid_Dx_value_m && valid_x_position_m))
						{
							cout << "Dx or x is not valid !" << endl ;
						}

						cout << "chi2: " << it->second.CalculateChi2Contribution(Dx_value_m, x_position_m, skip_Dx, skip_x)  << endl ;
						
						chi2 += it->second.CalculateChi2Contribution(Dx_value_m, x_position_m, skip_Dx, skip_x) ;
					}
				}
			}
		}
	}
	else
	{
		cout << "Error: fcn: the tfs file cannot be opened!" << endl ;
		exit(1) ;
	}

	tfsfile.close() ;

	return chi2 ;
}

void fcn(Int_t &npar, double *gin, double &f, double *MinuitParameter, int iflag)
{
	cout << "FCN starts" << endl ;

	double chi2 = 0 ; 
	
	ofstream perturbations("temporary_perturbation_b2.madx") ;

	int index_of_perturbation = 0 ;

	for(vector<TPerturbation>::iterator it = vector_of_Perturbation.begin() ; it != vector_of_Perturbation.end() ; ++it)
	{
		WriteMADXPerturbation(perturbations, it->name, it->type, it->absolute_value, MinuitParameter[index_of_perturbation]) ;

		index_of_perturbation++ ;
	}

	perturbations.close() ;

	ofstream momentum_perturbations("momentum_loss.madx") ;
	momentum_perturbations << "TOTEMDELTAP:=0.0;" << endl ;
	momentum_perturbations.close() ;

	system("/afs/cern.ch/user/m/mad/bin/madx job_sample_b2.madx > /dev/null") ;

	bool skip_Dx = true ;
	bool skip_x  = false ;

	chi2 += ProcessTFS(skip_Dx, skip_x) ;

	ofstream momentum_perturbations2("momentum_loss.madx") ;
	momentum_perturbations2 << "TOTEMDELTAP:=((-0.04157) / 2.0);" << endl ;
	momentum_perturbations2.close() ;

	system("/afs/cern.ch/user/m/mad/bin/madx job_sample_b2.madx > /dev/null") ;

	skip_Dx = false ;
	skip_x  = true ;

	chi2 += ProcessTFS(skip_Dx, skip_x) ;
	
	for(int i = 0 ; i < vector_of_Perturbation.size() ; i++)
	{
		double chi = (MinuitParameter[i] / vector_of_Perturbation[i].error) ;
		chi2 += (chi*chi) ;
	}

	cout << "chi2: " << chi2 << endl ;

	f = chi2  ;
}

void InitObservations()
{
	TObservationsPerDetector ObservationsPerDetector ;
	
	// XRPH.C6L5.B2

	ObservationsPerDetector.detector_name			= "\"XRPH.C6L5.B2\"" ;

	ObservationsPerDetector.Dx_m 				= 10.42e-2 ;							// TOTEM dispersion estimation
	ObservationsPerDetector.Dx_m_rel_error			= 3.85e-2 ;
	ObservationsPerDetector.valid_Dx			= true ;


	ObservationsPerDetector.beam_position_x_m 		= 0 ;
	ObservationsPerDetector.beam_position_x_m_rel_error 	= 0 ;
	ObservationsPerDetector.valid_x 			= false ;
	
	map_from_detector_name_to_ObservationsPerDetector[ObservationsPerDetector.detector_name] = ObservationsPerDetector ;

	// XRPH.D6L5.B2

	ObservationsPerDetector.detector_name			= "\"XRPH.D6L5.B2\"" ;

	ObservationsPerDetector.Dx_m 				= 10.21e-2 ;							// TOTEM dispersion estimation
	ObservationsPerDetector.Dx_m_rel_error			= 4.32e-2 ;
	ObservationsPerDetector.valid_Dx			= true ;


	ObservationsPerDetector.beam_position_x_m 		= 0 ;
	ObservationsPerDetector.beam_position_x_m_rel_error 	= 0 ;
	ObservationsPerDetector.valid_x 			= false ;
	
	map_from_detector_name_to_ObservationsPerDetector[ObservationsPerDetector.detector_name] = ObservationsPerDetector ;
	
	// XRPV.C6L5.B2

	ObservationsPerDetector.detector_name			= "\"XRPV.C6L5.B2\"" ;

	ObservationsPerDetector.Dx_m 				= 0 ;
	ObservationsPerDetector.Dx_m_rel_error			= 0 ;
	ObservationsPerDetector.valid_Dx			= false ;


	ObservationsPerDetector.beam_position_x_m 		= -3.415e-3 ;
	ObservationsPerDetector.beam_position_x_m_rel_error 	= ((0.5e-3) / ObservationsPerDetector.beam_position_x_m) ;	// From Jan's measurement
	ObservationsPerDetector.valid_x 			= true ;
	
	map_from_detector_name_to_ObservationsPerDetector[ObservationsPerDetector.detector_name] = ObservationsPerDetector ;

	// BPMSW.1L5.B2

	ObservationsPerDetector.detector_name			= "\"BPMSW.1L5.B2\"" ;

	ObservationsPerDetector.Dx_m 				= 0 ;
	ObservationsPerDetector.Dx_m_rel_error			= 0 ;
	ObservationsPerDetector.valid_Dx			= false ;


	ObservationsPerDetector.beam_position_x_m 		= -4.372e-3 ;
	ObservationsPerDetector.beam_position_x_m_rel_error 	= ((427.20e-6) / ObservationsPerDetector.beam_position_x_m) ; // After discussion with BPM experts on 2016.08.04
	ObservationsPerDetector.valid_x 			= false ;

	map_from_detector_name_to_ObservationsPerDetector[ObservationsPerDetector.detector_name] = ObservationsPerDetector ;

	ObservationsPerDetector.detector_name			= "\"BPM.5L5.B2\"" ;

	ObservationsPerDetector.Dx_m 				= 0 ;
	ObservationsPerDetector.Dx_m_rel_error			= 0 ;
	ObservationsPerDetector.valid_Dx			= false ;


	ObservationsPerDetector.beam_position_x_m 		= -3.774e-3 ;
	ObservationsPerDetector.beam_position_x_m_rel_error 	= ((427.20e-6) / ObservationsPerDetector.beam_position_x_m) ; // After discussion with BPM experts on 2016.08.04
	ObservationsPerDetector.valid_x 			= true ;

	map_from_detector_name_to_ObservationsPerDetector[ObservationsPerDetector.detector_name] = ObservationsPerDetector ;

	ObservationsPerDetector.detector_name			= "\"BPMSY.4L5.B2\"" ;

	ObservationsPerDetector.Dx_m 				= 0 ;
	ObservationsPerDetector.Dx_m_rel_error			= 0 ;
	ObservationsPerDetector.valid_Dx			= false ;


	ObservationsPerDetector.beam_position_x_m 		= -5.2e-03 ;
	ObservationsPerDetector.beam_position_x_m_rel_error 	= ((427.20e-6) / ObservationsPerDetector.beam_position_x_m) ; // After discussion with BPM experts on 2016.08.04
	ObservationsPerDetector.valid_x 			= true ;

	map_from_detector_name_to_ObservationsPerDetector[ObservationsPerDetector.detector_name] = ObservationsPerDetector ; 

/*	ObservationsPerDetector.detector_name			= "\"BPMS.2R5.B1\"" ;

	ObservationsPerDetector.Dx_m 				= 0 ;
	ObservationsPerDetector.Dx_m_rel_error			= 0 ;
	ObservationsPerDetector.valid_Dx			= false ;


	ObservationsPerDetector.beam_position_x_m 		= 3.271e-3 ;
	ObservationsPerDetector.beam_position_x_m_rel_error 	= ((427.20e-6) / ObservationsPerDetector.beam_position_x_m) ; // After discussion with BPM experts on 2016.08.04
	ObservationsPerDetector.valid_x 			= true ;

	map_from_detector_name_to_ObservationsPerDetector[ObservationsPerDetector.detector_name] = ObservationsPerDetector ;*/
}

void InitPerturbations()
{
	TPerturbation Perturbation ;
	
	Perturbation.name 	 	= "Half_crossing_angle_B2" ;
	Perturbation.type 	 	= horizontal_crossing_angle_B2 ;
	Perturbation.value 	 	= 0 ;
	Perturbation.error 	 	= Half_crossing_angle_B1_rel_error ;	
	Perturbation.upper_limit 	=  Half_crossing_angle_B1_limits ;
	Perturbation.lower_limit 	= -Half_crossing_angle_B1_limits ;
	Perturbation.step_size	 	= 0.001 ;
	Perturbation.absolute_value 	= Half_crossing_angle_B1 ;

	vector_of_Perturbation.push_back(Perturbation) ;

	Perturbation.name 	 	= "MQXA.1L5" ;
	Perturbation.type 	 	= quadrupole_position ;
	Perturbation.value 	 	= 0 ;
	Perturbation.error 	 	= Quadrupole_x_position_m_error ;	
	Perturbation.upper_limit 	=  Quadrupole_x_position_m_error ;
	Perturbation.lower_limit 	= -Quadrupole_x_position_m_error ;
	Perturbation.step_size	 	= 0.0001 ;
	Perturbation.absolute_value 	= 0 ;

	vector_of_Perturbation.push_back(Perturbation) ;

	Perturbation.name 	 	= "MQXB.A2L5" ;
	vector_of_Perturbation.push_back(Perturbation) ;
	Perturbation.name 	 	= "MQXB.B2L5" ;
	vector_of_Perturbation.push_back(Perturbation) ;
	Perturbation.name 	 	= "MQXA.3L5" ;
	vector_of_Perturbation.push_back(Perturbation) ;
	Perturbation.name 	 	= "MQY.4L5.B2" ;
	vector_of_Perturbation.push_back(Perturbation) ;
	Perturbation.name 	 	= "MQML.5L5.B2" ;
	vector_of_Perturbation.push_back(Perturbation) ;

/*
	Perturbation.name 	 	= "DELTA_X" ;
	Perturbation.type 	 	= delta_x ;
	Perturbation.value 	 	= 0 ;
	Perturbation.error 	 	= delta_x_error ;	
	Perturbation.upper_limit 	=  delta_x_error ;
	Perturbation.lower_limit 	= -delta_x_error ;
	Perturbation.step_size	 	= 0.0001 ;
	Perturbation.absolute_value 	= 0 ;


	vector_of_Perturbation.push_back(Perturbation) ;*/

	Perturbation.name 	 	= "MCBXH.1L5" ;
	Perturbation.type 	 	= kicker_strength ;
	Perturbation.value 	 	= 0 ;
	Perturbation.error 	 	= kicker_strength_error ;	
	Perturbation.upper_limit 	=  kicker_strength_error ;
	Perturbation.lower_limit 	= -kicker_strength_error ;
	Perturbation.step_size	 	= 0.0001 ;
	Perturbation.absolute_value 	= 0 ;

	vector_of_Perturbation.push_back(Perturbation) ;

	Perturbation.name 	 	= "MCBXH.2L5" ;
	Perturbation.type 	 	= kicker_strength ;
	Perturbation.value 	 	= 0 ;
	Perturbation.error 	 	= kicker_strength_error ;	
	Perturbation.upper_limit 	=  kicker_strength_error ;
	Perturbation.lower_limit 	= -kicker_strength_error ;
	Perturbation.step_size	 	= 0.0001 ;
	Perturbation.absolute_value 	= 0 ;

	vector_of_Perturbation.push_back(Perturbation) ;

	Perturbation.name 	 	= "MCBXH.3L5" ;
	Perturbation.type 	 	= kicker_strength ;
	Perturbation.value 	 	= 0 ;
	Perturbation.error 	 	= kicker_strength_error ;	
	Perturbation.upper_limit 	=  kicker_strength_error ;
	Perturbation.lower_limit 	= -kicker_strength_error ;
	Perturbation.step_size	 	= 0.0001 ;
	Perturbation.absolute_value 	= 0 ;

	vector_of_Perturbation.push_back(Perturbation) ;

}

void Init()
{
	InitObservations() ;
	InitPerturbations() ;
}

int main()
{
    Init() ;

    TMinuit *gMinuit = new TMinuit(30);
    gMinuit->SetFCN(fcn);

    Double_t arglist[20];
    Int_t ierflg = 0 ;
    arglist[0] = 1 ;
    gMinuit->mnexcm("SET ERR", arglist ,1,ierflg);

    int index_of_perturbation = 0 ;

    for(vector<TPerturbation>::iterator it = vector_of_Perturbation.begin() ; it != vector_of_Perturbation.end() ; ++it)
    {
    	gMinuit->mnparm(index_of_perturbation, it->name, it->value, it->step_size, it->lower_limit, it->upper_limit, ierflg);

	index_of_perturbation++ ;
    }

    arglist[0] = 1e-08 ;

    gMinuit->mnexcm("SET EPS", arglist ,1,ierflg);

    arglist[0] = 50000  ;
    arglist[1] = 100 ;

    gMinuit->mnexcm("MIGRAD", arglist ,2,ierflg);

	
}
	
