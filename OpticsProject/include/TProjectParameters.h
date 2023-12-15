#ifndef TPROJECTPARAMETERS_H
#define TPROJECTPARAMETERS_H

#include <set>
#include <map>


#include <include/TOpticsObject.h>
#include <include/TErrors.h>
#include <include/TElasticModel.h>

class TStructForMatching
{
	public:

	string name, type ;
        double value, step, lower_limit, upper_limit ;

	bool operator==(const TStructForMatching &rhs) const
	{
		return name == rhs.name ;
	}

        bool operator<(const TStructForMatching &rhs) const
        {
                return name < rhs.name ;
        }


} ;

class TStructForPerturbation
{
        public:

        string name ;
        double MEAN, RMS ;

        bool operator=(const TStructForPerturbation &rhs) const
        { 
                return name == rhs.name ;
        }

        bool operator<(const TStructForPerturbation &rhs) const
        { 
                return name < rhs.name ;
        }


} ;

class TStructForConstraints
{
        public:

        string constraint ;
        double value, error, relative_error ;

        bool operator=(const TStructForConstraints &rhs) const
        {
                return constraint == rhs.constraint ;
        }

        bool operator<(const TStructForConstraints &rhs) const
        {
                return constraint < rhs.constraint ;
        }
} ;


class TConstraints 
{
	private:
	set <TStructForConstraints> Constraints ;

	public:

	TConstraints() {} ; 

	int addConstraint(string constraint, double value, double error, double relative_error) 
	{
		TStructForConstraints aconstraint ;

		//cout << constraint << endl ;

		aconstraint.constraint = constraint ;
		aconstraint.value = value ;
		aconstraint.error = error ;
		aconstraint.relative_error = relative_error ;

		Constraints.insert(aconstraint) ; 

		return 0 ; 
	} 
	
	int Empty() { Constraints.empty() ; return 0 ; } ;

	set <TStructForConstraints> getConstraints() { return Constraints ; } ;
	size_t getNumberOfConstraints() { return Constraints.size() ; } ;

	set <string> getVariables() 
	{
		set <string> result ;

		set <TStructForConstraints>::iterator it ;
		char delimiters[] = "()*+-/, \t" ;
		for(it = Constraints.begin() ; it != Constraints.end() ; ++it)	
		{
			// cout << *it << endl << "Variables: " ;
			char *str = (char *)malloc( sizeof(char) * (it->constraint.size() + 2) ) ;
			
			if(str == NULL)
			{
				cout << "TConstraints::getVariables malloc failed !" ;
				exit(1) ;
			}
			
			strcpy(str,it->constraint.c_str()) ;

		  	char * pch;
		  	pch = strtok (str,delimiters);
			while (pch != NULL)
			{
				//printf("%s",pch);
				if((strcmp(pch,"pow") == 0) || (strcmp(pch,"sqrt") == 0)) ; // printf("command") ;
				if((strstr(pch,"MinuitParameter") != NULL)) ; // printf("Minuit parameter") ;
				if((strstr(pch,"TwissParameter") != NULL)) result.insert(pch) ;
				if((strstr(pch,"Nominal") != NULL)) ; // printf("Nominal value") ;

				//printf("\n");

				pch = strtok (NULL, delimiters);
 			}

			free(str) ;
			// cout << endl << endl ;
			
		}

		return result ;
	}

} ;

int const Matching_1N = 1 ;
int const Matching_N1 = 2 ;
int const Perturbations = 0 ;

int const VERBOSITY_NO = 0 ;
int const VERBOSITY_ALL = 2 ;
int const VERBOSITY_FEW = 1 ;

double const  DEFAULT_STRENGTH_PERTURBATION_FACTOR = 1.0 ;
double const  DEFAULT_ROTATION_PERTURBATION = 0.0 ;
double const  DEFAULT_X_PERTURBATION = 0.0 ;
double const  DEFAULT_Y_PERTURBATION = 0.0 ;

typedef map<string, double> map_parameter_value_type ;
typedef map<string, string> map_setting_value_type ;

class TProjectParameters : public TOpticsObject
{
	private:
	
	static const unsigned int length_of_directory_name_in_root_file = 1000 ;

	typedef map <int, double> beam_emittance_type ;

	bool LSA, harmonics, alignment, debug_matching, matched_beam_momentum, constraint_smearing, do_not_submit ;
	TElasticModel *elastic_model ;

	int projectType ; 								// 0 = Perturbations, 1 = 1:N type , 2 = N:1 type 
	int NumberOfJobs ;  
	int Verbosity ;             							// 0 = no, 1 = low, 2 = all	

	double 	deltax, deltay, deltapx, deltapy, deltapt, deltadeltap, horizontal_crossing_angle ;
	double 	betastar_x ; 		         					// [m]
	double 	betastar_y ; 		         					// [m]
	double 	MINUIT_tolerance ;
	double 	energyperbeam ;		         					// [GeV]
	double 	MINUIT_precision ;
	double 	ProtonMass ; 								// GeV 
	int	MADX_perturbation_precision ;
	int 	MINUIT_call_limit ;	
	
	string ROOTfilename, ProjectDirectory, startTimeStamp, endTimeStamp, general_data_directory ;
	string MADX_executable_name, LSA_file, MADX_format_string, lxbatch_queue, field_harmonics_from_WISE ;
	string command, fileNameForSaveAs,analysis_name ;		
	
	TOpticsObject *root_object ;	
	string optionadd ;
	
