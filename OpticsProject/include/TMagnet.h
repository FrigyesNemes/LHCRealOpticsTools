#ifndef TMAGNET_H
#define TMAGNET_H

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#include "TRandom.h"
#include <include/TBeamElement.h>
#include <include/TProjectParameters.h>
#include <include/TOpticalFunctions.h>

class TMagnet : public TBeamElement
{
	private :
		
		double strength ;
		bool strength_is_valid ;

		double strength_perturbation_factor, strength_perturbation_RMS, strength_perturbation_MEAN ;
		double rotation_perturbation, rotation_perturbation_RMS, rotation_perturbation_MEAN ;
		double x_perturbation, x_perturbation_RMS, x_perturbation_MEAN ;
		double y_perturbation, y_perturbation_RMS, y_perturbation_MEAN ;
		
		double strength_matching_factor, rotation_matching ;

		bool in_a_matched_beam ;
		bool matched_strength, matched_rotation ; 
		string nominal_strength_text ;
		string magnet_type ;
		int polarity, TOTEM_sign_conversion_2011_08_03_with_Huberts_rule ; 
		TProjectParameters *ProjectParameters ;
		
	
	public :
		TMagnet(string, TProjectParameters *, double, double, double, double, double, double, double, double, double, double, double, double, 
			bool, double, string, int, int, string) ; 
		int Print(int);
		int Save(TDirectory *, TFile *, OpticsArgumentType *) ;
		int LoadFromROOT(string, TFile *, OpticsArgumentType *) ;
                int GenerateMADXCode(ofstream &, string) ;
		
		double SetStrength(double astrength) { strength_is_valid = true ; strength = astrength ; return strength ; } 
		double setStrengthPerturbationFactor(double astrength_perturbation_factor) { return strength_perturbation_factor = astrength_perturbation_factor ; } 	
		double setRotationPerturbation(double arotation_perturbation) { return rotation_perturbation = arotation_perturbation ; } 			
		double setStrengthMatchingFactor(double astrength_matching_factor) { return strength_matching_factor = astrength_matching_factor ; } ;
		double setRotationMatching(double arotation_matching ) { return rotation_matching =  arotation_matching ; } ;		

		double GetStrength() { return strength ; } 
		double GetDeltaStrength() { return (strength_perturbation_factor - strength_perturbation_MEAN) ; } 
		double GetDeltaRotation() { return (rotation_perturbation - rotation_perturbation_MEAN) + 0  ; } 
		double getStrengthPerturbationFactor() { return strength_perturbation_factor ; } 
		double getRotationPerturbation() { return rotation_perturbation ; } 
		double getRotationMatching() { return rotation_matching ; } ;
		double getTotalRotation() { return (rotation_perturbation + rotation_matching) ; } ;
		double getTotalStrength() { return (strength_matching_factor + strength_perturbation_factor - strength_perturbation_MEAN) ; } ;		

		double getXPerturbation() { return x_perturbation ; } ; 
		double getYPerturbation() { return y_perturbation ; } ; 

		
}; 


TMagnet::TMagnet(string aname, TProjectParameters *aProjectParameters, 
	double astrength_perturbation_RMS, double astrength_perturbation_MEAN, double astrength_perturbation_factor,
	double arotation_perturbation_RMS, double arotation_perturbation_MEAN, double arotation_perturbation,
	double ax_perturbation_RMS, double ax_perturbation_MEAN, double ax_perturbation,
	double ay_perturbation_RMS, double ay_perturbation_MEAN, double ay_perturbation,
	bool ain_a_matched_beam, double aposition, string atype,
	int aTOTEM_sign_conversion_2011_08_03_with_Huberts_rule, int apolarity, string anominal_strength_text) : TBeamElement(aname, aposition, 0)
{

	position = aposition ; 
	type = "Magnet" ;
	magnet_type = atype ;
	ProjectParameters = aProjectParameters ;

	strength = 0 ;
	strength_is_valid = false ;
	Optical_functions_before_matching_present = false ;	
	strength_matching_factor = 0 ;
	rotation_matching = 0 ;

	TOTEM_sign_conversion_2011_08_03_with_Huberts_rule = aTOTEM_sign_conversion_2011_08_03_with_Huberts_rule ; 
	polarity = apolarity ;
	nominal_strength_text = anominal_strength_text ;
	in_a_matched_beam = ain_a_matched_beam ;

	strength_perturbation_factor = astrength_perturbation_factor ;
	strength_perturbation_RMS = astrength_perturbation_RMS ;
	strength_perturbation_MEAN = astrength_perturbation_MEAN ;

	rotation_perturbation = arotation_perturbation ;
	rotation_perturbation_RMS = arotation_perturbation_RMS ;
	rotation_perturbation_MEAN = arotation_perturbation_MEAN ;

    x_perturbation = ax_perturbation ;
    x_perturbation_RMS = ax_perturbation_RMS ;
    x_perturbation_MEAN = ax_perturbation_MEAN ;

    y_perturbation = ay_perturbation ;
    y_perturbation_RMS = ay_perturbation_RMS ;
    y_perturbation_MEAN = ay_perturbation_MEAN ;

	matched_strength = false ;
	matched_rotation = false ;

	if(ain_a_matched_beam)
	{
	        map <int, TStructForMatching > *matched_magnet_params = ProjectParameters->GetMatchedParameters() ;
        	map <int, TStructForMatching >::iterator it2;
		for(it2 = matched_magnet_params->begin() ; it2 != matched_magnet_params->end() ; ++it2)
		{
			if(it2->second.name.compare(name) == 0)
			{
				if(it2->second.type.compare("Rotation") == 0) matched_rotation = true ; 
				if(it2->second.type.compare("Strength") == 0) matched_strength = true ; 
			}
		}

	}
}


