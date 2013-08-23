// -------------------------------------------------------------------------
// -----                      R3BCaloCrystalHit source file                  -----
// -------------------------------------------------------------------------

#include "R3BCaloCrystalHit.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BCaloCrystalHit::R3BCaloCrystalHit()
  : FairMultiLinkedData(),
    fEnergy(-1.),
    fTime(-1.),
    fCrystalId(-1)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BCaloCrystalHit::R3BCaloCrystalHit(Int_t ident, Double_t energy, Double_t time) 
  : FairMultiLinkedData(),
    fEnergy(energy),
    fTime(time),
    fCrystalId(ident)
{
}
// -------------------------------------------------------------------------



R3BCaloCrystalHit::R3BCaloCrystalHit(const R3BCaloCrystalHit& right)
  : FairMultiLinkedData(right),
    fEnergy(right.fEnergy),
    fTime(right.fTime),
    fCrystalId(right.fCrystalId)
{
}



// -----   Destructor   ----------------------------------------------------
R3BCaloCrystalHit::~R3BCaloCrystalHit()
{
}
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void R3BCaloCrystalHit::Print(const Option_t* opt) const {
  cout << "-I- R3BCaloCrystalHit: a calo crystalHit in crystal identifier " << fCrystalId << endl;
  cout << "    Energy = " << fEnergy << " GeV" << endl;
  cout << "    Time " << fTime << " ns  " << endl;
}
// -------------------------------------------------------------------------