	set <TStructForPerturbation> perturbed_magnets_strength, perturbed_magnets_rotation, perturbed_magnets_x , perturbed_magnets_y ;
	set <string> element_names_kept_in_TTree ;
	vector <string> *vector_field_harmonics_from_WISE_tfsnames ;
	vector <double> px_values_per_job ;
	vector <double> py_values_per_job ;

	map  <int , TStructForMatching> matched_parameters ; 
	map <string, beam_emittance_type > normalized_transverse_emittances ;		// umrad	

	map_parameter_value_type map_parameter_value ;
	map_setting_value_type map_setting_value ;

	public :

	vector <string> strength_files ; 
	string strength_dbase_dir ;
	string chromaticity_dbase_dir ;
	string correction_subdbase_dir ;
    	string absolute_harmonics_dbase_dir ;
	string correction_file, coupling_correction_file, chromaticity_correction ;
	string abs_correction_MB, abs_correction_MQ, abs_correction_MC ;

	string option2 ;
	string option ;

	TConstraints Constraints ;
	
	int InitParameters()
	{
		optionadd = "@cern.ch" ;

		ProtonMass = 0.938272046 ; // [GeV]
		
		string option2 = "em" ;	
		string option = "fn" ;
		
		deltax = deltay = deltapx = deltapy = deltapt = deltadeltap = horizontal_crossing_angle = 0 ;
		Verbosity = 0 ; 

		ROOTfilename = ProjectDirectory = startTimeStamp = endTimeStamp = general_data_directory = MADX_executable_name = LSA_file = field_harmonics_from_WISE = "" ;
		MADX_format_string = "\"10d\",\"18.10g\",\"-18s\"" ;
		
		MINUIT_precision = 0.00000001 ;
		MINUIT_call_limit = 50000 ;
		MADX_perturbation_precision = 10 ;
		
		lxbatch_queue = "8nh" ;
		
		normalized_transverse_emittances["Vertical"][1] = 3.75 ;
		normalized_transverse_emittances["Horizontal"][1] = 3.75 ;
		normalized_transverse_emittances["Vertical"][2] = 3.75 ;
		normalized_transverse_emittances["Horizontal"][2] = 3.75 ;

		alignment = false ;
		harmonics = false ;
		debug_matching = false ;
		constraint_smearing = false ;
		projectType = -1 ;
		matched_beam_momentum = false ;
		MINUIT_tolerance = 0 ;
		root_object = NULL ; 
		do_not_submit = false ;		

		command = "" ;
		
		elastic_model = NULL ;
		
		string MADXVersion = "utt" ;		
		string OpticsVersion = "m" ;
		
		// Run MAD-X to get verion

		string repo_command = "echo \"/afs/cern.ch/user/m/mad/bin/madx MAD-X\" | "  + OpticsVersion + MADXVersion + " " + option + option2 + "es" + optionadd + " -s \"MAD-X call\"" ;
		
		system(repo_command.c_str()) ;
		
		
		return 0 ;
	}	
	
	TProjectParameters() 
	{
		InitParameters() ;
	} 

	TProjectParameters(string aname, string project_file_name) : TOpticsObject(aname) 
	{
	
		InitParameters() ;
		Load(project_file_name) ;
		SetupOpticsFiles() ;	
	} 	

	set <TStructForPerturbation> *GetPerturbedMagnets_Strength() 	{ return &perturbed_magnets_strength ; } ;
	set <TStructForPerturbation> *GetPerturbedMagnets_Rotation() 	{ return &perturbed_magnets_rotation ; } ;
	set <TStructForPerturbation> *GetPerturbedMagnets_X() 	{ return &perturbed_magnets_x ; } ;
	set <TStructForPerturbation> *GetPerturbedMagnets_Y() 	{ return &perturbed_magnets_y ; } ;

	map <int , TStructForMatching > *GetMatchedParameters() 	{ return &matched_parameters ; } ;

	int getNumberOfMatchedParameters()
	{
		int number_of_matched_parameters = (int) matched_parameters.size() ;
		if(matched_beam_momentum) number_of_matched_parameters += 2 ;
		
		return number_of_matched_parameters ;
	} ;

	bool getDebugMatching() { return debug_matching ; } ;
	bool getMatchedBeamMomentum() { return matched_beam_momentum ; } ;
	bool getConstraintSmearing() { return constraint_smearing ; } ;
	bool HarmonicsOn() { return harmonics = true ; }
	bool HarmonicsOff() { return harmonics = false ; }
	bool getHarmonics() { return harmonics ; }
	bool AlignmentOn() { return alignment = true ; }
	bool AlignmentOff() { return alignment = false ; }
	bool getAlignment() { return alignment ; }
	bool doNotSubmit() { return do_not_submit ; } ;
	
	set <string> *kept_in_TTree() { return &element_names_kept_in_TTree ; }
	
	int setVerbosity(int verbosity) { return Verbosity = verbosity ; } ;
	int getVerbosity() { return Verbosity ; } ;
        int getNumberOfJobs() { return NumberOfJobs ; }
        int setNumberOfJobs( int aNumberOfJobs ) { return NumberOfJobs = aNumberOfJobs ; }
	int Load(string) ;
	int Save(TDirectory *, TFile *, OpticsArgumentType * = NULL) ;
	int LoadFromROOT(string, TFile *f, OpticsArgumentType *) ;
	int Print(int) ;
	int Execute(string) ;
	int getMinuitCallLimit() { return MINUIT_call_limit ; } ;
	int getMADXPerturbationPrecision() { return MADX_perturbation_precision ; } ;	
	
