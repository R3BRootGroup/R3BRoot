// ---------------------------------------------------------------------------------------
// -----                               R3BMfiRawHit                                  -----
// -----                      Created 30-07-2014 by D.Kresan                         -----
// ---------------------------------------------------------------------------------------

#include "R3BMfiRawHit.h"

R3BMfiRawHit::R3BMfiRawHit()
    : fNxtId(0)
    , fGemexId(0)
    , fTrigger(0)
    , fTriggerTs(0)
    , fAdc(0)
    , fChannelId(0)
    , fNxtTs(0)
    , fEpoch(0)
    , fIndex(0)
{
}

R3BMfiRawHit::R3BMfiRawHit(UInt_t nxtId, UInt_t gemexId, UInt_t trigger, ULong_t triggerTs, UInt_t adc, UInt_t channelId, UInt_t nxtTs, UInt_t epoch, UInt_t index)
    : fNxtId(nxtId)
    , fGemexId(gemexId)
    , fTrigger(trigger)
    , fTriggerTs(triggerTs)
    , fAdc(adc)
    , fChannelId(channelId)
    , fNxtTs(nxtTs)
    , fEpoch(epoch)
    , fIndex(index)
{
}

ClassImp(R3BMfiRawHit)
