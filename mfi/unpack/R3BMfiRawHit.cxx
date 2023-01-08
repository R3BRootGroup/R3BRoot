/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

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

R3BMfiRawHit::R3BMfiRawHit(UInt_t nxtId,
                           UInt_t gemexId,
                           UInt_t trigger,
                           ULong_t triggerTs,
                           UInt_t adc,
                           UInt_t channelId,
                           UInt_t nxtTs,
                           UInt_t epoch,
                           UInt_t index)
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
