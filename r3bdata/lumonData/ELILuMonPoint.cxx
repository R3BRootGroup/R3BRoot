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
// -----                      ELILuMonPoint source file                  -----
// -------------------------------------------------------------------------

#include "ELILuMonPoint.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;

// -----   Default constructor   -------------------------------------------
ELILuMonPoint::ELILuMonPoint()
    : FairMCPoint()
    , fX_out(0.)
    , fY_out(0.)
    , fZ_out(0.)
    , fPx_out(0.)
    , fPy_out(0.)
    , fPz_out(0.)
    , fCrystalNb(-1)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
ELILuMonPoint::ELILuMonPoint(Int_t trackID,
                             Int_t detID,
                             Int_t copy,
                             TVector3 posIn,
                             TVector3 posOut,
                             TVector3 momIn,
                             TVector3 momOut,
                             Double_t tof,
                             Double_t length,
                             Double_t eLoss)
    : FairMCPoint(trackID, detID, posIn, momIn, tof, length, eLoss)
    , fX_out(posOut.X())
    , fY_out(posOut.Y())
    , fZ_out(posOut.Z())
    , fPx_out(momOut.X())
    , fPy_out(momOut.Y())
    , fPz_out(momOut.Z())
    , fCrystalNb(copy)
{
}
// -------------------------------------------------------------------------

ELILuMonPoint::ELILuMonPoint(const ELILuMonPoint& right)
    : FairMCPoint(right)
    , fX_out(right.fX_out)
    , fY_out(right.fY_out)
    , fZ_out(right.fZ_out)
    , fPx_out(right.fPx_out)
    , fPy_out(right.fPy_out)
    , fPz_out(right.fPz_out)
    , fCrystalNb(right.fCrystalNb)
{
}

// -----   Destructor   ----------------------------------------------------
ELILuMonPoint::~ELILuMonPoint() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void ELILuMonPoint::Print(const Option_t* opt) const
{
    cout << "-I- ELILuMonPoint: STS Point for track " << fTrackID << " in detector " << fDetectorID << endl;
    cout << "    Position (" << fX << ", " << fY << ", " << fZ << ") cm" << endl;
    cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz << ") GeV" << endl;
    cout << "    Time " << fTime << " ns,  Length " << fLength << " cm,  Energy loss " << fELoss * 1.0e06 << " keV"
         << endl;
}
// -------------------------------------------------------------------------

// -----   Point x coordinate from linear extrapolation   ------------------
Double_t ELILuMonPoint::GetX(Double_t z) const
{
    //  cout << fZ << " " << z << " " << fZ_out << endl;
    if ((fZ_out - z) * (fZ - z) >= 0.)
        return (fX_out + fX) / 2.;
    Double_t dz = fZ_out - fZ;
    return (fX + (z - fZ) / dz * (fX_out - fX));
}
// -------------------------------------------------------------------------

// -----   Point y coordinate from linear extrapolation   ------------------
Double_t ELILuMonPoint::GetY(Double_t z) const
{
    if ((fZ_out - z) * (fZ - z) >= 0.)
        return (fY_out + fY) / 2.;
    Double_t dz = fZ_out - fZ;
    //  if ( TMath::Abs(dz) < 1.e-3 ) return (fY_out+fY)/2.;
    return (fY + (z - fZ) / dz * (fY_out - fY));
}
// -------------------------------------------------------------------------

// -----   Public method IsUsable   ----------------------------------------
Bool_t ELILuMonPoint::IsUsable() const
{
    Double_t dz = fZ_out - fZ;
    if (TMath::Abs(dz) < 1.e-4)
        return kFALSE;
    return kTRUE;
}
// -------------------------------------------------------------------------

ClassImp(ELILuMonPoint)
