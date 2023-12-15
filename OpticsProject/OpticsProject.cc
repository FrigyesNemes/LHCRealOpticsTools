#include <iostream> 
#include <complex>

#include <include/TProject.h>
#include <include/TProjectParameters.h>

int main(int argc, char *argv[])
{

        if(argc != 2)
        {
                cout << "Usage: OpticsProject project_file_name" << endl ;
                exit(ERR_INCORRECT_PARAMETERS) ;
        }

        string project_filename(argv[1]) ;

	TProject *Project = new TProject("Project", project_filename) ;


	Project->Run() ;


	//Project->Print(0) ;  
	//Project->LoadFromROOT("",NULL) ;
	//Project->GetProjectParameters()->setROOTfilename("valami.root") ; 

	return 0 ; 
}
