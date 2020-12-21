//
// p(p,p)p elastic event generator 
//
//

#include "R3BElasticppGenerator.h"

#include "FairPrimaryGenerator.h"
#include "FairMCEventHeader.h"

#include "TDatabasePDG.h"
#include "TRandom.h"
#include "TMath.h"

// -----   Default constructor   ------------------------------------------
R3BElasticppGenerator::R3BElasticppGenerator()
{
}
// ------------------------------------------------------------------------

// -----   Standard constructor   -----------------------------------------
R3BElasticppGenerator::R3BElasticppGenerator(const Double_t Ekin) 
{
  fEkin = Ekin; 
  std::cout << "R3BElasticppGenerator: " << Ekin << std::endl;
}
// ------------------------------------------------------------------------

// -----   Destructor   ---------------------------------------------------
R3BElasticppGenerator::~R3BElasticppGenerator() {
}
// ------------------------------------------------------------------------

// -----   Public method ReadEvent   --------------------------------------
Bool_t R3BElasticppGenerator::ReadEvent(FairPrimaryGenerator* primGen) {


kinematics();

// Como los momentos están en MeV dividimos por 1000
//primGen->AddTrack(2212, p3x, p3y, p3z, 0.0, 0.0, -66.5 ); //GeV
primGen->AddTrack(2212, p3x/1000., p3y/1000., p3z/1000., 0.0, 0.0, 0. ); //GeV
primGen->AddTrack(2212, p4x/1000., p4y/1000., p4z/1000., 0.0, 0.0, 0. ); //GeV

  return kTRUE;
}

