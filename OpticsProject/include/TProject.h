#ifndef TPROJECT_H
#define TPROJECT_H

#include <TCanvas.h>
#include <TH1D.h>
#include <TError.h>

#include <TTree.h>
#include <include/TMatching.h>
#include <include/TProjectParameters.h>

class TProject : public TOpticsObject
{

	private:
		TProjectParameters *ProjectParameters ;
		TFile *configurationFile ; 		
		
	static const int rad_to_mrad = 1000 ;		
	static const int ratio_to_percent = 100 ;
	
	static const int print_out_Ly_near = 1 ;
	static const int print_out_Ly_far = 2 ;
	static const int print_out_Lx_near = 3 ; 
	static const int print_out_Lx_far = 4 ;

	static const int print_out_nu_y_near = 5 ;
	static const int print_out_nu_y_far = 6 ;
	static const int print_out_nu_x_near = 7 ;
	static const int print_out_nu_x_far = 8 ;
	
	static const int STRENGTH = 1 ;
	static const int ROTATION = 2 ;
	static const int BEAM_MOMENTUM = 3 ;	
	
	static const bool after_matching = true ;	
	static const bool before_matching = false ;		
	

	public:

		TProject(string, TProjectParameters *) ;
		TProject(string, string) ;
		int Save(TDirectory *) ;
		int LoadFromROOT(string,TFile *,OpticsArgumentType * = NULL) ;
		int Print(int) ;
		int Execute(string) ;
		int LoadTFSResults(string) ;
		int Run() ;
		int Analysis(string) ;
		int Analysis_per_optical_function(int, int) ;
		int Strength_analysis_per_magnet(std::string, int, int, int) ;

		TProjectParameters *GetProjectParameters() { return ProjectParameters ; } 
				
}; 

TProject::TProject(string aname, string project_filename)
{
	name = aname ;

        ProjectParameters = new TProjectParameters("Parameters",project_filename) ;
        insert( new TMatching("Matching", ProjectParameters)) ;	

	ProjectParameters->setRootObject(this) ;
}

TProject::TProject(string aname, TProjectParameters *aProjectParameters) : TOpticsObject(aname)
{

	name = aname ;
	ProjectParameters = aProjectParameters ;  
	insert( new TMatching("Matching", ProjectParameters)) ;
}

int TProject::Print(int depth)
{
	string prefix=Prefix(depth) ;

        cout << endl << prefix << "TProject Print ----------" << endl << endl ;
        cout << prefix << "Name: " << this->GetName() << endl ;
        cout << prefix << "Project directory: " << ProjectParameters->getProjectDirectory() ;

	ProjectParameters->Print(depth) ;

	TOpticsObject::Print(depth) ;

	return 0 ; 
}

int TProject::Save(TDirectory *dir)
{
//        configurationFile = new TFile((ProjectParameters->getProjectDirectory() + "/" + ProjectParameters->getROOTfilename()).c_str(),"CREATE") ;
        configurationFile = new TFile((ProjectParameters->getROOTfilename()).c_str(),"CREATE") ;
	configurationFile->cd() ;
	Directory = configurationFile->GetDirectory("") ;
	configurationFile->SetCompressionLevel(9);

	TOpticsObject::Save(Directory,configurationFile) ;
	ProjectParameters->Save(Directory,configurationFile) ;

	configurationFile->Write() ;
	configurationFile->Close() ;

	return 0 ;
}

int TProject::LoadFromROOT(string path, TFile *f, OpticsArgumentType *OpticsArgument)
{
//        configurationFile = new TFile((ProjectParameters->getProjectDirectory() + "/" + ProjectParameters->getROOTfilename()).c_str(),"READ") ;
        configurationFile = new TFile((ProjectParameters->getROOTfilename()).c_str(),"READ") ;

        TOpticsObject::LoadFromROOT(path,configurationFile) ;
        ProjectParameters->LoadFromROOT(path,configurationFile) ;

	configurationFile->Close() ;

        return 0 ;
}

int TProject::Execute(string dir)
{
	TOpticsObject::Execute(ProjectParameters->getProjectDirectory()) ;

	return 0 ; 
}

int TProject::LoadTFSResults(string dir)
{
        TOpticsObject::LoadTFSResults(ProjectParameters->getProjectDirectory()) ;

        return 0 ;
}