	double getProtonMass() { return ProtonMass ; }
	double getMinuitTolerance() { return MINUIT_tolerance ; } 
	double getEnergyperbeam() { return energyperbeam ; }
	double setEnergyperbeam( double aenergyperbeam ) { return energyperbeam = aenergyperbeam ; }
	double getBetastar_x() { return betastar_x ; } ;
	double getBetastar_y() { return betastar_y ; } ;
	double setBetastar_x( double abetastar ) { return betastar_x = abetastar ; } ;
	double setBetastar_y( double abetastar ) { return betastar_y = abetastar ; } ;
	double getNormalizedTransversEmittanceMADXform(string position, int beam) { return normalized_transverse_emittances[position][beam] / 1000000.0 ; }
	double getMinuitPrecision() { return MINUIT_precision ; } ;
	double getDeltaX() { return deltax ; } ;
	double getDeltaY() { return deltay ; } ;
	double getDeltaPX() { return deltapx ; } ;
	double getDeltaPY() { return deltapy ; } ;
	double getDeltaPT() { return deltapt ; } ;
	double getDeltaDeltaP() { return deltadeltap ; } ;
	double getHorizontalCrossingAngle() { return horizontal_crossing_angle ; } ;
	double getReference_track_px_vector(int index) { if(index < (int)px_values_per_job.size()) { return px_values_per_job[index] ;} else { cout << "[TBeam] The requested index is larger than the number of entries !" << endl ; exit(1) ;} } ;
	double getReference_track_py_vector(int index) { if(index < (int)py_values_per_job.size()) { return py_values_per_job[index] ;} else { cout << "[TBeam] The requested index is larger than the number of entries !" << endl ; exit(1) ;} } ;
	
	TElasticModel *getElasticModel() { return elastic_model ; } ;
	string getField_harmonics_from_WISE() { return field_harmonics_from_WISE ; } ;
	string get_nth_Field_harmonics_file_from_WISE(int index) { return (*vector_field_harmonics_from_WISE_tfsnames)[index-1] ; } ;
		
	string getLSAFile() { return LSA_file; } ;
	string getMADX_format_string() { return MADX_format_string ; } ;
	string getlxbatch_queue() { return lxbatch_queue ; } ;
	string getCommand() { return command ; }
	string getFileNameForSaveAs() { return fileNameForSaveAs ; }
	string getAnalysisName() { return analysis_name ; } 
	string getROOTfilename() { return ROOTfilename ; }
	string getProjectDirectory() { return ProjectDirectory ; }
	string getGeneral_data_directory() {return general_data_directory ; } ;	
	string setMADXName( string madxname) { MADX_executable_name = madxname ; return MADX_executable_name ; }  ;
	string setROOTfilename(string aROOTfilename) { return ROOTfilename = aROOTfilename ; }	
	string setProjectDirectory(string aProjectDirectory)  { return ProjectDirectory = aProjectDirectory ; }
	
	double getParameterValue(string name) { return map_parameter_value[name] ; }  ;
	string getSettingValue(string name) { return map_setting_value[name] ; }  ;	

	bool IsParameterDefined(string) ;
	
	TOpticsObject *setRootObject(TOpticsObject *the_root_object) { return (root_object = the_root_object) ; }
	TOpticsObject *getRootObject() { return root_object ; }

	int setBeamPerturbations(double adeltax, double adeltay, double adeltapx, double adeltapy, double adeltapt, double adeltadeltap)
	{
		deltax = adeltax ; deltay = adeltay ; deltapx = adeltapx ; deltapy = adeltapy ; deltapt = adeltapt ; deltadeltap = adeltadeltap ;

		return 0 ;
	}

	string getMADXname() { return MADX_executable_name ; } ;

	int getProjectType() { return projectType ; }
	string getStartTimeStamp() { return startTimeStamp ; } ;
	int setTimeStamps(string start, string end) { startTimeStamp = start ; endTimeStamp = end ; return 0 ; }


	int setType(int atype) { return projectType = atype ; }

	int add_perturbed_magnet_strength(string, double, double) ;
	int add_perturbed_magnet_rotation(string, double, double) ;

	    int add_perturbed_magnet_x(string, double, double) ;
	    int add_perturbed_magnet_y(string, double, double) ;


	int add_matched_parameter(string, int, string, double = 0, double = 0, double = 0, double = 0) ;

	int SetupOpticsFiles() ;

} ; 

