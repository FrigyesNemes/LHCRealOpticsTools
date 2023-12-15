#ifndef TBEAMELEMENT_H
#define TBEAMELEMENT_H

#include <include/TRealElement.h>
#include <include/TProjectParameters.h>

class TBeamElement : public TRealElement
{

	protected:
		double position, position_error ; 
		bool Optical_functions_before_matching_present ;				

	public:
		TBeamElement(string, double=0, double=0) ;
		int Execute(string) ;
		int Print(int) ; 
		int LoadTFS() ;
		int RetrieveOpticalFunctions(map <string, double> &);
}; 

TBeamElement::TBeamElement(string aname, double aposition, double aposition_error) : TRealElement(aname, aposition, aposition_error)
{
	type = "Beam element" ;
}

int TBeamElement::Execute(string directory)
{
	TOpticsObject::Execute(directory) ;

	return 0 ;
}

int TBeamElement::Print(int depth)
{
	string prefix = Prefix(depth) ;

        cout << endl << prefix << "TBeamElement Print ----------" << endl << endl ;
	cout << prefix << "Position: " << position << endl ;

        TOpticsObject::Print(depth) ;

        return 0 ;

}

int TBeamElement::RetrieveOpticalFunctions(map <string, double> &optical_functions)
{


        map<string, TOpticsObject *>::iterator Child_iterator ;

        for ( Child_iterator = Children.begin() ; Child_iterator != Children.end() ; Child_iterator++ )
        {
		if(Child_iterator->second->GetType().compare("Optical function") == 0) 
		{
			map <string, double> myopticalfunctions ;	
			((TBeamElement *)Child_iterator->second)->RetrieveOpticalFunctions(myopticalfunctions) ;
			
			map <string, double>::iterator it ;
			for(it = myopticalfunctions.begin() ; it != myopticalfunctions.end() ; ++it)
			{
				 optical_functions.insert(pair<string,double>("TwissParameter." + name + "." + it->first, it->second)) ;
				cout << ("TwissParameter_" + name + "_" + it->first) << endl ;
			}
		}		
        }

	return 0 ;
}

#endif
