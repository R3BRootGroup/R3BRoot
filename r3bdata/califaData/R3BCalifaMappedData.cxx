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

#include "R3BCalifaMappedData.h"

R3BCalifaMappedData::R3BCalifaMappedData()
    : fCrystalId(0)
    , fEnergy(0)
    , fNf(0)
    , fNs(0)
    , fTime(0)
    , fError(0)
    , fTot(0)
{
}

R3BCalifaMappedData::R3BCalifaMappedData(UShort_t crystalId,
                                         Int_t energy,
                                         Int_t nf,
                                         Int_t ns,
                                         uint64_t time,
                                         UChar_t error,
                                         Int_t tot)
    : fCrystalId(crystalId)
    , fEnergy(energy)
    , fNf(nf)
    , fNs(ns)
    , fTime(time)
    , fError(error)
    , fTot(tot)
{
}

ClassImp(R3BCalifaMappedData)
