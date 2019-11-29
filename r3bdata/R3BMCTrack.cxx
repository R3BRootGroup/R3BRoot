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
// -----                      R3BMCTrack source file                   -----
// -----                  Created 03/08/04                             -----
// -------------------------------------------------------------------------
#include "R3BMCTrack.h"

#include "FairLogger.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
R3BMCTrack::R3BMCTrack()
    : fPdgCode(0)
    , fMotherId(-1)
    , fPx(0.)
    , fPy(0.)
    , fPz(0.)
    , fStartX(0.)
    , fStartY(0.)
    , fStartZ(0.)
    , fStartT(0.)
    , fNPoints({})
    , fMass(0.)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BMCTrack::R3BMCTrack(Int_t pdgCode,
                       Int_t motherId,
                       Double_t px,
                       Double_t py,
                       Double_t pz,
                       Double_t x,
                       Double_t y,
                       Double_t z,
                       Double_t t,
                       std::array<int, kLAST + 1> nPoints)
    : fPdgCode(pdgCode)
    , fMotherId(motherId)
    , fPx(px)
    , fPy(py)
    , fPz(pz)
    , fStartX(x)
    , fStartY(y)
    , fStartZ(z)
    , fStartT(t)
    , fNPoints(nPoints)
    , fMass(0.)
{
}
// -------------------------------------------------------------------------

// -----   Copy constructor   ----------------------------------------------
R3BMCTrack::R3BMCTrack(const R3BMCTrack& right)
    : fPdgCode(right.fPdgCode)
    , fMotherId(right.fMotherId)
    , fPx(right.fPx)
    , fPy(right.fPy)
    , fPz(right.fPz)
    , fStartX(right.fStartX)
    , fStartY(right.fStartY)
    , fStartZ(right.fStartZ)
    , fStartT(right.fStartT)
    , fNPoints(right.fNPoints)
    , fMass(right.fMass)
{
}
// -------------------------------------------------------------------------

// -----   Constructor from TParticle   ------------------------------------
R3BMCTrack::R3BMCTrack(TParticle* part, Int_t fMC)
    : fPdgCode(part->GetPdgCode())
    , fMotherId(part->GetMother(0))
    , fPx(part->Px())
    , fPy(part->Py())
    , fPz(part->Pz())
    , fStartX(part->Vx())
    , fStartY(part->Vy())
    , fStartZ(part->Vz())
    , fStartT(0.)
    , fNPoints({})
    , fMass(part->GetMass())
{
    if (fMC == 0)
    {
        // G3
        fStartT = part->T() * 1e09;
    }
    else if (fMC == 1)
    {
        // G4
        fStartT = part->T();
    }
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
R3BMCTrack::~R3BMCTrack() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void R3BMCTrack::Print(Option_t* option) const
{
    cout << "Track " << option << ", mother : " << fMotherId << ", Type " << fPdgCode << ", momentum (" << fPx << ", "
         << fPy << ", " << fPz << ") GeV" << endl;
    cout << "       Ref " << GetNPoints(kREF) << ", DCH " << GetNPoints(kDCH) << ", CAL " << GetNPoints(kCAL)
         << ", LAND " << GetNPoints(kLAND) << ", GFI " << GetNPoints(kGFI) << ", mTOF " << GetNPoints(kMTOF)
         << ", dTOF " << GetNPoints(kDTOF) << ", TOF " << GetNPoints(kTOF) << ", TRACKER " << GetNPoints(kTRA)
         << ", CALIFA " << GetNPoints(kCALIFA) << ", MFI " << GetNPoints(kMFI) << ", PSP " << GetNPoints(kPSP)
         << ", VETO " << GetNPoints(kVETO) << ", STARTRACK " << GetNPoints(kSTARTRACK) << ", LUMON "
         << GetNPoints(kLUMON) << ", NeuLAND " << GetNPoints(kNEULAND) << endl;
#ifdef SOFIA
    cout << ", SCI " << GetNPoints(kSOFSCI) << ", AT " << GetNPoints(kSOFAT) << ", TRIM " << GetNPoints(kSOFTRIM)
         << ", MWPC1 " << GetNPoints(kSOFMWPC1) << ", TWIM " << GetNPoints(kSOFTWIM) << ", MWPC2 "
         << GetNPoints(kSOFMWPC2) << ", SOF ToF Wall " << GetNPoints(kSOFTofWall) << endl;
#endif
#ifdef GTPC
    cout << ", GTPC " << GetNPoints(kGTPC) << endl;
#endif
}
// -------------------------------------------------------------------------

// -----   Public method GetMass   -----------------------------------------
Double_t R3BMCTrack::GetMass() const
{
    //  if ( TDatabasePDG::Instance() ) {
    //    TParticlePDG* particle = TDatabasePDG::Instance()->GetParticle(fPdgCode);
    //    if ( particle ) return particle->Mass();
    //    else return 0.;
    //  }
    //  return 0.;
    return fMass;
}
// -------------------------------------------------------------------------

// -----   Public method GetRapidity   -------------------------------------
Double_t R3BMCTrack::GetRapidity() const
{
    Double_t e = GetEnergy();
    Double_t y = 0.5 * TMath::Log((e + fPz) / (e - fPz));
    return y;
}
// -------------------------------------------------------------------------

// -----   Public method GetNPoints   --------------------------------------
Int_t R3BMCTrack::GetNPoints(DetectorId detId) const
{
    if (detId < 0 || detId >= fNPoints.size())
    {
        LOG(ERROR) << "Unknown detector ID " << detId;
        return 0;
    }
    return fNPoints.at(detId);
}
// -------------------------------------------------------------------------

// -----   Public method SetNPoints   --------------------------------------
void R3BMCTrack::SetNPoints(Int_t iDet, Int_t nP)
{
    if (iDet < 0 || iDet >= fNPoints.size())
    {
        LOG(ERROR) << "Unknown detector ID " << iDet;
        return;
    }
    fNPoints[iDet] = nP;
}
// -------------------------------------------------------------------------

ClassImp(R3BMCTrack)