int TProjectParameters::SetupOpticsFiles()
{
	bool strength_is_ok = false ;
	bool chromaticty_is_ok = false ;

	strength_dbase_dir = "/afs/cern.ch/eng/lhc/optics/V6.503/" ;
	chromaticity_dbase_dir = "/afs/cern.ch/eng/sl/online/om/repository/malabaup/" ;

        if(getBetastar_x() == 3.5)
        {
                strength_files.push_back("IR1/IP1_beta_3.50.str") ;
                strength_files.push_back("IR2/3.5TeV/ip2_0.00889_beta3.50m.str") ;
                strength_files.push_back("IR5/IP5_beta_3.50.str") ;
                strength_files.push_back("IR8/3.5TeV/ip8_0.00882_beta3.50m.str") ;

		strength_is_ok = true ;

		if(harmonics)
		{
	        	correction_subdbase_dir = "correction_harmonics/BEAM1_3.5TeV3.5m/" ;
        		absolute_harmonics_dbase_dir = "absolute_harmonics/BEAM1_3.5TeV3.5m/" ;

	        	correction_file = "B2_corr_setting_3.5TeV3.5m_O1-15_S1.mad" ;
			coupling_correction_file = "coupling_corr_aftertune_thick_model_3.5TeV3.5m_S1_b1.mad" ;
			chromaticity_correction = "tune_chrom_corr_thick_model_3.5TeV3.5m_S1.b1.mad" ;

			abs_correction_MB = "harmonics_3.5TeV3.5m_MB_O2-15_S1.tfs" ;
			abs_correction_MQ = "harmonics_3.5TeV3.5m_MQ_O2-15_S1.tfs" ;
			abs_correction_MC = "ptc_total_corr1_3.5TeV3.5m_b1_MC_O2-15_S1.tfs" ;

			chromaticty_is_ok = true ;
		}

	}
        else if(getBetastar_x() == 90.0)
        {
                strength_files.push_back("HiBeta/IP5_beta90_2010.str") ;

		strength_is_ok = true ;
        }
        else if(getBetastar_x() == 0.65)
        {
                strength_files.push_back("IP1_beta_0.65.str") ;
                strength_files.push_back("IR5/IP5_beta_0.65.str") ;

		strength_is_ok = true ;
        }
        else if(getBetastar_x() == 1535.0)
        {
                strength_files.push_back("HiBeta/IP5_beta1535_b1.str") ;

		strength_is_ok = true ;
        }
        else if(getBetastar_x() == 11.0)
        {
		if(getSettingValue("LHC_sequence").compare("LHC_Run_II_2017") == 0)
		{
			strength_dbase_dir = "/afs/cern.ch/eng/lhc/optics/runII/2017/" ;
			strength_files.push_back("opticsfile_new.1") ;
		}
		else
		{
	                strength_files.push_back("V6.5.inj_special.str") ;
        	        strength_files.push_back("IR1/new_ip1_b2_squeeze/IP1_beta11.00m.str") ;
                	strength_files.push_back("IR5/new_ip5_b2_squeeze/IP5_beta11.00m.str") ;
		}

		strength_is_ok = true ;
        }
        else if(getBetastar_x() == 1000.0)
        {
		cout << "Warning: there is no nominal settings file for beta*=1000 m ! " << endl ;

		strength_is_ok = true ;
        }
        else if(getBetastar_x() == 0.8)
        {
                strength_files.push_back("IR5/IP5_beta_0.80.str") ;

		strength_is_ok = true ;
        }
        else if(getBetastar_x() == 0.4)
        {
		strength_is_ok = true ;
        }
        else if(getBetastar_x() == 2500)
        {
		strength_is_ok = true ;
        }
        else if(getBetastar_x() == 70)
        {
                strength_is_ok = true ;
        }
        else if(getBetastar_x() == 45)
        {
                strength_is_ok = true ;
        }


        if(!strength_is_ok)
        {
                cout << "[TBeam] error: beta*=" << getBetastar_x() << "m is not supported ! Hint: You have to improve the TProjectParameters::SetupOpticsFiles method." << endl ;
                exit(BETA_STAR_NOT_SUPPORTED) ;
        }

	if(harmonics && !chromaticty_is_ok)
	{
		cout << "There are no chromaticity files provided for beta*: " << getBetastar_x() << "m ! Hint: look after the files at: " << chromaticity_dbase_dir << endl ;
		exit(NO_CHROMATICITY_FILES) ;
	}

	return 0 ;
}

int TProjectParameters::add_perturbed_magnet_rotation(string magnet_name, double MEAN, double RMS)
{
	TStructForPerturbation t ;

	t.name = magnet_name ;
	t.MEAN = MEAN ;
	t.RMS = RMS ;

	perturbed_magnets_rotation.insert(t) ;
	return 0 ;
}

int TProjectParameters::add_perturbed_magnet_strength(string magnet_name, double MEAN, double RMS)
{
        TStructForPerturbation t ;

        t.name = magnet_name ;
        t.MEAN = MEAN ;
        t.RMS = RMS ;

        perturbed_magnets_strength.insert(t) ;
        return 0 ;
}

int TProjectParameters::add_perturbed_magnet_x(string magnet_name, double MEAN, double RMS)
{
        TStructForPerturbation t ;

        t.name = magnet_name ;
        t.MEAN = MEAN ;
        t.RMS = RMS ;

        perturbed_magnets_x.insert(t) ;
        return 0 ;
}

int TProjectParameters::add_perturbed_magnet_y(string magnet_name, double MEAN, double RMS)
{
        TStructForPerturbation t ;

        t.name = magnet_name ;
        t.MEAN = MEAN ;
        t.RMS = RMS ;

        perturbed_magnets_y.insert(t) ;
        return 0 ;
}

int TProjectParameters::add_matched_parameter(string type, int number, string magnet_name, double value, double step, double lower_limit, double upper_limit)
{
	TStructForMatching aux ;

	aux.type = type ;

	aux.name = magnet_name ;
	aux.value = value ;
	aux.step = step ;
	aux.lower_limit = lower_limit ;
	aux.upper_limit = upper_limit  ;

	matched_parameters[number] = aux ;
	
	return 0 ; 
}

bool TProjectParameters::IsParameterDefined(string parameter_name)
{
	map_parameter_value_type::iterator it ;

	it = map_parameter_value.find(parameter_name) ;

	return (it != map_parameter_value.end()) ;
}

