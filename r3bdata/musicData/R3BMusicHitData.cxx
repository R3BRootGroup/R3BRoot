// -------------------------------------------------------------------------
// -----                      R3BMusicHitData source file              -----
// -------------------------------------------------------------------------

#include "R3BMusicHitData.h"


// -----   Default constructor   -------------------------------------------
R3BMusicHitData::R3BMusicHitData()
: fSecID(0),
  fTheta(0.),
  fZ(0.) 
{
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
R3BMusicHitData::R3BMusicHitData(UChar_t secID, Double_t theta, 
                                                    Double_t z) 
  : fSecID(secID)
  , fTheta(theta)
  , fZ(z)
{
}
// -------------------------------------------------------------------------

ClassImp(R3BMusicHitData)
