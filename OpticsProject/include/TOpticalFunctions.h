#ifndef TOPTICALFUNCTIONS_H
#define TOPTICALFUNCTIONS_H

#include <include/TBeamElement.h>

class TOpticalFunctions : public TBeamElement
{

	private:

	double 	S, X, PX, Y, PY, BETX, BETY, BETA11, BETA12, BETA21, BETA22, MU1, MU2, MU3, DISP1, DISP2, DISP3, DISP4, RE11, RE12, RE13, RE14, RE21, RE22, RE23, RE24, RE31, 
		RE32, RE33, RE34, RE41, RE42, RE43, RE44, ROT, BSX, BSY ;


	public:
	
	 
	        static vector<string> optical_function_names ;

		TOpticalFunctions(string aname, double &aS, double &aX, double &aPX, double &aY, double &aPY, double &aBETX, double &aBETY, double &aBETA11, double &aBETA12, 
			double &aBETA21, double &aBETA22, double &aMU1, double &aMU2, double &aMU3, double &aDISP1, double &aDISP2, double &aDISP3, double &aDISP4, double &aRE11, 
			double &aRE12, double &aRE13, double &aRE14, double &aRE21, double &aRE22, double &aRE23, double &aRE24, double &aRE31, double &aRE32, double &aRE33, 
			double &aRE34, double &aRE41, double &aRE42, double &aRE43, double &aRE44, double &aROT, double &aBSX, double &aBSY) : TBeamElement(aname) 
                {
			type = "Optical function" ;

                        S = aS ; 	X = aX ;	PX = aPX ; 	Y = aY ; 	PY = aPY ;	BETX = aBETX ;		BETY = aBETY ;		BETA11 = aBETA11 ; BETA12 = aBETA12 ; 
                        BETA21 = aBETA21 ; BETA22 = aBETA22 ; MU1 = aMU1 ; MU2 = aMU2 ; MU3 = aMU3 ; DISP1 = aDISP1 ; DISP2 = aDISP2 ; DISP3 = aDISP3 ; DISP4 = aDISP4 ;
                        RE11 = aRE11 ; RE12 = aRE12 ; RE13 = aRE13 ; RE14 = aRE14 ; RE21 = aRE21 ; RE22 = aRE22 ; RE23 = aRE23 ; RE24 = aRE24 ;
                        RE31 = aRE31 ; 
                        RE32 = aRE32 ;
                        RE33 = aRE33 ;
                        RE34 = aRE34 ;
                        RE41 = aRE41 ;
                        RE42 = aRE42 ;
                        RE43 = aRE43 ;
                        RE44 = aRE44 ;
                        ROT = aROT ;
                        BSX = aBSX ;
                        BSY = aBSY ;
                } ;

		TOpticalFunctions(string aname) : TBeamElement(aname)
		{
			type = "Optical function" ;
			name = aname ;
		} ;

		int Load(ifstream &tfsfile) ;
		int LoadFromROOT(string path, TFile *f, OpticsArgumentType * = NULL) ;
		int Print(int depth) { depth++ ; return 0 ; } ;
		int Execute(string dir) { TBeamElement::Execute(dir) ; return 0 ; } ;
		int Save(TDirectory *dir, TFile *, OpticsArgumentType*) ;
		int RetrieveOpticalFunctions(map <string, double> &) ;

		double getLy() { return RE34 ; }
		double getLx() { return RE12 ; }
		double get_nu_x() { return RE11 ; }
		double get_nu_y() { return RE33 ; }
}; 

const int number_of_optical_functions = 37 ;
const string init[] = {"S", "X", "PX", "Y", "PY", "BETX", "BETY", "BETA11", "BETA12", "BETA21", "BETA22", "MU1", "MU2", "MU3",
                "DISP1", "DISP2", "DISP3", "DISP4", "RE11", "RE12", "RE13", "RE14", "RE21", "RE22", "RE23", "RE24", "RE31", "RE32", "RE33", "RE34",
                "RE41", "RE42", "RE43", "RE44", "ROT", "BSX", "BSY"} ;

vector<string> TOpticalFunctions::optical_function_names(init,init + number_of_optical_functions) ; 

int TOpticalFunctions::Load(ifstream &tfsfile)
{

	string line ; 

	while ( tfsfile.good() )
	{
		getline (tfsfile,line);

		if(line.substr(0,1).compare("@") != 0)  
		{
		//	cout << line << endl;
		}
	}

	tfsfile.close(); 

	return 0 ; 
}

