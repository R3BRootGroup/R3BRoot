// -------------------------------------------------------------------------
// -----                      R3BCaloCrystalHit source file                  -----
// -------------------------------------------------------------------------

#include "R3BCaloCrystalHit.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BCaloCrystalHit::R3BCaloCrystalHit() : FairMultiLinkedData() {
  fCrystalType = fCrystalCopy = fCrystalId = -1;
  fEnergy = fTime = -1; 
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BCaloCrystalHit::R3BCaloCrystalHit(Int_t type, Int_t copy, Int_t ident,
									 Double_t energy, Double_t time) 
  : FairMultiLinkedData() {
  
  fCrystalId = ident;
  fCrystalType = type;
  fCrystalCopy = copy;
  fEnergy = energy;
  fTime = time;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BCaloCrystalHit::~R3BCaloCrystalHit() { }
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void R3BCaloCrystalHit::Print(const Option_t* opt) const {
  cout << "-I- R3BCaloCrystalHit: a calo crystalHit in crystal type " << fCrystalType 
       << " and copy  " << fCrystalCopy << ". Uniqued crystal identifier: " << fCrystalId << endl;
  cout << "    Energy = " << fEnergy << " GeV" << endl;
  cout << "    Time " << fTime << " ns  " << endl;
}
// -------------------------------------------------------------------------
