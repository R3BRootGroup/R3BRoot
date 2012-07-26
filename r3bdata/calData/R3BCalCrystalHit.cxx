// -------------------------------------------------------------------------
// -----                 R3BCalCrystalHit source file                  -----
// -------------------------------------------------------------------------

#include "R3BCalCrystalHit.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BCalCrystalHit::R3BCalCrystalHit() : FairMultiLinkedData() {
  fCrystalNb = -1;
  fEnergy = -1; 
  fTime = -1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BCalCrystalHit::R3BCalCrystalHit(Int_t crysnb, Double_t energy, Double_t time) 
  : FairMultiLinkedData() {
  
  fCrystalNb     = crysnb;
  fEnergy        = energy;
  fTime          = time;

}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BCalCrystalHit::~R3BCalCrystalHit() { }
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void R3BCalCrystalHit::Print(const Option_t* opt) const {
  cout << "-I- R3BCalCrystalHit: a calo crystalHit in crystal number " << fCrystalNb << endl;
  cout << "    Energy = " << fEnergy << " GeV" << endl;
  cout << "    Time " << fTime << " ns  " << endl;
}
// -------------------------------------------------------------------------
