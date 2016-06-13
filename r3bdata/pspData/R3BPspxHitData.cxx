
#include "R3BPspxHitData.h"

// for the data analysis of the s438b PSPX detectors. 
// Introduced by Ina Syndikus, April 2016


R3BPspxHitData::R3BPspxHitData()
    : fDetector(0)
    , fU(0)
    , fV(0)
    , fX(0)
    , fY(0)
    , fEnergy(0)
    , fMultiplicity(0)
{
}

R3BPspxHitData::R3BPspxHitData(UChar_t detector,
				Float_t u,
				Float_t v,
				Float_t x,
				Float_t y,
				Float_t energy,
				UInt_t multiplicity)
    : fDetector(detector)
    , fU(u)
    , fV(v)
    , fX(x)
    , fY(y)
    , fEnergy(energy)
    , fMultiplicity(multiplicity)
{
}

ClassImp(R3BPspxHitData)
