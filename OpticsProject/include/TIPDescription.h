#ifndef TIPDESCRIPTION_H
#define TIPDESCRIPTION_H

#include <include/TBeamElement.h>

class TIPDescription : public TBeamElement
{
	TProjectParameters *ProjectParameters ;
	public :

	TIPDescription(string aname, double= 0, double = 0,TProjectParameters * = NULL) ; 
	int Print(int);
	int Save(TDirectory *, TFile *,OpticsArgumentType *) ;
}; 



TIPDescription::TIPDescription(string aname, double aposition, double apositionerror,TProjectParameters *aProjectParameters) : TBeamElement(aname, aposition, apositionerror)
{
	ProjectParameters = aProjectParameters ;
}


int TIPDescription::Print(int depth)
{
        string prefix=Prefix(depth) ;

        cout << endl << prefix << "TIPDescription Print ----------" << endl << endl ;
        cout << prefix << "Name: " << this->GetName() << endl ;

	TOpticsObject::Print(depth) ;

        return 0 ;
}

int TIPDescription::Save(TDirectory *dir, TFile *rootfile, OpticsArgumentType *OpticsArgument)
{
	TOpticsObject::Save(dir, rootfile, OpticsArgument) ;

	return 0 ; 
}

#endif
