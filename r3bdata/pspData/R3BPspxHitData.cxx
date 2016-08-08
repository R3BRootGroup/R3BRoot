
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
    , fXMultiplicity(0)
    , fYMultiplicity(0)
{
}

R3BPspxHitData::R3BPspxHitData(UShort_t detector,
				Float_t u,
				Float_t v,
				Float_t x,
				Float_t y,
				Float_t energy,
				UInt_t multiplicity,
				UInt_t x_multiplicity,
			        UInt_t y_multiplicity )
    : fDetector(detector)
    , fU(u)
    , fV(v)
    , fX(x)
    , fY(y)
    , fEnergy(energy)
    , fMultiplicity(multiplicity)
    , fXMultiplicity(x_multiplicity)
    , fYMultiplicity(y_multiplicity)
{
}

ClassImp(R3BPspxHitData)
