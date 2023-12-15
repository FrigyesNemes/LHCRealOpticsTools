#ifndef TROMANPOT_H
#define TROMANPOT_H

#include <include/TBeamElement.h>

class TRomanPot : public TBeamElement
{
	TProjectParameters *ProjectParameters ;

	public :
		TRomanPot(string aname,TProjectParameters *) ; 
		int Print(int);
		int Save(TDirectory *, TFile *, OpticsArgumentType *) ;
		int Execute(string) ;
}; 



TRomanPot::TRomanPot(string aname, TProjectParameters *aProjectParameters) : TBeamElement(aname)
{
	ProjectParameters = aProjectParameters ;
}


int TRomanPot::Print(int depth)
{
        string prefix=Prefix(depth) ;

        cout << endl << prefix << "TRomanPot Print ----------" << endl << endl ;
        cout << prefix << "Name: " << this->GetName() << endl ;

	TOpticsObject::Print(depth) ;

        return 0 ;
}

int TRomanPot::Execute(string dir)
{
	TOpticsObject::Execute(dir) ;

	return 0 ;
}


int TRomanPot::Save(TDirectory *dir, TFile *rootfile,OpticsArgumentType *OpticsArgument)
{
	TOpticsObject::Save(dir,rootfile,OpticsArgument) ;

	return 0 ; 
}

#endif
