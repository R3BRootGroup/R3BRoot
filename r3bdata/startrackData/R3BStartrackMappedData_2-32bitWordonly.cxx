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

// ----------------------------------------------------------------
// -----              R3BStartrackMappedData                    -----
// -----        (replacing  R3BStartrackerDigitHit)           -----
// -----          Created 26-05-2017 by M.Labiche             -----
// ----------------------------------------------------------------

#include "R3BStartrackMappedData.h"

/* If no problem with ucesb:

R3BStartrackMappedData::R3BStartrackMappedData()
    : fLadder(0)
    , fSide(0)
    , fAsic(0)
    , fStrip(0)
    , fEdep(0)
    , fTimeStp(0)
{
}

R3BStartrackMappedData::R3BStartrackMappedData(Int_t ladderId,
                              Int_t side,
                              Int_t asicId,
                              Int_t stripId,
                              Double_t edep,
                              Double_t timestp)
    : fLadder(ladderId)
    , fSide(side)
    , fAsic(asicId)
    , fStrip(stripId)
    , fEdep(edep)
    , fTimeStp(timestp)
{
}

R3BStartrackMappedData::R3BStartrackMappedData(const R3BStartrackMappedData& right)
    : fLadder(right.fLadder)
    , fSide(right.fSide)
    , fAsic(right.fAsic)
    , fStrip(right.fStrip)
    , fEdep(right.fEdep)
    , fTimeStp(right.fTimeStp)
{
}

*/
R3BStartrackMappedData::R3BStartrackMappedData()
    : fData(0)
{
}

R3BStartrackMappedData::R3BStartrackMappedData(Int_t data)
    : fData(data)

{
}

R3BStartrackMappedData::R3BStartrackMappedData(const R3BStartrackMappedData& right)
    : fData(right.fData)
{
}

R3BStartrackMappedData::R3BStartrackMappedData(const R3BStartrackMappedData& right1,
                                               const R3BStartrackMappedData& right2)
    : fData(right1.fData)
    , fData2(right2.fData)
{
}

ClassImp(R3BStartrackMappedData)