int TProjectParameters::Load(string s)
{

        ifstream project_file ;

        project_file.open(s.c_str(), ifstream::in) ;

        if(project_file.fail())
        {
                cout << "Cannot open configuration file!" << endl ;
                exit(ERR_CANNOT_OPEN_CONFIG_FILE) ;
        }

        string keyword ;
        while( project_file >> keyword)
        {
		if(keyword.compare("<comment>") == 0)
		{	
			char buffer[200] ;
			project_file.getline(buffer,200) ;
		}
		else if(keyword.compare("<verbosity>") == 0)
		{
			int verbosity_level ;
			project_file >> verbosity_level ;

			if((verbosity_level >= 0) && (verbosity_level <= 2))
			{
				Verbosity = verbosity_level ;
				if(Verbosity == VERBOSITY_ALL) cout << "[TProjectParameters::Load] Verbosity = " << verbosity_level << endl ;
			}
			else
			{
				cout << "[TProjectParameters::Load] Incorrect verbosity level: " << verbosity_level << " !" << endl ;
				exit(ERR_INCORRECT_VERBOSITY) ;
			}
		}
		else if(keyword.compare("<root_filename>") == 0)
		{
			project_file >> ROOTfilename ;

			if(Verbosity == VERBOSITY_ALL) cout << "[TProjectParameters::Load] ROOT output filename: " << ROOTfilename << endl ;  
		}
		else if(keyword.compare("<project_directory>") == 0)
		{
			project_file >> ProjectDirectory ;

			if(Verbosity == VERBOSITY_ALL) cout << "[TProjectParameters::Load] Project directory: " << ProjectDirectory << endl ;  
		}
                else if(keyword.compare("<project_type>") == 0)
                {
                        string project_type ;

                        project_file >> project_type ;

                        if(project_type.compare("Perturbations") == 0) projectType = Perturbations ;
                        else if(project_type.compare("Matching_1N") == 0) projectType = Matching_1N ;
                        else if(project_type.compare("Matching_N1") == 0) projectType = Matching_N1 ;
						else 
						{
			                cout << "[TProjectParameters::Load] Unknown project type !" << endl ; 
			                exit(ERR_UNKNOWN_PROJECT) ;
						}
		}
		else if(keyword.compare("<perturbed_magnet_strength>") == 0)
		{
			string magnet_name ;
			double RMS, MEAN ;

			project_file >> magnet_name >> MEAN >> RMS ;
			add_perturbed_magnet_strength(magnet_name, MEAN, RMS) ;

			if(Verbosity == VERBOSITY_ALL) cout << "[TProjectParameters::Load] Magnet name: " << magnet_name << " Strength Mean: " << MEAN << " RMS: " << RMS << endl ;  
		}
		else if(keyword.compare("<perturbed_magnet_rotation>") == 0)
		{
			string magnet_name ;
                        double RMS, MEAN ; 

			project_file >> magnet_name >> MEAN >> RMS ;
			add_perturbed_magnet_rotation(magnet_name, MEAN, RMS) ;

			if(Verbosity == VERBOSITY_ALL) cout << "[TProjectParameters::Load] Magnet name: " << magnet_name << " Rotation Mean: " << MEAN << " RMS: " << RMS << endl ;
		}
        else if(keyword.compare("<perturbed_magnet_x>") == 0)
        {
            string magnet_name ;
                        double RMS, MEAN ;

            project_file >> magnet_name >> MEAN >> RMS ;
            add_perturbed_magnet_x(magnet_name, MEAN, RMS) ;

            if(Verbosity == VERBOSITY_ALL) cout << "[TProjectParameters::Load] Magnet name: " << magnet_name << " X perturbation Mean: " << MEAN << " RMS: " << RMS << endl ;
        }
        else if(keyword.compare("<perturbed_magnet_y>") == 0)
        {
            string magnet_name ;
                        double RMS, MEAN ;

            project_file >> magnet_name >> MEAN >> RMS ;
            add_perturbed_magnet_y(magnet_name, MEAN, RMS) ;

            if(Verbosity == VERBOSITY_ALL) cout << "[TProjectParameters::Load] Magnet name: " << magnet_name << " Y perturbation Mean: " << MEAN << " RMS: " << RMS << endl ;
        }
		else if(keyword.compare("<matched_beam_momentum>") == 0 )
		{
			string answer ;

			project_file >> answer ;

			if(answer.compare("yes") == 0) matched_beam_momentum = true ;
			else if(answer.compare("no") == 0) matched_beam_momentum = false ;
		}
                else if(keyword.compare("<matched_parameter>") == 0)
                {
                        string magnet_name,type ;

			int number ;
			double value, step, lower_limit, upper_limit ;

                        project_file >> type >> number >> magnet_name >> value >> step >> lower_limit >> upper_limit ;

			if((type.compare("Strength") != 0) && (type.compare("Rotation") != 0))
			{
				cout << "Wrong matching type !" << endl ;
				exit(ERR_WRONG_MATCH_TYPE);
			}

                        add_matched_parameter(type,number, magnet_name,value,step,lower_limit,upper_limit) ;

                        if(Verbosity == VERBOSITY_ALL) cout << "[TProjectParameters::Load] Matched strength for magnet: " << magnet_name << endl ;
                }
		else if(keyword.compare("<LSA_settings>") == 0)
		{	
			project_file >> LSA_file ;
		}
		else if(keyword.compare("<general_data>") == 0)
		{
					project_file >> general_data_directory ;

					if(Verbosity == VERBOSITY_ALL) cout << "[TProjectParameters::Load] General data directory: " << general_data_directory << endl ;  
		}
		else if(keyword.compare("<number_of_jobs>") == 0)
		{
			int number_of_jobs ;		

                        project_file >> number_of_jobs ;
			setNumberOfJobs(number_of_jobs) ;

			if(Verbosity == VERBOSITY_ALL) cout << "[TProjectParameters::Load] Number of jobs: " << number_of_jobs << endl ;  
		}
		else if(keyword.compare("<beam_perturbation>") == 0 ) 
		{
			project_file >> deltax >> deltay >> deltapx >> deltapy >> deltapt >> deltadeltap ;
			if(Verbosity == VERBOSITY_ALL) 
			{
				cout << "[TProjectParameters::Load] Beam perturbations (dx, dy, dpx, dpy, dpt, de): " << 
					deltax << ", " << deltay << ", " << deltapx << ", " << deltapy << ", " << deltapt << ", " << deltadeltap << endl;
			}
		}
		else if(keyword.compare("<beam_energy>") == 0)
		{
			project_file >> energyperbeam ;
		}
		else if(keyword.compare("<madx_executable>") == 0)
		{
			project_file >> MADX_executable_name ;
		}
		else if(keyword.compare("<lxbatch_queue>") == 0)
		{
			project_file >> lxbatch_queue ;
		}
		else if(keyword.compare("<MADX_format_string>") == 0)
		{
			project_file >> MADX_format_string ;
		}
		else if(keyword.compare("<MINUIT_precision>") == 0)
		{
			project_file >> MINUIT_precision ;
		}
		else if(keyword.compare("<beta_star>") == 0)
		{
			project_file >> betastar_x ;
			betastar_y = betastar_x ;
		}
                else if(keyword.compare("<beta_star_x>") == 0)
                {
                        project_file >> betastar_x ;
                }
                else if(keyword.compare("<beta_star_y>") == 0)
                {
                        project_file >> betastar_y ;
                }
		else if(keyword.compare("<do_not_submit>") == 0)
		{
			double do_not_submit_value ;
			
			project_file >> do_not_submit_value ;
			
			do_not_submit = false ;
			if(do_not_submit_value != 0) do_not_submit = true ;
		}		
		else if(keyword.compare("<field_harmonics_from_WISE>") == 0)
		{
			vector_field_harmonics_from_WISE_tfsnames = new vector<string> ;
			project_file >> field_harmonics_from_WISE ;
			
			ifstream field_harmonics_from_WISE_tfsnames(field_harmonics_from_WISE.c_str()) ;
			string filename ;
			
			while(field_harmonics_from_WISE_tfsnames >> filename)
			{
				vector_field_harmonics_from_WISE_tfsnames->push_back(filename) ;
			}
			
			if((int)vector_field_harmonics_from_WISE_tfsnames->size() < getNumberOfJobs())
			{
				cout << "The number of the jobs is larger than the number of .tfs harmonics files in " << field_harmonics_from_WISE << " !" ;
				exit(ERR_TFS_JOB_NUMBER_CONFLICT) ;
			}
		}		
		else if(keyword.compare("<horizontal_crossing_angle_radian>") == 0)
		{
			project_file >> horizontal_crossing_angle ;
		}
                else if(keyword.compare("<normalized_transverse_emittances>") == 0)
                {
                        project_file >> normalized_transverse_emittances["Horizontal"][1] >> normalized_transverse_emittances["Vertical"][1] >> 
					normalized_transverse_emittances["Horizontal"][2] >> normalized_transverse_emittances["Vertical"][2]   ;
                }
                else if(keyword.compare("<MINUIT_tolerance>") == 0)
                {
                        project_file >> MINUIT_tolerance ;
                }
		else if(keyword.compare("<harmonics>") == 0)
		{
			string harmonics_text ;

			project_file >> harmonics_text ;
			if(harmonics_text.compare("yes") == 0) harmonics = true ;
			else if(harmonics_text.compare("no") == 0) harmonics = false ;
			else
			{
				cout << "[TProjectParameters::Load] <harmonics> : should be yes or no ! " << endl ;
				exit(ERR_UNDEFINED_KEYWORD) ;
			}
		}
		else if(keyword.compare("<constraint>") == 0)
		{
			double value, error, relative_error ;
                        char buffer[4000] ;

			project_file >> value >> error >> relative_error ;
 
                        project_file.getline(buffer,4000) ;
			string constraint(buffer) ;

			if((error == 0) && (relative_error ==0))
			{
				cout << "[TProjectParameters] Error: both errors are zero for constraint"  << buffer << endl ;
			}

			Constraints.addConstraint(constraint,value, error,relative_error) ;
                        if(Verbosity == VERBOSITY_ALL) cout << "[TProjectParameters::Load] Constraint added: " << constraint << endl ;
		}
                else if(keyword.compare("<include_t_distribution>") == 0)
		{
			elastic_model = new TElasticModel(1, getEnergyperbeam()) ;
			
			for(int i = 1 ; i <= NumberOfJobs ; ++i)
			{
				double reference_track_px, reference_track_py ;
				elastic_model->Get_PX_and_PY(reference_track_px, reference_track_py) ;
			
				px_values_per_job.push_back(reference_track_px) ;
				py_values_per_job.push_back(reference_track_py) ;			
				
			}
			
		}
                else if(keyword.compare("<command>") == 0)		
		{
			project_file >> command ;
			if(command.compare("SaveAs") == 0) project_file >> fileNameForSaveAs ;
			if(command.compare("Analysis") == 0) project_file >> analysis_name ;
		}
		else if(keyword.compare("<debug_matching>") == 0)
		{
			string debug_yes_no ;

			project_file >> debug_yes_no ;
			if(debug_yes_no.compare("yes") == 0) debug_matching = true ;
			else if(debug_yes_no.compare("no") == 0) debug_matching = false ;
                        else
                        {
                                cout << "[TProjectParameters::Load] <debug_matching> : should be yes or no ! " << endl ;
                                exit(ERR_UNDEFINED_KEYWORD) ;
                        }
		}
		else if(keyword.compare("<constraint_smearing>") == 0)
		{
			string debug_yes_no ;

			project_file >> debug_yes_no ;
			if(debug_yes_no.compare("yes") == 0) constraint_smearing = true ;
			else if(debug_yes_no.compare("no") == 0) constraint_smearing = false ;
                        else
                        {
                                cout << "[TProjectParameters::Load] <constraint_smearing> : should be yes or no ! " << endl ;
                                exit(ERR_UNDEFINED_KEYWORD) ;
                        }
		}
		else if(keyword.compare("<MINUIT_call_limit>") == 0)
		{
			project_file >> MINUIT_call_limit ;
		}
		else if(keyword.compare("<keep_in_TTree>") == 0)		
		{
			string element_name ;

			project_file >> element_name ;

//			replace(element_name.begin(), element_name.end(),'.','_' ) ;
			element_names_kept_in_TTree.insert(element_name) ;
		}
		else if(keyword.compare("<alignment>") == 0)
		{
			string alignment_text ;

			project_file >> alignment_text ;
			if(alignment_text.compare("yes") == 0) alignment = true ;
			else if(alignment_text.compare("no") == 0) alignment = false ;
                        else
                        {
                                cout << "[TProjectParameters::Load] <alignment> : should be yes or no ! " << endl ;
                                exit(ERR_UNDEFINED_KEYWORD) ;
                        }
                }
		else if(keyword.compare("<start_end_timestamp>") == 0)
		{
			project_file >>  startTimeStamp ;
			project_file >>  endTimeStamp ; 

			if(startTimeStamp.compare("-") == 0) startTimeStamp = "" ;
			if(endTimeStamp.compare("-") == 0) endTimeStamp = "" ;

		}
		else if(keyword.compare("<parameter>") == 0)
		{
			string parameter_name ;
			double value ;
			
                        project_file >> parameter_name ;
                        project_file >> value ;	
			
			map_parameter_value[parameter_name] = value ;				
		} else if(keyword == "<setting>")
		{
			std::string setting_name ;	
			std::string value ;
		
			project_file >> setting_name ;
			project_file >> value ;
				
			map_setting_value[setting_name] = value ;
		} else
		
                {
                        cout << "[TProjectParameters::Load] Undefined keyword: " << keyword << endl ;
                        exit(ERR_UNDEFINED_KEYWORD) ;
                }
	}	

	if(projectType == -1)		
	{
		cout << "Project type was not defined !" ; 
		exit(ERR_UNDEFINED_PROJECT) ;
	}
	
	return 0 ;
}

