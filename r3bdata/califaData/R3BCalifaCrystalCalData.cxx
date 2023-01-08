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

#include "R3BCalifaCrystalCalData.h"

R3BCalifaCrystalCalData::R3BCalifaCrystalCalData()
    : FairMultiLinkedData()
    , fEnergy(-1.)
    , fNf(-1.)
    , fNs(-1.)
    , fToT_Energy(-1.)
    , fTime(0)
    , fCrystalId(-1)
{
}

R3BCalifaCrystalCalData::R3BCalifaCrystalCalData(Int_t ident,
                                                 Double_t energy,
                                                 Double_t Nf,
                                                 Double_t Ns,
                                                 uint64_t time,
                                                 Double_t tot_energy)
    : FairMultiLinkedData()
    , fEnergy(energy)
    , fNf(Nf)
    , fNs(Ns)
    , fToT_Energy(tot_energy)
    , fTime(time)
    , fCrystalId(ident)
{
}

R3BCalifaCrystalCalData::R3BCalifaCrystalCalData(const R3BCalifaCrystalCalData& right)
    : FairMultiLinkedData(right)
    , fEnergy(right.fEnergy)
    , fNf(right.fNf)
    , fNs(right.fNs)
    , fToT_Energy(right.fToT_Energy)
    , fTime(right.fTime)
    , fCrystalId(right.fCrystalId)
{
}

ClassImp(R3BCalifaCrystalCalData);
