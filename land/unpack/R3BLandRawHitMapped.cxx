// ---------------------------------------------------------------------------------------
// -----                                                                             -----
// -----                       R3BLandRawHitMapped                                   -----
// -----                           Version 0.1                                       -----
// -----                 Created @ 03.2014 by M.I. Cherciu                           -----
// -----                                                                             -----
// ---------------------------------------------------------------------------------------

#include "R3BLandRawHitMapped.h"

R3BLandRawHitMapped::R3BLandRawHitMapped()
    : fBarId(0)
    , fSide(0)
    , fIs17(kFALSE)
{
}

R3BLandRawHitMapped::R3BLandRawHitMapped(Int_t sam, Int_t gtb, Int_t tacAddr, Int_t cal, Int_t clock, Int_t tacData, Int_t qdcData, Int_t barId, Int_t side, Bool_t is17)
    : R3BLandRawHit(sam, gtb, tacAddr, 0, cal, clock, tacData, qdcData)
    , fBarId(barId)
    , fSide(side)
    , fIs17(is17)
{
}

ClassImp(R3BLandRawHitMapped)
