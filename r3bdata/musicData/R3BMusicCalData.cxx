// -------------------------------------------------------------------------
// -----                      R3BMusicCalData source file              -----
// -------------------------------------------------------------------------

#include "R3BMusicCalData.h"

// -----   Default constructor   -------------------------------------------
R3BMusicCalData::R3BMusicCalData()
    : fAnodeID(0)
    , fDT(0.)
    , fEnergy(0.)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BMusicCalData::R3BMusicCalData(UShort_t anodeID, Double_t dt, Double_t energy)
    : fAnodeID(anodeID)
    , fDT(dt)
    , fEnergy(energy)
{
}
// -------------------------------------------------------------------------

ClassImp(R3BMusicCalData)
