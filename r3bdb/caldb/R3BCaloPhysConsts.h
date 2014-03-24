// This file is a modified version of GEANT4's PhysicalConstants.h


#ifndef R3BCALOPHYSCONST_H
#define R3BCALOPHYSCONST_H

#include "R3BCaloUnits.h"



namespace PhysConsts
{

using namespace Units;

static const Double_t     pi  = 3.14159265358979323846;
static const Double_t  twopi  = 2*pi;
static const Double_t halfpi  = pi/2;
static const Double_t     pi2 = pi*pi;

//
// 
//
static const Double_t Avogadro = 6.0221367e+23/mole;

//
// speed of light moved to Units
// to allow possible hep units (c=1) to MKS conversion
static const Double_t c_light   = Units::c_light;
static const Double_t c_squared = c_light * c_light;

//
// h     = 4.13566e-12 MeV*ns
// hbar  = 6.58212e-13 MeV*ns
// hbarc = 197.32705e-12 MeV*mm
//
//static const Double_t h_Planck    = 6.6260755e-34 * joule*s;
static const Double_t h_Planck      = 6.62606876e-34 * joule*s;
static const Double_t hbar_Planck   = h_Planck/twopi;
static const Double_t hbarc         = hbar_Planck * c_light;
static const Double_t hbarc_squared = hbarc * hbarc;

//
//
//
static const Double_t electron_charge = - eplus; // see Units
static const Double_t e_squared = eplus * eplus;

//
// amu_c2 - atomic equivalent mass unit
// amu    - atomic mass unit
//
static const Double_t electron_mass_c2 = 0.51099906 * MeV * hep2baseMass;
static const Double_t   proton_mass_c2 = 938.27231  * MeV * hep2baseMass;
static const Double_t  neutron_mass_c2 = 939.56563  * MeV * hep2baseMass;
static const Double_t           amu_c2 = 931.49432  * MeV * hep2baseMass;
static const Double_t              amu = amu_c2/c_squared;

//
// permeability of free space mu0    = 2.01334e-16 Mev*(ns*eplus)^2/mm
// permittivity of free space epsil0 = 5.52636e+10 eplus^2/(MeV*mm)
//
static const Double_t mu0      = 4*pi*1.e-7 * henry/m;
static const Double_t epsilon0 = 1./(c_squared*mu0);

//
// electromagnetic coupling = 1.43996e-12 MeV*mm/(eplus^2)
//
static const Double_t elm_coupling           = e_squared/(4*pi*epsilon0);
static const Double_t fine_structure_const   = elm_coupling/hbarc;
static const Double_t classic_electr_radius  = elm_coupling/electron_mass_c2;
static const Double_t electron_Compton_length = hbarc/electron_mass_c2;
static const Double_t Bohr_radius = electron_Compton_length/fine_structure_const;

static const Double_t alpha_rcl2 = fine_structure_const
                                   *classic_electr_radius
                                   *classic_electr_radius;

static const Double_t twopi_mc2_rcl2 = twopi*electron_mass_c2
                                             *classic_electr_radius
                                             *classic_electr_radius;
//
//
//
//static const Double_t k_Boltzmann = 8.617385e-11 * MeV*hep2baseEnergy/kelvin;
static const Double_t k_Boltzmann = 1.3806503e-23 * joule/kelvin;

//
//
//
static const Double_t STP_Temperature = 273.15*kelvin;
static const Double_t STP_Pressure    = 1.*atmosphere;
static const Double_t kGasThreshold   = 10.*mg/cm3;

//
// gravitational constant 
//
static const Double_t G_N = 6.673e-11 * m3/kg/s/s;


} // end namespace 

#endif 
