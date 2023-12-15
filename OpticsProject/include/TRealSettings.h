#ifndef TREALSETTINGS_H
#define TREALSETTINGS_H

#include <include/TOpticsObject.h>

class TRealSettings : public TOpticsObject
{
	protected:
		TRealSettings(string aname) : TOpticsObject(aname) {} ;
		int Print(int depth) { depth++ ; return 0 ; } ;
		int Execute(string dir) { TOpticsObject::Execute(dir) ; return 0 ; } ;
}; 
#endif
