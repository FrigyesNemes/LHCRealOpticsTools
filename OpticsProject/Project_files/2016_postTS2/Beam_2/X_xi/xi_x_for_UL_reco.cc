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
#include <iomanip>

const int beam_1 = 1 ;
const int beam_2 = 2 ;

int beam = beam_2 ;
string command = "" ;
string perturbations_name = "" ;
string plotname = "" ;
string twiss_filename = "" ;
double sign_of_x_coordinate = 1.0 ;

const string madx_executable = "/afs/cern.ch/user/m/mad/bin/madx" ;
const double abs_xi_upper_fraction = 0.3 ;
const double dxi_fraction = 0.001 ;

// Twiss file structure
const int number_of_words_in_twiss_line = 28 ;
const int x_position_in_twiss_file = 1 ;

const int Lx_position_in_twiss_file = 12 ;
const int Ly_position_in_twiss_file = 22 ;

const int dLxds_position_in_twiss_file = 16 ;
const int dLyds_position_in_twiss_file = 26 ;

const int Dx_position_in_twiss_file = 9 ;
const int Dy_position_in_twiss_file = 10 ;

const int vx_position_in_twiss_file = 11 ;
const int vy_position_in_twiss_file = 21 ;

vector <string> element_names ;
typedef map <string, TGraph *> map_from_element_name_to_PTGraph_type ;
typedef map <string, double> map_from_element_name_to_beam_position_type ;

map_from_element_name_to_PTGraph_type		map_from_element_name_to_x_PTGraph ;

map_from_element_name_to_PTGraph_type		map_from_element_name_to_Ly_PTGraph ;
map_from_element_name_to_PTGraph_type		map_from_element_name_to_Lx_PTGraph ;

map_from_element_name_to_PTGraph_type		map_from_element_name_to_dLy_ds_PTGraph ;
map_from_element_name_to_PTGraph_type		map_from_element_name_to_dLx_ds_PTGraph ;

map_from_element_name_to_PTGraph_type		map_from_element_name_to_vy_PTGraph ;
map_from_element_name_to_PTGraph_type		map_from_element_name_to_vx_PTGraph ;

map_from_element_name_to_PTGraph_type		map_from_element_name_to_Dy_PTGraph ;
map_from_element_name_to_PTGraph_type		map_from_element_name_to_Dx_PTGraph ;

map_from_element_name_to_PTGraph_type		map_from_element_name_to_Ly_over_dLy_ds_PTGraph ;

map_from_element_name_to_beam_position_type	map_from_element_name_to_beam_position ;

TGraph *GetGraph()
{
	TGraph *graph = new TGraph() ;

	return graph ;
}


void AllocateGraph(map_from_element_name_to_PTGraph_type *map)
{
	if(beam == beam_1)
	{
		// (*map)["\"XRPV.C6R5.B1\""] = new TGraph() ;

		(*map)["\"XRPH.C6R5.B1\""] = GetGraph() ;
		(*map)["\"XRPH.D6R5.B1\""] = GetGraph() ;

		(*map)["\"XRPV.D6R5.B1\""] = new TGraph() ;
		(*map)["\"XRPV.A6R5.B1\""] = new TGraph() ;
		(*map)["\"XRPH.A6R5.B1\""] = new TGraph() ;
		(*map)["\"XRPH.E6R5.B1\""] = new TGraph() ;
		(*map)["\"XRPH.B6R5.B1\""] = new TGraph() ;
		(*map)["\"XRPV.B6R5.B1\""] = new TGraph() ;
	}
	else if(beam == beam_2)
	{
		//(*map)["\"XRPV.C6L5.B2\""] = new TGraph() ;

		(*map)["\"XRPH.C6L5.B2\""] = GetGraph() ;
		(*map)["\"XRPH.D6L5.B2\""] = GetGraph() ;

		(*map)["\"XRPV.D6L5.B2\""] = new TGraph() ;
		(*map)["\"XRPV.A6L5.B2\""] = new TGraph() ;
		(*map)["\"XRPH.A6L5.B2\""] = new TGraph() ;
		(*map)["\"XRPH.E6L5.B2\""] = new TGraph() ;
		(*map)["\"XRPH.B6L5.B2\""] = new TGraph() ;
		(*map)["\"XRPV.B6L5.B2\""] = new TGraph() ;
	}
	else
	{
		cout << "Unknown beam " << beam << endl ;
		exit(1) ;
	}

}

void SetupDevices()
{
	if(beam == beam_1)
	{
		command = "job_sample_b1.madx" ;
		perturbations_name = "perturbations_b1.madx" ;
		plotname = "xi_as_a_function_of_x_graph_b1" ;
		twiss_filename = "result/twiss_lhcb1.tfs" ;
	}
	else if(beam == beam_2)
	{
		command = "job_sample_b2.madx" ;
		perturbations_name = "perturbations_b2.madx" ;
		plotname = "xi_as_a_function_of_x_graph_b2" ;
		twiss_filename = "result/twiss_lhcb2.tfs" ;
		sign_of_x_coordinate = -1.0 ;
	}
	else
	{
		cout << "Unknown beam " << beam << endl ;
		exit(1) ;
	}
}

