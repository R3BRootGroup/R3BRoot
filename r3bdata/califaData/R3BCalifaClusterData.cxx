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

#include "R3BCalifaClusterData.h"

R3BCalifaClusterData::R3BCalifaClusterData(std::vector<int> crystalList,
                                           double ene,
                                           double nf,
                                           double ns,
                                           double theta,
                                           double phi,
                                           ULong64_t time,
                                           uint32_t clusterType)
    : fCrystalList(crystalList)
    , fEnergy(ene)
    , fNf(nf)
    , fNs(ns)
    , fTheta(theta)
    , fPhi(phi)
    , fTime(time)
    , fClusterType(clusterType)
{
}

R3BCalifaClusterData::R3BCalifaClusterData(const R3BCalifaClusterData& right)
    : fCrystalList(right.fCrystalList)
    , fEnergy(right.fEnergy)
    , fNf(right.fNf)
    , fNs(right.fNs)
    , fTheta(right.fTheta)
    , fPhi(right.fPhi)
    , fTime(right.fTime)
    , fClusterType(right.fClusterType)
{
}

ClassImp(R3BCalifaClusterData);
