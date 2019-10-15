// -------------------------------------------------------------------------
// -----                      R3BMusicMappedData source file           -----
// -------------------------------------------------------------------------

#include "R3BMusicMappedData.h"


// -----   Default constructor   -------------------------------------------
R3BMusicMappedData::R3BMusicMappedData() 
: fSecID(0),
  fAnodeID(0),
  fTime(0),
  fEnergy(0) 
{
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
R3BMusicMappedData::R3BMusicMappedData(UChar_t secID, UChar_t anodeID,
					     UShort_t time, UShort_t energy) 
  : fSecID(secID)
  , fAnodeID(anodeID)
  , fTime(time)
  , fEnergy(energy)
{
}
// -------------------------------------------------------------------------

ClassImp(R3BMusicMappedData)
