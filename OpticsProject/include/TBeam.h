#ifndef TBEAM_H
#define TBEAM_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <list>
using namespace std;

#include <include/TProjectParameters.h>

#include <include/TRealElement.h>
#include <include/TMagnet.h>
#include <include/TIPDescription.h>
#include <include/TRomanPot.h>
#include <include/TBeamPositionMonitor.h>
#include <include/TBeamMarker.h>
#include <include/TOpticalFunctions.h>


class TBeam : public TRealElement
{

	private :

		int job_number, beam_number, sequence ;
		bool alignment, harmonics, perturbed, matched, postponed  ;
	        double deltax, deltay, deltapx, deltapy, deltapt, deltadeltap, horizontal_crossing_angle ;
		double matched_beam_momentum ;

		static string MADXtext ;
		TProjectParameters *ProjectParameters ;
		int Random_seed ;
		int matching_status ;

	public :
	
		static const int NOT_MATCHED = 0 ;
		static const int CONVERGED = 1 ;
		static const int FAILED = 2 ;
		static const int OK = 3 ;
		static const int PROBLEMS = 4 ;
	
		TBeam(string aname, TProjectParameters *, int, int, int, bool, bool, bool, TBeam *) ;
		int Print(int) ;
		int Save(TDirectory *, TFile *,OpticsArgumentType*) ;
		double getPerturbationDeltaDeltap() { return deltadeltap ; }

		int WriteHarmonicsPart1(ofstream &) ;
		int WriteHarmonicsPart2(ofstream &) ;
		int WriteAlignment(ofstream &) ;
		int WriteNominalMagnetSettings(ofstream &) ;
		int WritePTC(ofstream &,string) ;
		int WriteRotationErrors(ofstream &) ;
		int WriteCrossingAngles(ofstream &) ;
		int WriteInjectionStrength(ofstream &) ;
		int WriteMADXHeader(ofstream &) ;
		int WriteBeamPerturbations(ofstream &, string) ;
		int IncludePerturbations(ofstream &,string) ;
		int WriteBeamDefinition(ofstream &) ;
		int WriteSequence_Range(ofstream &) ;
		int WriteMagnetSettings(ofstream &,string) ;
		int IncludeLSAStrength(ofstream &) ;
		int LoadFromROOT(string, TFile *,OpticsArgumentType *) ;
		int GetMatchingStatus() { return matching_status ; } ;
		void WriteField_harmonics_from_WISE(ofstream &) ; 		

		virtual int GenerateMADXCode(ofstream &, string) ;
		int Execute(string) ;
		int LoadTFSResults(string) ;
		int LoadTFS(string) ; 
		int RetrieveAllOpticalFunctionName(map <string,double> &) ;
		
		double getDeltaDeltap() { return deltadeltap ; } ;
		double getTotalBeamMomentum() { return (deltadeltap + matched_beam_momentum) ; } ;
}; 

TBeam::TBeam(string aname, TProjectParameters *aProjectParameters, int ajob_number, int abeam_number, int aRandom_seed, bool aperturbed, bool amatched, bool apostponed, TBeam *beam_template = NULL) : TRealElement(aname)
{

	ProjectParameters = aProjectParameters ;

	job_number = ajob_number ;
	matching_status = NOT_MATCHED ;
	alignment = ProjectParameters->getAlignment() ;
	harmonics = ProjectParameters->getHarmonics() ;
	perturbed = aperturbed ;
	matched = amatched ;
	beam_number = abeam_number ;
	postponed = apostponed ;	
	Random_seed = aRandom_seed ;
	
	matched_beam_momentum = 0 ;
	
        sequence = (beam_number == 4 ? 2 : beam_number)  ;

	double position ;
	int polarity, TOTEM_sign_conversion_2011_08_03_with_Huberts_rule, TOTEM_sign_conversion_2012_08_01_with_Huberts_rule, beam ;
	string type, nominal_strength_text, magnet_name ;

	string magnet_data_filename = "" ;
        ifstream magnet_data ;

	if(ProjectParameters->getSettingValue("LHC_sequence").compare("LHC_Run_II") == 0)
	{
		magnet_data_filename = ProjectParameters->getGeneral_data_directory() + "/magnet_data_LHC_Run_II.txt" ;
	}
	else
	{
		magnet_data_filename = ProjectParameters->getGeneral_data_directory() + "/magnet_data.txt" ;
	}

        magnet_data.open((magnet_data_filename).c_str()) ;

	if(!magnet_data.is_open())
	{
		cout << "Error: TBeam::TBeam: unable to open magnet data file ! " << magnet_data_filename << endl ;
		exit(1) ;
	}

	TRandom3 *random = new TRandom3 ;
	random->SetSeed(Random_seed) ;
        TStructForPerturbation aux ;
        set <TStructForPerturbation>::iterator it ;
	set <TStructForPerturbation> *perturbed_magnets_strength = ProjectParameters->GetPerturbedMagnets_Strength() ;
        set <TStructForPerturbation> *perturbed_magnets_rotation = ProjectParameters->GetPerturbedMagnets_Rotation() ;
        set <TStructForPerturbation> *perturbed_magnets_x = ProjectParameters->GetPerturbedMagnets_X() ;
        set <TStructForPerturbation> *perturbed_magnets_y = ProjectParameters->GetPerturbedMagnets_Y() ;

	int element_number = 0 ;

        while(magnet_data >> magnet_name) 
        {
                magnet_data >> position >> beam >> type >> TOTEM_sign_conversion_2011_08_03_with_Huberts_rule >> TOTEM_sign_conversion_2012_08_01_with_Huberts_rule >> polarity >> nominal_strength_text ;

		if((beam_number == beam) || ((beam_number == 4) && (beam == 2))) 
		{

			double strength_perturbation_factor = DEFAULT_STRENGTH_PERTURBATION_FACTOR ;
			double strength_perturbation_RMS = 0 ;
			double strength_perturbation_MEAN = 1 ;

			double rotation_perturbation = DEFAULT_ROTATION_PERTURBATION ;
			double rotation_perturbation_RMS = 0 ;
			double rotation_perturbation_MEAN = 0 ;

                        double x_perturbation = DEFAULT_X_PERTURBATION ;
                        double x_perturbation_RMS = 0 ;
                        double x_perturbation_MEAN = 0 ;

                        double y_perturbation = DEFAULT_Y_PERTURBATION ;
                        double y_perturbation_RMS = 0 ;
                        double y_perturbation_MEAN = 0 ;

		        if(perturbed)
			{
			        aux.name = magnet_name ;
			        it = perturbed_magnets_strength->find(aux) ;

			        if(it != perturbed_magnets_strength->end()) 
				{
					if(ProjectParameters->getProjectType() == Perturbations)
					{
						if((4*element_number) == Random_seed )
						{


	                                                strength_perturbation_factor = it->MEAN + it->RMS ;
        	                                        strength_perturbation_RMS = 0 ;
                	                                strength_perturbation_MEAN = it->MEAN ;
						}
					}
					else if((ProjectParameters->getProjectType() == Matching_1N) || (ProjectParameters->getProjectType() == Matching_N1))
					{
						strength_perturbation_factor = random->Gaus(it->MEAN,it->RMS) ;
		                                strength_perturbation_RMS = it->RMS ;
        		                        strength_perturbation_MEAN = it->MEAN ;
					}
				}

			        it = perturbed_magnets_rotation->find(aux) ;

				if(it != perturbed_magnets_rotation->end()) 
				{

					if(ProjectParameters->getProjectType() == Perturbations)
                                        {
						if((4*element_number + 1) == Random_seed)
						{
							//cout << "rotation: " << magnet_name << " " << endl;

	                                               	rotation_perturbation = it->MEAN + it->RMS ;
        	                                        rotation_perturbation_RMS  = 0 ;
                	                                rotation_perturbation_MEAN = it->MEAN ;
						}
                                        }
					else if((ProjectParameters->getProjectType() == Matching_1N) || (ProjectParameters->getProjectType() == Matching_N1))
					{
						rotation_perturbation = random->Gaus(it->MEAN,it->RMS) ;
						rotation_perturbation_RMS  = it->RMS ;
						rotation_perturbation_MEAN = it->MEAN ;
					}
				}

                                it = perturbed_magnets_x->find(aux) ;

                                if(it != perturbed_magnets_x->end())
                                {

                                        if(ProjectParameters->getProjectType() == Perturbations)
                                        {
                                                if((4*element_number + 2) == Random_seed)
                                                {
                                                        //cout << "x: " << magnet_name << " " << endl;

                                                        x_perturbation = it->MEAN + it->RMS ;
                                                        x_perturbation_RMS  = 0 ;
                                                        x_perturbation_MEAN = it->MEAN ;
                                                }
                                        }
                                        else if((ProjectParameters->getProjectType() == Matching_1N) || (ProjectParameters->getProjectType() == Matching_N1))
                                        {
                                                x_perturbation = random->Gaus(it->MEAN,it->RMS) ;
                                                x_perturbation_RMS  = it->RMS ;
                                                x_perturbation_MEAN = it->MEAN ;
                                        }
                                }

                                it = perturbed_magnets_y->find(aux) ;

                                if(it != perturbed_magnets_y->end())
                                {

                                        if(ProjectParameters->getProjectType() == Perturbations)
                                        {
                                                if((4*element_number + 3) == Random_seed)
                                                {
                                                        //cout << "y: " << magnet_name << " " << endl;

                                                        y_perturbation = it->MEAN + it->RMS ;
                                                        y_perturbation_RMS  = 0 ;
                                                        y_perturbation_MEAN = it->MEAN ;
                                                }
                                        }
                                        else if((ProjectParameters->getProjectType() == Matching_1N) || (ProjectParameters->getProjectType() == Matching_N1))
                                        {
                                                y_perturbation = random->Gaus(it->MEAN,it->RMS) ;
                                                y_perturbation_RMS  = it->RMS ;
                                                y_perturbation_MEAN = it->MEAN ;
                                        }
                                }
			}


			TMagnet *new_magnet =  new TMagnet(magnet_name, ProjectParameters,
                                                         strength_perturbation_RMS, strength_perturbation_MEAN, strength_perturbation_factor,
                                                         rotation_perturbation_RMS, rotation_perturbation_MEAN, rotation_perturbation,
                                                         x_perturbation_RMS, x_perturbation_MEAN, x_perturbation,
                                                         y_perturbation_RMS, y_perturbation_MEAN, y_perturbation,
                                                         matched, position,
                                                         type, TOTEM_sign_conversion_2012_08_01_with_Huberts_rule, polarity ,nominal_strength_text ) ;

			insert(new_magnet) ;
		}

		++element_number ;
        }

        deltax = deltay = deltapx = deltapy = deltapt = deltadeltap = horizontal_crossing_angle = 0 ;

	if(perturbed)
	{
		if(ProjectParameters->getDeltaX() != 0) 	deltax  	= random->Gaus(0,ProjectParameters->getDeltaX()) ; 
		if(ProjectParameters->getDeltaY() != 0) 	deltay  	= random->Gaus(0,ProjectParameters->getDeltaY()) ;
		if(ProjectParameters->getDeltaPX() != 0) 	deltapx 	= random->Gaus(0,ProjectParameters->getDeltaPX()) ;
		if(ProjectParameters->getDeltaPY() != 0) 	deltapy 	= random->Gaus(0,ProjectParameters->getDeltaPY()) ;
		if(ProjectParameters->getDeltaPT() != 0) 	deltapt		= random->Gaus(0,ProjectParameters->getDeltaPT()) ;
		
		if(ProjectParameters->getDeltaDeltaP() != 0)
		{
			if(((beam_number == 2) || (beam_number == 4)) && (ProjectParameters->getParameterValue("Apply_the_same_beam_momentum_perturbation_to_beam_1_and_2") == 1.0))
			{
				if(beam_template == NULL)
				{
					cout << "Error [TBeam::TBeam]: You set parameter \"Apply_the_same_beam_momentum_perturbation_to_beam_1_and_2\" to 1.0  but the pointer to the beam template of beam 1 is NULL." << endl ;
					exit(1) ;
				}

				deltadeltap = beam_template->getDeltaDeltap() ;
			}
			else deltadeltap = random->Gaus(0,ProjectParameters->getDeltaDeltaP()) ;
		}
	}
	
	double sign_of_horizontal_crossing_angle = 0 ;

	if(beam_number == 1) sign_of_horizontal_crossing_angle = 1 ;
	if((beam_number == 2) || (beam_number == 4)) sign_of_horizontal_crossing_angle = -1 ;
	
	horizontal_crossing_angle = sign_of_horizontal_crossing_angle * ProjectParameters->getHorizontalCrossingAngle() ;
	

	TBeamMarker *new_marker ;

	if(beam_number == 1)	
	{
		new_marker = new TBeamMarker("XRPV.A4R5.B1",148.94399999999879469,0,ProjectParameters) ;
		insert(new_marker) ;
		new_marker = new TBeamMarker("XRPV.A6R5.B1",214.628,0,ProjectParameters) ;
		insert(new_marker) ;
		new_marker = new TBeamMarker("XRPV.B6R5.B1",220.000,0,ProjectParameters) ;
		insert(new_marker) ;
	}
	else if((beam_number == 2) || (beam_number == 4))
        {
		new_marker = new TBeamMarker("XRPV.A6L5.B2",214.628,0,ProjectParameters) ;
		insert(new_marker) ;
		new_marker = new TBeamMarker("XRPV.B6L5.B2",220.000,0,ProjectParameters) ;
		insert(new_marker) ;
        }
}