int TMagnet::Print(int depth)
{
        string prefix=Prefix(depth) ;

        cout << endl << prefix << "TMagnet Print ----------" << endl << endl ;
        cout << prefix << "Name: " << this->GetName() << endl ;
        if(matched_strength) 	cout << prefix << "Matched strength: " 		<< matched_strength << endl ;
        if(matched_rotation) 	cout << prefix << "Matched rotation: "		<< matched_rotation << endl ;

	TBeamElement::Print(depth+1) ;

        return 0 ;
}

int TMagnet::Save(TDirectory *dir, TFile *rootfile, OpticsArgumentType *OpticsArgument)
{

        TOpticsObject::Save(dir,rootfile,OpticsArgument) ;
	
	map<string, TOpticsObject *>::iterator Child_iterator ;

	for ( Child_iterator = Children.begin() ; Child_iterator != Children.end() ; Child_iterator++ ) 
	{
		if((Child_iterator->second)->GetName() == "Optical functions before matching") Optical_functions_before_matching_present = true ;
	}
	
	
        Properties = new TTree("Properties","This branch describes the properties of the magnet");

        Properties->Branch("Polarity",          &polarity,                                              "Polarity/Int_t");
        Properties->Branch("Sign_conversion",   &TOTEM_sign_conversion_2011_08_03_with_Huberts_rule,    "Sign_conversion/Int_t");
        Properties->Branch("Position",          &position,                                              "Position/D");
        Properties->Branch("Strength_valid",    &strength_is_valid,                                     "Strength_Valid/O");
        Properties->Branch("Strength",           &strength,                                             "Strength/D");
        Properties->Branch("Optical_functions_before_matching_present",           &Optical_functions_before_matching_present,            "Optical_functions_before_matching_present/O");	

	bool perturbed_strength = (strength_perturbation_factor != DEFAULT_STRENGTH_PERTURBATION_FACTOR) ;
	bool perturbed_rotation = (rotation_perturbation != DEFAULT_ROTATION_PERTURBATION) ;

        Properties->Branch("Perturbed_strength",	&perturbed_strength, "Perturbed_strength/O");
        Properties->Branch("Perturbed_rotation",    	&perturbed_rotation, "Perturbed_rotation/O");   
        Properties->Branch("InAMatchedBeam",    	&in_a_matched_beam,  "InAMatchedBeam/O");   	
	
        if(perturbed_strength) 
	{	
		Properties->Branch("Strength_perturbation_RMS", &strength_perturbation_RMS, "Strength_perturbation_RMS/D");
		Properties->Branch("Strength_perturbation_MEAN", &strength_perturbation_MEAN, "Strength_perturbation_MEAN/D");
		Properties->Branch("Strength_perturbation_factor", &strength_perturbation_factor, "Strength_perturbation_factor/D");
	}
	
        if(perturbed_rotation) 
	{
		Properties->Branch("Rotation_perturbation_factor_RMS", &rotation_perturbation_RMS, "Rotation_perturbation_factor_RMS/D");
		Properties->Branch("Rotation_perturbation_factor_MEAN", &rotation_perturbation_MEAN, "Rotation_perturbation_factor_MEAN/D");
		Properties->Branch("Rotation_perturbation_factor", &rotation_perturbation, "Rotation_perturbation_factor/D");
	}
	
	if(in_a_matched_beam)
	{
	        Properties->Branch("Strength_matched_?", 	&matched_strength, 			"Strength_matched/O");
        	Properties->Branch("Rotation_matched_?",	&matched_rotation, 			"Rotation_matched/O");
	        Properties->Branch("StrengthMatchingFactor",    &strength_matching_factor,		"StrengthMatchingFactor/D");		
	        Properties->Branch("RotationMatching",    	&rotation_matching,			"RotationMatching/D");				
	}

        Properties->Fill() ;

        return 0 ;
 
}