int main()
{

	SetupDevices() ;
	TFile *Generic = new TFile(("plots/" + plotname + ".root").c_str(), "RECREATE") ;

	AllocateGraph(&map_from_element_name_to_x_PTGraph) ;

	AllocateGraph(&map_from_element_name_to_Lx_PTGraph) ;
	AllocateGraph(&map_from_element_name_to_Ly_PTGraph) ;

	AllocateGraph(&map_from_element_name_to_dLx_ds_PTGraph) ;
	AllocateGraph(&map_from_element_name_to_dLy_ds_PTGraph) ;

	AllocateGraph(&map_from_element_name_to_vx_PTGraph) ;
	AllocateGraph(&map_from_element_name_to_vy_PTGraph) ;

	AllocateGraph(&map_from_element_name_to_Dx_PTGraph) ;
	AllocateGraph(&map_from_element_name_to_Dy_PTGraph) ;
	
	AllocateGraph(&map_from_element_name_to_Ly_over_dLy_ds_PTGraph) ;

	SetupDevices() ;
	int number_of_steps = int(abs_xi_upper_fraction / dxi_fraction) ;

	Double_t x[number_of_steps], y[number_of_steps] ;

	double xi = 0 ;

	for(int i = 0 ; i <= number_of_steps ; ++i)
	{

		ofstream perturbations(perturbations_name.c_str()) ;

		perturbations << "TOTEMDELTAP_IN_ADDITION:=" << xi << ";" << endl ;
		perturbations.close() ;

		system((madx_executable + " < " + command + " > /dev/null").c_str()) ;

		stringstream ss ;		
		ss << setfill('0') << setw(4) << i;

		ifstream twiss_file(twiss_filename.c_str()) ;
		
		string word = "" ;
		while(twiss_file >> word)
		{
			map_from_element_name_to_PTGraph_type::iterator it ;

			it = map_from_element_name_to_x_PTGraph.find(word) ;

			if(it != map_from_element_name_to_x_PTGraph.end())
			{
				string word_2 = "" ;

				double Lx = 0 ;
				double Ly = 0 ;

				double vx = 0 ;
				double vy = 0 ;

				double Dx = 0 ;
				double Dy = 0 ;

				double dLxds = 0 ;
				double dLyds = 0 ;

				double beam_x_position = 0 ;

				for(int j = 0 ; j < number_of_words_in_twiss_line ; ++j)
				{
					if(j == Lx_position_in_twiss_file)
					{
						twiss_file >> Lx ;
					}
					else if(j == Ly_position_in_twiss_file)
					{
						twiss_file >> Ly ;
					}
					else if(j == Dx_position_in_twiss_file)
					{
						twiss_file >> Dx ;
					}
					else if(j == Dy_position_in_twiss_file)
					{
						twiss_file >> Dy ;
					}
					else if(j == vx_position_in_twiss_file)
					{
						twiss_file >> vx ;
					}
					else if(j == vy_position_in_twiss_file)
					{
						twiss_file >> vy ;
					}
					else if(j == dLxds_position_in_twiss_file)
					{
						twiss_file >> dLxds ;
					}
					else if(j == dLyds_position_in_twiss_file)
					{
						twiss_file >> dLyds ;
					}
					else if(j == x_position_in_twiss_file)
					{
						twiss_file >> beam_x_position ;
					}
					else twiss_file >> word_2 ;
				}
				
				cout << "For xi " << xi << " found " << word << " Ly: " << Ly << " Lx: " << Lx  << " Dx: " << Dx  << " Dy: " << Dy  << "\t x: " << beam_x_position << endl ;


				if(xi == 0)
				{
					map_from_element_name_to_beam_position[word] = (sign_of_x_coordinate) * beam_x_position ;
				}

				map_from_element_name_to_x_PTGraph[word]->SetPoint(i, ((sign_of_x_coordinate) * beam_x_position) - map_from_element_name_to_beam_position[word] , xi) ;
				
				map_from_element_name_to_Lx_PTGraph[word]->SetPoint(i, -xi, Lx) ;
				map_from_element_name_to_Ly_PTGraph[word]->SetPoint(i, -xi, Ly) ;

				map_from_element_name_to_dLx_ds_PTGraph[word]->SetPoint(i, -xi, dLxds) ;
				map_from_element_name_to_dLy_ds_PTGraph[word]->SetPoint(i, -xi, dLyds) ;

				map_from_element_name_to_vx_PTGraph[word]->SetPoint(i, -xi, vx) ;
				map_from_element_name_to_vy_PTGraph[word]->SetPoint(i, -xi, vy) ;

				map_from_element_name_to_Dx_PTGraph[word]->SetPoint(i, -xi, Dx) ;
				map_from_element_name_to_Dy_PTGraph[word]->SetPoint(i, -xi, Dy) ;

				map_from_element_name_to_Ly_over_dLy_ds_PTGraph[word]->SetPoint(i, -xi, (Ly/dLyds)) ;

			
			}
		}
		
		twiss_file.close() ;

		string rm_command = "rm " + twiss_filename ;
		system(rm_command.c_str()) ;

		xi -= dxi_fraction ;
	}

	
	for(map_from_element_name_to_PTGraph_type::iterator it = map_from_element_name_to_x_PTGraph.begin() ; it != map_from_element_name_to_x_PTGraph.end(); ++it)
	{
		cout << it->first << endl ;

		string graph_name = it->first.substr(1, 4) + "_" + it->first.substr(6, 4) + "_" + it->first.substr(11, 2) ;

		it->second->SetName(graph_name.c_str()) ;
		it->second->GetXaxis()->SetTitle("x [m]") ;
		it->second->GetYaxis()->SetTitle("#xi") ;

		it->second->Write() ;
		
	} 

	Generic->Write() ;
	
	exit(0) ;
}
	
