#ifndef TMADX_H
#define TMADX_H

#include <string>
#include <iostream>
#include <algorithm>


#include <include/TLHC.h>
#include <include/TOpticalFunctions.h>
#include <stdlib.h>

class TMADX : public TOpticsObject
{
	private:
		TProjectParameters *ProjectParameters ;
		TMADX *target ; // a "Missile" type MADX objects have to know their target 
		bool perturbed, matched ;
		int Job_number ;

	public:
		static int number_of_calls ;

		TMADX(string, TProjectParameters *ProjectParameters, int, bool, bool, bool, TMADX *) ;
		int Save(TDirectory *, TFile *, OpticsArgumentType *) ;
		int Print(int) ;
		int Execute(string) ;

		int RetrieveAllOpticalFunctionName(map <string, double> &);
		int writeBatchFileToSubmit(string) ;
		int writeMatchFile(string);
		int writeMatchingLines(ofstream &) ;
		int writePerturbations(ofstream &,int) ;
		int writeConstraints(ofstream &) ;
		int readTFSPerBeam(ofstream &, int,bool=false) ;
		int readTarget(ofstream &) ;
		int readOpticalFunctions(ofstream &) ;
		int writeDefinitions(ofstream &) ; 
		int writeMinuitCode(ofstream &) ;
		int writeFCN(ofstream &) ;
		int writeMain(ofstream &) ;
		int writeRandomGenerator(ofstream &) ;
		int obtain_and_read_TFS_files_BeforeMatching(ofstream &, int) ;
		void write_linear_least_seuqare_fit(ofstream &) ;

}; 

int TMADX::number_of_calls = 0 ;

TMADX::TMADX(string aname, TProjectParameters *aProjectParameters, int aJob_number, bool aperturbed, bool amatched, bool postponed, TMADX *atarget = NULL) : TOpticsObject(aname)
{
	perturbed = aperturbed ;
	matched = amatched ;
	target = atarget ;
	Job_number = aJob_number ; 

	ProjectParameters = aProjectParameters ;

	TLHC *new_LHC = new TLHC("myLHC", ProjectParameters, Job_number, perturbed, matched, postponed) ;
	insert(new_LHC) ;
	
	SetupOptics() ;
}

int TMADX::Print(int depth)
{
	string prefix=Prefix(depth) ;

        cout << endl << prefix << "TMADX Print ----------" << endl << endl ;
        cout << prefix << "Name: " << this->GetName() << endl ;

	TOpticsObject::Print(depth) ;

	return 0 ;
}

int TMADX::Save(TDirectory *dir, TFile *rootfile, OpticsArgumentType *OpticsArgument)
{
	TOpticsObject::Save(dir,rootfile,OpticsArgument) ;

	return 0 ;
}

int TMADX::Execute(string dir)
{
	TOpticsObject::Execute(dir) ;

	if(matched) 
	{
		writeBatchFileToSubmit(dir) ;
		writeMatchFile(dir) ;
	}

	return 0 ;
}

int TMADX::writeMatchFile(string dir)
{
        ofstream matchfile ;
        matchfile.open((dir + "/" +this->GetName()  + "/match.C").c_str()) ;

        writeDefinitions(matchfile) ;
	writeFCN(matchfile) ;
	writeMain(matchfile) ;

        matchfile.close() ;
	return 0 ; 
} 

int TMADX::writeMain(ofstream &matchfile)
{

	const int beam_1 = 1 ;
	const int beam_4 = 4 ;

        matchfile << "" << endl ;
        matchfile << "void match(char *argument)" << endl ;
        matchfile << "{" << endl ;

        matchfile << "    working_directory = string(argument) ;" << endl ;
        matchfile <<  endl ;
	
	if(ProjectParameters->getParameterValue("reconstruction") == 1.0)
	{
		matchfile << "    // Open root files with pp scattering data" << endl ;
		
		if(ProjectParameters->getProjectType() == Matching_1N)
		{
			stringstream jobnumber_str ;
			jobnumber_str << Job_number ;
		
			matchfile << "    distributions_45b_56t = new TFile(\"" + ProjectParameters->getSettingValue("pp_scattering_data_directory") + "Project/Matching/Job" + jobnumber_str.str() + "/distributions_45b_56t.root\",\"READ\") ;" << endl ;
			matchfile << "    distributions_45t_56b = new TFile(\"" + ProjectParameters->getSettingValue("pp_scattering_data_directory") + "Project/Matching/Job" + jobnumber_str.str() + "/distributions_45t_56b.root\",\"READ\") ;" << endl ;
		}
		else if(ProjectParameters->getProjectType() == Matching_N1)
		{
			matchfile << "    distributions_45b_56t = new TFile(\"" + ProjectParameters->getSettingValue("pp_scattering_data_directory") + "distributions_45b_56t.root\",\"READ\") ;" << endl ;
			matchfile << "    distributions_45t_56b = new TFile(\"" + ProjectParameters->getSettingValue("pp_scattering_data_directory") + "distributions_45t_56b.root\",\"READ\") ;" << endl ;
		}
		
        	matchfile <<  endl ;

        	matchfile << "    x_R_F_vs_y_R_F_45b_56t = (TGraph *)distributions_45b_56t->Get(\"optics/matching input, full/g_x_R_F_vs_y_R_F\") ;" << endl ;
        	matchfile << "    x_R_N_vs_y_R_N_45b_56t = (TGraph *)distributions_45b_56t->Get(\"optics/matching input, full/g_x_R_N_vs_y_R_N\") ;" << endl  ;      
        	matchfile << "    x_R_F_vs_y_R_F_45t_56b = (TGraph *)distributions_45t_56b->Get(\"optics/matching input, full/g_x_R_F_vs_y_R_F\") ;" << endl  ;
        	matchfile << "    x_R_N_vs_y_R_N_45t_56b = (TGraph *)distributions_45t_56b->Get(\"optics/matching input, full/g_x_R_N_vs_y_R_N\") ;" << endl  ;

        	matchfile << "    x_L_F_vs_y_L_F_45b_56t = (TGraph *)distributions_45b_56t->Get(\"optics/matching input, full/g_x_L_F_vs_y_L_F\") ;" << endl  ;
        	matchfile << "    x_L_N_vs_y_L_N_45b_56t = (TGraph *)distributions_45b_56t->Get(\"optics/matching input, full/g_x_L_N_vs_y_L_N\") ;" << endl  ;      
        	matchfile << "    x_L_F_vs_y_L_F_45t_56b = (TGraph *)distributions_45t_56b->Get(\"optics/matching input, full/g_x_L_F_vs_y_L_F\") ;" << endl  ;
        	matchfile << "    x_L_N_vs_y_L_N_45t_56b = (TGraph *)distributions_45t_56b->Get(\"optics/matching input, full/g_x_L_N_vs_y_L_N\") ;" << endl  ;
        	matchfile <<  endl ;
	}	

        matchfile << "    // Nominal values for normalization" << endl ;
        matchfile << "" << endl ;

	double nominalstrength ;

        readTarget(matchfile) ;
	
	obtain_and_read_TFS_files_BeforeMatching(matchfile,beam_1) ;
	obtain_and_read_TFS_files_BeforeMatching(matchfile,beam_4) ;	
	
        writeRandomGenerator(matchfile) ;
        writeMinuitCode(matchfile) ;

	return 0 ;
}

int TMADX::writeFCN(ofstream &matchfile)
{

	const int beam_1 = 1 ;
	const int beam_4 = 4 ;

        matchfile << "" << endl ;
        matchfile << "void fcn(Int_t &npar, double *gin, double &f, double *MinuitParameter, int iflag)" << endl ;
        matchfile << "{" << endl ;
        // matchfile << "exit(1) ; " << endl ; // Remove this line later

        writePerturbations(matchfile, beam_1) ;
        writePerturbations(matchfile, beam_4) ;

        readOpticalFunctions(matchfile) ;
        writeConstraints(matchfile) ;


        matchfile <<  endl ;
        matchfile <<  endl ;
        matchfile << "    cout << \"CHI2: \" << chi2 << endl ;" << endl ;
        matchfile <<  endl ;
        matchfile << "    f=chi2  ;" << endl ;
        matchfile << "}" << endl ;
        matchfile <<  endl ;

	return 0 ;
}

int TMADX::RetrieveAllOpticalFunctionName(map <string, double> &AllOpticalFunction)
{
	((TLHC *)Children["myLHC"])->RetrieveAllOpticalFunctionName(AllOpticalFunction) ;
	return 0 ;
}

