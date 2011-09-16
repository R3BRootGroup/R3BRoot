// -------------------------------------------------------------------------
// -----                      R3BCaloCrystalHit source file                  -----
// -----               Modified 05/09/11 by Enrico Fiori                  -----
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
				     Double_t energy, Double_t time, Double_t theta, Double_t phi, Double_t rho) 
  : FairMultiLinkedData() {
  
  fCrystalId = ident;
  fCrystalType = type;
  fCrystalCopy = copy;
  fEnergy = energy;
  fTime = time;
  fCrystalTheta = theta ;  // EF 05/09/11
  fCrystalPhi   = phi   ;  // EF 05/09/11
  fCrystalRho   = rho   ;  // EF 05/09/11

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
