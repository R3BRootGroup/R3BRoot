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

#include "R3BCalifaHitData.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;

R3BCalifaHitData::R3BCalifaHitData()
    : FairMultiLinkedData()
    , fNbOfCrystalHits(0)
    , fEnergy(NAN)
    , fTheta(NAN)
    , fPhi(NAN)
    , fTime(0)
{
}

R3BCalifaHitData::R3BCalifaHitData(UInt_t Nb,
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

R3BCalifaHitData::R3BCalifaHitData(const R3BCalifaHitData& right)
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

R3BCalifaHitData::~R3BCalifaHitData() {}

void R3BCalifaHitData::Print(const Option_t* opt) const
{
    cout << "-I- R3BCalifaHitData: a CALIFA Hit made of " << fNbOfCrystalHits << " crystalHits. Energy = " << fEnergy
         << " GeV" << endl;
    cout << "    Theta(polar) " << fTheta << " deg  " << endl;
    cout << "    Phi(azimuthal) " << fPhi << " deg  " << endl;
}
// -------------------------------------------------------------------------
