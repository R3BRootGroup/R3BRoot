// ---------------------------------------------------------------------------------------
// -----                                                                             -----
// -----                       R3BLandRawHitMapped                                   -----
// -----                           Version 0.1                                       -----
// -----                 Created @ 03.2014 by M.I. Cherciu                           -----
// -----                                                                             -----
// ---------------------------------------------------------------------------------------

#include "R3BLandRawHitMapped.h"

R3BLandRawHitMapped::R3BLandRawHitMapped()
    : fIs17(kFALSE)
    , fBarId(0)
    , fSide(0)
    , fClock(0)
    , fTacData(0)
    , fQdcData(0)
{
}

R3BLandRawHitMapped::R3BLandRawHitMapped(Bool_t is17, Int_t barId, Int_t side, Int_t clock, Int_t tacData, Int_t qdcData)
    : fIs17(is17)
    , fBarId(barId)
    , fSide(side)
    , fClock(clock)
    , fTacData(tacData)
    , fQdcData(qdcData)
{
}

ClassImp(R3BLandRawHitMapped)