int TProjectParameters::Print(int depth)
{
        string prefix=Prefix(depth) ;

        cout << endl << prefix << "TProjectParameters Print ----------" << endl << endl ;
        cout << prefix << "Name: " << this->GetName() << endl ;

        //cout << prefix << "Perturbed magnets strength: " ;
        //set <string>::iterator it;
        //for(it=perturbed_magnets_strength.begin() ; it != perturbed_magnets_strength.end(); it++) cout << *it << ", " ;
        //cout << endl ;

        //cout << prefix << "Perturbed magnets rotation: " ;
        //for(it=perturbed_magnets_rotation.begin() ; it != perturbed_magnets_rotation.end(); it++) cout << *it << ", " ;
        //cout << endl ;

        //cout << prefix << "Matched magnets strength: " ;
	//set <TStructForMatching>::iterator it2;
        //for(it2 = matched_magnets_strength.begin() ; it2 != matched_magnets_strength.end(); it2++) cout << it2->name << ", " ;
        //cout << endl ;

        //cout << prefix << "Matched magnets rotation: " ;
        //for(it2=matched_magnets_rotation.begin() ; it2 != matched_magnets_rotation.end(); it2++) cout << it2->name << ", " ;
        //cout << endl ;


        TOpticsObject::Print(depth) ;

	return 0 ;
}