int TProject::Analysis_per_optical_function(int analyis_code, int before_or_after_matching)
{
	int numberofchildren = (int)Children["Matching"]->getNumberOfChildren() ;

        string filename_b1, filename_b2, marker_name_b1, marker_name_b2, x_axis_title ;
	string param_begin ;
	
	string filename_text ;
	if(before_or_after_matching == after_matching)	filename_text = "after" ;
	else if(before_or_after_matching == before_matching) filename_text = "before" ;
	
	string subdir_text ;
	if(before_or_after_matching == after_matching)	subdir_text = "After_matching" ;
	else if(before_or_after_matching == before_matching) subdir_text = "Before_matching" ;
	
	string optical_function_name_text ;
	if(before_or_after_matching == after_matching)	optical_function_name_text = "/     0.000 Optical functions" ;
	else if(before_or_after_matching == before_matching) optical_function_name_text = "/     0.000 Optical functions before matching" ;	

        Double_t Ly_distribution_upper_bound = 12 ;
        Double_t Ly_distribution_lower_bound = -12 ;

        if(analyis_code == print_out_Lx_near)
        {
                filename_b1 = "Lx_distribution_" + filename_text + "_matching_b1_near" ;
                filename_b2 = "Lx_distribution_" + filename_text + "_matching_b2_near" ;
                marker_name_b1 = "   214.628 XRPV.A6R5.B1" ;
                marker_name_b2 = "   214.628 XRPV.A6L5.B2" ;
                x_axis_title = "near" ;
		param_begin = "Lx" ;
		
	        Ly_distribution_upper_bound = 35 ;
        	Ly_distribution_lower_bound = -35 ;
		
        }
        else if(analyis_code == print_out_Lx_far)
        {
                filename_b1 = "Lx_distribution_" + filename_text + "_matching_b1_far" ;
                filename_b2 = "Lx_distribution_" + filename_text + "_matching_b2_far" ;
                marker_name_b1 = "   220.000 XRPV.B6R5.B1" ;
                marker_name_b2 = "   220.000 XRPV.B6L5.B2" ;
                x_axis_title = "far" ;
		param_begin = "Lx" ;
		
	        Ly_distribution_upper_bound = 35 ;
        	Ly_distribution_lower_bound = -35 ;
		
        }
        else if(analyis_code == print_out_Ly_near)
        {
                filename_b1 = "Ly_distribution_" + filename_text + "_matching_b1_near" ;
                filename_b2 = "Ly_distribution_" + filename_text + "_matching_b2_near" ;
		
                marker_name_b1 = "   214.628 XRPV.A6R5.B1" ;
                marker_name_b2 = "   214.628 XRPV.A6L5.B2" ;
                x_axis_title = "near" ;
		param_begin = "Ly" ;
		
	        Ly_distribution_upper_bound = 120 ;
	        Ly_distribution_lower_bound = -120 ;
		
        }
        else if(analyis_code == print_out_Ly_far)
        {
                filename_b1 = "Ly_distribution_" + filename_text + "_matching_b1_far" ;
                filename_b2 = "Ly_distribution_" + filename_text + "_matching_b2_far" ;
                marker_name_b1 = "   220.000 XRPV.B6R5.B1" ;
                marker_name_b2 = "   220.000 XRPV.B6L5.B2" ;
                x_axis_title = "far" ;
		param_begin = "Ly" ;
		
	        Ly_distribution_upper_bound = 120 ;
	        Ly_distribution_lower_bound = -120 ;
		
        }
        else if(analyis_code == print_out_nu_x_near)
        {
                filename_b1 = "nu_x_distribution_" + filename_text + "_matching_b1_near" ;
                filename_b2 = "nu_x_distribution_" + filename_text + "_matching_b2_near" ;
                marker_name_b1 = "   214.628 XRPV.A6R5.B1" ;
                marker_name_b2 = "   214.628 XRPV.A6L5.B2" ;
                x_axis_title = "near" ;
		param_begin = "nu_x" ;
        }
        else if(analyis_code == print_out_nu_x_far)
        {
                filename_b1 = "nu_x_distribution_" + filename_text + "_matching_b1_far" ;
                filename_b2 = "nu_x_distribution_" + filename_text + "_matching_b2_far" ;
                marker_name_b1 = "   220.000 XRPV.B6R5.B1" ;
                marker_name_b2 = "   220.000 XRPV.B6L5.B2" ;
                x_axis_title = "far" ;
		param_begin = "nu_x" ;
        }
        else if(analyis_code == print_out_nu_y_near)
        {
                filename_b1 = "nu_y_distribution_" + filename_text + "_matching_b1_near" ;
                filename_b2 = "nu_y_distribution_" + filename_text + "_matching_b2_near" ;
                marker_name_b1 = "   214.628 XRPV.A6R5.B1" ;
                marker_name_b2 = "   214.628 XRPV.A6L5.B2" ;
                x_axis_title = "near" ;
		param_begin = "nu_y" ;
        }
        else if(analyis_code == print_out_nu_y_far)
        {
                filename_b1 = "nu_y_distribution_" + filename_text + "_matching_b1_far" ;
                filename_b2 = "nu_y_distribution_" + filename_text + "_matching_b2_far" ;
                marker_name_b1 = "   220.000 XRPV.B6R5.B1" ;
                marker_name_b2 = "   220.000 XRPV.B6L5.B2" ;
                x_axis_title = "far" ;
		param_begin = "nu_y" ;
        }

        Int_t Ly_distribution_resolution = 200 ;

        TH1D *Ly_distribution_b1 = new TH1D(filename_b1.c_str(), filename_b1.c_str(), Ly_distribution_resolution,
                Ly_distribution_lower_bound, Ly_distribution_upper_bound) ;
		
        for(int i = 1 ; i < numberofchildren ; ++i)  // 1 job is the nominal, this why it start from 0 and there is < and not <=
        {
                stringstream ss ;
                ss << i ;
		
		if(((TBeam *)cd(("Matching/Job"+ ss.str() +"/Missile/myLHC/Beam_1/").c_str(), 0))->GetMatchingStatus() == TBeam::CONVERGED)
		{
			double target, missile, nominal ;
			target = missile = nominal = 0 ;

			if((analyis_code == print_out_Ly_far) || (analyis_code == print_out_Ly_near))
			{
	                	target = ((TOpticalFunctions *)cd(("Matching/Job"+ ss.str()+"/Target/myLHC/Beam_1/" + marker_name_b1  + "/     0.000 Optical functions").c_str(), 0))->getLy() ;
        	        	missile = ((TOpticalFunctions *)cd(("Matching/Job"+ ss.str()+"/Missile/myLHC/Beam_1/" + marker_name_b1  + optical_function_name_text).c_str(), 0))->getLy() ;
                		nominal = ((TOpticalFunctions *)cd("Matching/Anominal/MAD_X/myLHC/Beam_1/" + marker_name_b1  + "/     0.000 Optical functions", 0))->getLy() ;
			}
			else if((analyis_code == print_out_Lx_far) || (analyis_code == print_out_Lx_near))
                	{
                        	target = ((TOpticalFunctions *)cd(("Matching/Job"+ ss.str()+"/Target/myLHC/Beam_1/" + marker_name_b1  + "/     0.000 Optical functions").c_str(), 0))->getLx() ;
                        	missile = ((TOpticalFunctions *)cd(("Matching/Job"+ ss.str()+"/Missile/myLHC/Beam_1/" + marker_name_b1  + optical_function_name_text).c_str(), 0))->getLx() ;
                        	nominal = ((TOpticalFunctions *)cd("Matching/Anominal/MAD_X/myLHC/Beam_1/" + marker_name_b1  + "/     0.000 Optical functions", 0))->getLx() ;

                	}
                	else if((analyis_code == print_out_nu_x_far) || (analyis_code == print_out_nu_x_near))
                	{
                        	target = ((TOpticalFunctions *)cd(("Matching/Job"+ ss.str()+"/Target/myLHC/Beam_1/" + marker_name_b1  + "/     0.000 Optical functions").c_str(), 0))->get_nu_x() ;
                        	missile = ((TOpticalFunctions *)cd(("Matching/Job"+ ss.str()+"/Missile/myLHC/Beam_1/" + marker_name_b1  + optical_function_name_text).c_str(), 0))->get_nu_x() ;
                        	nominal = ((TOpticalFunctions *)cd("Matching/Anominal/MAD_X/myLHC/Beam_1/" + marker_name_b1  + "/     0.000 Optical functions", 0))->get_nu_x() ;
                	}
                	else if((analyis_code == print_out_nu_y_far) || (analyis_code == print_out_nu_y_near))
                	{
                        	target = ((TOpticalFunctions *)cd(("Matching/Job"+ ss.str()+"/Target/myLHC/Beam_1/" + marker_name_b1  + "/     0.000 Optical functions").c_str(), 0))->get_nu_y() ;
                        	missile = ((TOpticalFunctions *)cd(("Matching/Job"+ ss.str()+"/Missile/myLHC/Beam_1/" + marker_name_b1  + optical_function_name_text).c_str(), 0))->get_nu_y() ;
                        	nominal = ((TOpticalFunctions *)cd("Matching/Anominal/MAD_X/myLHC/Beam_1/" + marker_name_b1  + "/     0.000 Optical functions", 0))->get_nu_y() ;
                	}
			
			Ly_distribution_b1->Fill(100*((missile - target)/nominal)) ;			
		}
	



        }
	
	string near_far_text ;
	if(x_axis_title == "near") near_far_text = "RP215_m" ;
	if(x_axis_title == "far") near_far_text =  "RP220_m" ;

	string parameter_name = param_begin  + "_beam1_" + near_far_text ;
	cout << "<parameter> " << parameter_name << " \t" << Ly_distribution_b1->GetMean(1) << endl ;

        Ly_distribution_b1->SetTitle(("The distribution of #deltaL_{y," + x_axis_title + "}/L_{y,nominal," + x_axis_title + "} after matching for beam 1").c_str()) ;
        Ly_distribution_b1->GetXaxis()->SetTitle(("#deltaL_{y,"+ x_axis_title + "}/L_{y,nominal," + x_axis_title + "} [%]").c_str()) ;
        Ly_distribution_b1->GetYaxis()->SetTitle("Entries") ;

        TCanvas *c1 = new TCanvas ;
        Ly_distribution_b1->Draw() ;
	
	double underflow = Ly_distribution_b1->GetBinContent(0) ;
	double overflow = Ly_distribution_b1->GetBinContent(Ly_distribution_b1->GetNbinsX()+1) ;	
	
	if(overflow != 0)  cout << "[TProject::Analysis_per_optical_function] Warning ! Overflow in histogram : " << filename_b1 << ". Entries in overflow bin = " << overflow << endl  ;
	if(underflow != 0)  cout << "[TProject::Analysis_per_optical_function] Warning ! Underflow in histogram : " << filename_b1 << ". Entries in underflow bin = " << underflow << endl  ;	
	
//	if(Ly_distribution_b1->IsBinOverflow() != 0) cout << "[TProject::Analysis_per_optical_function] Overflow in histogram : " << filename_b1 << endl ;
	
        Ly_distribution_b1->SaveAs((ProjectParameters->getProjectDirectory() + "/Results/Optical_functions/" + subdir_text + "/Beam_1/" + filename_b1 + ".root").c_str()) ;

        TH1D *Ly_distribution_b2 = new TH1D(filename_b2.c_str(), filename_b2.c_str(), Ly_distribution_resolution,
                Ly_distribution_lower_bound, Ly_distribution_upper_bound);

        for(int i = 1 ; i < numberofchildren ; ++i)
        {
                stringstream ss ;
                ss << i ;

		if(((TBeam *)cd(("Matching/Job"+ ss.str() +"/Missile/myLHC/Beam_1/").c_str(), 0))->GetMatchingStatus() == TBeam::CONVERGED)
		{
			double target, missile, nominal ;
			target = missile = nominal = 0 ;

                	if((analyis_code == print_out_Ly_far) || (analyis_code == print_out_Ly_near))
                	{
                        	target = ((TOpticalFunctions *)cd(("Matching/Job"+ ss.str()+"/Target/myLHC/Beam_4/" + marker_name_b2  + "/     0.000 Optical functions").c_str(), 0))->getLy() ;
                        	missile = ((TOpticalFunctions *)cd(("Matching/Job"+ ss.str()+"/Missile/myLHC/Beam_4/" + marker_name_b2  + optical_function_name_text).c_str(), 0))->getLy() ;
                        	nominal = ((TOpticalFunctions *)cd("Matching/Anominal/MAD_X/myLHC/Beam_4/" + marker_name_b2  + "/     0.000 Optical functions", 0))->getLy() ;
                	}
                	else if((analyis_code == print_out_Lx_far) || (analyis_code == print_out_Lx_near))
                	{
                        	target = ((TOpticalFunctions *)cd(("Matching/Job"+ ss.str()+"/Target/myLHC/Beam_4/" + marker_name_b2  + "/     0.000 Optical functions").c_str(), 0))->getLx() ;
                        	missile = ((TOpticalFunctions *)cd(("Matching/Job"+ ss.str()+"/Missile/myLHC/Beam_4/" + marker_name_b2  + optical_function_name_text).c_str(), 0))->getLx() ;
                        	nominal = ((TOpticalFunctions *)cd("Matching/Anominal/MAD_X/myLHC/Beam_4/" + marker_name_b2  + "/     0.000 Optical functions", 0))->getLx() ;
                	}
                	else if((analyis_code == print_out_nu_x_far) || (analyis_code == print_out_nu_x_near))
                	{
                        	target = ((TOpticalFunctions *)cd(("Matching/Job"+ ss.str()+"/Target/myLHC/Beam_4/" + marker_name_b2  + "/     0.000 Optical functions").c_str(), 1))->get_nu_x() ;
                        	missile = ((TOpticalFunctions *)cd(("Matching/Job"+ ss.str()+"/Missile/myLHC/Beam_4/" + marker_name_b2  + optical_function_name_text).c_str(), 0))->get_nu_x() ;
                        	nominal = ((TOpticalFunctions *)cd("Matching/Anominal/MAD_X/myLHC/Beam_4/" + marker_name_b2  + "/     0.000 Optical functions", 0))->get_nu_x() ;
                	}
                	else if((analyis_code == print_out_nu_y_far) || (analyis_code == print_out_nu_y_near))
                	{
                        	target = ((TOpticalFunctions *)cd(("Matching/Job"+ ss.str()+"/Target/myLHC/Beam_4/" + marker_name_b2  + "/     0.000 Optical functions").c_str(), 2))->get_nu_y() ;
                        	missile = ((TOpticalFunctions *)cd(("Matching/Job"+ ss.str()+"/Missile/myLHC/Beam_4/" + marker_name_b2  + optical_function_name_text).c_str(), 2))->get_nu_y() ;
                        	nominal = ((TOpticalFunctions *)cd("Matching/Anominal/MAD_X/myLHC/Beam_4/" + marker_name_b2  + "/     0.000 Optical functions" ,2))->get_nu_y() ;
                	}

			Ly_distribution_b2->Fill(100*((missile - target)/nominal)) ;
		}
        }

        parameter_name = param_begin  + "_beam2_" + near_far_text ;
        cout << "<parameter> " << parameter_name << " \t" << Ly_distribution_b2->GetMean(1) << endl ;

        Ly_distribution_b2->SetTitle(("The distribution of #deltaL_{y," + x_axis_title + "}/L_{y,nominal," + x_axis_title + "} after matching for beam 2").c_str()) ;
        Ly_distribution_b2->GetXaxis()->SetTitle(("#deltaL_{y," + x_axis_title + "}/L_{y,nominal," + x_axis_title + "} [%]").c_str()) ;
        Ly_distribution_b2->GetYaxis()->SetTitle("Entries") ;

        TCanvas *c5 = new TCanvas ;
        Ly_distribution_b2->Draw() ;
        Ly_distribution_b2->SaveAs((ProjectParameters->getProjectDirectory() + "/Results/Optical_functions/" + subdir_text + "/Beam_4/" + filename_b2 + ".root").c_str()) ;

	return 0 ;
}

