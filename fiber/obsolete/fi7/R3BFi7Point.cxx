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

// -------------------------------------------------------------------------
// -----                      R3BFi7Point source file                  -----
// -------------------------------------------------------------------------

#include "R3BFi7Point.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;

// -----   Default constructor   -------------------------------------------
R3BFi7Point::R3BFi7Point()
    : FairMCPoint()
    , fX_out(0.)
    , fY_out(0.)
    , fZ_out(0.)
    , fPx_out(0.)
    , fPy_out(0.)
    , fPz_out(0.)
    , fModule(-1)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BFi7Point::R3BFi7Point(Int_t trackID,
                         Int_t detID,
                         Int_t plane,
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
    , fPx_out(momOut.Px())
    , fPy_out(momOut.Py())
    , fPz_out(momOut.Pz())
    , fModule(plane)
{
}
// -------------------------------------------------------------------------

R3BFi7Point::R3BFi7Point(const R3BFi7Point& right)
    : FairMCPoint(right)
    , fX_out(right.fX_out)
    , fY_out(right.fY_out)
    , fZ_out(right.fZ_out)
    , fPx_out(right.fPx_out)
    , fPy_out(right.fPy_out)
    , fPz_out(right.fPz_out)
    , fModule(right.fModule)
{
}

// -----   Destructor   ----------------------------------------------------
R3BFi7Point::~R3BFi7Point() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void R3BFi7Point::Print(const Option_t* opt) const
{
    cout << "-I- R3BFi7Point: STS Point for track " << fTrackID << " in detector " << fDetectorID << endl;
    cout << "    Position (" << fX << ", " << fY << ", " << fZ << ") cm" << endl;
    cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz << ") GeV" << endl;
    cout << "    Time " << fTime << " ns,  Length " << fLength << " cm,  Energy loss " << fELoss * 1.0e06 << " keV"
         << endl;
}
// -------------------------------------------------------------------------

// -----   Point x coordinate from linear extrapolation   ------------------
Double_t R3BFi7Point::GetX(Double_t z) const
{
    //  cout << fZ << " " << z << " " << fZ_out << endl;
    if ((fZ_out - z) * (fZ - z) >= 0.)
        return (fX_out + fX) / 2.;
    Double_t dz = fZ_out - fZ;
    return (fX + (z - fZ) / dz * (fX_out - fX));
}
// -------------------------------------------------------------------------

// -----   Point y coordinate from linear extrapolation   ------------------
Double_t R3BFi7Point::GetY(Double_t z) const
{
    if ((fZ_out - z) * (fZ - z) >= 0.)
        return (fY_out + fY) / 2.;
    Double_t dz = fZ_out - fZ;
    //  if ( TMath::Abs(dz) < 1.e-3 ) return (fY_out+fY)/2.;
    return (fY + (z - fZ) / dz * (fY_out - fY));
}
// -------------------------------------------------------------------------

// -----   Public method IsUsable   ----------------------------------------
Bool_t R3BFi7Point::IsUsable() const
{
    Double_t dz = fZ_out - fZ;
    if (TMath::Abs(dz) < 1.e-4)
        return kFALSE;
    return kTRUE;
}
// -------------------------------------------------------------------------

ClassImp(R3BFi7Point)
