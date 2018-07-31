// -------------------------------------------------------------------------
// -----            R3BAmsHitData source file                -----
// -------------------------------------------------------------------------

#include "R3BAmsHitData.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;


// -----   Default constructor   -------------------------------------------
R3BAmsHitData::R3BAmsHitData()
  : FairMultiLinkedData(),
    fDetId(-1),
    fNumHit(0),
    fX(0),
    fY(0),
    fZ(0),
    fEnergy(0)
{
}

// -----   Standard constructor   ------------------------------------------
R3BAmsHitData::R3BAmsHitData(Int_t detid, Int_t numhit, Double_t x, Double_t y, Double_t z, Double_t energy) 
  : FairMultiLinkedData(),
    fDetId(detid),
    fNumHit(numhit),
    fX(x),
    fY(y),
    fZ(z),
    fEnergy(energy)
{
}
// -------------------------------------------------------------------------

R3BAmsHitData::R3BAmsHitData(const R3BAmsHitData& right)
  : FairMultiLinkedData(right),
    fDetId(right.fDetId),
    fNumHit(right.fNumHit),
    fX(right.fX),
    fY(right.fY),
    fZ(right.fZ),
    fEnergy(right.fEnergy)
{
}

// -----   Destructor   ----------------------------------------------------
R3BAmsHitData::~R3BAmsHitData()
{
}

// -----   Public method Print   -------------------------------------------
void R3BAmsHitData::Print(const Option_t* opt) const {
  cout << "-I- R3BAmsHitData: a HitData in detector " << fDetId 
       << " number of hit " << fNumHit 
       << " position [X,Y,Z] = " << fX <<", " << fY <<", " << fZ << endl;
  cout << " Energy = " << fEnergy << " (GeV in sim)" << endl;
}
// -------------------------------------------------------------------------
