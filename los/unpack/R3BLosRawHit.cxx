// ---------------------------------------------------------------------------------------
// -----                                R3BLosRawHit                                 -----
// -----                      Created 14-04-2014 by D.Kresan                         -----
// ---------------------------------------------------------------------------------------

#include "R3BLosRawHit.h"

R3BLosRawHit::R3BLosRawHit()
    : fChannel(0)
    , fTdc(0)
    , fClock(0)
{
}

R3BLosRawHit::R3BLosRawHit(UShort_t channel, UShort_t tdc, UShort_t clock)
    : fChannel(channel)
    , fTdc(tdc)
    , fClock(clock)
{
}

ClassImp(R3BLosRawHit)
