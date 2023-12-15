#ifndef TLHC_H
#define TLHC_H

#include <include/TBeam.h>


class TLHC : public TOpticsObject
{
	public :
		TLHC(string aname,TProjectParameters *, int, bool, bool, bool) ; 
		int Print(int);
		int Save(TDirectory *, TFile *, OpticsArgumentType *) ;
		int RetrieveAllOpticalFunctionName(map <string, double> &);
		int Execute(string directory) { TOpticsObject::Execute(directory) ; return 0 ; } ;
}; 



TLHC::TLHC(string aname, TProjectParameters *ProjectParameters, int Job_number, bool perturbed, bool matched, bool postponed) : TOpticsObject(aname)
{
	// A seed number 0 forces TRandom to generate a random seed. Since the smallest Job_number is 0, Job_number + 1 is smallest seed. In addition 
        // different seed is generated for the two beams. 

	int beam_1_argument = 0 ; 
	int beam_4_argument = 0 ;

	if(ProjectParameters->getProjectType() == Perturbations)
	{
        	beam_1_argument = Job_number ;
        	beam_4_argument = Job_number ;
	}
	else if((ProjectParameters->getProjectType() == Matching_1N) || (ProjectParameters->getProjectType() == Matching_N1))
	{
	        beam_1_argument = 2 * Job_number + 1 ;
	        beam_4_argument = 2 * Job_number + 2 ;
	}

	TBeam *beam_1 = new TBeam("Beam_1", ProjectParameters, Job_number, 1, beam_1_argument, perturbed, matched, postponed) ;
	insert(beam_1) ;
	insert(new TBeam("Beam_4", ProjectParameters, Job_number, 4, beam_4_argument, perturbed, matched, postponed, beam_1)) ;
}


int TLHC::Print(int depth)
{
        string prefix=Prefix(depth) ;

        cout << endl << prefix << "TLHC Print ----------" << endl << endl ;
        cout << prefix << "Name: " << this->GetName() << endl ;

	TOpticsObject::Print(depth) ;

        return 0 ;
}

int TLHC::Save(TDirectory *dir, TFile *rootfile, OpticsArgumentType *OpticsArgument)
{
	TOpticsObject::Save(dir,rootfile,OpticsArgument) ;

	return 0 ; 
}

int TLHC::RetrieveAllOpticalFunctionName(map <string, double> &opticalfunctions)
{

	((TBeam *)Children["Beam_1"])->RetrieveAllOpticalFunctionName(opticalfunctions); 
	((TBeam *)Children["Beam_4"])->RetrieveAllOpticalFunctionName(opticalfunctions);

	return 0 ; 
}


#endif
