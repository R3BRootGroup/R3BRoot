#ifndef _INFO_HH_
#define _INFO_HH_

const int    MAX_STORY = 100; 	 // Total number of generated events
const double ENERGY    = 700.;           // Beam energy (MeV/u)
const double Exe       = 0.00000;        // Residual excitation energy (MeV) (change it for deeply bound states)
const bool   ISOTROPIC = true;           // Internal momentum spread (Gauss)
const double UNIT = 931.494061;          // Atomic mass unit MeV/c²


//Nuclear masses from http://wwwndc.jaea.go.jp/NuC/

//C-12
const int    A 	 = 12;                            // Mass number of the nucleus A
const double MA  = 12.*UNIT;		          // Nuclear mass of initital A nucleus(MeV/c²)
const double MB  = 11.009305404*UNIT + Exe;       // Nuclear mass of the residual fragment B (MeV/c²)
const double MOM_SIGMA = 100.00;                  // Internal momentum spread (Gauss)
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
const double Ma  = 1.00782503207*UNIT;  			 // Mass of the knocked-out nucleon (MeV/c²)
const double Mi  = 1.00782503207*UNIT;  			 // Mass of the scattered nucleon (MeV/c²)

//Constants
const double PI  = 3.14159265358979323846;

#endif