int TBeam::Print(int depth)
{
        string prefix = Prefix(depth) ;

        cout << endl << prefix << "TBeam Print ----------" << endl << endl ;
        cout << prefix << "Name: " << this->GetName() << endl ;

	TOpticsObject::Print(depth) ;

        return 0 ;
}

int TBeam::Save(TDirectory *dir, TFile *rootfile, OpticsArgumentType *OpticsArgument)
{

	TOpticsObject::Save(dir, rootfile, ProjectParameters->kept_in_TTree()) ;

        Properties = new TTree("Beam_perturbations","This branch describes the perturbations of the beam");

        Properties->Branch("X",  	&deltax, "X/D");
        Properties->Branch("Y",  	&deltay, "Y/D");
        Properties->Branch("PX", 	&deltapx, "PX/D");
        Properties->Branch("PY", 	&deltapy, "PY/D");
        Properties->Branch("PT",  	&deltapt, "PT/D");
        Properties->Branch("Delta_P", 	&deltadeltap, "Delta_P/D");
	
        Properties->Branch("Horizontal_crossing_angle", 	&horizontal_crossing_angle, "Horizontal_crossing_angle/D");	

       	Properties->Branch("Matched",			&matched, 		"Matched/O");
	
	if(matched)
	{
	        Properties->Branch("MatchedBeamMomentum", 	&matched_beam_momentum, "MatchedBeamMomentum/D");
	        Properties->Branch("Matching_status", 		&matching_status, 	"Matching_status/I");			
	}

        Properties->Fill() ;

	return 0 ; 
}

int TBeam::LoadFromROOT(string path, TFile *f,OpticsArgumentType *OpticsArgument)
{

	TOpticsObject::LoadFromROOT(path,f, ProjectParameters->kept_in_TTree()) ;

	string auxpath = path + "/" + name + "/Beam_perturbations" ; 
        Properties = (TTree *)f->Get((auxpath).c_str());

        Properties->SetBranchAddress("X",  		&deltax);
        Properties->SetBranchAddress("Y",  		&deltay);
        Properties->SetBranchAddress("PX", 		&deltapx);
        Properties->SetBranchAddress("PY", 		&deltapy);
        Properties->SetBranchAddress("PT",  		&deltapt);
        Properties->SetBranchAddress("Delta_P", 	&deltadeltap);
	
        Properties->SetBranchAddress("Horizontal_crossing_angle", 	&horizontal_crossing_angle);	
      	Properties->SetBranchAddress("Matched",				&matched);
	
	Properties->GetEntry(0) ;
		
	if(matched)
	{
		Properties->SetBranchAddress("MatchedBeamMomentum", 	&matched_beam_momentum) ;
	        Properties->SetBranchAddress("Matching_status", 	&matching_status) ;
		
	        Properties->GetEntry(0) ;
	}

	return 0 ;
}

int TBeam::RetrieveAllOpticalFunctionName(map <string, double> &opticalfunctions) 
{
	map<string, TOpticsObject *>::iterator Child_iterator ;

        for ( Child_iterator = Children.begin() ; Child_iterator != Children.end() ; Child_iterator++ )
	{

		if(	(Child_iterator->second->GetType().compare("Magnet") == 0) ||
			(Child_iterator->second->GetType().compare("Marker") == 0)) ((TBeamElement *)Child_iterator->second)->RetrieveOpticalFunctions(opticalfunctions);
	}

	return 0 ; 
}

