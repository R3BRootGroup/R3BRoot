// -------------------------------------------------------------------------
// -----                 R3BXBallCrystalHit source file                  -----
// -------------------------------------------------------------------------

#include "R3BXBallCrystalHit.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BXBallCrystalHit::R3BXBallCrystalHit() : FairMultiLinkedData() {
  fCrystalNb = -1;
  fEnergy = -1; 
  fTime = -1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BXBallCrystalHit::R3BXBallCrystalHit(Int_t crysnb, Double_t energy, Double_t time) 
  : FairMultiLinkedData() {
  
  fCrystalNb     = crysnb;
  fEnergy        = energy;
  fTime          = time;

}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BXBallCrystalHit::~R3BXBallCrystalHit() { }
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void R3BXBallCrystalHit::Print(const Option_t* opt) const {
  cout << "-I- R3BXBallCrystalHit: a calo crystalHit in crystal number " << fCrystalNb << endl;
  cout << "    Energy = " << fEnergy << " GeV" << endl;
  cout << "    Time " << fTime << " ns  " << endl;
}
// -------------------------------------------------------------------------
