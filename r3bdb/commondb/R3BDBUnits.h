/**
 \namespace Units

 This system of units is a modified version of GEANT4's G4UnitsTest
 (which is itself a modified version of SystemOfUnits.h)

 The basic units are those of the International System:

              meter             
              second             
              kilogram      
              ampere         
              degree kelvin          
              the amount of substance (mole)
              luminous intensity      (candela)
              radian                  
              steradian              


 The SI numerical value of the positron charge is defined here,
 as it is needed for conversion factor : positron charge = e_SI (coulomb)

 But, you shouldn't care what the basic MINOS units are.  Instead
 learn these rules to deal with values that have units:

 0) All variables are assumed hold values with Munits already applied.
 NEVER multiply a variable by an Munit.

 1) To store or pass a literal value, ALWAYS multiply by an Units:

 \code
     double energy = 1.0*Units::GeV;
     obj.SetTimeCut(10.0*Units:ns);
     
     root [1] .x mymacro.C(731*Units:km)
 \endcode

 2) To express a value with a given unit, divide by an Munit.  NEVER
 store the result.

 \code
     void print_energy(double energy) {
         cout << "The energy is " << energy/Units::GeV << " GeV\n";
     }
     //...
     print_energy(1200*Units::MeV);
 \endcode

*/


#ifndef R3BDBUNITS_H
#define R3BDBUNITS_H

//#define JOULE_AS_BASE

// Typedef's for Double_t
#ifndef ROOT_Rtypes
#if !defined(__CINT__) || defined(__MAKECINT__)
#include "Rtypes.h"
#endif
#endif


namespace Units 
{

