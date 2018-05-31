// ---------------------------------------------------------------------------
// -----                                                                 -----
// -----                      R3BAmsMappedData                           -----
// -----                  Created 12/05/2018 by J.L. Rodriguez           -----
// -----                                                                 -----
// ---------------------------------------------------------------------------

#include "R3BAmsMappedData.h"

R3BAmsMappedData::R3BAmsMappedData()
  : fDetId(0),
    fStripId(0),
    fEnergy(0)
{
}

//------------------------------

R3BAmsMappedData::R3BAmsMappedData(Int_t detId, Int_t stripId, Int_t energy)
  : fDetId(detId),
    fStripId(stripId),
    fEnergy(energy) 
{
}


ClassImp(R3BAmsMappedData)
