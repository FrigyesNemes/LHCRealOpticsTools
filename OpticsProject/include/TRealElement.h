#ifndef TREALELEMENT_H
#define TREALELEMENT_H

#include <iostream>
#include <fstream>
using namespace std;

#include <include/TOpticsObject.h>
#include <include/TRealSettings.h>

class TRealElement : public TOpticsObject
{
	private:
                double position, position_error ;
	public:

		TRealElement(string,double=0,double=0) ;
		int Print(int) ;
		virtual int PerturbWithGaussian() ;
		virtual int PerturbWithGivenValue() ;
		virtual int Import() ;
		virtual int GenerateMADXCode(ofstream &,string);
		int Execute(string directory) { TOpticsObject::Execute(directory) ; return 0 ; } ;
                double getPosition() { return position ;} ; 

        virtual int insert(TOpticsObject *opticsObject)
        {

                stringstream ss ;
                ss << setprecision(3) << fixed << setw(10) << setfill (' ') << ((TRealElement *)opticsObject)->getPosition() ;

		string key = ss.str() + " " + opticsObject->GetName() ;

                Children.insert(pair<string, TOpticsObject *>(key, opticsObject)) ;

                return 0 ;
        }
}; 

TRealElement::TRealElement(string aname, double aposition, double aposition_error) : TOpticsObject(aname)
{
	position = aposition ;
	position_error = aposition_error ;
}

int TRealElement::Print(int depth)
{
	TOpticsObject::Print(depth) ;
        return 0 ;

//	RealSettings.Print(depth+1) ;
}

int TRealElement::PerturbWithGaussian()
{
	return 0 ;  
}

int TRealElement::PerturbWithGivenValue()
{
	return 0 ;
}

int TRealElement::Import()
{
	return 0 ;
}

int TRealElement::GenerateMADXCode(ofstream &madxfile, string dir)
{
        map<string, TOpticsObject *>::iterator Child_iterator ;
        for ( Child_iterator = Children.begin() ; Child_iterator != Children.end() ; Child_iterator++ ) ((TRealElement *)Child_iterator->second)->GenerateMADXCode(madxfile,dir) ;

	return 0 ;
}

#endif