int TMagnet::LoadFromROOT(string path, TFile *f,OpticsArgumentType *OpticsArgument)
{

	insert(new TOpticalFunctions("Optical functions")) ;

	string auxpath = path + "/" + name + "/Properties" ; 

        Properties = (TTree *)f->Get((auxpath).c_str());

        Properties->SetBranchAddress("Polarity",          &polarity);
        Properties->SetBranchAddress("Sign_conversion",   &TOTEM_sign_conversion_2011_08_03_with_Huberts_rule);
        Properties->SetBranchAddress("Position",          &position);
        Properties->SetBranchAddress("Strength_valid",    &strength_is_valid);
        Properties->SetBranchAddress("Strength",           &strength);
        Properties->SetBranchAddress("Optical_functions_before_matching_present", &Optical_functions_before_matching_present);	
		
	bool perturbed_strength ;
	bool perturbed_rotation	;
		
        Properties->SetBranchAddress("Perturbed_strength",	&perturbed_strength);
        Properties->SetBranchAddress("Perturbed_rotation",    	&perturbed_rotation);
        Properties->SetBranchAddress("InAMatchedBeam",    	&in_a_matched_beam);
	
        Properties->GetEntry(0) ;
		
        if(perturbed_strength) 
	{	
		Properties->SetBranchAddress("Strength_perturbation_RMS", &strength_perturbation_RMS);
		Properties->SetBranchAddress("Strength_perturbation_MEAN", &strength_perturbation_MEAN);
		Properties->SetBranchAddress("Strength_perturbation_factor", &strength_perturbation_factor);
	}
	
        if(perturbed_rotation) 
	{
		Properties->SetBranchAddress("Rotation_perturbation_factor_RMS", &rotation_perturbation_RMS);
		Properties->SetBranchAddress("Rotation_perturbation_factor_MEAN", &rotation_perturbation_MEAN);
		Properties->SetBranchAddress("Rotation_perturbation_factor", &rotation_perturbation);
	}
	
	if(in_a_matched_beam)
	{
	        Properties->SetBranchAddress("Strength_matched_?", 	&matched_strength);
        	Properties->SetBranchAddress("Rotation_matched_?",	&matched_rotation);
	        Properties->SetBranchAddress("StrengthMatchingFactor",  &strength_matching_factor);
	        Properties->SetBranchAddress("RotationMatching",    	&rotation_matching);
	}

        Properties->GetEntry(0) ;
	

	if(Optical_functions_before_matching_present) insert(new TOpticalFunctions("Optical functions before matching")) ;
	
	TOpticsObject::LoadFromROOT(path,f) ;
	
	return 0 ;
}