    /// Convert from Munits temperature to Celcius
    double ToCelcius(double temperature);
    /// Convert from Munits temperature to Kelvin
    double ToKelvin(double temperature);
    /// Convert from Munits temperature to Fahrenheit
    double ToFahrenheit(double temperature);
    /// Convert from Celcius temperature to Munits
    double FromCelcius(double degrees_celcius);
    /// Convert from Kelvin temperature to Munits
    double FromKelvin(double degrees_kelvin);
    /// Convert from Fahrenheit temperature to Munits
    double FromFahrenheit(double degrees_fahrenheit);

// 
// Length [L]
//
static const Char_t* const base_length_name = "meter";

static const Double_t meter  = 1.;                  
static const Double_t meter2 = meter*meter;
static const Double_t meter3 = meter*meter*meter;

static const Double_t millimeter  = 0.001*meter;                        
static const Double_t millimeter2 = millimeter*millimeter;
static const Double_t millimeter3 = millimeter*millimeter*millimeter;

static const Double_t centimeter  = 10.*millimeter;   
static const Double_t centimeter2 = centimeter*centimeter;
static const Double_t centimeter3 = centimeter*centimeter*centimeter;

static const Double_t kilometer = 1000.*meter;                   
static const Double_t kilometer2 = kilometer*kilometer;
static const Double_t kilometer3 = kilometer*kilometer*kilometer;

static const Double_t micrometer = 1.e-6 *meter;             
static const Double_t  nanometer = 1.e-9 *meter;
static const Double_t  angstrom  = 1.e-10*meter;
static const Double_t  fermi     = 1.e-15*meter;

static const Double_t inch       = 2.54*centimeter;
static const Double_t mil        = 0.001*inch;
static const Double_t foot       = 12*inch;

static const Double_t      barn = 1.e-28*meter2;
static const Double_t millibarn = 1.e-3 *barn;
static const Double_t microbarn = 1.e-6 *barn;
static const Double_t  nanobarn = 1.e-9 *barn;
static const Double_t  picobarn = 1.e-12*barn;

// symbols
static const Double_t nm     = nanometer;
static const Double_t micron = micrometer;

static const Double_t mm  = millimeter;                        
static const Double_t mm2 = millimeter2;
static const Double_t mm3 = millimeter3;

static const Double_t cm  = centimeter;   
static const Double_t cm2 = centimeter2;
static const Double_t cm3 = centimeter3;

static const Double_t m  = meter;                  
static const Double_t m2 = meter2;
static const Double_t m3 = meter3;

static const Double_t km  = kilometer;                   
static const Double_t km2 = kilometer2;
static const Double_t km3 = kilometer3;

//
// Angle
//
static const Double_t radian      = 1.;                  
static const Double_t milliradian = 1.e-3*radian;
static const Double_t degree = (3.14159265358979323846/180.0)*radian;

static const Double_t   steradian = 1.;

// symbols
static const Double_t rad  = radian;
static const Double_t mrad = milliradian;
static const Double_t sr   = steradian;
static const Double_t deg  = degree;

//
// Time [T]
//
static const Char_t* const base_time_name = "second";

static const Double_t second      = 1.;
static const Double_t nanosecond  = 1.e-9 *second;
static const Double_t millisecond = 1.e-3 *second;
static const Double_t microsecond = 1.e-6 *second;
static const Double_t  picosecond = 1.e-12*second;

static const Double_t hertz = 1./second;
static const Double_t kilohertz = 1.e+3*hertz;
static const Double_t megahertz = 1.e+6*hertz;

static const Double_t minute = 60*second;
static const Double_t hour = 60*minute;
static const Double_t day = 24*hour;
static const Double_t year = 365.2422*day;
static const Double_t snowmass_year = 1.0E7*second;


// symbols
static const Double_t ns = nanosecond;
static const Double_t  s = second;
static const Double_t ms = millisecond;

//
// Mass [E][T^2][L^-2]
//
static const Char_t* const base_mass_name = "kilogram";

static const Double_t  kilogram = 1.;   
static const Double_t      gram = 1.e-3*kilogram;
static const Double_t milligram = 1.e-3*gram;
static const Double_t     tonne = 1000.*kilogram;
static const Double_t kilotonne = 1000.*tonne;

static const Double_t     pound = 0.45359237*kilogram;

// symbols
static const Double_t  kg = kilogram;
static const Double_t   g = gram;
static const Double_t  mg = milligram;
static const Double_t  kt = kilotonne;

//
// Electric current [Q][T^-1]
//
static const Char_t* const base_current_name = "Ampere";

static const Double_t      ampere = 1.;
static const Double_t milliampere = 1.e-3*ampere;
static const Double_t microampere = 1.e-6*ampere;
static const Double_t  nanoampere = 1.e-9*ampere;

//
// Electric charge [Q]
//
static const Char_t* const base_charge_name = "Coulomb";

static const Double_t coulomb = ampere*second;
//static const Double_t  e_SI = 1.60217733e-19;   // old charge in coulomb
static const Double_t    e_SI = 1.602176462e-19;  // positron charge in coulomb
static const Double_t   eplus = e_SI*coulomb;     // positron charge

static const Double_t femptocoloumb = coulomb*1e-15;    // fempto coulomb
static const Double_t   picocoloumb = coulomb*1e-12;    // pico coulomb

static const Double_t      fC = femptocoloumb;
static const Double_t      pC = picocoloumb;


// c   = 299.792458 mm/ns
// c^2 = 898.7404 (mm/ns)^2 
//
static const Double_t c_light   = 2.99792458e+8 * m/s;
static const Double_t c_squared = c_light * c_light;
static const Double_t c_2 = c_squared;
static const Double_t c_4 = c_2 * c_2;
//
// Energy [E]
//
#ifdef JOULE_AS_BASE
static const Char_t* const base_energy_name = "joule";

static const Double_t joule = kg*m*m/(s*s);

static const Double_t     electronvolt = e_SI*joule;
static const Double_t kiloelectronvolt = 1.e+3*electronvolt;
static const Double_t megaelectronvolt = 1.e+6*electronvolt; 
static const Double_t gigaelectronvolt = 1.e+9*electronvolt;
static const Double_t teraelectronvolt = 1.e+12*electronvolt;
static const Double_t petaelectronvolt = 1.e+15*electronvolt;

// both macro and microscopic energies are consistent
static const Double_t hep2baseEnergy   = 1.0;
static const Double_t hep2baseMass     = 1.0;
static const Double_t hep2baseMomentum = 1.0;

#else
static const Char_t* const base_energy_name = "GeV";

static const Double_t     electronvolt = 1.e-9;
static const Double_t kiloelectronvolt = 1.e-6;
static const Double_t megaelectronvolt = 1.e-3;
static const Double_t gigaelectronvolt = 1; 
static const Double_t teraelectronvolt = 1.e+3;
static const Double_t petaelectronvolt = 1.e+6;

//rwh: leave joule still defined in MKS units:
//  assume no one is going to convert from GeV to joules ...
static const Double_t joule = kg*m*m/(s*s);

// macro (MKS) and microscopic (GeV) energies are inconsistent
static const Double_t hep2baseEnergy   = e_SI*1.e+9;
static const Double_t hep2baseMomentum = hep2baseEnergy / c_light;
static const Double_t hep2baseMass     = hep2baseEnergy / c_squared;
#endif

// symbols
static const Double_t  eV = electronvolt;
static const Double_t keV = kiloelectronvolt;
static const Double_t MeV = megaelectronvolt;
static const Double_t GeV = gigaelectronvolt;
static const Double_t TeV = teraelectronvolt;
static const Double_t PeV = petaelectronvolt;

//
// Power [E][T^-1]
//
static const Double_t watt = joule/second;  // watt = 6.24150 e+3 * MeV/ns

//
// Force [E][L^-1]
//
static const Double_t newton = joule/meter;  // newton = 6.24150 e+9 * MeV/mm

//
// Pressure [E][L^-3]
//

static const Double_t hep_pascal = newton/m2;         // pascal = 6.24150 e+3 * MeV/mm3
static const Double_t bar        = 100000*hep_pascal; // bar    = 6.24150 e+8 * MeV/mm3
static const Double_t atmosphere = 101325*hep_pascal; // atm    = 6.32420 e+8 * MeV/mm3

//
// Electric potential [E][Q^-1]
//
#ifdef JOULE_AS_BASE
static const Char_t* const base_epotential_name = "volt";

static const Double_t megavolt = megaelectronvolt/eplus;
static const Double_t kilovolt = 1.e-3*megavolt;
static const Double_t     volt = 1.e-6*megavolt;
#else
static const Char_t* const base_epotential_name = "volt";

static const Double_t     volt = kg*m*m/(ampere*s*s*s);
static const Double_t kilovolt = 1.e+3*volt;
static const Double_t megavolt = 1.e+6*volt;
#endif
static const Double_t millivolt = 1.e-3*volt;
static const Double_t microvolt = 1.e-6*volt;
//
// Electric resistance [E][T][Q^-2]
//
static const Double_t ohm = volt/ampere;  // ohm = 1.60217e-16*(MeV/eplus)/(eplus/ns)

//
// Electric capacitance [Q^2][E^-1]
//
static const Double_t farad = coulomb/volt;  // farad = 6.24150e+24 * eplus/Megavolt
static const Double_t millifarad = 1.e-3*farad;
static const Double_t microfarad = 1.e-6*farad;
static const Double_t  nanofarad = 1.e-9*farad;
static const Double_t  picofarad = 1.e-12*farad;

//
// Magnetic Flux [T][E][Q^-1]
//
static const Double_t weber = volt*second;  // weber = 1000*megavolt*ns

//
// Magnetic Field [T][E][Q^-1][L^-2]
//
static const Char_t* const base_bfield_name = "Tesla";

static const Double_t tesla     = volt*second/meter2;  // tesla =0.001*megavolt*ns/mm2

static const Double_t gauss     = 1.e-4*tesla;
static const Double_t kilogauss = 1.e-1*tesla;

//
// Inductance [T^2][E][Q^-2]
//
static const Double_t henry = weber/ampere;  // henry = 1.60217e-7*MeV*(ns/eplus)**2

//
// Temperature
//
static const Double_t kelvin = 1.;

/// Triple point
static const Double_t standard_temperature = 273.15;

//
// Amount of substance
//
static const Double_t mole = 1.;

//
// Activity [T^-1]
//
static const Double_t becquerel = 1./second ;
static const Double_t curie = 3.7e+10 * becquerel;

//
// Absorbed dose [L^2][T^-2]
//
static const Double_t gray = joule/kilogram ;

//
// Luminous intensity [I]
//
static const Double_t candela = 1.;

//
// Luminous flux [I]
//
static const Double_t lumen = candela*steradian;

//
// Illuminance [I][L^-2]
//
static const Double_t lux = lumen/meter2;

//
// Miscellaneous
//
static const Double_t perCent     = 0.01 ;
static const Double_t perThousand = 0.001;
static const Double_t perMillion  = 0.000001;



} // end namespace 

#endif /* UNITS_H */