int TMADX::writeDefinitions(ofstream &matchfile)
{
        matchfile << "string working_directory ;" << endl ;
        matchfile << "int MADX_status = 0 ;" << endl ;	
        matchfile << "" << endl ;
	
	if(ProjectParameters->getParameterValue("reconstruction") == 1.0)
	{
		matchfile << "TFile *distributions_45b_56t ;" << endl ;
		matchfile << "TFile *distributions_45t_56b ;" << endl ;
		matchfile << endl ;	
		
		matchfile << "TGraph *x_R_F_vs_y_R_F_45b_56t ;" << endl ;
		matchfile << "TGraph *x_R_N_vs_y_R_N_45b_56t ;" << endl ;
		matchfile << "TGraph *x_R_F_vs_y_R_F_45t_56b ;" << endl ;
		matchfile << "TGraph *x_R_N_vs_y_R_N_45t_56b ;" << endl ;

		matchfile << "TGraph *x_L_F_vs_y_L_F_45b_56t ;" << endl ;
		matchfile << "TGraph *x_L_N_vs_y_L_N_45b_56t ;" << endl ;
		matchfile << "TGraph *x_L_F_vs_y_L_F_45t_56b ;" << endl ;
		matchfile << "TGraph *x_L_N_vs_y_L_N_45t_56b ;" << endl ;
		matchfile << endl ;			
	}

	double nominalstrength, perturbation_factor ;

	nominalstrength = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()["Anominal"]->getChildren()["MAD_X"]->getChildren()["myLHC"]->getChildren()["Beam_1"]->getChildren()["    29.335 MQXA.1R5"]))->GetStrength()) ;
        matchfile << "double    Nominal_MQXA_1R5 = " << nominalstrength << " ;" << endl ;
	nominalstrength = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()["Anominal"]->getChildren()["MAD_X"]->getChildren()["myLHC"]->getChildren()["Beam_1"]->getChildren()["    37.550 MQXB.A2R5"]))->GetStrength()) ;
        matchfile << "double    Nominal_MQXB_A2R5 = " << nominalstrength << " ;" << endl ;
	nominalstrength = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()["Anominal"]->getChildren()["MAD_X"]->getChildren()["myLHC"]->getChildren()["Beam_1"]->getChildren()["    44.050 MQXB.B2R5"]))->GetStrength()) ;
        matchfile << "double    Nominal_MQXB_B2R5 = "  << nominalstrength << " ;" << endl ;
	nominalstrength = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()["Anominal"]->getChildren()["MAD_X"]->getChildren()["myLHC"]->getChildren()["Beam_1"]->getChildren()["    53.335 MQXA.3R5"]))->GetStrength()) ;
        matchfile << "double    Nominal_MQXA_3R5 = "  << nominalstrength << " ;" << endl ;
	nominalstrength = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()["Anominal"]->getChildren()["MAD_X"]->getChildren()["myLHC"]->getChildren()["Beam_1"]->getChildren()["   171.253 MQY.4R5.B1"]))->GetStrength()) ;
	matchfile << "double    Nominal_MQY_4R5_B1 = "  << nominalstrength << " ;" << endl ;
	nominalstrength = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()["Anominal"]->getChildren()["MAD_X"]->getChildren()["myLHC"]->getChildren()["Beam_1"]->getChildren()["   198.890 MQML.5R5.B1"]))->GetStrength()) ;
        matchfile << "double    Nominal_MQML_5R5_B1 = "  << nominalstrength << " ;" << endl ;
	nominalstrength = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()["Anominal"]->getChildren()["MAD_X"]->getChildren()["myLHC"]->getChildren()["Beam_4"]->getChildren()["    29.335 MQXA.1L5"]))->GetStrength()) ;
        matchfile << "double    Nominal_MQXA_1L5 = "  << nominalstrength << " ;" << endl ;
	nominalstrength = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()["Anominal"]->getChildren()["MAD_X"]->getChildren()["myLHC"]->getChildren()["Beam_4"]->getChildren()["    37.550 MQXB.A2L5"]))->GetStrength()) ;
        matchfile << "double    Nominal_MQXB_A2L5 = "  << nominalstrength << " ;" << endl ;
	nominalstrength = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()["Anominal"]->getChildren()["MAD_X"]->getChildren()["myLHC"]->getChildren()["Beam_4"]->getChildren()["    44.050 MQXB.B2L5"]))->GetStrength()) ;
        matchfile << "double    Nominal_MQXB_B2L5 = "  << nominalstrength << " ;" << endl ;
	nominalstrength = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()["Anominal"]->getChildren()["MAD_X"]->getChildren()["myLHC"]->getChildren()["Beam_4"]->getChildren()["    53.335 MQXA.3L5"]))->GetStrength()) ;
        matchfile << "double   Nominal_MQXA_3L5 = "  << nominalstrength << " ;" << endl ;
	nominalstrength = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()["Anominal"]->getChildren()["MAD_X"]->getChildren()["myLHC"]->getChildren()["Beam_4"]->getChildren()["   171.253 MQY.4L5.B2"]))->GetStrength()) ;
        matchfile << "double    Nominal_MQY_4L5_B2 = "  << nominalstrength << " ;" << endl ;
	nominalstrength = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()["Anominal"]->getChildren()["MAD_X"]->getChildren()["myLHC"]->getChildren()["Beam_4"]->getChildren()["   198.890 MQML.5L5.B2"]))->GetStrength()) ;
        matchfile << "double    Nominal_MQML_5L5_B2 = "  << nominalstrength << " ;" << endl ;
        matchfile << "" << endl ;


	stringstream ss1 ;
	ss1 << Job_number ;
	string jobkey = "Job" + ss1.str() ;

        perturbation_factor = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_1"]->getChildren()["    29.335 MQXA.1R5"]))->GetDeltaStrength()) ;
        matchfile << "double    DeltaS_MQXA_1R5 = " << perturbation_factor << " ;" << endl ;
        perturbation_factor = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_1"]->getChildren()["    37.550 MQXB.A2R5"]))->GetDeltaStrength()) ;
        matchfile << "double    DeltaS_MQXB_A2R5 = " << perturbation_factor << " ;" << endl ;
        perturbation_factor = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_1"]->getChildren()["    44.050 MQXB.B2R5"]))->GetDeltaStrength()) ;
        matchfile << "double    DeltaS_MQXB_B2R5 = "  << perturbation_factor << " ;" << endl ;
        perturbation_factor = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_1"]->getChildren()["    53.335 MQXA.3R5"]))->GetDeltaStrength()) ;
        matchfile << "double    DeltaS_MQXA_3R5 = "  << perturbation_factor << " ;" << endl ;
        perturbation_factor = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_1"]->getChildren()["   171.253 MQY.4R5.B1"]))->GetDeltaStrength()) ;
        matchfile << "double    DeltaS_MQY_4R5_B1 = "  << perturbation_factor << " ;" << endl ;
        perturbation_factor = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_1"]->getChildren()["   198.890 MQML.5R5.B1"]))->GetDeltaStrength()) ;
        matchfile << "double    DeltaS_MQML_5R5_B1 = "  << perturbation_factor << " ;" << endl ;
        perturbation_factor = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_4"]->getChildren()["    29.335 MQXA.1L5"]))->GetDeltaStrength()) ;
        matchfile << "double    DeltaS_MQXA_1L5 = "  << perturbation_factor << " ;" << endl ;
        perturbation_factor = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_4"]->getChildren()["    37.550 MQXB.A2L5"]))->GetDeltaStrength()) ;
        matchfile << "double    DeltaS_MQXB_A2L5 = "  << perturbation_factor << " ;" << endl ;
        perturbation_factor = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_4"]->getChildren()["    44.050 MQXB.B2L5"]))->GetDeltaStrength()) ;
        matchfile << "double    DeltaS_MQXB_B2L5 = "  << perturbation_factor << " ;" << endl ;
        perturbation_factor = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_4"]->getChildren()["    53.335 MQXA.3L5"]))->GetDeltaStrength()) ;
        matchfile << "double    DeltaS_MQXA_3L5 = "  << perturbation_factor << " ;" << endl ;
        perturbation_factor = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_4"]->getChildren()["   171.253 MQY.4L5.B2"]))->GetDeltaStrength()) ;
        matchfile << "double    DeltaS_MQY_4L5_B2 = "  << perturbation_factor << " ;" << endl ;
        perturbation_factor = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_4"]->getChildren()["   198.890 MQML.5L5.B2"]))->GetDeltaStrength()) ;
        matchfile << "double    DeltaS_MQML_5L5_B2 = "  << perturbation_factor << " ;" << endl ;
	
        matchfile << "" << endl ;



        nominalstrength = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_1"]->getChildren()["    29.335 MQXA.1R5"]))->GetDeltaRotation()) ;
        matchfile << "double    DeltaR_MQXA_1R5 = " << nominalstrength << " ;" << endl ;
        nominalstrength = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_1"]->getChildren()["    37.550 MQXB.A2R5"]))->GetDeltaRotation()) ;
        matchfile << "double    DeltaR_MQXB_A2R5 = " << nominalstrength << " ;" << endl ;
        nominalstrength = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_1"]->getChildren()["    44.050 MQXB.B2R5"]))->GetDeltaRotation()) ;
        matchfile << "double    DeltaR_MQXB_B2R5 = "  << nominalstrength << " ;" << endl ;
        nominalstrength = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_1"]->getChildren()["    53.335 MQXA.3R5"]))->GetDeltaRotation()) ;
        matchfile << "double    DeltaR_MQXA_3R5 = "  << nominalstrength << " ;" << endl ;
        nominalstrength = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_1"]->getChildren()["   171.253 MQY.4R5.B1"]))->GetDeltaRotation()) ;
        matchfile << "double    DeltaR_MQY_4R5_B1 = "  << nominalstrength << " ;" << endl ;
        nominalstrength = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_1"]->getChildren()["   198.890 MQML.5R5.B1"]))->GetDeltaRotation()) ;
        matchfile << "double    DeltaR_MQML_5R5_B1 = "  << nominalstrength << " ;" << endl ;
        nominalstrength = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_4"]->getChildren()["    29.335 MQXA.1L5"]))->GetDeltaRotation()) ;
        matchfile << "double    DeltaR_MQXA_1L5 = "  << nominalstrength << " ;" << endl ;
        nominalstrength = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_4"]->getChildren()["    37.550 MQXB.A2L5"]))->GetDeltaRotation()) ;
        matchfile << "double    DeltaR_MQXB_A2L5 = "  << nominalstrength << " ;" << endl ;
        nominalstrength = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_4"]->getChildren()["    44.050 MQXB.B2L5"]))->GetDeltaRotation()) ;
        matchfile << "double    DeltaR_MQXB_B2L5 = "  << nominalstrength << " ;" << endl ;
        nominalstrength = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_4"]->getChildren()["    53.335 MQXA.3L5"]))->GetDeltaRotation()) ;
        matchfile << "double    DeltaR_MQXA_3L5 = "  << nominalstrength << " ;" << endl ;
        nominalstrength = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_4"]->getChildren()["   171.253 MQY.4L5.B2"]))->GetDeltaRotation()) ;
        matchfile << "double    DeltaR_MQY_4L5_B2 = "  << nominalstrength << " ;" << endl ;
        nominalstrength = (((TMagnet *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_4"]->getChildren()["   198.890 MQML.5L5.B2"]))->GetDeltaRotation()) ;
        matchfile << "double    DeltaR_MQML_5L5_B2 = "  << nominalstrength << " ;" << endl ;
        matchfile << "" << endl ;


        nominalstrength = (((TBeam *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_1"]))->getPerturbationDeltaDeltap()) ;
        matchfile << "double    DeltaP_beam1 = "  << nominalstrength << " ;" << endl ;
        nominalstrength = (((TBeam *)(ProjectParameters->getRootObject()->getChildren()["Matching"]->getChildren()[jobkey]->getChildren()["Missile"]->getChildren()["myLHC"]->getChildren()["Beam_4"]))->getPerturbationDeltaDeltap()) ;
        matchfile << "double    DeltaP_beam4 = "  << nominalstrength << " ;" << endl ;

        vector<string>::iterator strit ;

	matchfile << "int number_of_optical_functions = " << TOpticalFunctions::optical_function_names.size() << " ;" << endl ;
	matchfile << "const string optical_function_names[] = {" ;
        unsigned int i = 0 ;
        for(strit = TOpticalFunctions::optical_function_names.begin() ; strit != TOpticalFunctions::optical_function_names.end()  ; ++strit)
        {
                matchfile << "\"" << (*strit) << "\"" ;
		if(i < TOpticalFunctions::optical_function_names.size()-1) matchfile << ", " ;
                ++i ;
        }
	matchfile << "} ;" << endl << endl ;

	set <string> twiss_optical_function_variables = ProjectParameters->Constraints.getVariables() ;

	map <string, double> AllOpticalFunction ;
	target->RetrieveAllOpticalFunctionName(AllOpticalFunction) ;

//	map <string, double>::iterator it2 ;
//	for(it2 = AllOpticalFunction.begin() ; it2 != AllOpticalFunction.end() ; it2++)
//			cout << it2->first ;

	set <string>::iterator it ;
	for(it = twiss_optical_function_variables.begin() ; it != twiss_optical_function_variables.end() ; ++it) 
	{
                if(ProjectParameters->getVerbosity() == VERBOSITY_ALL) cout << "[TMADX::writeDefinitions] Looking for this optical function:" << *it << endl;

		using std::replace;
		string aux = *it ;
		replace(aux.begin(), aux.end(),'_','.' ) ;  

		matchfile << "double " << *it << " ; " << endl ; 

/*		map <string, double>::iterator it_search = AllOpticalFunction.find(aux) ;
		if(it_search != AllOpticalFunction.end())
		{
		}
		else
		{
			cout << *it << " appear in constrain but not an existing optical variable name !" << endl ;
			exit(1) ;
		}*/

	}


	int number_of_constraints = (int) ProjectParameters->Constraints.getNumberOfConstraints() ;
	stringstream ss ; ss << number_of_constraints ;
	
	matchfile << endl ;

	matchfile << "double condition_variable_target[" << ss.str() << "] ;"  << endl ; 

	matchfile << "double condition_variable_to_match[" << ss.str() << "] ;"  << endl << endl ; 
	matchfile << "double condition_variable_error[" << ss.str() << "] ;"  << endl << endl ;
	matchfile << "double condition_variable_smearing[" << ss.str() << "] ;"  << endl << endl ;	

	return 0 ;
} 

int TMADX::writeBatchFileToSubmit(string dir) 
{

	if(matched) // for a nominal job there is no submit script generated
	{
	        ofstream batchfiletosubmit ;
		string directoryname = dir + "/" +this->GetName() ;

	        batchfiletosubmit.open((directoryname  + "/submit.sh").c_str()) ;

	        batchfiletosubmit << "\#\!\/bin\/bash" << endl << endl ;
	        batchfiletosubmit << "cd " << directoryname << "\;" << endl ;
	        // batchfiletosubmit << "source /afs/cern.ch/user/f/fnemes/bin/ENV2.sh" << endl ;
	        batchfiletosubmit << "root -l -b  \"" << directoryname << "/match.C(\\\"" << directoryname << "\\\")\"" << endl ;

	        batchfiletosubmit.close() ;

	}

	return 0 ;
}

int TMADX::writeMatchingLines(ofstream &) 
{
	return 0 ;
}

int TMADX::readOpticalFunctions(ofstream &matchfile) 
{
	++number_of_calls ;
	
	stringstream str_number_of_calls ;
	str_number_of_calls << number_of_calls ;
	
	string auxstring_str  = ("auxstring_" + str_number_of_calls.str()) ;
	string auxstring_str2 = ("auxstring2_" + str_number_of_calls.str()) ;

	if( ProjectParameters->getVerbosity() == VERBOSITY_ALL) cout << "TMADX madxname: " << ProjectParameters->getMADXname() << endl ;

        matchfile << "    string " << auxstring_str << " =  working_directory + string(\"/myLHC/Beam_1/opticsproject.madx\")  ;" << endl ;
        matchfile << "    MADX_status = system((\"" + ProjectParameters->getMADXname() + " < \" + " << auxstring_str << " + \" > /dev/null\").c_str()) ;" << endl ;
        matchfile << "    if(MADX_status) { cout << \"MADX problem:\" << MADX_status << endl ; exit(1) ; }" << endl ;	
        matchfile << "    string " << auxstring_str2 << " =  working_directory + string(\"/myLHC/Beam_4/opticsproject.madx\")  ;" << endl ;
        matchfile << "    MADX_status = system((\"" + ProjectParameters->getMADXname() + " < \" + " << auxstring_str2 << " + \" > /dev/null\").c_str()) ;" << endl ;
        matchfile << "    if(MADX_status) { cout << \"MADX problem:\" << MADX_status << endl ; exit(1) ; }" << endl ;		

        matchfile << "" << endl ;

	return 0 ;
}

int TMADX::writePerturbations(ofstream &matchfile, int beam_number) 
{
	++number_of_calls ;
	
	stringstream str_number_of_calls ;
	str_number_of_calls << number_of_calls ;
	
	string filename_pert = ("perturbations_" + str_number_of_calls.str()) ;

	stringstream beam_string ;
	beam_string << beam_number ;

        matchfile << "" << endl ;
        matchfile << "    ofstream " << filename_pert << " ((working_directory + string(\"/myLHC/Beam_" + beam_string.str() + "/temporary_perturbation.madx\")).c_str()) ;" << endl ;
        matchfile << "" << endl ;

	int ParameterNumber = 0 ;	
        map <int, TStructForMatching>::iterator it ;
        map <int, TStructForMatching> *matched_params = ProjectParameters->GetMatchedParameters() ;
        for(it = matched_params->begin() ; it != matched_params->end(); it++)
        {
                stringstream ss ;
                ss << it->first ;

		if(it->second.type.compare("Strength") == 0)
	        	matchfile << "    " << filename_pert << " << \"D" + it->second.name + " := \" << MinuitParameter[" + ss.str() + "] << \" ; \"  << endl ;" << endl ;
		else if(it->second.type.compare("Rotation") == 0)
        		matchfile << "    " << filename_pert << " << \"DPSI" + it->second.name + " := \" << MinuitParameter[" + ss.str() + "] << \" ; \"  << endl ;" << endl ;
        }

	if(ProjectParameters->getMatchedBeamMomentum())
	{
		stringstream ss1 ;
		ss1 << matched_params->size() ;
	        matchfile << "    " << filename_pert << " << \"DDELTADELTAP_B1 := \" << MinuitParameter[" + ss1.str() + "] << \" ; \" << endl ;" << endl ;

		stringstream ss2 ;
		++ParameterNumber ;
		ss2 << matched_params->size()+1 ;
	        matchfile << "    " << filename_pert << " << \"DDELTADELTAP_B4 := \" << MinuitParameter[" + ss2.str() + "] << \" ; \" << endl ;" << endl ;
	}

	return 0 ;
}

int TMADX::readTFSPerBeam(ofstream &matchfile, int beam_number, bool target)
{

	++number_of_calls ;
	
	stringstream str_number_of_calls ;
	str_number_of_calls << number_of_calls ;

	stringstream ss ;
	ss << beam_number ;
	
	string tfsfile_name = ("tfsfile_" + str_number_of_calls.str()) ;
	string auxstring_str  = ("auxstring_" + str_number_of_calls.str()) ;
	string line_str  = ("line_" + str_number_of_calls.str()) ;
	string tfsname_str  = ("tfsname_" + str_number_of_calls.str()) ;
	string index_str  = ("index_" + str_number_of_calls.str()) ;


        matchfile << "    ifstream " << tfsfile_name << " ;" << endl ;


        matchfile <<  "    string " << auxstring_str << " =  working_directory + string(\"/myLHC/Beam_" + ss.str() + "/opticsproject.tfs\")  ;" << endl ;

	if(target)
	{
	        matchfile <<  "    int " << index_str << " = " << auxstring_str << ".find(\"Missile\") ;" << endl ;
        	matchfile <<  "    " << auxstring_str << ".replace(" << index_str << ",7,\"Target\",6) ;" << endl ;
	}

        matchfile << "    " << tfsfile_name << ".open(" << auxstring_str << ".c_str()) ;" << endl ;

        matchfile << "    string " << line_str << " ;" << endl ;
        matchfile << "    string " << tfsname_str << " ;"  << endl ;
        matchfile << "    if(" << tfsfile_name << ".is_open())"  << endl ;
        matchfile << "    {"  << endl ;
        matchfile << "        while ( " << tfsfile_name << ".good() )"  << endl ;
        matchfile << "        {"  << endl ;
        matchfile << "            getline (" << tfsfile_name << "," << line_str << ");"  << endl ;
        matchfile << "            if(" << line_str << ".substr(0,1).compare(\"$\") == 0) break ;"  << endl ;
        matchfile << "        }"  << endl ;
        matchfile << "        if(" << tfsfile_name << ".good())" << endl ;
        matchfile << "        {"  << endl ;
        matchfile << "            while(" << tfsfile_name << " >> " << tfsname_str << ")"  << endl ;
        matchfile << "            {"  << endl ;
        matchfile << "                double value ;" << endl ;
        matchfile <<  "                for(int j = 0 ; j < number_of_optical_functions ; ++j)" << endl ;
        matchfile << "                {" << endl ;
        matchfile << "                    " << tfsfile_name << " >> value ;" << endl ;
        matchfile << "                    string aux = " << tfsname_str << ".substr(1," << tfsname_str << ".size()-2) ;" << endl ;
        matchfile << "                    size_t pos = aux.find(\".\") ;" << endl ;
        matchfile << "                    while( pos != string::npos)" << endl ;
        matchfile << "                    {" << endl ;
        matchfile << "                        aux.replace(pos, 1, \"_\") ;" << endl ;
        matchfile << "                        pos = aux.find(\".\") ;" << endl ;
        matchfile << "                    }" << endl ;
        matchfile << "                    string variable_name = \"TwissParameter_\" + aux + \"_\" + optical_function_names[j] ;" << endl ;

        set <string> twiss_optical_function_variables = ProjectParameters->Constraints.getVariables() ;
        set <string>::iterator opit ;

        for(opit = twiss_optical_function_variables.begin() ; opit != twiss_optical_function_variables.end() ; ++opit)
        matchfile << "                   if(variable_name.compare(\"" + (*opit) + "\") == 0 ) " + (*opit) + " = value ; " << endl ;
        matchfile << "               }" << endl ;
        matchfile << "            }"  << endl ;
        matchfile << "        }"  << endl ;
        matchfile << "    }" << endl << endl ;

	return 0 ; 
}

void TMADX::write_linear_least_seuqare_fit(ofstream &matchfile)
{
	matchfile << "        double theta_x_star = rad_to_urad * (-(TwissParameter_XRPV_B6L5_B2_RE12*pow(TwissParameter_XRPV_B6R5_B1_RE11,2)*xFL) - TwissParameter_XRPV_B6L5_B2_RE11*TwissParameter_XRPV_B6R5_B1_RE11*TwissParameter_XRPV_B6R5_B1_RE12*xFL + " << endl ;
	matchfile << "		     TwissParameter_XRPV_B6L5_B2_RE11*TwissParameter_XRPV_B6L5_B2_RE12*TwissParameter_XRPV_B6R5_B1_RE11*xFR + pow(TwissParameter_XRPV_B6L5_B2_RE11,2)*TwissParameter_XRPV_B6R5_B1_RE12*xFR - " << endl ;
	matchfile << "		     TwissParameter_XRPV_A6L5_B2_RE12*pow(TwissParameter_XRPV_B6L5_B2_RE11,2)*xNL - TwissParameter_XRPV_A6L5_B2_RE12*pow(TwissParameter_XRPV_B6R5_B1_RE11,2)*xNL - " << endl ;
	matchfile << "		     pow(TwissParameter_XRPV_A6R5_B1_RE11,2)*(TwissParameter_XRPV_B6L5_B2_RE12*xFL - TwissParameter_XRPV_B6R5_B1_RE12*xFR + TwissParameter_XRPV_A6L5_B2_RE12*xNL) + " << endl ;
	matchfile << "		     TwissParameter_XRPV_A6R5_B1_RE12*(pow(TwissParameter_XRPV_B6L5_B2_RE11,2) + pow(TwissParameter_XRPV_B6R5_B1_RE11,2))*xNR + " << endl ;
	matchfile << "		     pow(TwissParameter_XRPV_A6L5_B2_RE11,2)*(-(TwissParameter_XRPV_B6L5_B2_RE12*xFL) + TwissParameter_XRPV_B6R5_B1_RE12*xFR + TwissParameter_XRPV_A6R5_B1_RE12*xNR) - " << endl ;
	matchfile << "		     TwissParameter_XRPV_A6R5_B1_RE11*(TwissParameter_XRPV_A6R5_B1_RE12*TwissParameter_XRPV_B6L5_B2_RE11*xFL + TwissParameter_XRPV_A6R5_B1_RE12*TwissParameter_XRPV_B6R5_B1_RE11*xFR - " << endl ;
	matchfile << "			TwissParameter_XRPV_B6L5_B2_RE11*TwissParameter_XRPV_B6L5_B2_RE12*xNR + TwissParameter_XRPV_B6R5_B1_RE11*TwissParameter_XRPV_B6R5_B1_RE12*xNR) + " << endl ;
	matchfile << "		     TwissParameter_XRPV_A6L5_B2_RE11*(-((TwissParameter_XRPV_A6R5_B1_RE11*TwissParameter_XRPV_A6R5_B1_RE12 - TwissParameter_XRPV_B6L5_B2_RE11*TwissParameter_XRPV_B6L5_B2_RE12 + TwissParameter_XRPV_B6R5_B1_RE11*TwissParameter_XRPV_B6R5_B1_RE12)*xNL) + " << endl ;
	matchfile << "			TwissParameter_XRPV_A6L5_B2_RE12*(TwissParameter_XRPV_B6L5_B2_RE11*xFL + TwissParameter_XRPV_B6R5_B1_RE11*xFR + TwissParameter_XRPV_A6R5_B1_RE11*xNR)))/" << endl ;
	matchfile << "		   (pow(TwissParameter_XRPV_A6R5_B1_RE12,2)*pow(TwissParameter_XRPV_B6L5_B2_RE11,2) + 2*TwissParameter_XRPV_A6R5_B1_RE11*TwissParameter_XRPV_A6R5_B1_RE12*TwissParameter_XRPV_B6L5_B2_RE11*TwissParameter_XRPV_B6L5_B2_RE12 + " ;
	matchfile << "		     pow(TwissParameter_XRPV_A6R5_B1_RE11,2)*pow(TwissParameter_XRPV_B6L5_B2_RE12,2) + pow(TwissParameter_XRPV_A6R5_B1_RE12,2)*pow(TwissParameter_XRPV_B6R5_B1_RE11,2) + " << endl ;
	matchfile << "		     pow(TwissParameter_XRPV_B6L5_B2_RE12,2)*pow(TwissParameter_XRPV_B6R5_B1_RE11,2) + pow(TwissParameter_XRPV_A6L5_B2_RE12,2)*" << endl ;
	matchfile << "		      (pow(TwissParameter_XRPV_A6R5_B1_RE11,2) + pow(TwissParameter_XRPV_B6L5_B2_RE11,2) + pow(TwissParameter_XRPV_B6R5_B1_RE11,2)) + " << endl ;
	matchfile << "		     2*(-(TwissParameter_XRPV_A6R5_B1_RE11*TwissParameter_XRPV_A6R5_B1_RE12) + TwissParameter_XRPV_B6L5_B2_RE11*TwissParameter_XRPV_B6L5_B2_RE12)*TwissParameter_XRPV_B6R5_B1_RE11*TwissParameter_XRPV_B6R5_B1_RE12 + " << endl ;
	matchfile << "		     (pow(TwissParameter_XRPV_A6R5_B1_RE11,2) + pow(TwissParameter_XRPV_B6L5_B2_RE11,2))*pow(TwissParameter_XRPV_B6R5_B1_RE12,2) + " << endl ;
	matchfile << "		     2*TwissParameter_XRPV_A6L5_B2_RE11*TwissParameter_XRPV_A6L5_B2_RE12*(TwissParameter_XRPV_A6R5_B1_RE11*TwissParameter_XRPV_A6R5_B1_RE12 - TwissParameter_XRPV_B6L5_B2_RE11*TwissParameter_XRPV_B6L5_B2_RE12 + " << endl ;
	matchfile << "			TwissParameter_XRPV_B6R5_B1_RE11*TwissParameter_XRPV_B6R5_B1_RE12) + pow(TwissParameter_XRPV_A6L5_B2_RE11,2)*" << endl ;
	matchfile << "		      (pow(TwissParameter_XRPV_A6R5_B1_RE12,2) + pow(TwissParameter_XRPV_B6L5_B2_RE12,2) + pow(TwissParameter_XRPV_B6R5_B1_RE12,2))) ;" << endl ;
}

int TMADX::writeConstraints(ofstream &matchfile) 
{

	readTFSPerBeam(matchfile, 1) ;
	readTFSPerBeam(matchfile, 4) ;

	if(ProjectParameters->getDebugMatching())
	{
	    set <string> twiss_optical_function_variables = ProjectParameters->Constraints.getVariables() ;
	    set <string>::iterator opit ;
	    for(opit = twiss_optical_function_variables.begin() ; opit != twiss_optical_function_variables.end() ; ++opit)
		    matchfile << "    cout << \"" << (*opit) << " = \" << " << (*opit) << " << endl ;" << endl ;
	}

	set<TStructForConstraints>::iterator it ;
	set<TStructForConstraints> constraints = ProjectParameters->Constraints.getConstraints() ;

	int i = 0 ;
	for(it = constraints.begin() ;  it != constraints.end() ; ++it)
	{
		stringstream ss ; ss << i ;

		matchfile << "    condition_variable_to_match[" <<  ss.str() << "] = " << it->constraint << " ;" << endl ;
		i++ ;
	}
	
	string verbose_reconstruction_fit_string = "Q" ;
	if(ProjectParameters->getParameterValue("verbose_reconstruction_fit") == 1) verbose_reconstruction_fit_string = "" ;

	double theta_x_star_boundary_urad = ProjectParameters->getParameterValue("theta_x_star_boundary_urad") ;
	stringstream theta_x_star_boundary_urad_string, theta_x_star_boundary_urad_minus_string ;
	theta_x_star_boundary_urad_string << theta_x_star_boundary_urad ;		
	theta_x_star_boundary_urad_minus_string << -theta_x_star_boundary_urad ;
	
	
	if((ProjectParameters->getParameterValue("reconstruction_45b_56t") == 1.0) || (ProjectParameters->getParameterValue("vertex_reconstruction_45b_56t") == 1.0))
	{
		matchfile << endl ;	
		matchfile << "    TProfile *theta_x_star_L_theta_x_star_R_profile = new TProfile(\"theta_x_star_L_theta_x_star_R_profile\",\"theta_x_star_L_theta_x_star_R_profile\", 100, -200, 200) ;" << endl ;
		matchfile << "    TProfile *theta_x_star_delta_x_star_profile = new TProfile(\"theta_x_star_delta_x_star_profile\",\"theta_x_star_delta_x_star_profile\", 100, -150, 150) ;" << endl ;	
		matchfile << endl ;			

		matchfile << "    const double desired_angle = TMath::Pi() / 4 ;" << endl ;
		matchfile << "    const double meter_to_mm = 1000.0 ;" << endl ;
		matchfile << "    const double meter_to_um = 1000000.0 ;" << endl ;
		matchfile << "    const double rad_to_urad = 1000000.0 ;" << endl ;
		matchfile << endl ;			

		matchfile << "    for(int i = 0 ; i < x_R_F_vs_y_R_F_45b_56t->GetN() ; ++i)" << endl ;
		matchfile << "    {" << endl ;
		matchfile << "        double xFR, yFR, xNR, yNR ;" << endl ;
		matchfile << endl ;
		matchfile << "        x_R_F_vs_y_R_F_45b_56t->GetPoint(i, yFR, xFR);" << endl ;
		matchfile << "        x_R_N_vs_y_R_N_45b_56t->GetPoint(i, yNR, xNR);" << endl ;
		matchfile << endl ;			

		matchfile << "        xFR /= meter_to_mm ;" << endl ;
		matchfile << "        xNR /= meter_to_mm ;" << endl ;
		matchfile << "        yFR /= meter_to_mm ;" << endl ;
		matchfile << "        yNR /= meter_to_mm ;" << endl ;

		matchfile << "        double theta_x_star_R = rad_to_urad * ((TwissParameter_XRPV_B6R5_B1_RE11 * xNR - TwissParameter_XRPV_A6R5_B1_RE11 * xFR) / (TwissParameter_XRPV_A6R5_B1_RE12 * TwissParameter_XRPV_B6R5_B1_RE11 - TwissParameter_XRPV_A6R5_B1_RE11 * TwissParameter_XRPV_B6R5_B1_RE12)) ;" << endl ;
		matchfile << endl ;	

		matchfile << "        double xFL, yFL, xNL, yNL ;" << endl ;
		matchfile << endl ;			

		matchfile << "        x_L_F_vs_y_L_F_45b_56t->GetPoint(i, yFL, xFL);" << endl ;
		matchfile << "        x_L_N_vs_y_L_N_45b_56t->GetPoint(i, yNL, xNL);" << endl ;
		matchfile << endl ;

		matchfile << "        xFL /= meter_to_mm ;" << endl ;
		matchfile << "        xNL /= meter_to_mm ;" << endl ;
		matchfile << "        yFL /= meter_to_mm ;" << endl ;
		matchfile << "        yNL /= meter_to_mm ;" << endl ;

		matchfile << "        double theta_x_star_L = rad_to_urad * ((TwissParameter_XRPV_B6L5_B2_RE11 * xNL - TwissParameter_XRPV_A6L5_B2_RE11 * xFL) /  (TwissParameter_XRPV_A6L5_B2_RE12 * TwissParameter_XRPV_B6L5_B2_RE11 - TwissParameter_XRPV_A6L5_B2_RE11 * TwissParameter_XRPV_B6L5_B2_RE12)) ;" << endl ;

		matchfile << "        double x_rotated = theta_x_star_L * cos(desired_angle) - theta_x_star_R * sin(desired_angle) ;" << endl ;
		matchfile << "        double y_rotated = theta_x_star_L * sin(desired_angle) + theta_x_star_R * cos(desired_angle) ;" << endl ;

		matchfile << "        theta_x_star_L_theta_x_star_R_profile->Fill(x_rotated, y_rotated) ;" << endl ;
		matchfile << endl ;		

		if(ProjectParameters->getParameterValue("vertex_reconstruction_45b_56t") == 1.0)
		{
			matchfile << "        double x_star_R = meter_to_um * ((TwissParameter_XRPV_A6R5_B1_RE12 * xFR - TwissParameter_XRPV_B6R5_B1_RE12 * xNR) / (TwissParameter_XRPV_A6R5_B1_RE12 * TwissParameter_XRPV_B6R5_B1_RE11 - TwissParameter_XRPV_A6R5_B1_RE11 * TwissParameter_XRPV_B6R5_B1_RE12)) ;" << endl ;
			matchfile << "        double x_star_L = meter_to_um * ((TwissParameter_XRPV_A6L5_B2_RE12 * xFL - TwissParameter_XRPV_B6L5_B2_RE12 * xNL) / (TwissParameter_XRPV_A6L5_B2_RE12 * TwissParameter_XRPV_B6L5_B2_RE11 - TwissParameter_XRPV_A6L5_B2_RE11 * TwissParameter_XRPV_B6L5_B2_RE12)) ;" << endl ;
			matchfile << endl ;		

			matchfile << "        double delta_x_star = x_star_R - x_star_L ;" << endl ;
			matchfile << endl ;		
		
			if(ProjectParameters->getParameterValue("linear_least_seuqare_fit") == 1.0)
			{
				write_linear_least_seuqare_fit(matchfile) ;
			}
			else
			{
				matchfile << "        double theta_x_star = (theta_x_star_L - theta_x_star_R) / 2 ;" << endl ;
       				matchfile << endl ;
			}
			
			matchfile << "        theta_x_star_delta_x_star_profile->Fill(theta_x_star, delta_x_star) ;" << endl ;
			matchfile << endl ;		
			

		}
		
		matchfile << "    }" << endl ;
		matchfile << endl ;
		matchfile << "    TFitResultPtr fit_result = theta_x_star_L_theta_x_star_R_profile->Fit(\"pol1\",\"S" + verbose_reconstruction_fit_string + "\",\"\"," + theta_x_star_boundary_urad_minus_string.str() + ", " + theta_x_star_boundary_urad_string.str() + ") ;" << endl ;
		
		if(ProjectParameters->getParameterValue("vertex_reconstruction_45b_56t") == 1.0)
		{				
			matchfile << "    TFitResultPtr fit_result2 = theta_x_star_delta_x_star_profile->Fit(\"pol1\",\"SQ\",\"\","+ theta_x_star_boundary_urad_minus_string.str() +", "+ theta_x_star_boundary_urad_string.str() + ") ;" << endl ;		
		}
		
		
		matchfile << "    theta_x_star_L_theta_x_star_R_profile->Delete() ;" << endl ;
		matchfile << "    theta_x_star_delta_x_star_profile->Delete() ;" << endl ;		

		
		
	}

	if((ProjectParameters->getParameterValue("reconstruction_45t_56b") == 1.0) || (ProjectParameters->getParameterValue("vertex_reconstruction_45t_56b") == 1.0))
	{

		matchfile << "    TProfile *theta_x_star_L_theta_x_star_R_profile_45t_56b = new TProfile(\"theta_x_star_L_theta_x_star_R_profile_45t_56b\",\"theta_x_star_L_theta_x_star_R_profile_45t_56b\", 100, -200, 200) ;" << endl ;
		matchfile << "    TProfile *theta_x_star_delta_x_star_profile_45t_56b = new TProfile(\"theta_x_star_delta_x_star_profile_45t_56b\",\"theta_x_star_delta_x_star_profile_45t_56b\", 100, -150, 150) ;" << endl ;	
		
		matchfile << "    const double desired_angle = TMath::Pi() / 4 ;" << endl ;
		matchfile << "    const double meter_to_mm = 1000.0 ;" << endl ;
		matchfile << "    const double meter_to_um = 1000000.0 ;" << endl ;
		matchfile << "    const double rad_to_urad = 1000000.0 ;" << endl ;
		matchfile << endl ;			
		
		matchfile << "    for(int i = 0 ; i < x_R_F_vs_y_R_F_45t_56b->GetN() ; ++i)" << endl ;
		matchfile << "    {" << endl ;
		matchfile << "        double xFR, yFR, xNR, yNR ;" << endl ;
		matchfile << endl ;
		matchfile << "        x_R_F_vs_y_R_F_45t_56b->GetPoint(i, yFR, xFR);" << endl ;
		matchfile << "        x_R_N_vs_y_R_N_45t_56b->GetPoint(i, yNR, xNR);" << endl ;
		matchfile << endl ;			

		matchfile << "        xFR /= meter_to_mm ;" << endl ;
		matchfile << "        xNR /= meter_to_mm ;" << endl ;
		matchfile << "        yFR /= meter_to_mm ;" << endl ;
		matchfile << "        yNR /= meter_to_mm ;" << endl ;

		matchfile << "        double theta_x_star_R = rad_to_urad * ((TwissParameter_XRPV_B6R5_B1_RE11 * xNR - TwissParameter_XRPV_A6R5_B1_RE11 * xFR) / (TwissParameter_XRPV_A6R5_B1_RE12 * TwissParameter_XRPV_B6R5_B1_RE11 - TwissParameter_XRPV_A6R5_B1_RE11 * TwissParameter_XRPV_B6R5_B1_RE12)) ;" << endl ;
		matchfile << endl ;	

		matchfile << "        double xFL, yFL, xNL, yNL ;" << endl ;
		matchfile << endl ;			

		matchfile << "        x_L_F_vs_y_L_F_45t_56b->GetPoint(i, yFL, xFL);" << endl ;
		matchfile << "        x_L_N_vs_y_L_N_45t_56b->GetPoint(i, yNL, xNL);" << endl ;
		matchfile << endl ;

		matchfile << "        xFL /= meter_to_mm ;" << endl ;
		matchfile << "        xNL /= meter_to_mm ;" << endl ;
		matchfile << "        yFL /= meter_to_mm ;" << endl ;
		matchfile << "        yNL /= meter_to_mm ;" << endl ;

		matchfile << "        double theta_x_star_L = rad_to_urad * ((TwissParameter_XRPV_B6L5_B2_RE11 * xNL - TwissParameter_XRPV_A6L5_B2_RE11 * xFL) /  (TwissParameter_XRPV_A6L5_B2_RE12 * TwissParameter_XRPV_B6L5_B2_RE11 - TwissParameter_XRPV_A6L5_B2_RE11 * TwissParameter_XRPV_B6L5_B2_RE12)) ;" << endl ;

		matchfile << "        double x_rotated = theta_x_star_L * cos(desired_angle) - theta_x_star_R * sin(desired_angle) ;" << endl ;
		matchfile << "        double y_rotated = theta_x_star_L * sin(desired_angle) + theta_x_star_R * cos(desired_angle) ;" << endl ;

		matchfile << "        theta_x_star_L_theta_x_star_R_profile_45t_56b->Fill(x_rotated, y_rotated) ;" << endl ;
		matchfile << endl ;		

		if(ProjectParameters->getParameterValue("vertex_reconstruction_45t_56b") == 1.0)
		{	
			matchfile << "        double x_star_R = meter_to_um * ((TwissParameter_XRPV_A6R5_B1_RE12 * xFR - TwissParameter_XRPV_B6R5_B1_RE12 * xNR) / (TwissParameter_XRPV_A6R5_B1_RE12 * TwissParameter_XRPV_B6R5_B1_RE11 - TwissParameter_XRPV_A6R5_B1_RE11 * TwissParameter_XRPV_B6R5_B1_RE12)) ;" << endl ;
			matchfile << "        double x_star_L = meter_to_um * ((TwissParameter_XRPV_A6L5_B2_RE12 * xFL - TwissParameter_XRPV_B6L5_B2_RE12 * xNL) / (TwissParameter_XRPV_A6L5_B2_RE12 * TwissParameter_XRPV_B6L5_B2_RE11 - TwissParameter_XRPV_A6L5_B2_RE11 * TwissParameter_XRPV_B6L5_B2_RE12)) ;" << endl ;
			matchfile << endl ;		

			matchfile << "        double delta_x_star = x_star_R - x_star_L ;" << endl ;
			matchfile << endl ;		
		
			if(ProjectParameters->getParameterValue("linear_least_seuqare_fit") == 1.0)
			{
				write_linear_least_seuqare_fit(matchfile) ;				
			}
			else
			{
				matchfile << "        double theta_x_star = (theta_x_star_L - theta_x_star_R) / 2 ;" << endl ;
       				matchfile << endl ;
			}

			matchfile << "        theta_x_star_delta_x_star_profile_45t_56b->Fill(theta_x_star, delta_x_star) ;" << endl ;
			matchfile << endl ;			

		}
		
		matchfile << "    }" << endl ;
		matchfile << endl ;
		matchfile << "    TFitResultPtr fit_result3 = theta_x_star_L_theta_x_star_R_profile_45t_56b->Fit(\"pol1\",\"S" + verbose_reconstruction_fit_string + "\",\"\"," + theta_x_star_boundary_urad_minus_string.str() + ", " + theta_x_star_boundary_urad_string.str() + ") ;" << endl ;
		matchfile << "    theta_x_star_L_theta_x_star_R_profile_45t_56b->Delete() ;" << endl ;
		
		if(ProjectParameters->getParameterValue("vertex_reconstruction_45t_56b") == 1.0)
		{		
			matchfile << "    TFitResultPtr fit_result4 = theta_x_star_delta_x_star_profile_45t_56b->Fit(\"pol1\",\"SQ\",\"\","+ theta_x_star_boundary_urad_minus_string.str() +", "+ theta_x_star_boundary_urad_string.str() + ") ;" << endl ;
		}
		
		matchfile << "    theta_x_star_delta_x_star_profile_45t_56b->Delete() ;" << endl ;

	}


	matchfile << endl ;
	matchfile << "    double chi2 = 0 ; " << endl << endl ;

	stringstream ss ; ss << constraints.size() ;
	matchfile << "    for(int i = 0 ; i < " << ss.str() << " ; ++i)" << endl ;
        if(ProjectParameters->getConstraintSmearing())
		matchfile << "        chi2 += pow((condition_variable_to_match[i] - condition_variable_target[i] + condition_variable_smearing[i])/condition_variable_error[i],2)  ;" << endl ;
	else
		matchfile << "        chi2 += pow((condition_variable_to_match[i] - condition_variable_target[i])/condition_variable_error[i],2)  ;" << endl ;
	matchfile << endl ;		
		
	// If requested the contribution from the reconstructed theta_x_star right/left distribution is added to the chi2
	
	// 45b_56t

	if((ProjectParameters->getParameterValue("reconstruction_45b_56t") == 1.0))
	{
		matchfile << "    chi2 += pow(fit_result->Parameter(1) / fit_result->ParError(1),2) ;" << endl << endl ;
	}

	// 45t_56b

	if(ProjectParameters->getParameterValue("reconstruction_45t_56b") == 1.0)
	{
		matchfile << "    chi2 += pow(fit_result3->Parameter(1) / fit_result3->ParError(1),2) ;" << endl << endl ;
	}

	if(ProjectParameters->getParameterValue("calculate_the_average_of_the_two_diagonals") == 1.0)
	{
		matchfile << "    chi2 += pow((fit_result2->Parameter(1) + fit_result4->Parameter(1)) / (fit_result2->ParError(1) + fit_result4->ParError(1)),2) ;" << endl << endl ;
	}
	else
	{
		if(ProjectParameters->getParameterValue("vertex_reconstruction_45b_56t") == 1.0)
		{
			matchfile << "    chi2 += pow(fit_result2->Parameter(1) / fit_result2->ParError(1),2) ;" << endl << endl ;
		}
		
		if(ProjectParameters->getParameterValue("vertex_reconstruction_45t_56b") == 1.0)
		{
			matchfile << "    chi2 += pow(fit_result4->Parameter(1) / fit_result4->ParError(1),2) ;" << endl << endl ;
		}
	}

	return 0 ;
}

int TMADX::obtain_and_read_TFS_files_BeforeMatching(ofstream &matchfile, int beam_number)
{
	stringstream beam_string ;
	beam_string << beam_number ;

	++number_of_calls ;
	
	stringstream str_number_of_calls ;
	str_number_of_calls << number_of_calls ;
	
	string filename_pert = ("perturbations_" + str_number_of_calls.str()) ;

        matchfile << "" << endl ;
        matchfile << "    ofstream " << filename_pert << "((working_directory + string(\"/myLHC/Beam_" + beam_string.str() + "/temporary_perturbation.madx\")).c_str()) ;" << endl ;
        matchfile << "    " << filename_pert << " << endl ;" << endl ;	
        matchfile << "" << endl ;
	
	matchfile <<  "    auxstring =  working_directory + string(\"/myLHC/Beam_" << beam_string.str() << "/opticsproject.madx\")  ;" << endl ;
    	matchfile <<  "    MADX_status = system((\"/afs/cern.ch/user/m/mad/bin/madx < \" + auxstring + \" > /dev/null\").c_str()) ;" << endl ;
        matchfile << "    if(MADX_status) { cout << \"MADX problem:\" << MADX_status << endl ; exit(1) ; }" << endl ;		
	matchfile << endl  ;
	
	matchfile <<  "    string auxstring_b" << beam_string.str() << " =  working_directory + string(\"/myLHC/Beam_" << beam_string.str() << "/opticsproject.tfs\")  ;" << endl ;	
	matchfile <<  "    string auxstring_before_b" << beam_string.str() << " =  working_directory + string(\"/myLHC/Beam_" << beam_string.str() << "/opticsproject_before_matching.tfs\")  ;" << endl ;
	
    	matchfile <<  "    MADX_status = system((\"mv \" + auxstring_b" << beam_string.str() << " + \" \" + auxstring_before_b" << beam_string.str() << " ).c_str()) ;" << endl ;			
        matchfile << "    if(MADX_status) { cout << \"MADX problem:\" << MADX_status << endl ; exit(1) ; }" << endl ;		
	
	return 1 ;
}


int TMADX::readTarget(ofstream &matchfile)
{
	set<TStructForConstraints>::iterator it ;
        set<TStructForConstraints> constraints = ProjectParameters->Constraints.getConstraints() ;

	matchfile << endl ;

	matchfile <<  "    string auxstring =  working_directory + string(\"/myLHC/Beam_1/opticsproject.madx\")  ;" << endl ;
    	matchfile <<  "    int index = auxstring.find(\"Missile\") ;" << endl ;
        matchfile <<  "    auxstring.replace(index,7,\"Target\",6) ;" << endl ;
    	matchfile <<  "    MADX_status = system((\"/afs/cern.ch/user/m/mad/bin/madx < \" + auxstring + \" > /dev/null\").c_str()) ;" << endl ;
        matchfile << "    if(MADX_status) { cout << \"MADX problem:\" << MADX_status << endl ; exit(1) ; }" << endl ;		
	matchfile << endl  ;

        matchfile <<  "    auxstring =  working_directory + string(\"/myLHC/Beam_4/opticsproject.madx\")  ;" << endl ;
        matchfile <<  "    index = auxstring.find(\"Missile\") ;" << endl ;
        matchfile <<  "    auxstring.replace(index,7,\"Target\",6) ;" << endl ;
        matchfile <<  "    MADX_status = system((\"/afs/cern.ch/user/m/mad/bin/madx < \" + auxstring + \" > /dev/null\").c_str()) ;" << endl ;
        matchfile << "    if(MADX_status) { cout << \"MADX problem:\" << MADX_status << endl ; exit(1) ; }" << endl ;		
	matchfile << endl  ;

	if(ProjectParameters->getParameterValue("exit_if_target_ready") == 1.0)
	{
		matchfile << endl  ;	
		matchfile << "    exit(1) ;" << endl ;
	}

	readTFSPerBeam(matchfile,1,true) ;
	readTFSPerBeam(matchfile,4,true) ;

        if(ProjectParameters->getDebugMatching())
        {
            set <string> twiss_optical_function_variables = ProjectParameters->Constraints.getVariables() ;
            set <string>::iterator opit ;
            for(opit = twiss_optical_function_variables.begin() ; opit != twiss_optical_function_variables.end() ; ++opit)
                    matchfile << "    cout << \"" << (*opit) << " = \" << " << (*opit) << " << endl ;" << endl ;
        }


	matchfile << endl ;
        matchfile << "    cout << \"Target twiss files have been read in.\" << endl ;" << endl ;

	// Initializing target constraints
        int i = 0 ;
        for(it = constraints.begin() ;  it != constraints.end() ; ++it)
        {

		// Only those constraints are calculated which contain some twiss variable. 
		// The others are design constraints, which do not depend on the target, they are 0.

               	if(ProjectParameters->getProjectType() == Matching_N1)
               	{
                	stringstream ss ; 
			ss << i ;
		
                	matchfile << "    condition_variable_target[" <<  ss.str() << "] = " << it->value << " ;" << endl ;
		}
		else if((ProjectParameters->getProjectType() == Matching_1N))
		{
                	stringstream ss ; 
			ss << i ;
			
			if(it->constraint.find("MinuitParameter[") == string::npos) // not a design parameter
			                	matchfile << "    condition_variable_target[" <<  ss.str() << "] = " << it->constraint << " ;" << endl ;
			else
						matchfile << "    condition_variable_target[" <<  ss.str() << "] = 0 ;" << endl ; // for design parameters
		}

               	i++ ;
        }

	matchfile << endl ; 

	// Initializing errors and smearing
	
	i = 0 ;
        for(it = constraints.begin() ;  it != constraints.end() ; ++it)
        {
		stringstream ss ; ss << i ;
		string error_text ;

		if(it->error == 0)
		{
			stringstream errt ;
			errt << it->relative_error ;

			error_text = "(" + errt.str() + " * condition_variable_target[" + ss.str() + "]) ;" ;
		}
		else
		{
			stringstream errt ;
			errt << it->error ;

			error_text = errt.str() ;
		}

		matchfile << "    condition_variable_error[" <<  ss.str() << "] = " + error_text + " ;" << endl ;
		
		if(it->constraint.find("MinuitParameter[") == string::npos) // not a design parameter
			matchfile << "    condition_variable_smearing[" <<  ss.str() << "] = " + error_text + " ;" << endl ;		
		else
			matchfile << "    condition_variable_smearing[" <<  ss.str() << "] = 0 ;" << endl ;				

                i++ ;
        }

	matchfile << endl ;
	
	return 0 ;
}

int TMADX::writeMinuitCode(ofstream &matchfile) 
{
        matchfile << "    TMinuit *gMinuit = new TMinuit(30);" << endl ;
        matchfile << "    gMinuit->SetFCN(fcn);" << endl ;
        matchfile << "" << endl ;
        matchfile << "    Double_t arglist[20];" << endl ;
        matchfile << "    Int_t ierflg = 0 ;" << endl ;
        matchfile << "    arglist[0] = 1 ;" << endl ;
        matchfile << "    gMinuit->mnexcm(\"SET ERR\", arglist ,1,ierflg);" << endl ;
        matchfile << "" << endl ;

        map <int, TStructForMatching> *matched_magnet_params = ProjectParameters->GetMatchedParameters() ;
	map <int, TStructForMatching>::iterator it2 ;

	for(it2 = matched_magnet_params->begin() ; it2 != matched_magnet_params->end(); it2++)
	{
		stringstream ss, value, ulimit, llimit, step ; 
                ss << it2->first ; ;
		value <<   it2->second.value ;
		ulimit <<  it2->second.upper_limit ;
		llimit <<  it2->second.lower_limit ;
		step <<    it2->second.step ;

		if(it2->second.type.compare("Strength") == 0)
			matchfile << "    gMinuit->mnparm(" + ss.str() + ",\"Strength_" + it2->second.name + "\", " + value.str() + " , " + step.str()  + "," + llimit.str() + ", " + ulimit.str() + ", ierflg);" << endl ; 

		else if(it2->second.type.compare("Rotation") == 0)
			matchfile << "    gMinuit->mnparm(" + ss.str() + ",\"Rotation_" + it2->second.name + "\", " + value.str() + " , " + step.str()  + "," + llimit.str() + ", " + ulimit.str() + ", ierflg);" << endl ; 

	}

	if(ProjectParameters->getMatchedBeamMomentum())
	{
		stringstream ss1 ;
		ss1 << matched_magnet_params->size() ;
		matchfile << "    gMinuit->mnparm(" + ss1.str() + ",\"xi_beam1\", 0, 0.001,-0.1,0.1,ierflg) ;" << endl ;

		stringstream ss2 ;
                ss2 << matched_magnet_params->size() + 1 ;
                matchfile << "    gMinuit->mnparm(" + ss2.str() + ",\"xi_beam2\", 0, 0.001,-0.1,0.1,ierflg) ;" << endl ;
	}


	stringstream ss ;
	ss << ProjectParameters->getMinuitPrecision() ;

        matchfile << "    arglist[0] = " + ss.str() + " ;" << endl ;
        matchfile << "    gMinuit->mnexcm(\"SET EPS\", arglist ,1,ierflg);" << endl ;
        matchfile << "" << endl ;
        matchfile << "    arglist[0] = " << ProjectParameters->getMinuitCallLimit() << "  ;" << endl ;
        matchfile << "    arglist[1] = " << ProjectParameters->getMinuitTolerance() << " ;" << endl ;
        matchfile << "" << endl ;
        matchfile << "    gMinuit->mnexcm(\"MIGRAD\", arglist ,2,ierflg);" << endl ;
        matchfile << "" << endl ;
        matchfile << "    exit(0) ;" << endl ;
        matchfile << "" << endl ;
        matchfile << "}" << endl ;       

	return 0 ;
}

int TMADX::writeRandomGenerator(ofstream &matchfile)
{
        matchfile << "    TRandom3 *random = new TRandom3() ;" << endl ;
        matchfile << "" << endl ;
//        matchfile << "    error[1] = random->Gaus(0,0.5) ;      " << endl ;
//        matchfile << "    error[2] = random->Gaus(0,condition_variable2_target*0.01*0.4) ;      " << endl ;
//        matchfile << "    error[3] = random->Gaus(0,0.5*0.001) ;        " << endl ;
//        matchfile << "    error[4] = random->Gaus(0,0.7*0.001) ;        " << endl ;
//        matchfile << "    error[5] = random->Gaus(0,0.5) ;      " << endl ;
//        matchfile << "    error[6] = random->Gaus(0,condition_variable6_target*0.01*1.4) ;      " << endl ;
//        matchfile << "    error[7] = random->Gaus(0,1.1*0.001) ;        " << endl ;
//        matchfile << "    error[8] = random->Gaus(0,0.9*0.001) ;        " << endl ;
//        matchfile << "    error[9] = random->Gaus(0,1) ;        " << endl ;
//        matchfile << "    error[10] = random->Gaus(0,1) ;       " << endl ;
//        matchfile << "    error[11] = random->Gaus(0,1) ;       " << endl ;
//        matchfile << "    error[12] = random->Gaus(0,1) ;       " << endl ;
//        matchfile << "    error[13] = random->Gaus(0,0.002) ;   " << endl ;
//        matchfile << "    error[14] = random->Gaus(0,0.005) ;   " << endl ;
//        matchfile << "" << endl ;
	
	return 0 ; 
}

 #endif
