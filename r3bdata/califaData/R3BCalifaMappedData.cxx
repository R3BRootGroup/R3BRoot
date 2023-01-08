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

#include "R3BCalifaMappedData.h"

R3BCalifaMappedData::R3BCalifaMappedData()
    : fCrystalId(0)
    , fEnergy(0)
    , fNf(0)
    , fNs(0)
    , fFebexTime(0)
    , fWrts(0)
    , fOverFlow(0)
    , fPileup(0)
    , fDiscard(0)
    , fTot(0)
{
}

R3BCalifaMappedData::R3BCalifaMappedData(UShort_t crystalId,
                                         int16_t energy,
                                         int16_t nf,
                                         int16_t ns,
                                         uint64_t febextime,
                                         uint64_t wrts,
                                         uint32_t ov,
                                         uint16_t pu,
                                         uint16_t dc,
                                         uint16_t tot)
    : fCrystalId(crystalId)
    , fEnergy(energy)
    , fNf(nf)
    , fNs(ns)
    , fFebexTime(febextime)
    , fWrts(wrts)
    , fOverFlow(ov)
    , fPileup(pu)
    , fDiscard(dc)
    , fTot(tot)
{
}

ClassImp(R3BCalifaMappedData);