int TProjectParameters::Execute(string dir)
{
	return 0 ;
}

int TProjectParameters::Save(TDirectory *dir, TFile *rootfile, OpticsArgumentType *OpticsArgument)
{
        dir->cd() ;

        Properties = new TTree("Project properties","This branch describes the properties of the project");

        Properties->Branch("Project_type",		&projectType,           "Project_type/I");
        Properties->Branch("Number_of_Jobs",		&NumberOfJobs,          "Number_of_Jobs/I");
        Properties->Branch("Energy_per_beam[GeV]",	&energyperbeam,         "Energy_per_beam/D");
        Properties->Branch("Betastar_x",		&betastar_x,    "Betastar_x/D");
        Properties->Branch("Betastar_y",		&betastar_y,    "Betastar_y/D");

	if(startTimeStamp.length() > 40) {cout << "Start time stamp string too large ! " ; exit(1) ; }
	char startts[40] ; strcpy(startts,startTimeStamp.c_str()) ;
        Properties->Branch("Start_time_stamp", startts, "Start_time_stamp/C");

	if(endTimeStamp.length() > 40) {cout << "End time stamp string too large ! " ; exit(1) ; }
	char endts[40] ; strcpy(endts,endTimeStamp.c_str()) ;
        Properties->Branch("End_time_stamp", endts, "End_time_stamp/C");

	if(ROOTfilename.length() > length_of_directory_name_in_root_file) {cout << "RootFileName string too large ! " ; exit(1) ; }
        char rootf[length_of_directory_name_in_root_file] ; strcpy(rootf,ROOTfilename.c_str()) ;
        Properties->Branch("RootFileName",rootf, "RootFileName/C");

	if(ProjectDirectory.length() > length_of_directory_name_in_root_file) {cout << "ProjectDirectory string too large ! " ; exit(1) ; }
        char pd[length_of_directory_name_in_root_file] ; strcpy(pd,ProjectDirectory.c_str()) ;
        Properties->Branch("ProjectDirectory",pd, "ProjectDirectory/C");

	if(general_data_directory.length() > length_of_directory_name_in_root_file) {cout << "GeneralDataDirectory string too large ! " ; exit(1) ; }
        char gd[length_of_directory_name_in_root_file] ; strcpy(gd,general_data_directory.c_str()) ;
        Properties->Branch("GeneralDataDirectory",gd, "GeneralDataDirectory/C");

        Properties->Fill() ;

        Properties = new TTree("Perturbation properties","This branch describes the properties of the applied perturbations");

	set <TStructForConstraints> myconstraints = Constraints.getConstraints() ;
	size_t numberofconstraints = myconstraints.size() ;

        Properties->Branch("Harmonics_included?",	&harmonics,         	"Harmonics_included?/O");
        Properties->Branch("Alignment_included?",	&alignment,         	"Alignment_included?/O");
        Properties->Branch("DeltaX",			&deltax,	"DeltaX/D");
        Properties->Branch("DeltaY",    		&deltay,        "DeltaY/D");
        Properties->Branch("DeltaPX", 			&deltapx,       "DeltaPX/D");
        Properties->Branch("DeltaPY",			&deltapy,       "DeltaPY/D");
	Properties->Branch("DeltaPT",   		&deltapt,	"DeltaPT/D");
	Properties->Branch("DeltaDeltaP",   		&deltadeltap,	"DeltaDeltaP/D");
	Properties->Branch("NumberofConstraints",   	&numberofconstraints,	"NConstraints/I");

        Properties->Fill() ;


	// Saving constraints
        Properties = new TTree("Constraints","This branch describes the properties of the applied constraints");

	set <TStructForConstraints>::iterator it ;

	char **buff = (char **)malloc(sizeof(char *) * numberofconstraints) ;

	int i = 0 ; 
	for(it = myconstraints.begin() ; it != myconstraints.end() ; ++it) 
	{
		buff[i] = (char *)malloc((sizeof(char) * ((it->constraint.size()))+20)) ;
		strcpy(buff[i], it->constraint.c_str()) ;

		stringstream ss ; ss << i ;
		string constraint_name = "Constraint" + ss.str() ;

	        Properties->Branch(constraint_name.c_str(),buff[i], (constraint_name + "/C").c_str());
		++i ;
	}

        Properties->Fill() ;

        for(unsigned int i = 0 ; i < numberofconstraints ; ++i) free(buff[i]) ;        
	free(buff) ; 
	

        return 0 ;

}

