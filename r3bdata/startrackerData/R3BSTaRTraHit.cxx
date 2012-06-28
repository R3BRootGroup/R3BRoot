// -------------------------------------------------------------------------
// -----                      R3BSTaRTraHit source file                  -----
// -------------------------------------------------------------------------

#include "R3BSTaRTraHit.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BSTaRTraHit::R3BSTaRTraHit() : FairHit() {
  fEnergy     = fTof    = 0.;
  fDetectorID = -1;
}

// -----   Destructor   ----------------------------------------------------
R3BSTaRTraHit::~R3BSTaRTraHit() { }

// -----   Public method Print   -------------------------------------------
void R3BSTaRTraHit::Print(const Option_t* opt) const {
  cout << "-I- R3BSTaRTraHit: Tracker Hit in detector " << fDetectorID << endl;
  cout << "    Tof: " << fTof << " cm,  Energy loss "
       << fEnergy*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------

ClassImp(R3BSTaRTraHit)
