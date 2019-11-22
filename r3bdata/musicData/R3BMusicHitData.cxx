// -------------------------------------------------------------------------
// -----                      R3BMusicHitData source file              -----
// -------------------------------------------------------------------------

#include "R3BMusicHitData.h"

// -----   Default constructor   -------------------------------------------
R3BMusicHitData::R3BMusicHitData()
    : fTheta(0.)
    , fZ(0.)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BMusicHitData::R3BMusicHitData(Double_t theta, Double_t z)
    : fTheta(theta)
    , fZ(z)
{
}
// -------------------------------------------------------------------------

ClassImp(R3BMusicHitData)
