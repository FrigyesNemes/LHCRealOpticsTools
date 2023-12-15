#ifndef TJOB_H
#define TJOB_H

#include <stdlib.h>
#include <include/TMADX.h>

const int NOMINAL_JOB_NUMBER = -1 ; 

class TMagnet ;

class TJob : public TOpticsObject
{
	private:
		string WorkingDirectory ;

	public:
		TJob(string, TProjectParameters *, int) ;
		int Save(TDirectory *,TFile *, OpticsArgumentType *) ;
		int Print(int) ;
		int Execute(string) ;
		double GetStrength(string) ;
	
}; 

TJob::TJob(string aname, TProjectParameters *ProjectParameters,int Job_number) : TOpticsObject(aname)
{

	type = "Job" ;

	if(Job_number == NOMINAL_JOB_NUMBER)
	{
		insert(new TMADX("MAD_X" , ProjectParameters,Job_number, false, false, false) ) ;
	}
        else if(ProjectParameters->getProjectType() == Perturbations)
        {
                insert(new TMADX("MAD_X" , ProjectParameters,Job_number, true, false, false) ) ;
        }
	else if(ProjectParameters->getProjectType() == Matching_1N) 	
	{
		TMADX *target = new TMADX("Target" , ProjectParameters , Job_number, true, false, true ) ;

		insert(target) ;
	 	insert(new TMADX("Missile", ProjectParameters, Job_number, false , true , true, target )) ;
	}
	else if(ProjectParameters->getProjectType() == Matching_N1)  
	{
                TMADX *target = new TMADX("Target" , ProjectParameters , Job_number, false, false, true) ;

                insert(target) ;
                insert( new TMADX("Missile", ProjectParameters, Job_number, true , true , true, target )) ;
	}
	else
	{
		cout << "[TJob] error: unknown project type: " << ProjectParameters->getProjectType() << endl ;
		exit(1) ;
	}


}

double TJob::GetStrength(string magnet_name)
{
	TMagnet *magnet = (TMagnet *)(Children["MAD_X"]->getChildren()["myLHC"]->getChildren()["Beam_1"]->getChildren()[magnet_name]) ;

	return magnet->GetStrength() ;
}


int TJob::Print(int depth)
{	

	TOpticsObject::Print(depth);

	string prefix=Prefix(depth) ;

	cout << endl << prefix << "TJob Print ----------" << endl << endl ;
	cout << prefix << "Name: " << this->GetName() << endl ;
	cout << prefix << "Working directory: " << WorkingDirectory ; 
	
	return 0 ; 
}

int TJob::Save(TDirectory *dir, TFile *rootfile, OpticsArgumentType *OpticsArgument)
{

	TOpticsObject::Save(dir,rootfile,OpticsArgument) ; 

	rootfile->Flush() ;

	return 0 ; 
}

int TJob::Execute(string dir)
{
        TOpticsObject::Execute(dir) ;

        return 0 ;
}

#endif
