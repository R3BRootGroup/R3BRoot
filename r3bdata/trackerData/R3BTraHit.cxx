// -------------------------------------------------------------------------
// -----                      R3BTraHit source file                  -----
// -------------------------------------------------------------------------

#include "R3BTraHit.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BTraHit::R3BTraHit() : FairHit() {
  fEnergy     = fTof    = 0.;
  fDetectorID = -1;
}

// -----   Destructor   ----------------------------------------------------
R3BTraHit::~R3BTraHit() { }

// -----   Public method Print   -------------------------------------------
void R3BTraHit::Print(const Option_t* opt) const {
  cout << "-I- R3BTraHit: Tracker Hit in detector " << fDetectorID << endl;
  cout << "    Tof: " << fTof << " cm,  Energy loss "
       << fEnergy*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------

ClassImp(R3BTraHit)
