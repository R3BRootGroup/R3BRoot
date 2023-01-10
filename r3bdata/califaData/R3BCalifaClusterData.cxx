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

#include "R3BCalifaClusterData.h"

R3BCalifaClusterData::R3BCalifaClusterData()
    : FairMultiLinkedData()
    , fNbOfCrystalHits(0)
    , fEnergy(NAN)
    , fTheta(NAN)
    , fPhi(NAN)
    , fTime(0)
{
}

R3BCalifaClusterData::R3BCalifaClusterData(UInt_t Nb,
                                           Double_t ene,
                                           Double_t nf,
                                           Double_t ns,
                                           Double_t theta,
                                           Double_t phi,
                                           ULong64_t time)
    : FairMultiLinkedData()
    , fNbOfCrystalHits(Nb)
    , fEnergy(ene)
    , fNf(nf)
    , fNs(ns)
    , fTheta(theta)
    , fPhi(phi)
    , fTime(time)
{
}

R3BCalifaClusterData::R3BCalifaClusterData(const R3BCalifaClusterData& right)
    : FairMultiLinkedData(right)
    , fNbOfCrystalHits(right.fNbOfCrystalHits)
    , fEnergy(right.fEnergy)
    , fNf(right.fNf)
    , fNs(right.fNs)
    , fTheta(right.fTheta)
    , fPhi(right.fPhi)
    , fTime(right.fTime)
{
}

ClassImp(R3BCalifaClusterData);
