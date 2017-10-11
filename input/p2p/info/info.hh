#ifndef _INFO_HH_
#define _INFO_HH_

#include <math.h>

const int    MAX_STORY = 10240; 	 // Total number of generated events
const double ENERGY    = 800.;           // Beam energy (MeV/u)
const double Exe       = 0.0;        // Residual excitation energy (MeV) (change it for deeply bound states)
const bool   ISOTROPIC = true;           // Internal momentum spread (Gauss)
//const double UNIT = 931.494061;          // Atomic mass unit MeV/c�
const double UNIT = 931.4940954;          // Atomic mass unit MeV/c�


//Nuclear masses from http://wwwndc.jaea.go.jp/NuC/
// WARNING (M. Winkel): website above lists ATOMIC masses!

//C-12
/*
const int    A 	 = 12;                            // Mass number of the nucleus A
const double MA  = 12.*UNIT;		          // Nuclear mass of initital A nucleus(MeV/c�)
const double MB  = 11.009305404*UNIT + Exe;       // Nuclear mass of the residual fragment B (MeV/c�)
const double MOM_SIGMA = 100.00;                  // Internal momentum spread (Gauss)
*/
/*
//Pb-208
const int    A 	 = 208;
const double MA  = 207.976651189*UNIT;
const double MB  = 206.977419033*UNIT + Exe;
const double MOM_SIGMA = 100.00;
*/
//O-18
/*
const int    A 	 = 18;
const double MA  = 17.99915961286*UNIT;
const double MB  = 17.008448873*UNIT + Exe;
const double MOM_SIGMA = 0.1;
*/

// Ca-48
// Binding Energies from http://www.nndc.bnl.gov/chart/chartNuc.jsp
const int   A   = 48;
const double MA = A*UNIT-44.2234;
const double MB = (A-1)*UNIT-35.7089 + Exe;
const double MOM_SIGMA = 112.0;  // Per dimension

const double Ma  = 938.272;  			 // Mass of the knocked-out nucleon (MeV/c�)
const double Mi  = 938.272;  			 // Mass of the scattered nucleon (MeV/c�)

//Constants
const double PI = M_PI;

#endif
