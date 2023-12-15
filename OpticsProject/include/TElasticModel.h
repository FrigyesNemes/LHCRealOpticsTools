
#include <TF1.h>
#include "TRandom3.h"
#include "TMath.h"


class TElasticModel
{
	int model_number ;
	double Beam_energy_GeV ;
	TRandom3 random_numbers ;	
	
	TF1 *particle_distribution ;
	
	double	abs_t_min_GeV2 ;
	double	abs_t_max_GeV2 ;


	public:
	
	TElasticModel(int, double) ;
	void Get_PX_and_PY(double &, double &) ;
} ;

Double_t exponential_distribution_from_Jan_2014_01_15(Double_t *x, Double_t *par)
{
      return 530.0 * exp(-19.6 * (x[0])) ;
}

TElasticModel::TElasticModel(int amodel_number, double aBeam_energy_GeV)
{
	model_number = amodel_number ;
	Beam_energy_GeV = aBeam_energy_GeV ;
	
	abs_t_min_GeV2 = 0 ;
	abs_t_max_GeV2 = 3 ;

	if(model_number == 1)
	{
		particle_distribution = new TF1("particle_distribution", exponential_distribution_from_Jan_2014_01_15, abs_t_min_GeV2, abs_t_max_GeV2 * 1.05, 0) ;
	}
	
	
}

void TElasticModel::Get_PX_and_PY(double &px, double &py)
{

		
	double minus_t_GeV2 = particle_distribution->GetRandom() ;
	
	double maxdegree = 360 ;		
	double phi_IP5_degree = random_numbers.Uniform(0, maxdegree) ;
	double phi_IP5 = phi_IP5_degree * ((2*TMath::Pi()) / 360) ;
	
	double relative_momentum = sqrt(minus_t_GeV2)/Beam_energy_GeV ;

	px = relative_momentum * cos(phi_IP5) ;
	py = relative_momentum * sin(phi_IP5) ;			
}