int TBeam::Execute(string dir)
{
	TOpticsObject::Execute(dir) ;

	string dirname = dir  + "/" + this->GetName() ;
	
	map<string, TOpticsObject *>::iterator Child_iterator ;
	
	if(perturbed)
	{
		string perturbations_filename = dirname + "/perturbations.madx" ;	
		ofstream perturbations ;
		perturbations.open(perturbations_filename.c_str()) ;
		
		// Magnet strength perturbations

	        for ( Child_iterator = Children.begin() ; Child_iterator != Children.end() ; Child_iterator++ )
			if((Child_iterator->second)->GetType().compare("Magnet") == 0)
			{
				if(((TMagnet *)Child_iterator->second)->getStrengthPerturbationFactor() != DEFAULT_STRENGTH_PERTURBATION_FACTOR)
					perturbations << "PERTSTR" << (Child_iterator->second)->GetName() << " = " << setprecision(ProjectParameters->getMADXPerturbationPrecision()) <<  ((TMagnet *)Child_iterator->second)->getStrengthPerturbationFactor() << " ;" << endl ; 
			}
			
		perturbations << endl ;
		
		// Magnet rotation perturbations		
			
	        for ( Child_iterator = Children.begin() ; Child_iterator != Children.end() ; Child_iterator++ )
			if((Child_iterator->second)->GetType().compare("Magnet") == 0)
			{
				if(((TMagnet *)Child_iterator->second)->getRotationPerturbation() != DEFAULT_ROTATION_PERTURBATION)
					perturbations << "PERTDPSI" << (Child_iterator->second)->GetName() << " = " << setprecision(ProjectParameters->getMADXPerturbationPrecision()) << ((TMagnet *)Child_iterator->second)->getRotationPerturbation() << " ;" << endl ; 

                                if(((TMagnet *)Child_iterator->second)->getXPerturbation() != DEFAULT_X_PERTURBATION)
                                        perturbations << "PERT_X_" << (Child_iterator->second)->GetName() << " = " << setprecision(ProjectParameters->getMADXPerturbationPrecision()) << ((TMagnet *)Child_iterator->second)->getXPerturbation() << " ;" << endl ;

				if(((TMagnet *)Child_iterator->second)->getYPerturbation() != DEFAULT_Y_PERTURBATION)
                                        perturbations << "PERT_Y_" << (Child_iterator->second)->GetName() << " = " << setprecision(ProjectParameters->getMADXPerturbationPrecision()) << ((TMagnet *)Child_iterator->second)->getYPerturbation() << " ;" << endl ;
			}
			
		perturbations << endl ;			
		
		// Beam perturbations				
			
                perturbations << "DELTAX = "  		<< setprecision(ProjectParameters->getMADXPerturbationPrecision()) 	<< deltax  	<< " ;" << endl ;
                perturbations << "DELTAY = " 		<< setprecision(ProjectParameters->getMADXPerturbationPrecision()) 	<< deltay  	<< " ;" << endl ;
                perturbations << "DELTAPX = "  		<< setprecision(ProjectParameters->getMADXPerturbationPrecision()) 	<< deltapx 	<< " ;" << endl ;
                perturbations << "DELTAPY = " 		<< setprecision(ProjectParameters->getMADXPerturbationPrecision())  	<< deltapy 	<< " ;" << endl ;
                perturbations << "DELTAPT = " 		<< setprecision(ProjectParameters->getMADXPerturbationPrecision())  	<< deltapt 	<< " ;" << endl ;
                perturbations << "DELTADELTAP = "	<< setprecision(ProjectParameters->getMADXPerturbationPrecision()) 	<< deltadeltap 	<< " ;" << endl ;
		
		if(ProjectParameters->getElasticModel() != NULL)
		{
			stringstream ss ;
			ss << beam_number ;
			
			double sign = 1 ;
			
			if(beam_number > 1 ) sign = -1 ;
			
	                perturbations << "PERT_REFERENCE_TRACK_PX_B" + ss.str() << " = " << setprecision(ProjectParameters->getMADXPerturbationPrecision()) << (sign * ProjectParameters->getReference_track_px_vector(job_number-1)) << " ;" << endl ;			
	                perturbations << "PERT_REFERENCE_TRACK_PY_B" + ss.str() << " = " << setprecision(ProjectParameters->getMADXPerturbationPrecision()) << (sign * ProjectParameters->getReference_track_py_vector(job_number-1)) << " ;" << endl ;						
		}

		perturbations.close() ;
	}

	string filename = dirname + "/opticsproject.madx" ;
	ofstream madxfile ;
	madxfile.open(filename.c_str()) ;

	GenerateMADXCode(madxfile, dir) ;

	if(matched) 
	{
	}
	else
	{
		if(ProjectParameters->getVerbosity() == VERBOSITY_ALL) cout << "TBeam Execute: MADX name = " + ProjectParameters->getMADXname() << endl ;

		if(!postponed)
		{
			system((ProjectParameters->getMADXname() + " < " + filename + " > /dev/null").c_str()) ;
			LoadTFS(dirname) ;
		}
	}	

	return 0 ;
}

int TBeam::LoadTFSResults(string dir)
{
        TOpticsObject::LoadTFSResults(dir) ;

        string dirname = dir  + "/" + this->GetName() ;

        LoadTFS(dirname) ;

        return 0 ;
}

class TRecord
{
	public :
	string name;
        string string_value ;
	double value ;
};

