// ---------------------------------------------------------------------------------------
// -----                                                                             -----
// -----                       R3BStarTrackRawHitMapped                              -----
// -----                       from R3BLandRawHitMapped                              -----
// -----                 Created @ 03.2014 by M. Labiche                             -----
// -----                                                                             -----
// ---------------------------------------------------------------------------------------

#include "R3BStarTrackRawHitMapped.h"

R3BStarTrackRawHitMapped::R3BStarTrackRawHitMapped()
    : fModuleId(0)
    , fSide(0)
    , fAsicId(asicId)
    , fStripID(stripId)
{
}

R3BStarTrackHitMapped::R3BStarTrackRawHitMapped( UShort_t adc_data, ULong_t time, UShort_t moduleId, UShort_t side, UShort_t asicId, UShort_t stripId)
  : R3BStarTrackRawHit(adc_data, time)
    , fModuleId(moduleId)
    , fSide(side)
    , fAsicId(asicId)
    , fStripID(stripId)
{
}

ClassImp(R3BStarTrackRawHitMapped)
