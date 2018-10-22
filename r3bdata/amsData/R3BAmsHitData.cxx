// -------------------------------------------------------------------------
// -----            R3BAmsHitData source file                          -----
// -------------------------------------------------------------------------

#include "R3BAmsHitData.h"
#include "FairLogger.h"
#include <iostream>

// -----   Default constructor   -------------------------------------------
R3BAmsHitData::R3BAmsHitData()
  : FairMultiLinkedData(),
    fDetId(-1),
    fNumHit(0),
    fX(0),
    fY(0),
    fEnergy(0)
{
}

// -----   Standard constructor   ------------------------------------------
R3BAmsHitData::R3BAmsHitData(Int_t detid, Int_t numhit, Double_t x, Double_t y, Double_t energy) 
  : FairMultiLinkedData(),
    fDetId(detid),
    fNumHit(numhit),
    fX(x),
    fY(y),
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
    fEnergy(right.fEnergy)
{
}

// -----   Destructor   ----------------------------------------------------
R3BAmsHitData::~R3BAmsHitData()
{
}

// -----   Public method Print   -------------------------------------------
void R3BAmsHitData::Print(const Option_t* opt) const {
  LOG(INFO) << "-I- R3BAmsHitData: a HitData in detector " << fDetId 
       << " number of hit " << fNumHit 
       << " position [X,Y,Z] = " << fX <<", " << fY << FairLogger::endl;
  LOG(INFO) << " Energy = " << fEnergy << " (GeV in sim)" << FairLogger::endl;
}
// -------------------------------------------------------------------------