int TBeam::LoadTFS( string directory )
{
	map<int, TRecord > optical_functions ;
	vector<string>::iterator strit ;
 
	TRecord arecord ; 
	arecord.string_value = "" ; arecord.value = 0 ;

	int i = 0 ;
	for(strit = TOpticalFunctions::optical_function_names.begin() ; strit != TOpticalFunctions::optical_function_names.end()  ; ++strit) 
	{
		arecord.name = (*strit) ;
		optical_functions.insert(pair<int, TRecord>(i, arecord)) ;
		++i ;
	}

	ifstream tfsfile ;
	string filename = directory + "/opticsproject.tfs" ;
	string line ;
        string tfsname ;

	tfsfile.open(filename.c_str()) ;

	if(tfsfile.is_open())
        {
                while ( tfsfile.good() )
                {
                        getline (tfsfile,line);

                        if(line.substr(0,1).compare("$") == 0) break ;
                }

                while(tfsfile >> tfsname)
                {
				map<int, TRecord>::iterator it;

 				for(it = optical_functions.begin() ; it != optical_functions.end() ; ++it) 
				{
					tfsfile >> it->second.string_value ;
					if(it->second.string_value.compare("nan") == 0) it->second.value = -1 ; else 
					{
						stringstream ss(it->second.string_value) ;
						ss >> it->second.value ;
					}
				}

			map<string, TOpticsObject *>::iterator Child_iterator ;
		        for ( Child_iterator = Children.begin() ; Child_iterator != Children.end() ; Child_iterator++ ) 
			{

				if(tfsname.find((Child_iterator->second)->GetName()) != string::npos)
				{
					if(ProjectParameters->getVerbosity() == VERBOSITY_ALL) cout << "TBeam::LoadTFS Loading optical function for:"  << tfsname << endl ;
					
					(Child_iterator->second)->insert(new TOpticalFunctions("Optical functions", 
						optical_functions[0].value, 	optical_functions[1].value, 	optical_functions[2].value, 	optical_functions[3].value, 	optical_functions[4].value, 
						optical_functions[5].value, 	optical_functions[6].value, 	optical_functions[7].value, 	optical_functions[8].value, 	optical_functions[9].value, 	
						optical_functions[10].value,  	optical_functions[11].value, 	optical_functions[12].value, 	optical_functions[13].value, 	optical_functions[14].value,
					 	optical_functions[15].value,    optical_functions[16].value, 	optical_functions[17].value, 	optical_functions[18].value, 	optical_functions[19].value, 	
						optical_functions[20].value,    optical_functions[21].value, 	optical_functions[22].value, 	optical_functions[23].value, 	optical_functions[24].value, 	
						optical_functions[25].value,    optical_functions[26].value, 	optical_functions[27].value, 	optical_functions[28].value, 	optical_functions[29].value, 	
						optical_functions[30].value,    optical_functions[31].value, 	optical_functions[32].value, 	optical_functions[33].value, 	optical_functions[34].value, 	
						optical_functions[35].value,	optical_functions[36].value)) ;
                			break ;
				}
			}
                }

        }
        else
	{
		cout << "[TBeam::LoadTFS] Unable to open " << filename << " ! " << endl ;
		//exit(1) ;
	}

	tfsfile.close() ;

	if(perturbed)
	{
		filename = directory + "/opticsproject_before_matching.tfs" ;

		tfsfile.open(filename.c_str()) ;

		if(tfsfile.is_open())
        	{
                	while ( tfsfile.good() )
                	{
                        	getline (tfsfile,line);

                        	if(line.substr(0,1).compare("$") == 0) break ;
                	}

                	while(tfsfile >> tfsname)
                	{
					map<int, TRecord>::iterator it;

					for(it = optical_functions.begin() ; it != optical_functions.end() ; ++it) 
					{
						tfsfile >> it->second.string_value ;
						if(it->second.string_value.compare("nan") == 0) it->second.value = -1 ; else 
						{
							stringstream ss(it->second.string_value) ;
							ss >> it->second.value ;
						}
					}

				map<string, TOpticsObject *>::iterator Child_iterator ;
		        	for ( Child_iterator = Children.begin() ; Child_iterator != Children.end() ; Child_iterator++ ) 
				{

					if(tfsname.find((Child_iterator->second)->GetName()) != string::npos)
					{
						if(ProjectParameters->getVerbosity() == VERBOSITY_ALL) cout << "TBeam::LoadTFS Loading optical function for:"  << tfsname << endl ;

						(Child_iterator->second)->insert(new TOpticalFunctions("Optical functions before matching", 
							optical_functions[0].value, 	optical_functions[1].value, 	optical_functions[2].value, 	optical_functions[3].value, 	optical_functions[4].value, 
							optical_functions[5].value, 	optical_functions[6].value, 	optical_functions[7].value, 	optical_functions[8].value, 	optical_functions[9].value, 	
							optical_functions[10].value,  	optical_functions[11].value, 	optical_functions[12].value, 	optical_functions[13].value, 	optical_functions[14].value,
					 		optical_functions[15].value,    optical_functions[16].value, 	optical_functions[17].value, 	optical_functions[18].value, 	optical_functions[19].value, 	
							optical_functions[20].value,    optical_functions[21].value, 	optical_functions[22].value, 	optical_functions[23].value, 	optical_functions[24].value, 	
							optical_functions[25].value,    optical_functions[26].value, 	optical_functions[27].value, 	optical_functions[28].value, 	optical_functions[29].value, 	
							optical_functions[30].value,    optical_functions[31].value, 	optical_functions[32].value, 	optical_functions[33].value, 	optical_functions[34].value, 	
							optical_functions[35].value,	optical_functions[36].value)) ;

                				break ;
					}
				}
                	}

        	}
        	else
		{
			cout << "[TBeam::LoadTFS] Error : unable to open " << filename << " ! " << endl ;
//			exit(1) ;
		}

		tfsfile.close() ;
	}
	

    	ifstream strength_file ;
	filename = directory + "/opticsproject_magnet_strength.txt" ;
	strength_file.open(filename.c_str()) ;
	
	string magnet_strength_name ;

	if(strength_file.is_open())
	{
		while(strength_file >> magnet_strength_name)
        	{
			string equal_sign, semicolon ;
			double strength_value ;

			strength_file >> equal_sign ;
			strength_file >> strength_value ;
			strength_file >> semicolon ; 

			size_t pos = magnet_strength_name.find("-") ;
			string magnet_name = magnet_strength_name.substr(0,pos) ; 
			transform(magnet_name.begin(), magnet_name.end(), magnet_name.begin(), ::toupper);

			map<string, TOpticsObject *>::iterator Child_iterator ;
			for ( Child_iterator = Children.begin() ; Child_iterator != Children.end() ; Child_iterator++ )
				if(magnet_name.find((Child_iterator->second)->GetName()) != string::npos) ((TMagnet *)Child_iterator->second)->SetStrength(strength_value) ;
		}
	}
	else
	{
		cout << "[TBeam::LoadTFS] Unable to open " << filename << " ! " << endl ;
		// exit(1) ;
	}

	strength_file.close() ;
	
	if(perturbed)
	{

    		ifstream perturbations ;
		filename = directory + "/perturbations.madx" ;
		perturbations.open(filename.c_str()) ;
	
		if(perturbations.is_open())
		{
		
			const string strength_token = "PERTSTR" ;
			const string rotation_token = "PERTDPSI" ;
			const string beam_token = "DELTA" ;		
		
			string word ;		
			
			while(perturbations >> word)
        		{
				string equal_sign, comma ;
				
				if(word.find(strength_token) != string::npos)
				{

					double strength_perturbation_factor ;
					
					perturbations >> equal_sign >> strength_perturbation_factor >> comma ;
				
					string element_name = word.substr(strength_token.length(),word.length()) ;
					
					map<string, TOpticsObject *>::iterator Child_iterator ;
					for ( Child_iterator = Children.begin() ; Child_iterator != Children.end() ; Child_iterator++ )
						if(element_name.find((Child_iterator->second)->GetName()) != string::npos) 
							((TMagnet *)Child_iterator->second)->setStrengthPerturbationFactor(strength_perturbation_factor) ;
					
				}
				else if(word.find(rotation_token) != string::npos)
				{
					double rotation_perturbation ;				
					
					perturbations >> equal_sign >> rotation_perturbation >> comma ;				
					
					string element_name = word.substr(strength_token.length(),word.length()) ;
					
					map<string, TOpticsObject *>::iterator Child_iterator ;
					for ( Child_iterator = Children.begin() ; Child_iterator != Children.end() ; Child_iterator++ )
						if(element_name.find((Child_iterator->second)->GetName()) != string::npos) 
							((TMagnet *)Child_iterator->second)->setRotationPerturbation(rotation_perturbation) ;

					
				}
				else if(word.find(beam_token) != string::npos)
				{
					double beam_perturbation ;				
					perturbations >> equal_sign >> beam_perturbation >> comma ;
					
					if(word == "DELTAX") deltax = beam_perturbation ;
					if(word == "DELTAY") deltay = beam_perturbation ;
					if(word == "DELTAPX") deltapx = beam_perturbation ;
					if(word == "DELTAPY") deltapy = beam_perturbation ;
					if(word == "DELTAPT") deltapt = beam_perturbation ;
					if(word == "DELTADELTAP") deltadeltap = beam_perturbation ;
				}
			}

		}
		else
		{
			cout << "[TBeam::LoadTFS] Unable to open " << filename << " ! " << endl ;
//			exit(1) ;
		}

		perturbations.close() ;
		
	}
	
	if(matched)
	{
    		ifstream results ;
		
		int results_position = directory.find("myLHC") ;
		string results_directory = directory.substr(0,results_position) ;
		filename = results_directory + "submit.shout" ;
		results.open(filename.c_str()) ;
		
		if(results.is_open())
		{

			const string strength_token = "Strength_" ;
			const string rotation_token = "Rotation_" ;
			const string beam_token = "xi_" ;		
			
	                while (results.good() )
        	        {
                	        getline (results, line);
				
                        	if(line.find("FROM MIGRAD    STATUS=CONVERGED") != string::npos)
				{ 

					matching_status = CONVERGED ;
					
	                	        getline (results, line);
	                	        getline (results, line);
	                	        getline (results, line);									
					break ;
					
				}
				else if(line.find("FROM MIGRAD    STATUS=FAILED") != string::npos)
				{
					matching_status = FAILED ;				
					cout << "Warning [TBeam::LoadTFS]: STATUS=FAILED " << filename << " ! " << endl ;
					
	                	        getline (results, line);
	                	        getline (results, line);
	                	        getline (results, line);									
					break ;
				}
				else if(line.find("FROM MIGRAD    STATUS=PROBLEMS") != string::npos)
				{
					matching_status = PROBLEMS ;
					cout << "Warning [TBeam::LoadTFS]: STATUS=PROBLEMS" << filename << " ! " << endl ;
					
	                	        getline (results, line);
	                	        getline (results, line);
	                	        getline (results, line);									
					break ;
				}
	                }
		
			if(matching_status == NOT_MATCHED)
			{
				cout << "Error TBeam::LoadTFS]: Unknown status : " << filename << endl ;
				exit(1) ;
			}
					
			{
				for(int i = 0 ; i < ProjectParameters->getNumberOfMatchedParameters() ; ++i)
        			{
					int parameter_number ;
					string parameter_name ;
					double value, error ;
					string step_size, first_derivative ;

					results >> parameter_number >> parameter_name >> value >> error >> step_size >> first_derivative ;
					if(first_derivative.find("at") != string::npos) 
					{
						string at_limit ;
						results >> at_limit >> at_limit ;

						cout << "Warning [TBeam::LoadTFS]: variable is at limit : " << parameter_name << " in " << filename << " ! " << endl ;
					} ;

					if(parameter_name.find(strength_token) != string::npos)
					{

						string element_name = parameter_name.substr(strength_token.length(),parameter_name.length()) ;

						map<string, TOpticsObject *>::iterator Child_iterator ;
						for ( Child_iterator = Children.begin() ; Child_iterator != Children.end() ; Child_iterator++ )
							if(element_name.find((Child_iterator->second)->GetName()) != string::npos) 
							{
								((TMagnet *)Child_iterator->second)->setStrengthMatchingFactor(value) ;
							}

					}

					if(parameter_name.find(rotation_token) != string::npos)
					{

						string element_name = parameter_name.substr(rotation_token.length(),parameter_name.length()) ;

						map<string, TOpticsObject *>::iterator Child_iterator ;
						for ( Child_iterator = Children.begin() ; Child_iterator != Children.end() ; Child_iterator++ )
							if(element_name.find((Child_iterator->second)->GetName()) != string::npos) 
							{
								((TMagnet *)Child_iterator->second)->setRotationMatching(value) ;
							}

					}

					if(parameter_name.find(beam_token) != string::npos)
					{
						if((parameter_name.find("xi_beam1") != string::npos) && beam_number == 1) matched_beam_momentum = value ;
						if((parameter_name.find("xi_beam2") != string::npos) && ((beam_number == 2) || (beam_number == 4))) matched_beam_momentum = value ;					
					}
				}
			}
		}
		else
		{
			cout << "Unable to open " << filename << " ! " ;
			exit(1) ;
		}
		
		results.close() ;

	}
		
	return 0 ; 
}

