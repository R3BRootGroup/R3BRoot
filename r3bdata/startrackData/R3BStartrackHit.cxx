// -------------------------------------------------------------------------
// -----                      R3BStartrackHit source file                  -----
// -------------------------------------------------------------------------

#include "R3BStartrackHit.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BStartrackHit::R3BStartrackHit() : FairHit() {
  fEnergy     = fTof    = 0.;
  fDetectorID = -1;
}

// -----   Destructor   ----------------------------------------------------
R3BStartrackHit::~R3BStartrackHit() { }

// -----   Public method Print   -------------------------------------------
void R3BStartrackHit::Print(const Option_t* opt) const {
  cout << "-I- R3BStartrackHit: Tracker Hit in detector " << fDetectorID << endl;
  cout << "    Tof: " << fTof << " cm,  Energy loss "
       << fEnergy*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------

ClassImp(R3BStartrackHit)
