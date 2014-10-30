#ifndef __HEADERS_HH__
#define __HEADERS_HH__

//A structure to store momentum and angles of two
//scattered particles in CM system
struct cm_values
{
	//Internal cluster
	double	e_clust;
	double	p_clust;
	double	theta_clust;
	//Scattered particle
	double	e_scat;
	double	p_scat;
	double	theta_scat;
	//indicates satisfactory kinematics (i.e. energy & momentum conservation)
	bool 	good;
	double  T;
};

TVector3 		  DREHUNG(TVector3 v1,TVector3 v2);
double 			  CINEMA(double,double,double);
cm_values 	          CENMASS(double,double,double,double,bool);
double 			  momentum_CM(double,double,double);
double 			  get_T(double,double);
std::pair<double, double> Lorentz(double,double,double,double);
void 			  run();
#endif //__HEADERS_HH__