int TBeam::WriteHarmonicsPart1(ofstream &madxfile)  
{
	madxfile << "print, text=\"Harmonics1: On\";\"" << endl  ;
        madxfile << "" << endl ;
        madxfile << "!##################################" << endl ;
        madxfile << " !!! b2-correction for beam 1" << endl ; 
        madxfile << "!##################################" << endl  ;
	string filename = ProjectParameters->chromaticity_dbase_dir + ProjectParameters->correction_subdbase_dir + ProjectParameters->correction_file ;
        madxfile << "call, file=\"" + filename + "\";" << endl ;
        madxfile << "" << endl ;
        madxfile << "" << endl ;
        madxfile << "!##################################" << endl ;
        madxfile << "! Coupling correction from thick model" << endl ;
        madxfile << "!##################################" << endl ;
	filename = ProjectParameters->chromaticity_dbase_dir + ProjectParameters->correction_subdbase_dir + ProjectParameters->coupling_correction_file ;
        madxfile << "call, file=\"" + filename + "\";" << endl ;
        madxfile << "" << endl ;
        madxfile << "" << endl ;
        madxfile << "!###################################" << endl ;
        madxfile << "! Correct tunes and chromaticities" << endl ;
        madxfile << "!###################################" << endl ;
	filename = ProjectParameters->chromaticity_dbase_dir + ProjectParameters->correction_subdbase_dir + ProjectParameters->chromaticity_correction ;
        madxfile << "call, file=\"" + filename + "\";" << endl ;

	return 0 ;
}

int TBeam::WriteHarmonicsPart2(ofstream &madxfile) 
{
        madxfile << "print, text=\"Harmonics2: On\";" << endl ;
        madxfile << ""  << endl ;
	string filename = ProjectParameters->chromaticity_dbase_dir +  ProjectParameters->absolute_harmonics_dbase_dir + ProjectParameters->abs_correction_MB ;
        madxfile << "  readmytable, file=\"" + filename + "\"," << endl ;
        madxfile << "  table=errors_read;" << endl ;
        madxfile << "  ptc_read_errors,overwrite;" << endl ;
        madxfile << "" << endl ;
        madxfile << "" << endl ;
	filename = ProjectParameters->chromaticity_dbase_dir +  ProjectParameters->absolute_harmonics_dbase_dir + ProjectParameters->abs_correction_MQ ;
        madxfile << "  readmytable, file=\"" + filename + "\"," << endl ;
        madxfile << "  table=errors_read;" << endl ;
        madxfile << "  ptc_read_errors;" << endl ;
        madxfile << "" << endl ;
        madxfile << "" << endl ;
	filename = ProjectParameters->chromaticity_dbase_dir +  ProjectParameters->absolute_harmonics_dbase_dir + ProjectParameters->abs_correction_MC ;
        madxfile << "  readmytable, file=\"" + filename + "\"," << endl ;
        madxfile << "  table=errors_read;" << endl ;
        madxfile << "  ptc_read_errors,overwrite;" << endl ;

	return 0 ;
}

int TBeam::WriteAlignment(ofstream &madxfile)
{

	madxfile << "print, text=\"Alignment: On\";"  << endl ;
        madxfile << ""  << endl ;
        madxfile << "!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"  << endl ;
        madxfile << "!         Call alignment errors"  << endl ;
        madxfile << "!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"  << endl ;
        madxfile << "! read errors and assign"  << endl ;
        madxfile << "! readtable, file=\"LHC-egeoc-b1.tfs\";" << endl ;
        madxfile << " readtable, file=\"/afs/cern.ch/eng/sl/online/om/repository/core/THICK_MODEL_INPUTS/ERRORS/alignment_errors/LHC-egeoc-b1.tfs\";" << endl ;
        madxfile << "!readtable, file=\"/afs/cern.ch/user/h/hagen/public/MAD/AB-ABP/geometry_no_uncertainty/LHC-egeoc-0001-b1.tfs\";"  << endl ;
        madxfile << ""  << endl ;
        madxfile << " n_elem = table(egeoc, tablelength);    value, n_elem ;" << endl ;
        madxfile << ""  << endl ;
        madxfile << " one_elem(j_elem) : macro = {" << endl ;
        madxfile << "     select, flag=error, clear;"  << endl ;
        madxfile << "     select, flag=error, range=tabstring(egeoc, name, j_elem);"  << endl ;
        madxfile << "     ealign, dx     = table(egeoc,  mc_dx,    j_elem),"  << endl ;
        madxfile << "             dy     = table(egeoc,  mc_dy,    j_elem),"  << endl ;
        madxfile << "             ds     = table(egeoc,  mc_ds,    j_elem),"  << endl ;
        madxfile << "             dphi   = table(egeoc,  mc_phi,   j_elem),"  << endl ;
        madxfile << "             dtheta = table(egeoc,  mc_theta, j_elem),"  << endl ;
        madxfile << "             dpsi   = table(egeoc,  mc_psi,   j_elem);"  << endl ;
        madxfile << "  }"  << endl ;
        madxfile << ""  << endl ;
        madxfile << " i_elem = 0;"  << endl ;
        madxfile << " while (i_elem < n_elem) { i_elem = i_elem + 1; exec,  one_elem($i_elem); }"  << endl ;
        madxfile << ""  << endl ;
        madxfile << ""  << endl ;
        madxfile << "! check that errors are correct (magnets in sector 12 only)"  << endl ;
        madxfile << "! set,    format=\"12.6f\";"  << endl ;
	
	return 0 ;
}

int TBeam::WriteNominalMagnetSettings(ofstream &madxfile)
{

	for(size_t i = 0 ; i < ProjectParameters->strength_files.size() ; ++i) 
		madxfile << "call,file=\"" +  ProjectParameters->strength_dbase_dir +  ProjectParameters->strength_files[i] + "\";"  << endl ;

	return 0 ;
}

int TBeam::WriteRotationErrors(ofstream &madxfile)
{
        madxfile << "" << endl ;
        madxfile << "!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl ;
        madxfile << "!         Call error subroutines (nominal machine and new IT/D1)" << endl ;
        madxfile << "!                         and error tables" << endl ;
        madxfile << "!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl ;
        madxfile << "" << endl ;
        madxfile << "! Error routine and measured error table for nominal LHC" << endl ;
        madxfile << "" << endl ;
        madxfile << "call,file=\"db5/measured_errors/Msubroutines.madx\";" << endl ;
        madxfile << "readtable, file=\"db5/measured_errors/rotations_Q2_integral.tab\";" << endl ;
        madxfile << "" << endl ;

	return 0 ;
}

