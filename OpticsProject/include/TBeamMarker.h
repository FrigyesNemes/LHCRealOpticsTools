#ifndef TBEAMMARKER_H
#define TBEAMMARKER_H

#include <include/TBeamElement.h>

class TBeamMarker : public TBeamElement
{
	TProjectParameters *ProjectParameters ;

	public :
		TBeamMarker(string aname, double = 0, double = 0, TProjectParameters *ProjectParameters=NULL) ; 
		int Print(int);
		int Save(TDirectory *, TFile *,OpticsArgumentType *) ;
               int LoadFromROOT(string, TFile *, OpticsArgumentType * = NULL) ;
}; 



TBeamMarker::TBeamMarker(string aname, double aposition, double apositionerror, TProjectParameters *aProjectParameters) : TBeamElement(aname, aposition, apositionerror)
{
	type = "Marker" ;
	ProjectParameters = aProjectParameters ;
}


int TBeamMarker::Print(int depth)
{
        string prefix=Prefix(depth) ;

        cout << endl << prefix << "TMarker Print ----------" << endl << endl ;
        cout << prefix << "Name: " << this->GetName() << endl ;

	TOpticsObject::Print(depth) ;

        return 0 ;
}

int TBeamMarker::Save(TDirectory *dir, TFile *rootfile,OpticsArgumentType *OpticsArgument)
{
	TOpticsObject::Save(dir,rootfile) ;
	
	map<string, TOpticsObject *>::iterator Child_iterator ;

	for ( Child_iterator = Children.begin() ; Child_iterator != Children.end() ; Child_iterator++ ) 
	{
		if((Child_iterator->second)->GetName() == "Optical functions before matching") Optical_functions_before_matching_present = true ;
	}
	
        Properties = new TTree("Properties","This branch describes the properties of the magnet");
        Properties->Branch("Optical_functions_before_matching_present",           &Optical_functions_before_matching_present,            "Optical_functions_before_matching_present/O");		
	
        Properties->Fill() ;

	return 0 ; 
}

int TBeamMarker::LoadFromROOT(string path, TFile *f,OpticsArgumentType *OpticsArgument)
{

        insert(new TOpticalFunctions("Optical functions")) ;
	
	string auxpath = path + "/" + name + "/Properties" ; 

        Properties = (TTree *)f->Get((auxpath).c_str());
        Properties->SetBranchAddress("Optical_functions_before_matching_present", &Optical_functions_before_matching_present);		
        Properties->GetEntry(0) ;
	
	if(Optical_functions_before_matching_present) 	insert(new TOpticalFunctions("Optical functions before matching")) ;	
	
        TOpticsObject::LoadFromROOT(path,f) ;	

        return 0 ;
}


#endif