int TProject::Strength_analysis_per_magnet(string magnet_name, int beam, int type, int before_or_after_matching) 
{

	int numberofchildren = (int)Children["Matching"]->getNumberOfChildren() ;
	
	string titletext, subdirectory ;
	
	if(before_or_after_matching == after_matching)
	{
		titletext = "after" ;
		subdirectory = "After_matching/" ;
	}
	else if(before_or_after_matching == before_matching)
	{
		titletext = "before" ;	
		subdirectory = "Before_matching/" ;
	}
	

	map<string, string> map_element_name_element_key ;

	map_element_name_element_key["MBCS2.1R5"]=      "     6.500 MBCS2.1R5" ;
	map_element_name_element_key["MQXA.1R5"]=       "    29.335 MQXA.1R5" ;
	map_element_name_element_key["MCBXH.1R5"]=      "    29.842 MCBXH.1R5" ;
	map_element_name_element_key["MCBXV.1R5"]=      "    29.842 MCBXV.1R5" ;
	map_element_name_element_key["MQXB.A2R5"]=      "    37.550 MQXB.A2R5" ;
	map_element_name_element_key["MCBXH.2R5"]=      "    38.019 MCBXH.2R5" ;
	map_element_name_element_key["MCBXV.2R5"]=      "    38.019 MCBXV.2R5" ;
	map_element_name_element_key["MQXB.B2R5"]=      "    44.050 MQXB.B2R5" ;
	map_element_name_element_key["MQSX.3R5"]=       "    46.719 MQSX.3R5" ;
	map_element_name_element_key["MQXA.3R5"]=       "    53.335 MQXA.3R5" ;
	map_element_name_element_key["MCBXH.3R5"]=      "    53.814 MCBXH.3R5" ;
	map_element_name_element_key["MCBXV.3R5"]=      "    53.814 MCBXV.3R5" ;
	map_element_name_element_key["MCSX.3R5"]=       "    53.814 MCSX.3R5" ;
	map_element_name_element_key["MCTX.3R5"]=       "    53.814 MCTX.3R5" ;
	map_element_name_element_key["MCOSX.3R5"]=      "    54.297 MCOSX.3R5" ;
	map_element_name_element_key["MCOX.3R5"]=       "    54.297 MCOX.3R5" ;
	map_element_name_element_key["MCSSX.3R5"]=      "    54.297 MCSSX.3R5" ;
	map_element_name_element_key["MBXW.A4R5"]=      "    63.022 MBXW.A4R5" ;
	map_element_name_element_key["MBXW.B4R5"]=      "    67.288 MBXW.B4R5" ;
	map_element_name_element_key["MBXW.C4R5"]=      "    71.554 MBXW.C4R5" ;
	map_element_name_element_key["MBXW.D4R5"]=      "    75.820 MBXW.D4R5" ;
	map_element_name_element_key["MBXW.E4R5"]=      "    80.086 MBXW.E4R5" ;
	map_element_name_element_key["MBXW.F4R5"]=      "    84.352 MBXW.F4R5" ;
	map_element_name_element_key["MBRC.4R5.B1"]=    "   162.625 MBRC.4R5.B1" ;
	map_element_name_element_key["MCBYV.A4R5.B1"]=  "   164.888 MCBYV.A4R5.B1" ;
	map_element_name_element_key["MCBYH.4R5.B1"]=   "   166.185 MCBYH.4R5.B1" ;
	map_element_name_element_key["MCBYV.B4R5.B1"]=  "   167.481 MCBYV.B4R5.B1" ;
	map_element_name_element_key["MQY.4R5.B1"]=     "   171.253 MQY.4R5.B1" ;
	map_element_name_element_key["MQML.5R5.B1"]=    "   198.890 MQML.5R5.B1" ;
	map_element_name_element_key["MCBCH.5R5.B1"]=   "   199.982 MCBCH.5R5.B1" ;
	map_element_name_element_key["XRPV.A4R5.B1"]=   "   148.944 XRPV.A4R5.B1" ;
	map_element_name_element_key["XRPV.A6R5.B1"]=   "   214.628 XRPV.A6R5.B1" ;
	map_element_name_element_key["XRPV.B6R5.B1"]=   "   220.000 XRPV.B6R5.B1" ;
	map_element_name_element_key["MBCS2.1L5"]=      "     6.500 MBCS2.1L5" ;
	map_element_name_element_key["MQXA.1L5"]=       "    29.335 MQXA.1L5" ;
	map_element_name_element_key["MCBXV.1L5"]=      "    29.842 MCBXV.1L5" ;
	map_element_name_element_key["MCBXH.1L5"]=      "    29.842 MCBXH.1L5" ;
	map_element_name_element_key["MQXB.A2L5"]=      "    37.550 MQXB.A2L5" ;
	map_element_name_element_key["MCBXV.2L5"]=      "    38.019 MCBXV.2L5" ;
	map_element_name_element_key["MCBXH.2L5"]=      "    38.019 MCBXH.2L5" ;
	map_element_name_element_key["MQXB.B2L5"]=      "    44.050 MQXB.B2L5" ;
	map_element_name_element_key["MQSX.3L5"]=       "    46.719 MQSX.3L5" ;
	map_element_name_element_key["MQXA.3L5"]=       "    53.335 MQXA.3L5" ;
	map_element_name_element_key["MCTX.3L5"]=       "    53.814 MCTX.3L5" ;
	map_element_name_element_key["MCSX.3L5"]=       "    53.814 MCSX.3L5" ;
	map_element_name_element_key["MCBXV.3L5"]=      "    53.814 MCBXV.3L5" ;
	map_element_name_element_key["MCBXH.3L5"]=      "    53.814 MCBXH.3L5" ;
	map_element_name_element_key["MCSSX.3L5"]=      "    54.297 MCSSX.3L5" ;
	map_element_name_element_key["MCOX.3L5"]=       "    54.297 MCOX.3L5" ;
	map_element_name_element_key["MCOSX.3L5"]=      "    54.297 MCOSX.3L5" ;
	map_element_name_element_key["MBXW.A4L5"]=      "    63.022 MBXW.A4L5" ;
	map_element_name_element_key["MBXW.B4L5"]=      "    67.288 MBXW.B4L5" ;
	map_element_name_element_key["MBXW.C4L5"]=      "    71.554 MBXW.C4L5" ;
	map_element_name_element_key["MBXW.D4L5"]=      "    75.820 MBXW.D4L5" ;
	map_element_name_element_key["MBXW.E4L5"]=      "    80.086 MBXW.E4L5" ;
	map_element_name_element_key["MBXW.F4L5"]=      "    84.352 MBXW.F4L5" ;
	map_element_name_element_key["MBRC.4L5.B2"]=    "   162.625 MBRC.4L5.B2" ;
	map_element_name_element_key["MCBYV.A4L5.B2"]=  "   164.888 MCBYV.A4L5.B2" ;
	map_element_name_element_key["MCBYH.4L5.B2"]=   "   166.185 MCBYH.4L5.B2" ;
	map_element_name_element_key["MCBYV.B4L5.B2"]=  "   167.481 MCBYV.B4L5.B2" ;
	map_element_name_element_key["MQY.4L5.B2"]=     "   171.253 MQY.4L5.B2" ;
	map_element_name_element_key["MCBCH.5L5.B2"]=   "   193.898 MCBCH.5L5.B2" ;
	map_element_name_element_key["MQML.5L5.B2"]=    "   198.890 MQML.5L5.B2" ;
	map_element_name_element_key["XRPV.A6L5.B2"]=   "   214.628 XRPV.A6L5.B2" ;
	map_element_name_element_key["XRPV.B6L5.B2"]=   "   220.000 XRPV.B6L5.B2" ;

	string element_key = map_element_name_element_key[magnet_name] ;

	string distribution_name = "" ;
	string type_name = "" ;
	
	if(type == STRENGTH)
	{ 
		distribution_name = magnet_name + "_k_distribution" ;
		type_name = "strength" ;
	}
	else if(type == ROTATION)
	{
		distribution_name = magnet_name + "_rot_distribution" ;	
		type_name = "rotation" ;		
	}
	else if(type == BEAM_MOMENTUM)
	{
		distribution_name = magnet_name + "_deltadeltap_distribution" ;	
		type_name = "beam_mometnum" ;		
	}
	else
	{
		cout << "[TProject::Strength_analysis_per_magnet] Error : Wrong type !" << endl ;
		exit(1) ;
	}
	
	TH1D *distribution = new TH1D(distribution_name.c_str() ,distribution_name.c_str(), 100, -4, 4);
	
	for(int i = 1 ; i < numberofchildren ; ++i)
	{	
		stringstream ss ; 
		ss << i ; 
		
			string beam_string = "" ;
			if(beam == 1) beam_string = "Beam_1" ;
			else if(beam == 2) beam_string = "Beam_2" ;
			else if(beam == 4) beam_string = "Beam_4" ;		
			
		if(((TBeam *)cd(("Matching/Job"+ ss.str() +"/Missile/myLHC/" + beam_string + "/").c_str(),0))->GetMatchingStatus() == TBeam::CONVERGED)		
		{
			
			if(type == STRENGTH)
			{
				double normalized_strength ;

				if(before_or_after_matching == after_matching)
				{
					normalized_strength = ratio_to_percent*((TMagnet *)(cd("Matching/Job"+ ss.str()+"/Missile/myLHC/" + beam_string + "/" + element_key, 0)))->getTotalStrength() ;
				}
				else if(before_or_after_matching == before_matching)			
				{
					normalized_strength = ratio_to_percent*((TMagnet *)(cd("Matching/Job"+ ss.str()+"/Missile/myLHC/" + beam_string + "/" + element_key, 0)))->GetDeltaStrength() ;

				}
				else
				{
					cout << "[TProject::Strength_analysis_per_magnet] : Unknown option !" ;
					exit(1) ;
				}

    				distribution->Fill(normalized_strength) ;
			}
			else if(type == ROTATION)
			{
				double rotation ;

				if(before_or_after_matching == after_matching)
				{
	    				rotation = ((TMagnet *)(cd("Matching/Job"+ ss.str()+"/Missile/myLHC/" + beam_string + "/" + element_key, 0)))->getTotalRotation() * rad_to_mrad ;
				}
				else if(before_or_after_matching == before_matching)			
				{
	    				rotation = ((TMagnet *)(cd("Matching/Job"+ ss.str()+"/Missile/myLHC/" + beam_string + "/" + element_key, 0)))->GetDeltaRotation() * rad_to_mrad ;
				}
				else
				{
					cout << "[TProject::Strength_analysis_per_magnet] : Unknown option !" ;
					exit(1) ;
				}

   				distribution->Fill(rotation) ;
			}
			else if(type == BEAM_MOMENTUM)
			{
				double momentum ;

				if(before_or_after_matching == after_matching)
				{
	    				momentum = ((TBeam *)(cd("Matching/Job"+ ss.str()+"/Missile/myLHC/" + beam_string, 0)))->getTotalBeamMomentum() * ratio_to_percent ;
				}
				else if(before_or_after_matching == before_matching)			
				{
	    				momentum = ((TBeam *)(cd("Matching/Job"+ ss.str()+"/Missile/myLHC/" + beam_string, 0)))->getDeltaDeltap() * ratio_to_percent ;
				}
				else
				{
					cout << "[TProject::Strength_analysis_per_magnet] : Unknown option !" ;
					exit(1) ;
				}

				cout << i << "   " << momentum << endl ;
   				distribution->Fill(momentum) ;
			}
			else
			{
				cout << "[TProject::Strength_analysis_per_magnet] Error : Wrong type !" << endl ;
				exit(1) ;
			}
		}
	}

	TCanvas *c2 = new TCanvas ;

	if(type == STRENGTH)
	{ 
		distribution->SetTitle((magnet_name + " strength distribution " + titletext + " matching").c_str()) ;
		distribution->GetXaxis()->SetTitle(("#deltak_{" + magnet_name + "}/k_{" + magnet_name + ",nominal}[%]").c_str()) ;
		distribution->GetYaxis()->SetTitle("Entries") ;
		distribution->Draw() ;
		
		distribution->SaveAs((ProjectParameters->getProjectDirectory() + "/Results/Magnet_strengths/" + subdirectory + magnet_name + "_k_" + titletext + "_matching.root").c_str()) ;
	}
	else if(type == ROTATION)
	{
		distribution->SetTitle((magnet_name + " rotation distribution " + titletext + " matching").c_str()) ;
		distribution->GetXaxis()->SetTitle(("#delta#phi_{" + magnet_name + "}[mrad]").c_str()) ;
		distribution->GetYaxis()->SetTitle("Entries") ;
		distribution->Draw() ;
		
		distribution->SaveAs((ProjectParameters->getProjectDirectory() + "/Results/Magnet_rotations/" + subdirectory + magnet_name + "_rot_" + titletext + "_matching.root").c_str()) ;
	}
	else if(type == BEAM_MOMENTUM)
	{
		distribution->SetTitle((magnet_name + " beam momentum distribution " + titletext + " matching").c_str()) ;
		distribution->GetXaxis()->SetTitle(("#deltap_{" + magnet_name + "}/p_{" + magnet_name + "}[%]").c_str()) ;
		distribution->GetYaxis()->SetTitle("Entries") ;
		distribution->Draw() ;
		
		distribution->SaveAs((ProjectParameters->getProjectDirectory() + "/Results/Beam_momentum/" + subdirectory + magnet_name + "_deltadeltap_" + titletext + "_matching.root").c_str()) ;
	}
	
	
	delete distribution ;
	
	return 1 ;
}