int TMagnet::GenerateMADXCode(ofstream &madxfile, string dir)
{

	bool LSA = (ProjectParameters->getLSAFile().compare("") != 0) ;

	if(matched_strength || (strength_perturbation_factor != DEFAULT_STRENGTH_PERTURBATION_FACTOR) || LSA)	
	{
		string match_text = "" ;
		if(in_a_matched_beam && matched_strength) match_text = "D" + name ;
		else match_text = "0" ;

	        string perturbation_text = "" ;
	        if(strength_perturbation_factor != DEFAULT_STRENGTH_PERTURBATION_FACTOR) 
		{
			perturbation_text = "PERTSTR" + name ;		
			
//			stringstream ss ;
//			ss << strength_perturbation_factor ;
//			perturbation_text = ss.str() ;

		}
		else perturbation_text = "1.0" ;

		string strength_text ;
	
		if(LSA) 
		{
			stringstream sign ;
        		sign << TOTEM_sign_conversion_2011_08_03_with_Huberts_rule ;
		
		        string abs_text_left = "" ; // For bipolar magnets one has to take the absolute value of the strength.
	        	string abs_text_right = "" ;  

		        if((magnet_type.compare("quadrupole")==0) || (magnet_type.compare("norma_separation_dipole") == 0) || (magnet_type.compare("supra_separation_dipole") == 0) || (magnet_type.compare("triplet") == 0))
        		{
                		abs_text_left = "(" ;        
	                	abs_text_right = ")" ; 
		        }

			strength_text =  "(" + abs_text_left + "LSA." + name + abs_text_right + "*(" + perturbation_text + " + " + match_text + ")) * (" + sign.str() + ")" ; // put it together
		}
		else strength_text =  "(" + nominal_strength_text + ")"  + perturbation_text + match_text ; // nominal 

		stringstream pol ;
		pol << polarity ;
		string pol_text = ", polarity=" + pol.str() + ";\n"  ;

		if((magnet_type.compare("hkicker") == 0)  || (magnet_type.compare("vkicker") == 0)) 	madxfile << name << ", KICK := " << strength_text << pol_text ;
		if((magnet_type.compare("quadrupole") == 0) || (magnet_type.compare("triplet") == 0))	madxfile << name << ", K1 := " << strength_text << pol_text ;
		if(magnet_type.compare("inner_corrector") == 0) 					madxfile << name << ", K1S := " << strength_text << pol_text ;

		if((magnet_type.compare("norma_separation_dipole") == 0) || (magnet_type.compare("supra_separation_dipole") == 0))
		{

			string strength_text2 ;

			if(LSA)
			{
	                        size_t pos = name.find(".") ;
				strength_text2 = "( " + strength_text + " * l." + name.substr(0,pos) + " )" ;
			}
			else
			{
                                strength_text2 = strength_text ;
                                replace(strength_text2.begin(), strength_text2.end(), 'a', 'k') ;
                        }


			madxfile << name << ", K0 := (" << strength_text << ") , ANGLE := " << strength_text2 << pol_text ;
		}

		if(magnet_type.compare("corrector")==0) 
		{

			if((name.compare("MCSX.3R5") == 0)  || (name.compare("MCSX.3L5") == 0)) 	madxfile << name << ", KNL := {0, 0, " << strength_text << "*l.MCSX, 0, 0, 0}" << pol_text ;
			if((name.compare("MCTX.3R5") == 0)  || (name.compare("MCTX.3L5") == 0)) 	madxfile << name << ", KNL := {0, 0, 0, 0, 0, " << strength_text << "*l.MCTX}" << pol_text ;
			if((name.compare("MCOSX.3R5") == 0) || (name.compare("MCOSX.3L5") == 0))	madxfile << name << ", KSL := {0, 0, 0, " << strength_text << "*l.MCOSX}" << pol_text ;
			if((name.compare("MCOX.3R5") == 0) || (name.compare("MCOX.3L5") == 0))		madxfile << name << ", KNL := {0, 0, 0, " << strength_text << "*l.MCOX}" << pol_text ;
			if((name.compare("MCSSX.3R5") == 0) || (name.compare("MCSSX.3L5") == 0))	madxfile << name << ", KSL := {0, 0, " << strength_text << "*l.MCSSX, 0}" << pol_text ;
		}
	}


	if((matched_rotation || (rotation_perturbation != DEFAULT_ROTATION_PERTURBATION)) || (x_perturbation != DEFAULT_X_PERTURBATION) || (y_perturbation != DEFAULT_Y_PERTURBATION))
	{
		madxfile << endl ; 
		madxfile << "select, flag=error, clear;" << endl ;
		madxfile << "select,flag=error, pattern=" << name << ";" << endl ;
		
        	madxfile << "ealign" ;
		
		if(matched_rotation || (rotation_perturbation != DEFAULT_ROTATION_PERTURBATION))
		{

                	string match_text = "" ;
                	if(in_a_matched_beam && matched_rotation) 
			{
				match_text = " + DPSI" + name ;
			}

			string perturbation_text = "PERTDPSI" + name ;

			madxfile << ", dpsi= " + perturbation_text + match_text << "  " ;
		}

		if(x_perturbation != DEFAULT_X_PERTURBATION)
    		{
        		madxfile << ", dx= PERT_X_" << name << "  " ;

    		}

		if(y_perturbation != DEFAULT_Y_PERTURBATION)
    		{

        		madxfile << ", dy= PERT_Y_" << name << "  " ;
    		}
		
        	madxfile << " ;" << endl ;
	}
	
	
	return 0 ;
}

#endif
