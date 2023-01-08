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

// -----------------------------------------------------------------------------
// -----                       R3BStratrackMappedData                      -----
// -----                           Version 0.1                             -----
// -----                   Created 15/5/2017 by M.Labiche                  -----
// -----                   based on initial R3B unpacker:                  -----
// -----                startrack/unpack/StartrackRawHit class             -----
// -----------------------------------------------------------------------------

#include "R3BStartrackMappedData.h"

R3BStartrackMappedData::R3BStartrackMappedData()
    : fTS(0)
    , fTSExt(0)
    ,
    // fWRvhb(0),
    // fWRhb(0),
    // fWRlb(0),
    fWordtype(0)
    , fHitbit(0)
    , fLadderId(0)
    , fSide(0)
    , fAsicId(0)
    , fStripId(0)
    , fADCdata(0)
    , fTimevhb(0)
    , fTimehb(0)
    , fTimelb(0)
    , fTimeExtvhb(0)
    , fTimeExthb(0)
    , fTimeExtlb(0)
    , fInfoCode(0)
    , fInfoField(0)
{
}

//------------------------------
/**
 *
 *
 */
// R3BStartrackMappedData::R3BStartrackMappedData(UInt_t wrvhb, UInt_t wrhb, UInt_t wrlb, Int_t wordtype, Int_t hitbit,
// Int_t ladderId, Int_t side, Int_t asicId, Int_t stripId, UInt_t adc_data, UInt_t timevhb, UInt_t timehb, UInt_t
// timelb, UInt_t timeExtvhb, UInt_t timeExthb, UInt_t timeExtlb, Int_t infofield, Int_t infocode)
R3BStartrackMappedData::R3BStartrackMappedData(ULong_t ts,
                                               ULong_t tsext,
                                               Int_t wordtype,
                                               Int_t hitbit,
                                               Int_t ladderId,
                                               Int_t side,
                                               Int_t asicId,
                                               Int_t stripId,
                                               UInt_t adc_data,
                                               UInt_t timevhb,
                                               UInt_t timehb,
                                               UInt_t timelb,
                                               UInt_t timeExtvhb,
                                               UInt_t timeExthb,
                                               UInt_t timeExtlb,
                                               Int_t infofield,
                                               Int_t infocode)
    : fTS(ts)
    , fTSExt(tsext)
    ,
    // fWRvhb(wrvhb),
    // fWRhb(wrhb),
    // fWRlb(wrlb),
    fWordtype(wordtype)
    , fHitbit(hitbit)
    , fLadderId(ladderId)
    , fSide(side)
    , fAsicId(asicId)
    , fStripId(stripId)
    , fADCdata(adc_data)
    , fTimevhb(timevhb)
    , fTimehb(timehb)
    , fTimelb(timelb)
    , fTimeExtvhb(timeExtvhb)
    , fTimeExthb(timeExthb)
    , fTimeExtlb(timeExtlb)
    , fInfoCode(infocode)
    , fInfoField(infofield)
{
}

ClassImp(R3BStartrackMappedData)