int TProject::Analysis(string analysis_name)
{

	gErrorIgnoreLevel = kWarning;

        // Optical functions before matching -----------------------------------------------------------------------------

	Analysis_per_optical_function(print_out_Ly_near, before_matching) ;
	Analysis_per_optical_function(print_out_Ly_far, before_matching) ;
	Analysis_per_optical_function(print_out_Lx_near, before_matching) ;
	Analysis_per_optical_function(print_out_Lx_far, before_matching) ;

        Analysis_per_optical_function(print_out_nu_y_near, before_matching) ;
        Analysis_per_optical_function(print_out_nu_y_far, before_matching) ;
        Analysis_per_optical_function(print_out_nu_x_near, before_matching) ;
        Analysis_per_optical_function(print_out_nu_x_far, before_matching) ;
	
        // Optical functions after matching -----------------------------------------------------------------------------

	Analysis_per_optical_function(print_out_Ly_near, after_matching) ;
	Analysis_per_optical_function(print_out_Ly_far, after_matching) ;
	Analysis_per_optical_function(print_out_Lx_near, after_matching) ;
	Analysis_per_optical_function(print_out_Lx_far, after_matching) ;
	
        Analysis_per_optical_function(print_out_nu_y_near, after_matching) ;
        Analysis_per_optical_function(print_out_nu_y_far, after_matching) ;
        Analysis_per_optical_function(print_out_nu_x_near, after_matching) ;
        Analysis_per_optical_function(print_out_nu_x_far, after_matching) ;
	
        // Magnet strengths after matching -----------------------------------------------------------------------------

	Strength_analysis_per_magnet("MQXA.1R5",1,STRENGTH, after_matching) ;
	Strength_analysis_per_magnet("MQXB.A2R5",1,STRENGTH, after_matching) ;
	Strength_analysis_per_magnet("MQXB.B2R5",1,STRENGTH, after_matching) ;		
	Strength_analysis_per_magnet("MQXA.3R5",1,STRENGTH, after_matching) ;
	Strength_analysis_per_magnet("MQY.4R5.B1",1,STRENGTH, after_matching) ;	
	Strength_analysis_per_magnet("MQML.5R5.B1",1,STRENGTH, after_matching) ;	

	Strength_analysis_per_magnet("MQXA.1L5",4,STRENGTH, after_matching) ;
	Strength_analysis_per_magnet("MQXB.A2L5",4,STRENGTH, after_matching) ;
	Strength_analysis_per_magnet("MQXB.B2L5",4,STRENGTH, after_matching) ;		
	Strength_analysis_per_magnet("MQXA.3L5",4,STRENGTH, after_matching) ;
	Strength_analysis_per_magnet("MQY.4L5.B2",4,STRENGTH, after_matching) ;	
	Strength_analysis_per_magnet("MQML.5L5.B2",4,STRENGTH, after_matching) ;	
	
        // Magnet rotations after matching -----------------------------------------------------------------------------	

	Strength_analysis_per_magnet("MQXA.1R5",1,ROTATION, after_matching) ;
	Strength_analysis_per_magnet("MQXB.A2R5",1,ROTATION, after_matching) ;
	Strength_analysis_per_magnet("MQXB.B2R5",1,ROTATION, after_matching) ;		
	Strength_analysis_per_magnet("MQXA.3R5",1,ROTATION, after_matching) ;
	Strength_analysis_per_magnet("MQY.4R5.B1",1,ROTATION, after_matching) ;	
	Strength_analysis_per_magnet("MQML.5R5.B1",1,ROTATION, after_matching) ;	

	Strength_analysis_per_magnet("MQXA.1L5",4,ROTATION, after_matching) ;
	Strength_analysis_per_magnet("MQXB.A2L5",4,ROTATION, after_matching) ;
	Strength_analysis_per_magnet("MQXB.B2L5",4,ROTATION, after_matching) ;		
	Strength_analysis_per_magnet("MQXA.3L5",4,ROTATION, after_matching) ;
	Strength_analysis_per_magnet("MQY.4L5.B2",4,ROTATION, after_matching) ;	
	Strength_analysis_per_magnet("MQML.5L5.B2",4,ROTATION, after_matching) ;	

        // Magnet strengths before matching -----------------------------------------------------------------------------

	Strength_analysis_per_magnet("MQXA.1R5",1,STRENGTH, before_matching) ;
	Strength_analysis_per_magnet("MQXB.A2R5",1,STRENGTH, before_matching) ;
	Strength_analysis_per_magnet("MQXB.B2R5",1,STRENGTH, before_matching) ;		
	Strength_analysis_per_magnet("MQXA.3R5",1,STRENGTH, before_matching) ;
	Strength_analysis_per_magnet("MQY.4R5.B1",1,STRENGTH, before_matching) ;	
	Strength_analysis_per_magnet("MQML.5R5.B1",1,STRENGTH, before_matching) ;	

	Strength_analysis_per_magnet("MQXA.1L5",4,STRENGTH, before_matching) ;
	Strength_analysis_per_magnet("MQXB.A2L5",4,STRENGTH, before_matching) ;
	Strength_analysis_per_magnet("MQXB.B2L5",4,STRENGTH, before_matching) ;		
	Strength_analysis_per_magnet("MQXA.3L5",4,STRENGTH, before_matching) ;
	Strength_analysis_per_magnet("MQY.4L5.B2",4,STRENGTH, before_matching) ;	
	Strength_analysis_per_magnet("MQML.5L5.B2",4,STRENGTH, before_matching) ;	
	
        // Magnet rotations before matching -----------------------------------------------------------------------------		

	Strength_analysis_per_magnet("MQXA.1R5",1,ROTATION, before_matching) ;
	Strength_analysis_per_magnet("MQXB.A2R5",1,ROTATION, before_matching) ;
	Strength_analysis_per_magnet("MQXB.B2R5",1,ROTATION, before_matching) ;		
	Strength_analysis_per_magnet("MQXA.3R5",1,ROTATION, before_matching) ;
	Strength_analysis_per_magnet("MQY.4R5.B1",1,ROTATION, before_matching) ;	
	Strength_analysis_per_magnet("MQML.5R5.B1",1,ROTATION, before_matching) ;	

	Strength_analysis_per_magnet("MQXA.1L5",4,ROTATION, before_matching) ;
	Strength_analysis_per_magnet("MQXB.A2L5",4,ROTATION, before_matching) ;
	Strength_analysis_per_magnet("MQXB.B2L5",4,ROTATION, before_matching) ;		
	Strength_analysis_per_magnet("MQXA.3L5",4,ROTATION, before_matching) ;
	Strength_analysis_per_magnet("MQY.4L5.B2",4,ROTATION, before_matching) ;	
	Strength_analysis_per_magnet("MQML.5L5.B2",4,ROTATION, before_matching) ;
	
        // Beam momentum before matching -----------------------------------------------------------------------------			
	
	cout << "Before" << endl ;
	Strength_analysis_per_magnet("Beam 1", 1, BEAM_MOMENTUM, before_matching) ;
	
	Strength_analysis_per_magnet("Beam 4", 4, BEAM_MOMENTUM, before_matching) ;
	
        // Beam momentum after matching -----------------------------------------------------------------------------				
	
	cout << "After" << endl ;	
	Strength_analysis_per_magnet("Beam 1", 1, BEAM_MOMENTUM, after_matching) ;
	
	Strength_analysis_per_magnet("Beam 4", 4, BEAM_MOMENTUM, after_matching) ;	
	
        return 0 ;
}

int TProject::Run()
{
	string command = ProjectParameters->getCommand() ;

	if(command.compare("") == 0)
	{
		cout << "There is no command in the project file !" << endl ;
		exit(NO_COMMAND) ;
	}
	else if(command.compare("Execute") == 0) 
	{
		Execute("") ;
	}
	else if(command.compare("LoadTFS_SaveROOT") == 0)
	{
	        LoadTFSResults("") ;
	        Save(NULL) ;
	}
	else if(command.compare("loadfromroot") == 0)
	{
	        LoadFromROOT("",NULL) ;
	}
        else if(command.compare("SaveAs") == 0)
	{
	        LoadFromROOT("",NULL) ;
		ProjectParameters->setROOTfilename(ProjectParameters->getFileNameForSaveAs()) ;
		Save(NULL) ;		
	}
        else if(command.compare("Analysis") == 0)
	{
	        LoadFromROOT("",NULL) ;
		Analysis(ProjectParameters->getAnalysisName()) ;
	}

	return 0 ;
}

#endif
