#ifndef TBEAMPOSITIONMONITOR_H
#define TBEAMPOSITIONMONITOR_H

#include <include/TBeamElement.h>

class TBeamPositionMonitor : public TBeamElement
{
	private :
		TProjectParameters *ProjectParameters ;

	public :
		TBeamPositionMonitor(string aname,double=0,double=0,TProjectParameters * = NULL) ; 
		int Print(int);
		int Save(TDirectory *, TFile *, OpticsArgumentType *) ;
}; 



TBeamPositionMonitor::TBeamPositionMonitor(string aname, double aposition, double apositionerror,
	TProjectParameters *aProjectParameters) : TBeamElement(aname, aposition, apositionerror)
{
	ProjectParameters = aProjectParameters ;
}


int TBeamPositionMonitor::Print(int depth)
{
        string prefix=Prefix(depth) ;

        cout << endl << prefix << "TBeamPositionMonitor Print ----------" << endl << endl ;
        cout << prefix << "Name: " << this->GetName() << endl ;

	TOpticsObject::Print(depth) ;

        return 0 ;
}

int TBeamPositionMonitor::Save(TDirectory *dir, TFile *rootfile, OpticsArgumentType *OpticsArgument)
{
	TOpticsObject::Save(dir,rootfile,OpticsArgument) ;

	return 0 ; 
}

#endif
