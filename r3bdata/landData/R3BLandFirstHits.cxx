// -------------------------------------------------------------------------
// -----                      R3BLandPoint source file                  -----
// -------------------------------------------------------------------------

#include "R3BLandFirstHits.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BLandFirstHits::R3BLandFirstHits() {
  fx0 = fy0 = fz0 = fT0 =  fx1 = fy1 = fz1 = fT1 = fx2 = fy2 = fz2 = fT2 = 
  fx3 = fy3 = fz3 = fT3= fx4 = fy4 = fz4 = fT4 = fx5 = fy5 = fz5 = fT5 = 0.;
}
R3BLandFirstHits::R3BLandFirstHits(Double_t x0, Double_t y0, Double_t z0, Double_t T0,
                                   Double_t x1, Double_t y1, Double_t z1, Double_t T1,
                                   Double_t x2, Double_t y2, Double_t z2, Double_t T2,
                                   Double_t x3, Double_t y3, Double_t z3, Double_t T3,
                                   Double_t x4, Double_t y4, Double_t z4, Double_t T4,
                                   Double_t x5, Double_t y5, Double_t z5, Double_t T5){
    fx0    = x0;
    fy0    = y0;
    fz0    = z0;
    fT0    = T0;
    fx1    = x1;
    fy1    = y1;
    fz1    = z1;
    fT1    = T1;
    fx2    = x2;
    fy2    = y2;
    fz2    = z2;
    fT2    = T2;
    fx3    = x3;
    fy3    = y3;
    fz3    = z3;
    fT3    = T3;
    fx4    = x4;
    fy4    = y4;
    fz4    = z4;
    fT4    = T4;
    fx5    = x5;
    fy5    = y5;
    fz5    = z5;
    fT5    = T5;
}

// -----   Destructor   ----------------------------------------------------
R3BLandFirstHits::~R3BLandFirstHits() { }

// -----   Public method Print   -------------------------------------------
void R3BLandFirstHits::Print(const Option_t* opt) const {

  cout << "-I- R3BLandFirstHits: " << endl;
  cout << "x0: " << fx0 << " y0: " << fy0 << " z0: " << fz0<< " T0: " << fT0 << endl;
  cout << "x1: " << fx1 << " y1: " << fy1 << " z1: " << fz1<< " T1: " << fT1 << endl;
  cout << "x2: " << fx2 << " y2: " << fy2 << " z2: " << fz2<< " T2: " << fT2 << endl;
  cout << "x3: " << fx3 << " y3: " << fy3 << " z3: " << fz3<< " T3: " << fT3 << endl;
  cout << "x4: " << fx4 << " y4: " << fy4 << " z4: " << fz4<< " T4: " << fT4 << endl;
  cout << "x5: " << fx5 << " y5: " << fy5 << " z5: " << fz5<< " T5: " << fT5 << endl;
 
}
// -------------------------------------------------------------------------

ClassImp(R3BLandFirstHits)
