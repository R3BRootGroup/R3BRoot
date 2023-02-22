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

#include "R3BCalifaPoint.h"

#include <iostream>

R3BCalifaPoint::R3BCalifaPoint()
    : FairMCPoint()
    , fCrystalId(-1)
    , fNf(0.)
    , fNs(0.)
{
}

R3BCalifaPoint::R3BCalifaPoint(Int_t trackID,
                               Int_t detID,
                               Int_t ident,
                               TVector3 posIn,
                               TVector3 momIn,
                               Double_t tof,
                               Double_t length,
                               Double_t eLoss,
                               Double_t Nf,
                               Double_t Ns,
                               UInt_t EventId)
    : FairMCPoint(trackID, detID, posIn, momIn, tof, length, eLoss, EventId)
    , fCrystalId(ident)
    , fNf(Nf)
    , fNs(Ns)
{
}

R3BCalifaPoint::R3BCalifaPoint(const R3BCalifaPoint& right)
    : FairMCPoint(right)
    , fCrystalId(right.fCrystalId)
    , fNf(right.fNf)
    , fNs(right.fNs)
{
}

R3BCalifaPoint::~R3BCalifaPoint() {}

ClassImp(R3BCalifaPoint);
