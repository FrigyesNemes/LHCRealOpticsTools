#ifndef TMATCHING_H
#define TMATCHING_H

#include <map>
#include <sstream>
#include <iostream>

#include <TTree.h>
#include <include/TJob.h>

class TMatching : public TOpticsObject
{
	private:
		string WorkingDirectory ;
		TProjectParameters *ProjectParameters ;

	public:
		TMatching(string, TProjectParameters *) ;
		~TMatching() ;

		int Print(int) ;
		int Save(TDirectory *, TFile *,OpticsArgumentType *) ;
		int Execute(string) ; 

}; 

TMatching::TMatching(string aname, TProjectParameters *aProjectParameters) : TOpticsObject(aname)
{
	ProjectParameters = aProjectParameters ;

	TJob *nominal = new TJob("Anominal", ProjectParameters,-1) ; // The name of the nominal jobs begins with "A" to advance the jobs with name "Job..." by abc ordering of the map. Then it is the first in the execution order.
	insert(nominal) ;
	
	for(int i = 1 ; i <= ProjectParameters->getNumberOfJobs() ; ++i) 
	{
		stringstream ss ; 

		ss << i ; 

		insert(new TJob("Job"+ ss.str(), ProjectParameters,i)) ;
	}
}

TMatching::~TMatching()
{
}

int TMatching::Print(int depth)
{

	string prefix = Prefix(depth) ;

        cout << endl << prefix << "TMatching Print ----------" << endl << endl ;
        cout << prefix << "Name: " << this->GetName() << endl ;
        cout << prefix << "Working directory: " << WorkingDirectory ;

	TOpticsObject::Print(depth+1) ;

	return 0 ; 
}

int TMatching::Save(TDirectory *dir, TFile *rootfile, OpticsArgumentType *OpticsArgument)
{
	TOpticsObject::Save(dir, rootfile) ;

	Int_t Number_Of_Jobs = ProjectParameters->getNumberOfJobs() ;

        Properties = new TTree("Properties","This branch describes the properties of the matching");
        Properties->Branch("Number_of_jobs",&Number_Of_Jobs,"Number_of_jobs/I");

        Properties->Fill() ;

	return 0 ;
}

int TMatching::Execute(string dir)
{
	TOpticsObject::Execute(dir) ;
	
	string command = "find " + dir + " -name \\*.sh -exec bsub -o {}out -q " + ProjectParameters->getlxbatch_queue() + " {} \\;" ;

	system(("find " + dir + " -name \\*.sh -exec chmod +x {} \\;").c_str() ) ;
	if(!ProjectParameters->doNotSubmit())
	{
		cout << "I am here"  << command << endl ;
		system(command.c_str() ) ;
	}

	return 0 ;
}


#endif
