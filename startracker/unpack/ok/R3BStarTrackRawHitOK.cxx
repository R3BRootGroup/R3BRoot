// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                        R3BStarTrackRawHit                         -----
// -----                           Version 0.1                             -----
// -----                    Created 28.05.2013 by M. Labiche               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include "R3BStarTrackRawHit.h"


R3BStarTrackRawHit::R3BStarTrackRawHit()
: fModuleId(0),
fSide(0),
fAsicId(0),
fStripId(0),
fADCdata(0),
fTime(0)
{
}

R3BStarTrackRawHit::R3BStarTrackRawHit(UShort_t moduleId, UShort_t side, UShort_t asicId, UShort_t stripId, UShort_t adc_data, ULong_t time)
: fModuleId(moduleId),
fSide(side),
fAsicId(asicId),
fStripId(stripId),
fADCdata(adc_data),
fTime(time)
{
}


ClassImp(R3BStarTrackRawHit)