void R3BElasticppGenerator::kinematics(){


	//double p1 = fEkin ;//1639; // MeV/c
	double mp = 938.272 ; // MeV/c^2 (proton mass)
	double m1 = mp ;
	double m2 = mp ;
	double m3 = mp ;
	double m4 = mp ;
	double p1 = sqrt( TMath::Power(fEkin+m1,2) - TMath::Power(m1,2) ) ;
	double p1J = (p1)*1.60218e-13 ; // J (Julios)
	int p3i = 10 ;
	int p3f = p1 ; //1639; 
	int N = p3f - p3i ;
	int Z = 1 ;
	int z = 1 ;
	double e = 1.602176565*1e-19 ;
	double e0 = 8.8541878176*1e-12 ;

	double p3 = gRandom->Uniform(p3f, p3i) ;
	double omega = gRandom->Uniform(0, 2*TMath::Pi()) ;
	double omega4 = omega + TMath::Pi() ;
	
	// Calculamos theta LAB
	double E3 = sqrt( TMath::Power(m3,2) + TMath::Power(p3,2) ) ;
	double E1 = sqrt( TMath::Power(m1,2) + TMath::Power(p1,2) ) ;
	double Etot = E1 + m2 ;
	double Etot_cm = sqrt( TMath::Power(m1,2) + TMath::Power(m2,2) + 2*E1*m2 ) ;
	double E3_cm = (1/2.) * ( Etot_cm + ( ( TMath::Power(m3,2) - TMath::Power(m4,2) ) / Etot_cm ) ) ;
	double cos_theta = ( E3*Etot - E3_cm*Etot_cm ) / ( p1*p3 ) ;
	double theta_rad = TMath::ACos(cos_theta) ;
	double theta = TMath::RadToDeg()*theta_rad ;
	
	// Calculamos phi LAB
	double tg_phi = TMath::Sin(theta_rad) / ( (p1/p3) - TMath::Cos(theta_rad) ) ;
	double phi_rad = TMath::ATan( tg_phi ) ;
	double phi = TMath::RadToDeg()*phi_rad ;
	
	// Calculamos theta CM
	double p3_cm = sqrt( TMath::Power(E3_cm,2) - TMath::Power(m3,2) ) ;
	double gamma = Etot/Etot_cm ;
	double beta = sqrt( 1 - (1/TMath::Power(gamma,2)) ) ;
	double cos_theta_cm = ( 1 / (beta*p3_cm) ) * ( (E3/gamma) - E3_cm ) ;
	double theta_cm_rad = TMath::ACos( cos_theta_cm ) ;
	double theta_cm = TMath::RadToDeg()*theta_cm_rad ;
	
	// Calculamos los momentos de m3 y m4 en x,y,z en el LAB
	
	p3z = p3*TMath::Cos(theta_rad) ;
	double p3y_abs = sqrt( ( TMath::Power(p3,2) - TMath::Power(p3z,2) ) / ( 1 + TMath::Power( TMath::Tan(omega) , 2 ) ) ) ;
	double p3x_abs = abs ( p3y_abs*TMath::Tan(omega) ) ;
	p3x = 0. ;
	p3y = 0. ;
	double p4 = p3*( TMath::Sin(theta_rad) / TMath::Sin(phi_rad) ) ;
	p4z = p4*TMath::Cos(phi_rad) ;
	double p4y_abs = sqrt( ( TMath::Power(p4,2) - TMath::Power(p4z,2) ) / ( 1 + TMath::Power( TMath::Tan(omega4) , 2 ) ) ) ;
	double p4x_abs = abs ( p4y_abs*TMath::Tan(omega4) ) ;
	p4x = 0. ;
	p4y = 0. ;
	
	// 1er cuadrante
	if ( omega > 0 && omega < TMath::Pi()/2){
		//std::cout << "p3 Cuadrante 1 " << std::endl ;
		//std::cout << "omega = " << omega << std::endl ;
		p3x = p3x_abs ;
		p3y = p3y_abs ;
		p4x = - p4x_abs ;
		p4y = - p4y_abs ;
	}
	// 2o cuadrante
	else if ( omega > TMath::Pi()/2 && omega < TMath::Pi() ){
		//std::cout << "p3 Cuadrante 2 " << std::endl ;
		//std::cout << "omega = " << omega << std::endl ;
		p3x = p3x_abs ;
		p3y = - p3y_abs ;
		p4x = - p4x_abs ;
		p4y = p4y_abs ;
	}
	// 3er cuadrante
	else if ( omega > TMath::Pi() && omega < (3.0/2.0)*TMath::Pi() ){
		//std::cout << "p3 Cuadrante 3 " << std::endl ;
		//std::cout << "omega = " << omega << std::endl ;
		p3x = - p3x_abs ;
		p3y = - p3y_abs ;
		p4x = p4x_abs ;
		p4y = p4y_abs ;
	}
	// 4o cuadrante
	else if ( omega > (3.0/2.0)*TMath::Pi() && omega < 2*TMath::Pi() ){
		//std::cout << "p3 Cuadrante 4 " << std::endl ;
		//std::cout << "omega = " << omega << std::endl ;
		p3x = - p3x_abs ;
		p3y = p3y_abs ;
		p4x = p4x_abs ;
		p4y = - p4y_abs ;
	}
	
	double Ek3 = sqrt(TMath::Power(p3,2) + TMath::Power(m3,2)) - m3 ; 
	double Ek4 = sqrt(TMath::Power(p4,2) + TMath::Power(m4,2)) - m4 ;
	 
	// Calculamos la sección eficaz diferencial 
	double cte = z*Z*TMath::Power( ( TMath::Power(e,2)/(16.0*TMath::Pi()*e0) ) , 2 ) ;
	double sigmaSI_rut = cte/( TMath::Power(p1J,2)*TMath::Power(TMath::Sin(theta_cm_rad/2),4) ) ; //m^2
	double sigma_rut = sigmaSI_rut/(1e-28) ; //barn
	double sigma = sigma_rut * ( 1 - TMath::Power(beta,2)*TMath::Power(TMath::Sin(theta_cm_rad/2),2) ) ;
	
	//std::cout << "p1 = " << p1 << " MeV" << std::endl ;
	//std::cout << "p3 = " << p3 << " MeV" << std::endl ;
	//std::cout << "theta = " << theta << std::endl ;
	//std::cout << "omega = " << omega << std::endl ;
	//std::cout << "p3x = " << p3x << " MeV" << std::endl ;
	//std::cout << "p3y = " << p3y << " MeV" << std::endl ;
	//std::cout << "p3z = " << p3z << " MeV" << std::endl ;
	//std::cout << "p4 = " << p4 << " MeV" << std::endl ;
	//std::cout << "p4x = " << p4x << " MeV" << std::endl ;
	//std::cout << "p4y = " << p4y << " MeV" << std::endl ;
	//std::cout << "p4z = " << p4z << " MeV" << std::endl ;
	//std::cout << "p3x_abs = " << p3x_abs << std::endl ;
	//std::cout << "p3y_abs = " << p3y_abs << std::endl ;

	double E4 = sqrt( TMath::Power(m4,2) + TMath::Power(p4,2) ) ;
	//std::cout << "CHECK TOTAL ENERGY " << std::endl ;
	//std::cout << "E1+m2 = " << E1+m2 << " MeV" << std::endl ;
	//std::cout << "E3+E4 = " << E3+E4 << " MeV" << std::endl ;
	//std::cout << "CHECK P " << std::endl ;
	//std::cout << "p1 = " << p1 << " MeV" << std::endl ;
	//std::cout << "p3cos(theta) + p4cos(phi) = " << (p3*TMath::Cos(theta_rad)) + (p4*TMath::Cos(phi_rad)) << " MeV" << std::endl ;
	//std::cout << "CHECK KINETIC ENERGY " << std::endl ;
	//std::cout << " Ek1 = " << fEkin << " MeV" << std::endl ;
	//std::cout << " Ek3 = " << Ek3 << " MeV" << std::endl ;
	//std::cout << " Ek4 = " << Ek4 << " MeV" << std::endl ;
	//std::cout << " Ek3 + Ek4 = " << Ek3 + Ek4 << " MeV" << std::endl ;
	//std::cout << "END CHECK " << std::endl ;

}


// ------------------------------------------------------------------------
ClassImp(R3BElasticppGenerator)

