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

#include "R3BCalifaPoint.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;

R3BCalifaPoint::R3BCalifaPoint()
    : FairMCPoint()
    ,
    fCrystalId(-1)
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
    , fNs(Nf)
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

void R3BCalifaPoint::Print(const Option_t* opt) const
{
    cout << "R3BCalifaPoint: CALIFA Point for track " << fTrackID << " in crystal " << fCrystalId << endl;
    cout << "    Position (" << fX << ", " << fY << ", " << fZ << ") cm" << endl;
    cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz << ") GeV" << endl;
    cout << "    Time " << fTime << " ns,  Length " << fLength << " cm,  Energy loss " << fELoss * 1.0e06 << " keV"
         << endl;
    cout << "Nf " << fNf << ", Ns " << fNs << endl;
}

ClassImp(R3BCalifaPoint);
