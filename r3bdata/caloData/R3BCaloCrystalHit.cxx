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
    fNf(-1.),
    fNs(-1.),
    fTime(-1.),
    fCrystalId(-1)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BCaloCrystalHit::R3BCaloCrystalHit(Int_t ident, Double_t energy, Double_t Nf, Double_t Ns, Double_t time) 
  : FairMultiLinkedData(),
    fEnergy(energy),
    fNf(Nf),
    fNs(Ns),
    fTime(time),
    fCrystalId(ident)
{
}
// -------------------------------------------------------------------------



R3BCaloCrystalHit::R3BCaloCrystalHit(const R3BCaloCrystalHit& right)
  : FairMultiLinkedData(right),
    fEnergy(right.fEnergy),
    fNf(right.fNf),
    fNs(right.fNs),
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
  cout << "    Nf = " << fNf << " GeV; fNs =" << fNs << " GeV" << endl;
  cout << "    Time =" << fTime << " ns  " << endl;
}
// -------------------------------------------------------------------------