int TOpticalFunctions::LoadFromROOT(string path, TFile *f, OpticsArgumentType *OpticsArgument)
{

        TBeamElement::LoadFromROOT(path,f) ;
        path += "/" + name + "/Properties" ;

        Properties = (TTree *)f->Get((path).c_str());
	
	if(Properties != NULL)
	{
	
//	        Properties->SetBranchAddress("S", &S);
//	        Properties->SetBranchAddress("X", &X);
//	        Properties->SetBranchAddress("PX", &PX);
//	        Properties->SetBranchAddress("Y", &Y);
//	        Properties->SetBranchAddress("PY", &PY);
//	        Properties->SetBranchAddress("BETX", &BETX);
//	        Properties->SetBranchAddress("BETY", &BETY);
//	        Properties->SetBranchAddress("BETA11", &BETA11);
//	        Properties->SetBranchAddress("BETA12", &BETA12);
//	        Properties->SetBranchAddress("BETA21", &BETA21);
//	        Properties->SetBranchAddress("BETA22", &BETA22);
//	        Properties->SetBranchAddress("MU1", &MU1);
//	        Properties->SetBranchAddress("MU2", &MU2);
//	        Properties->SetBranchAddress("MU3", &MU3);
//	        Properties->SetBranchAddress("DISP1", &DISP1);
//	        Properties->SetBranchAddress("DISP2", &DISP2);
//	        Properties->SetBranchAddress("DISP3", &DISP3);
//	        Properties->SetBranchAddress("DISP4", &DISP4);
	        Properties->SetBranchAddress("RE11", &RE11);
	        Properties->SetBranchAddress("RE12", &RE12);
//	        Properties->SetBranchAddress("RE13", &RE13);
//	        Properties->SetBranchAddress("RE14", &RE14);
//	        Properties->SetBranchAddress("RE21", &RE21);
//	        Properties->SetBranchAddress("RE22", &RE22);
//	        Properties->SetBranchAddress("RE23", &RE23);
//	        Properties->SetBranchAddress("RE24", &RE24);
//	        Properties->SetBranchAddress("RE31", &RE31);
//	        Properties->SetBranchAddress("RE32", &RE32);
	        Properties->SetBranchAddress("RE33", &RE33);
	        Properties->SetBranchAddress("RE34", &RE34);
//	        Properties->SetBranchAddress("RE41", &RE41);
//	        Properties->SetBranchAddress("RE42", &RE42);
//	        Properties->SetBranchAddress("RE43", &RE43);
//	        Properties->SetBranchAddress("RE44", &RE44);
//	        Properties->SetBranchAddress("ROT", &ROT);
//	        Properties->SetBranchAddress("BSX", &BSX);
//	        Properties->SetBranchAddress("BSY", &BSY);

	        Properties->GetEntry(0) ;
		
	}
	
        return 0 ;
}

