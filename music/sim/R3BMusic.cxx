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
// -----                        R3BMusic source file                   -----
// -----                  Created 08/10/19  by JL Rodriguez            -----
// -------------------------------------------------------------------------

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairVolume.h"

#include "R3BLogger.h"
#include "R3BMCStack.h"
#include "R3BMusic.h"
#include "R3BMusicPoint.h"

#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TVirtualMCStack.h"

#define U_MEV 931.4940954

// -----   Default constructor
R3BMusic::R3BMusic()
    : R3BMusic("")
{
}

R3BMusic::R3BMusic(const TString& geoFile, const TGeoTranslation& trans, const TGeoRotation& rot)
    : R3BMusic(geoFile, { trans, rot })
{
}

// -----   Standard constructor
R3BMusic::R3BMusic(const TString& geoFile, const TGeoCombiTrans& combi)
    : R3BDetector("R3BMusic", kMUSIC, geoFile, combi)
    , fMusicCollection(new TClonesArray("R3BMusicPoint"))
    , fPosIndex(0)
{
    ResetParameters();
}

R3BMusic::~R3BMusic()
{
    if (fMusicCollection)
    {
        fMusicCollection->Delete();
        delete fMusicCollection;
    }
}

void R3BMusic::Initialize()
{
    FairDetector::Initialize();

    R3BLOG(info, "");
    R3BLOG(debug, "Vol (McId) def " << gMC->VolId("Anode"));
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BMusic::ProcessHits(FairVolume* vol)
{

    if (gMC->IsTrackEntering())
    {
        gGeoManager->cd(gMC->CurrentVolPath());
        Int_t nodeId = gGeoManager->GetNodeId();
    }
    if (gMC->IsTrackEntering())
    {
        fELoss = 0.;
        fNSteps = 0; // FIXME
        fTime = gMC->TrackTime() * 1.0e09;
        fLength = gMC->TrackLength();
        gMC->TrackPosition(fPosIn);
        gMC->TrackMomentum(fMomIn);
        fEinc = gMC->Etot() - gMC->TrackMass(); // be aware!! Relativistic mass!
    }

    // Sum energy loss for all steps in the active volume
    Double_t dE = gMC->Edep() * 1000.;                          // in MeV
    Double_t post_E = (gMC->Etot() - gMC->TrackMass()) * 1000.; // in MeV
    TString ptype = gMC->GetStack()->GetCurrentTrack()->GetName();

    Double_t M_in = gMC->TrackMass() * 1000.;
    Double_t fA_in = M_in / U_MEV;
    Double_t fZ_in = gMC->TrackCharge();

    fELoss += dE / 1000.; // back to GeV

    if (dE > 0)
    {

        fNSteps++;

        // Set additional parameters at exit of active volume. Create R3BMusicPoint.
        if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared())
        {

            fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
            fParentTrackID = gMC->GetStack()->GetCurrentParentTrackNumber();
            fVolumeID = vol->getMCid();
            fDetCopyID = vol->getCopyNo();
            fTrackPID = gMC->TrackPid();
            fUniqueID = gMC->GetStack()->GetCurrentTrack()->GetUniqueID();

            gMC->TrackPosition(fPosOut);
            gMC->TrackMomentum(fMomOut);

            if (fELoss == 0.)
                return kFALSE;

            AddPoint(fTrackID,
                     fVolumeID,
                     fDetCopyID,
                     fZ_in,
                     fA_in,
                     TVector3(fPosIn.X(), fPosIn.Y(), fPosIn.Z()),
                     TVector3(fPosOut.X(), fPosOut.Y(), fPosOut.Z()),
                     TVector3(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz()),
                     TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
                     fTime,
                     fLength,
                     fELoss);

            // Increment number of MusicPoints for this track
            R3BStack* stack = (R3BStack*)gMC->GetStack();
            stack->AddPoint(kMUSIC);

            ResetParameters();
        }
    }

    return kTRUE;
}

// -----   Public method EndOfEvent   -----------------------------------------
void R3BMusic::EndOfEvent()
{
    if (fVerboseLevel)
        Print();

    fMusicCollection->Clear();
    ResetParameters();
}

// -----   Public method Register   -------------------------------------------
void R3BMusic::Register()
{
    R3BLOG(debug, "");
    FairRootManager::Instance()->Register("MusicPoint", GetName(), fMusicCollection, kTRUE);
}

// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BMusic::GetCollection(Int_t iColl) const
{
    if (iColl == 0)
        return fMusicCollection;
    else
        return NULL;
}

// -----   Public method Print   ----------------------------------------------
void R3BMusic::Print(Option_t* option) const
{
    Int_t nHits = fMusicCollection->GetEntriesFast();
    LOG(info) << "R3BMusic: " << nHits << " points registered in this event";
}

// -----   Public method Reset   ----------------------------------------------
void R3BMusic::Reset()
{
    fMusicCollection->Clear();
    ResetParameters();
}

// -----   Public method CopyClones   -----------------------------------------
void R3BMusic::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
    Int_t nEntries = cl1->GetEntriesFast();
    R3BLOG(info, nEntries << " entries to add");
    TClonesArray& clref = *cl2;
    R3BMusicPoint* oldpoint = NULL;
    for (Int_t i = 0; i < nEntries; i++)
    {
        oldpoint = (R3BMusicPoint*)cl1->At(i);
        Int_t index = oldpoint->GetTrackID() + offset;
        oldpoint->SetTrackID(index);
        new (clref[fPosIndex]) R3BMusicPoint(*oldpoint);
        fPosIndex++;
    }
    R3BLOG(info, cl2->GetEntriesFast() << " merged entries");
}

// -----   Private method AddPoint   --------------------------------------------
R3BMusicPoint* R3BMusic::AddPoint(Int_t trackID,
                                  Int_t detID,
                                  Int_t volid,
                                  Double_t Z,
                                  Double_t A,
                                  TVector3 posIn,
                                  TVector3 posOut,
                                  TVector3 momIn,
                                  TVector3 momOut,
                                  Double_t time,
                                  Double_t length,
                                  Double_t eLoss)
{
    TClonesArray& clref = *fMusicCollection;
    Int_t size = clref.GetEntriesFast();
    if (fVerboseLevel > 1)
    {
        R3BLOG(info,
               "at (" << posIn.X() << ", " << posIn.Y() << ", " << posIn.Z() << ") cm,  detector " << detID
                      << ", track " << trackID << ", energy loss " << eLoss * 1e06 << " keV");
    }
    return new (clref[size])
        R3BMusicPoint(trackID, detID, volid, Z, A, posIn, posOut, momIn, momOut, time, length, eLoss);
}

// -----  Public method CheckIfSensitive  ----------------------------------
Bool_t R3BMusic::CheckIfSensitive(std::string name)
{
    if (TString(name).Contains("Anode"))
    {
        LOG(debug) << "Found MUSIC geometry from ROOT file: " << name;
        return kTRUE;
    }
    return kFALSE;
}

ClassImp(R3BMusic);
