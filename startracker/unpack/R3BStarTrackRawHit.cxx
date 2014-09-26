// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                        R3BStarTrackRawHit                         -----
// -----                           Version 0.1                             -----
// -----                    Created 28.05.2013 by M. Labiche               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include "R3BStarTrackRawHit.h"


R3BStarTrackRawHit::R3BStarTrackRawHit()
: fWordType(0),
fHitBit(0),
fModuleId(0),
fSide(0),
fAsicId(0),
fStripId(0),
fADCdata(0),
fTime(0),
fInfoField(0),
fInfoCode(0)
{
}

R3BStarTrackRawHit::R3BStarTrackRawHit(UShort_t wordtype, UShort_t hitbit, UShort_t moduleId, UShort_t side, UShort_t asicId, UShort_t stripId, UShort_t adc_data, ULong_t time, ULong_t infofield, ULong_t infocode)
: fWordType(wordtype),
fHitBit(hitbit),
fModuleId(moduleId),
fSide(side),
fAsicId(asicId),
fStripId(stripId),
fADCdata(adc_data),
fTime(time),
  fInfoField(infofield),
  fInfoCode(infocode)
{
}


ClassImp(R3BStarTrackRawHit)
