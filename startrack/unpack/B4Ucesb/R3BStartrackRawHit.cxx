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
// -----                                                                   -----
// -----                        R3BStartrackRawHit                         -----
// -----                           Version 0.1                             -----
// -----                    Created 28.05.2013 by M. Labiche               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include "R3BStartrackRawHit.h"

R3BStartrackRawHit::R3BStartrackRawHit()
    : fWRvhb(0)
    , fWRhb(0)
    , fWRlb(0)
    , fWordType(0)
    , fHitBit(0)
    , fModuleId(0)
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
    , fTime47lb(0)
    , fInfoField(0)
    , fInfoCode(0)
{
}

// R3BStartrackRawHit::R3BStartrackRawHit(UInt_t wrvhb, UInt_t wrhb, UInt_t wrlb, UInt_t wordtype, UInt_t hitbit, UInt_t
// moduleId, UInt_t side, UInt_t asicId, UInt_t stripId, UInt_t adc_data, UInt_t timevhb, UInt_t timehb, UInt_t timelb,
// UInt_t timeExtvhb, UInt_t timeExthb, UInt_t timeExtlb, UInt_t infofield, UInt_t infocode)
R3BStartrackRawHit::R3BStartrackRawHit(UInt_t wrvhb,
                                       UInt_t wrhb,
                                       UInt_t wrlb,
                                       UInt_t wordtype,
                                       UInt_t hitbit,
                                       UInt_t moduleId,
                                       UInt_t side,
                                       UInt_t asicId,
                                       UInt_t stripId,
                                       UInt_t adc_data,
                                       ULong_t timevhb,
                                       ULong_t timehb,
                                       UInt_t timelb,
                                       UInt_t timeExtvhb,
                                       UInt_t timeExthb,
                                       UInt_t timeExtlb,
                                       UInt_t timecode47lb,
                                       UInt_t infofield,
                                       UInt_t infocode)
    : fWRvhb(wrvhb)
    , fWRhb(wrhb)
    , fWRlb(wrlb)
    , fWordType(wordtype)
    , fHitBit(hitbit)
    , fModuleId(moduleId)
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
    , fTime47lb(timecode47lb)
    , fInfoField(infofield)
    , fInfoCode(infocode)
{
}

ClassImp(R3BStartrackRawHit)
