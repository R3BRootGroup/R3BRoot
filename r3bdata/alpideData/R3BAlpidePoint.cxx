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
// -----                      R3BAlpidePoint source file               -----
// -------------------------------------------------------------------------

#include "R3BAlpidePoint.h"

#include <iostream>

// -----   Default constructor   -------------------------------------------
R3BAlpidePoint::R3BAlpidePoint()
    : FairMCPoint()
{
    fSensorID = 0;
    fX_out = fY_out = fZ_out = fEloss = 0.;
    fPx_out = fPy_out = fPz_out = 0.;
    fPid = 0;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BAlpidePoint::R3BAlpidePoint(Int_t trackID,
                               Int_t detID,
                               Int_t sensorID,
                               TVector3 posIn,
                               TVector3 posOut,
                               TVector3 momIn,
                               TVector3 momOut,
                               Double_t tof,
                               Double_t length,
                               Double_t eLoss,
                               Int_t PId)
    : FairMCPoint(trackID, detID, posIn, momIn, tof, length, eLoss)
{
    fSensorID = sensorID;
    fX_out = posOut.X();
    fY_out = posOut.Y();
    fZ_out = posOut.Z();
    fEloss = eLoss;
    fPid = PId;
    fPx_out = momOut.Px();
    fPy_out = momOut.Py();
    fPz_out = momOut.Pz();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
R3BAlpidePoint::~R3BAlpidePoint() {}
// -------------------------------------------------------------------------

// -----   Point x coordinate from linear extrapolation   ------------------
Double_t R3BAlpidePoint::GetX(Double_t z) const
{
    if ((fZ_out - z) * (fZ - z) >= 0.)
        return (fX_out + fX) / 2.;
    Double_t dz = fZ_out - fZ;
    return (fX + (z - fZ) / dz * (fX_out - fX));
}
// -------------------------------------------------------------------------

// -----   Point y coordinate from linear extrapolation   ------------------
Double_t R3BAlpidePoint::GetY(Double_t z) const
{
    if ((fZ_out - z) * (fZ - z) >= 0.)
        return (fY_out + fY) / 2.;
    Double_t dz = fZ_out - fZ;
    return (fY + (z - fZ) / dz * (fY_out - fY));
}
// -------------------------------------------------------------------------

// -----   Public method IsUsable   ----------------------------------------
Bool_t R3BAlpidePoint::IsUsable() const
{
    Double_t dz = fZ_out - fZ;
    if (TMath::Abs(dz) < 1.e-4)
        return kFALSE;
    return kTRUE;
}
// -------------------------------------------------------------------------

ClassImp(R3BAlpidePoint);
