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

#include "R3BMwpcPoint.h"

#include "FairLogger.h"

// -----   Default constructor   -------------------------------------------
R3BMwpcPoint::R3BMwpcPoint()
    : FairMCPoint()
{
    fX_out = fY_out = fZ_out = 0.;
    fPx_out = fPy_out = fPz_out = 0.;
}

// -----   Standard constructor   ------------------------------------------
R3BMwpcPoint::R3BMwpcPoint(Int_t trackID,
                           Int_t detID,
                           Int_t detCopyID,
                           TVector3 posIn,
                           TVector3 posOut,
                           TVector3 momIn,
                           TVector3 momOut,
                           Double_t tof,
                           Double_t length,
                           Double_t eLoss)
    : FairMCPoint(trackID, detID, posIn, momIn, tof, length, eLoss)
{
    fDetCopyID = detCopyID;
    fX_out = posOut.X();
    fY_out = posOut.Y();
    fZ_out = posOut.Z();
    fPx_out = momOut.Px();
    fPy_out = momOut.Py();
    fPz_out = momOut.Pz();
}

// -----   Destructor   ----------------------------------------------------
R3BMwpcPoint::~R3BMwpcPoint() {}

// -----   Public method Print   -------------------------------------------
void R3BMwpcPoint::Print(const Option_t* opt) const
{
    LOG(INFO) << "R3BMwpcPoint: Point for track " << fTrackID << " in detector " << fDetectorID << "\n"
              << "    Position (" << fX << ", " << fY << ", " << fZ << ") cm \n"
              << "    Momentum (" << fPx << ", " << fPy << ", " << fPz << ") GeV \n"
              << "    Time " << fTime << " ns,  Length " << fLength << " cm,  Energy loss " << fELoss * 1.0e06
              << " keV";
}

// -----   Point x coordinate from linear extrapolation   ------------------
Double_t R3BMwpcPoint::GetX(Double_t z) const
{
    //  cout << fZ << " " << z << " " << fZ_out << endl;
    if ((fZ_out - z) * (fZ - z) >= 0.)
        return (fX_out + fX) / 2.;
    Double_t dz = fZ_out - fZ;
    return (fX + (z - fZ) / dz * (fX_out - fX));
}

// -----   Point y coordinate from linear extrapolation   ------------------
Double_t R3BMwpcPoint::GetY(Double_t z) const
{
    if ((fZ_out - z) * (fZ - z) >= 0.)
        return (fY_out + fY) / 2.;
    Double_t dz = fZ_out - fZ;
    //  if ( TMath::Abs(dz) < 1.e-3 ) return (fY_out+fY)/2.;
    return (fY + (z - fZ) / dz * (fY_out - fY));
}

// -----   Public method IsUsable   ----------------------------------------
Bool_t R3BMwpcPoint::IsUsable() const
{
    Double_t dz = fZ_out - fZ;
    if (TMath::Abs(dz) < 1.e-4)
        return kFALSE;
    return kTRUE;
}

ClassImp(R3BMwpcPoint);