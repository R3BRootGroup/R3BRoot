// -------------------------------------------------------------------------
// -----                      R3BLandPoint source file                  -----
// -------------------------------------------------------------------------

#include "R3BLandHit.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BLandHit::R3BLandHit() : FairHit() {
  fEnergy     = fTof    = 0.;
  fSaturnPaddleNb = -1;
  fDetectorID = -1;
}

// -----   Destructor   ----------------------------------------------------
R3BLandHit::~R3BLandHit() { }

// -----   Public method Print   -------------------------------------------
void R3BLandHit::Print(const Option_t* opt) const {
  cout << "-I- R3BLandHit: LAND Hit in detector " << fDetectorID << endl;
  cout << "    Tof: " << fTof << " cm,  Energy loss "
       << fEnergy*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------

ClassImp(R3BLandHit)
