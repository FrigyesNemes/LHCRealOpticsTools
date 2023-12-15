#ifndef TOPTICSOBJECT_H
#define TOPTICSOBJECT_H

#include <string>
#include <map>

#include <sys/stat.h>

using namespace std;

#include <TFile.h>
#include <TDirectory.h>

typedef set <string> OpticsArgumentType ; 

class TOpticsObject
{


	protected:

		string name;
		string type ;

		TOpticsObject() {}  ;
		TOpticsObject(string, string="") ;
		TDirectory *Directory ;
		TTree *Properties ;
		map<string, TOpticsObject *> Children ;

	public:
		void SetName(string aname) { name=aname ; } ;
		string GetName() { return name ; } ;
		string GetType() { return type ; } ;

		virtual int Save(TDirectory *, TFile *, OpticsArgumentType *) ;
		virtual int LoadFromROOT(string, TFile *, OpticsArgumentType *) ;
		virtual int Print(int) = 0 ; 
		virtual int Execute(string) = 0 ;
		virtual int LoadTFSResults(string) ;
		map<string, TOpticsObject *> getChildren() { return Children ; }
		TOpticsObject &operator [](string key) { return *Children[key] ; } 
		string SetupOptics() ;
 
		string Prefix(int length) { string prefix = "" ; for(int i = 0 ; i < length ; ++i ) prefix += "    " ; return prefix ;} ;

		virtual int insert(TOpticsObject *opticsObject) 
		{
			Children.insert(pair<string, TOpticsObject *>(opticsObject->GetName() , opticsObject )) ;

			return 0 ;
		}

		size_t getNumberOfChildren()
		{
			return Children.size() ;
		}
		
		TOpticsObject *cd(string, int) ;

} ; 

TOpticsObject::TOpticsObject(string aname, string atype)
{	
	name = aname ;
	type = atype ;
}	

string TOpticsObject::SetupOptics()
{	
	string valami = "Start" ;
	
	return valami ;
	
}	

TOpticsObject *TOpticsObject::cd(string apath, int mode)
{
	TOpticsObject *act = NULL ;
	char path[400] ;
	char delimiters[] = "/" ;	
  	char * pch;

	act = this ;
	strcpy(path,apath.c_str()) ;
  	pch = strtok (path,delimiters);
	
	while (pch != NULL)
	{
		act = act->getChildren()[pch] ;
		pch = strtok (NULL, delimiters);
	}
	
	return act ;
}

int TOpticsObject::Save(TDirectory *dir, TFile *rootfile, OpticsArgumentType *OpticsArgument=NULL)
{
	Directory = dir->mkdir(this->GetName().c_str()) ;
        Directory->cd() ;	

        map<string, TOpticsObject *>::iterator Child_iterator ;
        for ( Child_iterator = Children.begin() ; Child_iterator != Children.end() ; Child_iterator++ ) 
	{
		if(OpticsArgument != NULL)
		{
			
			if(OpticsArgument->find(Child_iterator->second->GetName()) != OpticsArgument->end()) 
			{
				Child_iterator->second->Save(Directory,rootfile) ;
			}		
		}
		else
			Child_iterator->second->Save(Directory,rootfile) ;
	}

        Directory->cd() ;	

	return 0 ;	
}

int TOpticsObject::LoadFromROOT(string path, TFile *f, OpticsArgumentType *OpticsArgument=NULL)
{
	path += "/" + name ;

        map<string, TOpticsObject *>::iterator Child_iterator ;
        for ( Child_iterator = Children.begin() ; Child_iterator != Children.end() ; Child_iterator++ ) 
	{	
		
		if(OpticsArgument != NULL)
		{
			
			if(OpticsArgument->find(Child_iterator->second->GetName()) != OpticsArgument->end()) 
			{
				Child_iterator->second->LoadFromROOT(path,f) ;
			}		
		}
		else
			Child_iterator->second->LoadFromROOT(path,f) ;
	
	}
	
        return 0 ;
}

int TOpticsObject::Print(int depth)
{
        map<string, TOpticsObject *>::iterator Child_iterator ;
        for ( Child_iterator = Children.begin() ; Child_iterator != Children.end() ; Child_iterator++ ) Child_iterator->second->Print(depth+1) ;
	
	return 0 ;
}


int TOpticsObject::Execute(string directory)
{
	
	string my_dir ;
	if((type.compare("Magnet") != 0) && (type.compare("Marker") != 0))
	{
		my_dir = directory + "/" + this->GetName() ;
		mkdir(my_dir.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}
	else 
	{
		my_dir = directory ;
	}

        map<string, TOpticsObject *>::iterator Child_iterator ;
        for ( Child_iterator = Children.begin() ; Child_iterator != Children.end() ; Child_iterator++ ) Child_iterator->second->Execute(my_dir) ;

        return 0 ;
}

int TOpticsObject::LoadTFSResults(string directory)
{

        string my_dir ;
        if((type.compare("Magnet") != 0) && (type.compare("Marker") != 0))
        {
                my_dir = directory + "/" + this->GetName() ;
        }
        else
        {
                my_dir = directory ;
        }
	
        map<string, TOpticsObject *>::iterator Child_iterator ;
        for ( Child_iterator = Children.begin() ; Child_iterator != Children.end() ; Child_iterator++ ) Child_iterator->second->LoadTFSResults(my_dir) ;

        return 0 ;
}

#endif
