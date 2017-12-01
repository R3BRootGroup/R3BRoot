#include "R3BPspxHitData.h"

R3BPspxHitData::R3BPspxHitData()
    : fDetector(0)
    , fU(0)
    , fV(0)
    , fX(0)
    , fY(0)
    , fSigmaX(0)
    , fSigmaY(0)
    , fEnergy(0)
    , fMultiplicity(0)
    , fMultiplicityX(0)
    , fMultiplicityY(0)
{
}

R3BPspxHitData::R3BPspxHitData(UShort_t detector,
                               Float_t u,
                               Float_t v,
                               Float_t x,
                               Float_t y,
                               Float_t sigma_x,
                               Float_t sigma_y,
                               Float_t energy,
                               UShort_t multiplicity,
                               UShort_t multiplicity_x,
                               UShort_t multiplicity_y)
    : fDetector(detector)
    , fU(u)
    , fV(v)
    , fX(x)
    , fY(y)
    , fSigmaX(sigma_x)
    , fSigmaY(sigma_y)
    , fEnergy(energy)
    , fMultiplicity(multiplicity)
    , fMultiplicityX(multiplicity_x)
    , fMultiplicityY(multiplicity_y)
{
}

ClassImp(R3BPspxHitData)