int TBeam::WritePTC(ofstream &madxfile, string dir)
{
        madxfile << ""  << endl ;
        madxfile << "!################################"  << endl ;
        madxfile << "! PTC TWISS"  << endl ;
        madxfile << "!################################"  << endl ;
        madxfile << ""  << endl ;
        madxfile << "  ptc_create_universe;"  << endl ;
        madxfile << "  ptc_create_layout,model=2,method=2,nst=1,exact,resplit,thin=.0005,xbend=.0005;"  << endl ;
        madxfile << "  ptc_align;"  << endl ;
        madxfile << "  "  << endl ;
	
	double LorentzGamma = ProjectParameters->getEnergyperbeam() / ProjectParameters->getProtonMass() ;
	double LorentzBeta = sqrt(1 - pow(1/LorentzGamma,2)) ;
	double LorentzGammaBeta = LorentzGamma * LorentzBeta ;
	double HorizontalEmittance = ProjectParameters->getNormalizedTransversEmittanceMADXform("Horizontal", sequence) ;
	double VerticalEmittance = ProjectParameters->getNormalizedTransversEmittanceMADXform("Vertical", sequence) ;
//	cout << sequence << endl ;
	
        madxfile << "  BSX:=sqrt(table(ptc_twiss,BETX)*" << setprecision(8) << HorizontalEmittance << "/ " << setprecision(3) << LorentzGammaBeta << ");"  << endl ;
        madxfile << "  BSY:=sqrt(table(ptc_twiss,BETY)*" << setprecision(8) << VerticalEmittance   << "/ " << setprecision(3) << LorentzGammaBeta << ");"  << endl ;
        madxfile << "  rot:=table(ptc_twiss,re14)/table(ptc_twiss,re34);"  << endl ;
        madxfile << endl ;
        madxfile << "  select, flag=ptc_twiss,pattern=\"^xrpv.*\",column=name,s,x,px,y,py,betx,bety,BETA11,BETA12,BETA21,BETA22,MU1,MU2,MU3,DISP1,DISP2,DISP3,DISP4,re11,re12,re13,re14,re21,re22,re23,re24,re31,re32,re33,re34,re41,re42,re43,re44,rot,BSX,BSY;"  << endl ;
        madxfile << "  select, flag=ptc_twiss,pattern=\"ip5\";"  << endl ;
        madxfile << "  select, flag=ptc_twiss,pattern=\"^MCB.*\";"  << endl ;
        madxfile << "  select, flag=ptc_twiss,pattern=\"^MQ.*\";"  << endl ;
        madxfile << "  select, flag=ptc_twiss,pattern=\"^MB.*\";"  << endl ;
        madxfile << endl ;

        madxfile << endl ;

        if(perturbed && harmonics) WriteHarmonicsPart2(madxfile) ;

	stringstream ss ;
	ss << beam_number ;

	string beamMomentumMatched = "" ;
	if(matched && ProjectParameters->getMatchedBeamMomentum())
		beamMomentumMatched = " + DDELTADELTAP_B" + ss.str() ;

	string beamPert = "" ;
	
	stringstream horizontal_crossing_angle_string ;
	horizontal_crossing_angle_string << horizontal_crossing_angle ;
	
	string perturbed_PERT_REFERENCE_TRACK_PX_string, perturbed_PERT_REFERENCE_TRACK_PY_string ;
	
	if(perturbed && (ProjectParameters->getElasticModel() != NULL))
	{
		perturbed_PERT_REFERENCE_TRACK_PX_string = " + PERT_REFERENCE_TRACK_PX_B" + ss.str() ;
		perturbed_PERT_REFERENCE_TRACK_PY_string = " + PERT_REFERENCE_TRACK_PY_B" + ss.str() ;		
	}
	
	string vertical_beam_separation_m_string = "" ;

	if(ProjectParameters->IsParameterDefined("vertical_beam_separation_m"))
	{
		
		stringstream vertical_beam_separation_m_stringstream  ;
		vertical_beam_separation_m_stringstream << ProjectParameters->getParameterValue("vertical_beam_separation_m") ;
		vertical_beam_separation_m_string = vertical_beam_separation_m_stringstream.str() ;
	}
	
	if (perturbed || ProjectParameters->getMatchedBeamMomentum()) beamPert = ", X=DELTAX, Y=(DELTAY + " + vertical_beam_separation_m_string + " ), PX=(" + horizontal_crossing_angle_string.str() + perturbed_PERT_REFERENCE_TRACK_PX_string + " + DELTAPX), PY=(DELTAPY" + perturbed_PERT_REFERENCE_TRACK_PY_string + "), PT=0-DELTAPT, deltap=DELTADELTAP" + beamMomentumMatched ;

        string filename = dir + "/" + this->GetName() + "/opticsproject.tfs" ;

        madxfile << "  ptc_twiss,icase=5,no=2, RMATRIX, BETX=" << ProjectParameters->getBetastar_x() << ", BETY=" << ProjectParameters->getBetastar_y() <<
                        ", DX=0, DY=0.0,ALFX=0, ALFY=0, MUX=0, MUY=0, DPX=0, DY=0" << beamPert << ", table,file='" + filename + "';"  << endl ;

        madxfile << endl ;

	if(ProjectParameters->getParameterValue("DoNotWriteMagnetSettings") != 1.0)
		WriteMagnetSettings(madxfile,dir) ;

        madxfile << endl ;
        madxfile << "  ptc_end;"  << endl ;
        madxfile << endl ;
        madxfile << "stop;"  << endl ;

	return 0 ;
}