int TOpticalFunctions::RetrieveOpticalFunctions(map <string, double> &opticalfunctions)
{
//	opticalfunctions.insert(pair<string,double>("S",S)) ;
//	opticalfunctions.insert(pair<string,double>("X",X)) ;
//	opticalfunctions.insert(pair<string,double>("PX",PX)) ;
//	opticalfunctions.insert(pair<string,double>("Y",Y)) ;
//	opticalfunctions.insert(pair<string,double>("PY",PY)) ;
//	opticalfunctions.insert(pair<string,double>("BETX",BETX)) ;
//	opticalfunctions.insert(pair<string,double>("BETY",BETY)) ;
//	opticalfunctions.insert(pair<string,double>("BETA11",BETA11)) ;
//	opticalfunctions.insert(pair<string,double>("BETA12",BETA12)) ;
//	opticalfunctions.insert(pair<string,double>("BETA21",BETA21)) ;
//	opticalfunctions.insert(pair<string,double>("BETA22",BETA22)) ;
//	opticalfunctions.insert(pair<string,double>("MU1",MU1)) ;
//	opticalfunctions.insert(pair<string,double>("MU2",MU2)) ;
//	opticalfunctions.insert(pair<string,double>("MU3",MU3)) ;
//	opticalfunctions.insert(pair<string,double>("DISP1",DISP1)) ;
//	opticalfunctions.insert(pair<string,double>("DISP2",DISP2)) ;
//	opticalfunctions.insert(pair<string,double>("DISP3",DISP3)) ;
//	opticalfunctions.insert(pair<string,double>("DISP4",DISP4)) ;
	opticalfunctions.insert(pair<string,double>("RE11",RE11)) ;
	opticalfunctions.insert(pair<string,double>("RE12",RE12)) ;
//	opticalfunctions.insert(pair<string,double>("RE13",RE13)) ;
//	opticalfunctions.insert(pair<string,double>("RE14",RE14)) ;
//	opticalfunctions.insert(pair<string,double>("RE21",RE21)) ;
//	opticalfunctions.insert(pair<string,double>("RE22",RE22)) ;
//	opticalfunctions.insert(pair<string,double>("RE23",RE23)) ;
//	opticalfunctions.insert(pair<string,double>("RE24",RE24)) ;
//	opticalfunctions.insert(pair<string,double>("RE31",RE31)) ;
//	opticalfunctions.insert(pair<string,double>("RE32",RE32)) ;
	opticalfunctions.insert(pair<string,double>("RE33",RE33)) ;
	opticalfunctions.insert(pair<string,double>("RE34",RE34)) ;
//	opticalfunctions.insert(pair<string,double>("RE41",RE41)) ;
//	opticalfunctions.insert(pair<string,double>("RE42",RE42)) ;
//	opticalfunctions.insert(pair<string,double>("RE43",RE43)) ;
//	opticalfunctions.insert(pair<string,double>("RE44",RE44)) ;
//	opticalfunctions.insert(pair<string,double>("ROT",ROT)) ;
//	opticalfunctions.insert(pair<string,double>("BSX",BSX)) ;
//	opticalfunctions.insert(pair<string,double>("BSY",BSY)) ;

	return 0  ;
}

int TOpticalFunctions::Save(TDirectory *dir, TFile *rootfile, OpticsArgumentType *OpticsArgument)
{
	TBeamElement::Save(dir,rootfile) ;

        Properties = new TTree("Properties","This branch describes the properties of the optical functions");

//        Properties->Branch("S", &S, "S/D");
//        Properties->Branch("X", &X, "X/D");
//        Properties->Branch("PX", &PX, "PX/D");
//        Properties->Branch("Y", &Y, "Y/D");
//        Properties->Branch("PY", &PY, "PY/D");
//        Properties->Branch("BETX", &BETX, "BETX/D");
//        Properties->Branch("BETY", &BETY, "BETY/D");
//        Properties->Branch("BETA11", &BETA11, "BETA11/D");
//        Properties->Branch("BETA12", &BETA12, "BETA12/D");
//        Properties->Branch("BETA21", &BETA21, "BETA21/D");
//        Properties->Branch("BETA22", &BETA22, "BETA22/D");
//        Properties->Branch("MU1", &MU1, "MU1/D");
//        Properties->Branch("MU2", &MU2, "MU2/D");
//        Properties->Branch("MU3", &MU3, "MU3/D");
//        Properties->Branch("DISP1", &DISP1, "DISP1/D");
//        Properties->Branch("DISP2", &DISP2, "DISP2/D");
//        Properties->Branch("DISP3", &DISP3, "DISP3/D");
//        Properties->Branch("DISP4", &DISP4, "DISP4/D");
          Properties->Branch("RE11", &RE11, "RE11/D");
          Properties->Branch("RE12", &RE12, "RE12/D");
//        Properties->Branch("RE13", &RE13, "RE13/D");
//        Properties->Branch("RE14", &RE14, "RE14/D");
//        Properties->Branch("RE21", &RE21, "RE21/D");
//        Properties->Branch("RE22", &RE22, "RE22/D");
//        Properties->Branch("RE23", &RE23, "RE23/D");
//        Properties->Branch("RE24", &RE24, "RE24/D");
//        Properties->Branch("RE31", &RE31, "RE31/D");
//        Properties->Branch("RE32", &RE32, "RE32/D");
          Properties->Branch("RE33", &RE33, "RE33/D");
          Properties->Branch("RE34", &RE34, "RE34/D");
//        Properties->Branch("RE41", &RE41, "RE41/D");
//        Properties->Branch("RE42", &RE42, "RE42/D");
//        Properties->Branch("RE43", &RE43, "RE43/D");
//        Properties->Branch("RE44", &RE44, "RE44/D");
//        Properties->Branch("ROT", &ROT, "ROT/D");
//        Properties->Branch("BSX", &BSX, "BSX/D");
//        Properties->Branch("BSY", &BSY, "BSY/D");

        Properties->Fill() ;
	
	return 0 ;


}

#endif
