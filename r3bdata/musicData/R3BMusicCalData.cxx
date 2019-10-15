// -------------------------------------------------------------------------
// -----                      R3BMusicCalData source file              -----
// -------------------------------------------------------------------------

#include "R3BMusicCalData.h"


// -----   Default constructor   -------------------------------------------
R3BMusicCalData::R3BMusicCalData()
: fSecID(0),
  fAnodeID(0),
  fDT(0.),
  fEnergy(0.) 
{
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
R3BMusicCalData::R3BMusicCalData(UChar_t secID, UChar_t anodeID,
			                Double_t dt, Double_t energy) 
  : fSecID(secID)
  , fAnodeID(anodeID)
  , fDT(dt)
  , fEnergy(energy)
{
}
// -------------------------------------------------------------------------

ClassImp(R3BMusicCalData)