void TBeam::WriteField_harmonics_from_WISE(ofstream &madxfile)
{

        madxfile << "  " << endl ;
        madxfile << "      ! Scripts tailored for measured errors in LHC" << endl ;
        madxfile << "      call, file=\"/afs/cern.ch/user/h/hagen/public/MAD/Esubroutines.madx\";" << endl ;
        madxfile << "      call, file=\"/afs/cern.ch/eng/lhc/optics/V6.500/measured_errors/Msubroutines.madx\";" << endl ;
        madxfile << "      call, file=\"/afs/cern.ch/eng/lhc/optics/V6.500/measured_errors/Ealign_Subroutines.madx\";" << endl ;
        madxfile << "  " << endl ;
        madxfile << "      ! Activate multipoles except b1, a1 when processing field quality table" << endl ;
        madxfile << "      exec, ON_SYST;" << endl ;
        madxfile << "      ON_B1S = 0; ON_A1S = 0;" << endl ;
        madxfile << "      eoption,add=true;" << endl ;
        madxfile << "  " << endl ;
        madxfile << "      ! Rotations of magnets into tunnel reference frame" << endl ;
        madxfile << "      readtable, file=\"/afs/cern.ch/eng/lhc/optics/V6.500/measured_errors/rotations.tab\";" << endl ;
        madxfile << "  " << endl ;
        madxfile << "      ! WISE magnetic field quality (both beams and all magnet types in same table)" << endl ;

	string filename = ProjectParameters->get_nth_Field_harmonics_file_from_WISE(job_number) ;
	
        madxfile << "      readtable, file=\"" + filename + "\";" << endl ;

        madxfile << "  " << endl ;
        madxfile << "  !   call,   file=\"/afs/cern.ch/eng/lhc/optics/V6.500/measured_errors/Efcomp_MQX.madx\";" << endl ;
        madxfile << "  !   call,   file=\"/afs/cern.ch/eng/lhc/optics/V6.500/measured_errors/Efcomp_MB.madx\";" << endl ;
        madxfile << "  !   call,      file=\"/afs/cern.ch/user/h/hagen/public/MAD/Efcomp_MQ.madx\";" << endl ;
        madxfile << "  !   call,      file=\"/afs/cern.ch/eng/lhc/optics/V6.500/measured_errors/Efcomp_MQY.madx\";" << endl ;
        madxfile << "  !   call,      file=\"/afs/cern.ch/eng/lhc/optics/V6.500/measured_errors/Efcomp_MBX.madx\";" << endl ;
        madxfile << "  !   call,      file=\"/afs/cern.ch/eng/lhc/optics/V6.500/measured_errors/Efcomp_MQW.madx\";" << endl ;
        madxfile << "  " << endl ;
        madxfile << "      call,      file=\"/afs/cern.ch/user/h/hagen/public/MAD/Efcomp_MB.madx\";  ! safe" << endl ;
        madxfile << "      call,      file=\"/afs/cern.ch/eng/lhc/optics/V6.500/measured_errors/Efcomp_MBRB.madx\"; ! safe" << endl ;
        madxfile << "  !   call,      file=\"/afs/cern.ch/eng/lhc/optics/V6.500/measured_errors/Efcomp_MBRC.madx\"; ! leads to +++ memory access outside program range, fatal +++" << endl ;
        madxfile << "      call,      file=\"/afs/cern.ch/eng/lhc/optics/V6.500/measured_errors/Efcomp_MBRS.madx\"; ! safe" << endl ;
        madxfile << "      call,      file=\"/afs/cern.ch/eng/lhc/optics/V6.500/measured_errors/Efcomp_MBX.madx\";  ! safe" << endl ;
        madxfile << "      call,      file=\"/afs/cern.ch/eng/lhc/optics/V6.500/measured_errors/Efcomp_MBW.madx\";  ! safe" << endl ;
        madxfile << "      call,      file=\"/afs/cern.ch/eng/lhc/optics/V6.500/measured_errors/Efcomp_MBXW.madx\"; ! safe" << endl ;
        madxfile << "      call,      file=\"/afs/cern.ch/user/h/hagen/public/MAD/Efcomp_MQ.madx\"; ! safe" << endl ;
        madxfile << "      call,      file=\"/afs/cern.ch/user/h/hagen/public/MAD/Efcomp_MQS.madx\"; ! safe" << endl ;
        madxfile << "      call,      file=\"/afs/cern.ch/eng/lhc/optics/V6.500/measured_errors/Efcomp_MQM.madx\"; ! safe" << endl ;
        madxfile << "      call,      file=\"/afs/cern.ch/eng/lhc/optics/V6.500/measured_errors/Efcomp_MQMC.madx\"; ! safe" << endl ;
        madxfile << "      call,      file=\"/afs/cern.ch/eng/lhc/optics/V6.500/measured_errors/Efcomp_MQML.madx\"; ! safe" << endl ;
        madxfile << "      call,      file=\"/afs/cern.ch/user/h/hagen/public/MAD/Efcomp_MQT.madx\"; ! safe" << endl ;
        madxfile << "      call,      file=\"/afs/cern.ch/eng/lhc/optics/V6.500/measured_errors/Efcomp_MQTL.madx\"; ! safe" << endl ;
        madxfile << "      call,      file=\"/afs/cern.ch/eng/lhc/optics/V6.500/measured_errors/Efcomp_MQW.madx\"; ! safe" << endl ;
        madxfile << "      call,      file=\"/afs/cern.ch/eng/lhc/optics/V6.500/measured_errors/Efcomp_MQX.madx\"; ! safe" << endl ;
        madxfile << "      call,      file=\"/afs/cern.ch/user/h/hagen/public/MAD/Efcomp_MQSX.madx\"; ! safe" << endl ;
        madxfile << "      call,      file=\"/afs/cern.ch/eng/lhc/optics/V6.500/measured_errors/Efcomp_MQY.madx\"; ! safe" << endl ;
        madxfile << "  " << endl ;
        madxfile << "  " << endl ;
        madxfile << "      ! End of WISE assigned imperfections and your code continues here" << endl ;
        madxfile << "      !   exit ;" << endl ;
        madxfile << "  " << endl ;

	
}

int TBeam::WriteMagnetSettings(ofstream &madxfile, string dir)
{

	string filename = dir + "/" + this->GetName() + "/opticsproject_magnet_strength.txt" ;	
	madxfile << "system, \"[ -f " + filename + " ] && rm " + filename + "\" ;" << endl ;
	madxfile << "assign, echo = \"" + filename + "\" ;" << endl << endl ;	

	if(beam_number == 1)
	{
		madxfile << "  value, MBCS2.1R5->KS;" << endl ;
		madxfile << "  value, MQXA.1R5->K1;" << endl  ;
		madxfile << "  value, MCBXH.1R5->KICK;" << endl  ;
		madxfile << "  value, MCBXV.1R5->KICK;" << endl  ;
		madxfile << "  value, MQXB.A2R5->K1;" << endl  ;
		madxfile << "  value, MCBXH.2R5->KICK;" << endl  ;
		madxfile << "  value, MCBXV.2R5->KICK;"  << endl ;
		madxfile << "  value, MQXB.B2R5->K1;" << endl  ;
		madxfile << "  value, MQSX.3R5->K1S;" << endl  ;
		madxfile << "  value, MQXA.3R5->K1;" << endl  ;
		madxfile << "  value, MCBXH.3R5->KICK;" << endl  ;
		madxfile << "  value, MCBXV.3R5->KICK;" << endl  ;
		madxfile << "  value, MCSX.3R5->K2L;" << endl  ;
		madxfile << "  value, MCTX.3R5->K5L;"  << endl  ;
		madxfile << "  value, MCOSX.3R5->K3SL;"  << endl ;
		madxfile << "  value, MCOX.3R5->K3L;" << endl  ;
		madxfile << "  value, MCSSX.3R5->K2SL;"  << endl ;

		madxfile << "  value, MBXW.A4R5->ANGLE;"  << endl ;
		madxfile << "  value, MBXW.B4R5->ANGLE;"  << endl ;
		madxfile << "  value, MBXW.C4R5->ANGLE;" << endl  ;
		madxfile << "  value, MBXW.D4R5->ANGLE;"  << endl ;
		madxfile << "  value, MBXW.E4R5->ANGLE;"  << endl ;
		madxfile << "  value, MBXW.F4R5->ANGLE;"  << endl  ;
		madxfile << "  value, MBRC.4R5.B1->ANGLE;" << endl  ;
		madxfile << "  value, MCBYV.A4R5.B1->KICK;"  << endl ;
		madxfile << "  value, MCBYH.4R5.B1->KICK;"  << endl ;
		madxfile << "  value, MCBYV.B4R5.B1->KICK;" << endl  ;
		madxfile << "  value, MQY.4R5.B1->K1;" << endl  ;
		madxfile << "  value, MQML.5R5.B1->K1;"  << endl ;
		madxfile << "  value, MCBCH.5R5.B1->KICK;"  << endl << endl << endl ;
	}
	else
	{
		madxfile << "  value, MBCS2.1L5->KS;"  << endl ;
		madxfile << "  value, MQXA.1L5->K1;"  << endl ;
		madxfile << "  value, MCBXV.1L5->KICK;"  << endl ;
		madxfile << "  value, MCBXH.1L5->KICK;"  << endl ;
		madxfile << "  value, MQXB.A2L5->K1;"  << endl ;
		madxfile << "  value, MCBXV.2L5->KICK;"  << endl ;
		madxfile << "  value, MCBXH.2L5->KICK;"  << endl ; 
		madxfile << "  value, MQXB.B2L5->K1;"  << endl ;
		madxfile << "  value, MQSX.3L5->K1S;"  << endl ;
		madxfile << "  value, MQXA.3L5->K1;"  << endl ;
		madxfile << "  value, MCTX.3L5->K5L;"  << endl ;
		madxfile << "  value, MCSX.3L5->K2L;"  << endl ;
		madxfile << "  value, MCBXV.3L5->KICK;"  << endl ;
		madxfile << "  value, MCBXH.3L5->KICK;" << endl  ;
		madxfile << "  value, MCSSX.3L5->K2SL;"  << endl ;
		madxfile << "  value, MCOX.3L5->K3L;"  << endl ; 
		madxfile << "  value, MCOSX.3L5->K3SL;"  << endl ;

		madxfile << "  value, MBXW.A4L5->ANGLE;"  << endl ;
		madxfile << "  value, MBXW.B4L5->ANGLE;"  << endl ;
		madxfile << "  value, MBXW.C4L5->ANGLE;"  << endl ;
		madxfile << "  value, MBXW.D4L5->ANGLE;"  << endl ;
		madxfile << "  value, MBXW.E4L5->ANGLE;"  << endl ;
		madxfile << "  value, MBXW.F4L5->ANGLE;"  << endl ;
		madxfile << "  value, MBRC.4L5.B2->ANGLE;"  << endl ;
		madxfile << "  value, MCBYV.A4L5.B2->KICK;"  << endl ;
		madxfile << "  value, MCBYH.4L5.B2->KICK;"  << endl ;
		madxfile << "  value, MCBYV.B4L5.B2->KICK;"  << endl ;
		madxfile << "  value, MQY.4L5.B2->K1;"  << endl ;
		madxfile << "  value, MCBCH.5L5.B2->KICK;"  << endl ;
		madxfile << "  value, MQML.5L5.B2->K1;"  << endl << endl << endl ;
	}

	return 0 ;
}

