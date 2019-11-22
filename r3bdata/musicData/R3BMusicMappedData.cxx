// -------------------------------------------------------------------------
// -----                      R3BMusicMappedData source file           -----
// -------------------------------------------------------------------------

#include "R3BMusicMappedData.h"

// -----   Default constructor   -------------------------------------------
R3BMusicMappedData::R3BMusicMappedData()
    : fAnodeID(0)
    , fTime(0)
    , fEnergy(0)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BMusicMappedData::R3BMusicMappedData(UShort_t anodeID, UShort_t time, UShort_t energy)
    : fAnodeID(anodeID)
    , fTime(time)
    , fEnergy(energy)
{
}
// -------------------------------------------------------------------------

ClassImp(R3BMusicMappedData)
