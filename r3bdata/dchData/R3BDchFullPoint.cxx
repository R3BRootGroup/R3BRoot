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

// -------------------------------------------------------------------------
// -----                      R3BDchFullPoint source file                  -----
// -------------------------------------------------------------------------

#include "R3BDchFullPoint.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;

// -----   Default constructor   -------------------------------------------
R3BDchFullPoint::R3BDchFullPoint()
    : FairMCPoint()
    , fLocalX(0.)
    , fLocalY(0.)
    , fLocalZ(0.)
    , fLocalPx(0.)
    , fLocalPy(0.)
    , fLocalPz(0.)
    , fModule(-1)
    , fLayer(-1)
    , fCell(-1)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BDchFullPoint::R3BDchFullPoint(Int_t trackId,
                                 Int_t mod,
                                 Int_t layer,
                                 Int_t cell,
                                 TVector3 pos,
                                 TVector3 lpos,
                                 TVector3 mom,
                                 TVector3 lmom,
                                 Double_t tof,
                                 Double_t length,
                                 Double_t eLoss)
    : FairMCPoint(trackId, mod, pos, mom, tof, length, eLoss)
    , fLocalX(lpos.X())
    , fLocalY(lpos.Y())
    , fLocalZ(lpos.Z())
    , fLocalPx(lmom.X())
    , fLocalPy(lmom.Y())
    , fLocalPz(lmom.Z())
    , fModule(mod)
    , fLayer(layer)
    , fCell(cell)
{
}
// -------------------------------------------------------------------------

R3BDchFullPoint::R3BDchFullPoint(const R3BDchFullPoint& right)
    : FairMCPoint(right)
    , fLocalX(right.fLocalX)
    , fLocalY(right.fLocalY)
    , fLocalZ(right.fLocalZ)
    , fLocalPx(right.fLocalPx)
    , fLocalPy(right.fLocalPy)
    , fLocalPz(right.fLocalPz)
    , fModule(right.fModule)
    , fLayer(right.fLayer)
    , fCell(right.fCell)
{
}

// -----   Destructor   ----------------------------------------------------
R3BDchFullPoint::~R3BDchFullPoint() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void R3BDchFullPoint::Print(const Option_t* opt) const
{
    cout << "-I- R3BDchFullPoint: STS Point for track " << fTrackID << " in detector " << fDetectorID << endl;
    cout << "    Position (" << fX << ", " << fY << ", " << fZ << ") cm" << endl;
    cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz << ") GeV" << endl;
    cout << "    Time " << fTime << " ns,  Length " << fLength << " cm,  Energy loss " << fELoss * 1.0e06 << " keV"
         << endl;
}
// -------------------------------------------------------------------------

// -----   Public method IsUsable   ----------------------------------------
Bool_t R3BDchFullPoint::IsUsable() const { return kTRUE; }
// -------------------------------------------------------------------------

ClassImp(R3BDchFullPoint)