int TBeam::WriteCrossingAngles(ofstream &madxfile)
{
        madxfile << endl ;
	madxfile << "!crossing angles"  << endl ;
        madxfile << "on_x1                :=  0 ;"  << endl ;
        madxfile << "on_x2                :=  0 ;"  << endl ;
        madxfile << "on_x5                :=  0;"  << endl ;
        madxfile << "on_x8                :=  0 ;"  << endl ;
        madxfile << ""  << endl ;
        madxfile << "!parallel separation"  << endl ;
        madxfile << "on_sep1              :=  0 ;"  << endl ;
        madxfile << "on_sep2              :=  0 ;"  << endl ;
        madxfile << "on_sep5              :=  0 ;"  << endl ;
        madxfile << "on_sep8              :=  0 ;"  << endl ;
        madxfile << ""  << endl ;
        madxfile << "!experimental magnets + compensators"  << endl ;
        madxfile << "on_alice             :=  0 ;"  << endl ;
        madxfile << "on_lhcb              :=  0 ;"  << endl ;
        madxfile << "ON_ATLAS             :=  0 ;"  << endl ;
        madxfile << "ON_CMS               :=  0 ;"  << endl ;
        madxfile << "ON_LHCB              :=  0 ;"  << endl ;
        madxfile << ""  << endl ;
        madxfile << "on_sol_cms := 7000/" << setiosflags(ios::fixed) << setprecision(2) << ProjectParameters->getEnergyperbeam() << " ;"  << endl ;
        madxfile << endl ;

	return 0 ;
}

int TBeam::WriteInjectionStrength(ofstream &madxfile) 
{
	if(ProjectParameters->getSettingValue("LHC_sequence").compare("LHC_Run_II") == 0)
	{
	}
	else if(ProjectParameters->getSettingValue("LHC_sequence").compare("LHC_Run_II_2017") == 0)
        {
		// madxfile << "call, file=\"/afs/cern.ch/eng/lhc/optics/runII/2017/opticsfile_new.1\";" << endl ;
        }
	else
	{
		madxfile << "call,file=\"/afs/cern.ch/eng/lhc/optics/V6.503/V6.5.inj.str\";"  << endl ;
	}
	
	return 0 ;	
}

int TBeam::IncludeLSAStrength(ofstream &madxfile)
{
	string LSA_file = ProjectParameters->getLSAFile() ;

	if(LSA_file.compare("") != 0)
	        madxfile << "call,file=\"" << LSA_file << "\";"  << endl ;

        return 0 ;
}


int TBeam::WriteSequence_Range(ofstream &madxfile)
{
        string range = (beam_number == 1 ? "XRPV.B6R5.B1" : "XRPV.B6L5.B2") ;
        madxfile << "use,period=lhcb" << sequence << ",range=ip5/" << range << ";" << endl ;
        madxfile << "mylhcbeam=" << beam_number << ";" << endl ;

	return 0 ;
}

int TBeam::WriteBeamDefinition(ofstream &madxfile)
{
        madxfile << ""  << endl ;
        madxfile << "Beam,particle=proton,sequence=lhcb" << sequence << ",energy=" << setiosflags(ios::fixed) << setprecision(2) << ProjectParameters->getEnergyperbeam() << ",NPART=1.15E11,sige=1.1e-4;"  << endl ;
        madxfile << "use,  sequence=lhcb" << sequence << "; mylhcbeam=" << beam_number << ";"  << endl ;

	return 0 ;
}

int TBeam::WriteBeamPerturbations(ofstream &madxfile, string dir)
{

        if(perturbed)
        {
	        madxfile <<  endl ;
		madxfile << "call,file=\"" + dir + "/" + GetName() + "/perturbations.madx\" ;" << endl ;		
	        madxfile << endl ;		
        }

	return 0 ;
}

int TBeam::IncludePerturbations(ofstream &madxfile, string dir)
{

	madxfile << "call,file=\"" + dir + "/" + GetName() + "/temporary_perturbation.madx\" ;" << endl ;

	return 0 ;
}

int TBeam::WriteMADXHeader(ofstream &madxfile)
{
        madxfile << "Option,  warn,info;" << endl ; 
        madxfile << endl ;

	string nominal_sequence_directory = "" ;
        string nominal_seqname = "" ;

	if(ProjectParameters->getSettingValue("LHC_sequence").compare("LHC_Run_II") == 0)
        {
                        nominal_sequence_directory = "/afs/cern.ch/eng/lhc/optics/runII/2016/" ;
                	madxfile << "System,\"ln -fns /afs/cern.ch/eng/lhc/optics/runII/2016 db5\";"  << endl ;

                        nominal_seqname =  nominal_sequence_directory + (beam_number == 4 ? "lhcb4_as-built.seq" : "lhc_as-built.seq") ;
        }
	else if(ProjectParameters->getSettingValue("LHC_sequence").compare("LHC_Run_II_2017") == 0)
	{
			nominal_sequence_directory = "/afs/cern.ch/eng/lhc/optics/runII/2017/" ;
	        	madxfile << "System,\"ln -fns /afs/cern.ch/eng/lhc/optics/runII/2017 db5\";"  << endl ;

			nominal_seqname =  nominal_sequence_directory + (beam_number == 4 ? "lhcb4_as-built.seq" : "lhc_as-built.seq") ;
	}
	else
	{
			nominal_sequence_directory = "/afs/cern.ch/eng/lhc/optics/V6.503/" ;
	        	madxfile << "System,\"ln -fns /afs/cern.ch/eng/lhc/optics/V6.503 db5\";"  << endl ;

			nominal_seqname =  nominal_sequence_directory + (beam_number == 4 ? "beam_four.seq" : "V6.5.seq") ;
	}


	if(ProjectParameters->getField_harmonics_from_WISE() != "") madxfile << "system, \"ln -fns /afs/cern.ch/eng/lhc/optics/V6.500/measured_errors dm\";" << endl ;
	madxfile << "set, format=\"" + ProjectParameters->getMADX_format_string() + "\";" <<  endl ;
        madxfile << endl ;
        madxfile << "option,-echo;"  << endl ;
        madxfile << endl ;

        madxfile << "call,file=\"" << nominal_seqname << "\";"  << endl ;
        madxfile << "seqedit,sequence=lhcb" << sequence << ";flatten;cycle,start=IP5;endedit;"  << endl ;

        madxfile << endl ;
        madxfile << "mylhcbeam:=" << beam_number << " ;"  << endl ;
        madxfile << endl ;
        madxfile << endl ;
        madxfile << "NRJ:= 3500.0  ; // collision"  << endl ;

	return 0 ;
}


int TBeam::GenerateMADXCode(ofstream &madxfile,string dir)
{	

	WriteMADXHeader(madxfile) ;

	WriteInjectionStrength(madxfile) ;
	WriteNominalMagnetSettings(madxfile) ;
	IncludeLSAStrength(madxfile) ;
	
	WriteBeamPerturbations(madxfile, dir) ;

	if(matched) IncludePerturbations(madxfile, dir) ; 

	WriteBeamDefinition(madxfile) ;
	WriteCrossingAngles(madxfile) ;
	WriteSequence_Range(madxfile) ;

        if (perturbed && alignment)
	{
		WriteAlignment(madxfile)	;
		WriteRotationErrors(madxfile) ;
	}

        if(perturbed && harmonics) WriteHarmonicsPart1(madxfile) ;

	map<string, TOpticsObject *>::iterator Child_iterator ;
        for ( Child_iterator = Children.begin() ; Child_iterator != Children.end() ; Child_iterator++ ) ((TRealElement *)Child_iterator->second)->GenerateMADXCode(madxfile,dir) ;
	
	if(perturbed && (ProjectParameters->getField_harmonics_from_WISE() != "")) WriteField_harmonics_from_WISE(madxfile) ;	

	WritePTC(madxfile,dir) ;


	return 0 ;
}

#endif