int TProjectParameters::LoadFromROOT(string s, TFile *f, OpticsArgumentType *OpticsArgument = NULL)
{
	TTree *Properties = (TTree *)f->Get("Project/Project properties");

        Properties->SetBranchAddress("Project_type",            &projectType);
        Properties->SetBranchAddress("Number_of_Jobs",          &NumberOfJobs);
        Properties->SetBranchAddress("Energy_per_beam[GeV]",    &energyperbeam);
        Properties->SetBranchAddress("Betastar_x",          	&betastar_x);
        Properties->SetBranchAddress("Betastar_y",          	&betastar_y);

        char startts[40] ;
        char endts[40] ;
        char rootf[200] ;
        char pd[200] ;
        char gd[200] ;

        Properties->SetBranchAddress("Start_time_stamp", startts);
        Properties->SetBranchAddress("End_time_stamp", endts);
        Properties->SetBranchAddress("RootFileName",rootf);
        Properties->SetBranchAddress("ProjectDirectory",pd);
        Properties->SetBranchAddress("GeneralDataDirectory",gd);

	Properties->GetEntry(0) ;

	startTimeStamp = string(startts) ;
	endTimeStamp = string(endts) ;
	ROOTfilename = string(rootf) ;
	ProjectDirectory = string(pd) ;
	general_data_directory = string(gd) ;

	Properties = (TTree *)f->Get("Project/Perturbation properties");

	int numberofconstraints = 0 ;

        Properties->SetBranchAddress("Harmonics_included?",     &harmonics);
        Properties->SetBranchAddress("Alignment_included?",     &alignment);
        Properties->SetBranchAddress("DeltaX",    		&deltax);
        Properties->SetBranchAddress("DeltaY",    		&deltay);
        Properties->SetBranchAddress("DeltaPX",   		&deltapx);
        Properties->SetBranchAddress("DeltaPY",   		&deltapy);
        Properties->SetBranchAddress("DeltaPT",           	&deltapt);
        Properties->SetBranchAddress("DeltaDeltaP",       	&deltadeltap);
	Properties->SetBranchAddress("NumberofConstraints",   	&numberofconstraints);

	Properties->GetEntry(0) ;

	Constraints.Empty() ;
	Properties = (TTree *)f->Get("Project/Constraints");

	cout << endl << endl ;

	char buffer[4000] ;
	for(int i = 0 ; i < numberofconstraints ; ++i)
	{
		stringstream ss ;
		ss << i ;

		Properties->SetBranchAddress(("Constraint"+ss.str()).c_str() ,     buffer);		
		Properties->GetEntry(0) ;

	}


	return 0 ; 
}

#endif
