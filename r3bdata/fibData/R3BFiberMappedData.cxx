/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ----------------------------------------------------------------------
// -----              R3BFiberMappedData header file                -----
// -----         Created 13/04/22 by J.L. Rodriguez-Sanchez         -----
// ----------------------------------------------------------------------

#include "R3BFiberMappedData.h"

R3BFiberMappedData::R3BFiberMappedData()
    : fSide(0)
    , fChannel(0)
    , fIsLeading()
    , fCoarse(-1)
    , fFine(-1)
{
}

R3BFiberMappedData::R3BFiberMappedData(UInt_t side,
                                       UInt_t channel,
                                       Bool_t is_leading,
                                       Int_t coarse,
                                       Int_t fine)
    : fSide(side)
    , fChannel(channel)
    , fIsLeading(is_leading)
    , fCoarse(coarse)
    , fFine(fine)
{
}


UInt_t R3BFiberMappedData::GetChannel() const { return fChannel; }

Int_t R3BFiberMappedData::GetCoarse() const { return fCoarse; }

Int_t R3BFiberMappedData::GetFine() const { return fFine; }

UInt_t R3BFiberMappedData::GetSide() const { return fSide; }

Bool_t R3BFiberMappedData::IsBottom() const { return 1 == fSide; }

Bool_t R3BFiberMappedData::IsTop() const { return 2 == fSide; }

Bool_t R3BFiberMappedData::IsTrigger() const { return 3 == fSide; }

Bool_t R3BFiberMappedData::IsLeading() const { return fIsLeading; }

Bool_t R3BFiberMappedData::IsTrailing() const { return !fIsLeading; }

Bool_t R3BFiberMappedData::IsMAPMT() const { return 1 == fSide; }

Bool_t R3BFiberMappedData::IsSPMT() const { return 2 == fSide; }

Bool_t R3BFiberMappedData::IsMAPMTTrigger() const { return 3 == fSide; }

Bool_t R3BFiberMappedData::IsSPMTTrigger() const { return 4 == fSide; }

ClassImp(R3BFiberMappedData);
