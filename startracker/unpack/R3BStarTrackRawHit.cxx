// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                        R3BStarTrackRawHit                         -----
// -----                           Version 0.1                             -----
// -----                    Created 28.05.2013 by M. Labiche               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include "R3BStarTrackRawHit.h"


R3BStarTrackRawHit::R3BStarTrackRawHit()
  : fWR(0),
    fWRlb(0),
fWordType(0),
fHitBit(0),
fModuleId(0),
fSide(0),
fAsicId(0),
fStripId(0),
fADCdata(0),
fTime(0),
fTimelb(0),
fTimeExt(0),
fTimeExtlb(0),
fInfoField(0),
fInfoCode(0)
{
}

R3BStarTrackRawHit::R3BStarTrackRawHit(ULong_t wr, ULong_t wrlb, UInt_t wordtype, UInt_t hitbit, UInt_t moduleId, UInt_t side, UInt_t asicId, UInt_t stripId, UInt_t adc_data, ULong_t time, ULong_t timelb, ULong_t timeExt, ULong_t timeExtlb, ULong_t infofield, UInt_t infocode)
  : fWR(wr),
fWRlb(wrlb),
fWordType(wordtype),
fHitBit(hitbit),
fModuleId(moduleId),
fSide(side),
fAsicId(asicId),
fStripId(stripId),
fADCdata(adc_data),
fTime(time),
fTimelb(timelb),
fTimeExt(timeExt),
fTimeExtlb(timeExtlb),
fInfoField(infofield),
fInfoCode(infocode)
{
}


ClassImp(R3BStarTrackRawHit)
