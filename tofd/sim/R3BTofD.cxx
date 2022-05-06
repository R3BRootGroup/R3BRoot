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

// ---------------------------------------------------------------
// -----                        R3BTofD                      -----
// -----          Created 18/03/2022 by J.L. Rodriguez       -----
// -----          Updated 05/05/2022 by M. Feijoo            -----
// ---------------------------------------------------------------

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairVolume.h"

#include "R3BLogger.h"
#include "R3BMCStack.h"
#include "R3BTofD.h"
#include "R3BTofdPoint.h"

#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TVirtualMCStack.h"

R3BTofD::R3BTofD()
    : R3BTofD("")
{
}

R3BTofD::R3BTofD(const TString& geoFile, const TGeoTranslation& trans, const TGeoRotation& rot)
    : R3BTofD(geoFile, { trans, rot })
{
}

R3BTofD::R3BTofD(const TString& geoFile, const TGeoCombiTrans& combi)
    : R3BDetector("R3BTofD", kTOFD, geoFile, combi)
    , fTofdCollection(new TClonesArray("R3BTofdPoint"))
    , fPosIndex(0)
{
    ResetParameters();
}

R3BTofD::~R3BTofD()
{
    if (fTofdCollection)
    {
        fTofdCollection->Delete();
        delete fTofdCollection;
    }
}

void R3BTofD::Initialize()
{
    FairDetector::Initialize();
    R3BLOG(INFO, "");
    R3BLOG(DEBUG, "Vol (McId) def " << gMC->VolId("Paddle"));
}

void R3BTofD::SetSpecialPhysicsCuts()
{
    R3BLOG(INFO, "");
    if (gGeoManager)
    {
        TGeoMedium* plastic = gGeoManager->GetMedium("plasticFormTOF");
        if (plastic && 1 == 0)
        {
            // Setting processes for plasticFormTOF only
            gMC->Gstpar(plastic->GetId(), "LOSS", 3);
            gMC->Gstpar(plastic->GetId(), "STRA", 1.0);
            gMC->Gstpar(plastic->GetId(), "PAIR", 1.0);
            gMC->Gstpar(plastic->GetId(), "COMP", 1.0);
            gMC->Gstpar(plastic->GetId(), "PHOT", 1.0);
            gMC->Gstpar(plastic->GetId(), "ANNI", 1.0);
            gMC->Gstpar(plastic->GetId(), "BREM", 1.0);
            gMC->Gstpar(plastic->GetId(), "HADR", 1.0);
            gMC->Gstpar(plastic->GetId(), "DRAY", 1.0);
            gMC->Gstpar(plastic->GetId(), "DCAY", 1.0);
            gMC->Gstpar(plastic->GetId(), "MULS", 1.0);
            gMC->Gstpar(plastic->GetId(), "RAYL", 1.0);

            // Setting Energy-CutOff for plasticFormTOF Only
            Double_t cutE = fCutE; // GeV

            R3BLOG(INFO, "plasticFormTOF Medium Id " << plastic->GetId() << " Energy Cut-Off : " << cutE << " GeV");
            // plastic
            gMC->Gstpar(plastic->GetId(), "CUTGAM", cutE);
            gMC->Gstpar(plastic->GetId(), "CUTELE", cutE);
            gMC->Gstpar(plastic->GetId(), "CUTNEU", cutE);
            gMC->Gstpar(plastic->GetId(), "CUTHAD", cutE);
            gMC->Gstpar(plastic->GetId(), "CUTMUO", cutE);
            gMC->Gstpar(plastic->GetId(), "BCUTE", cutE);
            gMC->Gstpar(plastic->GetId(), "BCUTM", cutE);
            gMC->Gstpar(plastic->GetId(), "DCUTE", cutE);
            gMC->Gstpar(plastic->GetId(), "DCUTM", cutE);
            gMC->Gstpar(plastic->GetId(), "PPCUTM", -1.);
        }
    } //! gGeoManager
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BTofD::ProcessHits(FairVolume* vol)
{
    // Simple Det plane

    if (gMC->IsTrackEntering())
    {
        fELoss = 0.;
        fTime_in = gMC->TrackTime() * 1.0e09;
        fLength_in = gMC->TrackLength();
        gMC->TrackPosition(fPosIn);
        gMC->TrackMomentum(fMomIn);
    }

    // Sum energy loss for all steps in the active volume
    fELoss += gMC->Edep();

    // Set additional parameters at exit of active volume. Create R3BTofdPoint.
    if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared())
    {
        fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
        fVolumeID = vol->getCopyNo();
        gMC->TrackPosition(fPosOut);
        gMC->TrackMomentum(fMomOut);
        if (fELoss == 0.)
            return kFALSE;

        fTime_out = gMC->TrackTime() * 1.0e09; // also in case particle is stopped in detector, or decays...
        fLength_out = gMC->TrackLength();
        fTime = (fTime_out + fTime_in) / 2.;
        fLength = (fLength_out + fLength_in) / 2.;

        if (gMC->IsTrackExiting())
        {
            const Double_t* oldpos;
            const Double_t* olddirection;
            Double_t newpos[3];
            Double_t newdirection[3];
            Double_t safety;

            gGeoManager->FindNode(fPosOut.X(), fPosOut.Y(), fPosOut.Z());
            oldpos = gGeoManager->GetCurrentPoint();
            olddirection = gGeoManager->GetCurrentDirection();

            for (Int_t i = 0; i < 3; i++)
            {
                newdirection[i] = -1 * olddirection[i];
            }

            gGeoManager->SetCurrentDirection(newdirection);
            safety = gGeoManager->GetSafeDistance();

            gGeoManager->SetCurrentDirection(-newdirection[0], -newdirection[1], -newdirection[2]);

            for (Int_t i = 0; i < 3; i++)
            {
                newpos[i] = oldpos[i] - (3 * safety * olddirection[i]);
            }

            fPosOut.SetX(newpos[0]);
            fPosOut.SetY(newpos[1]);
            fPosOut.SetZ(newpos[2]);
        }

        AddHit(fTrackID,
               fVolumeID,
               TVector3(fPosIn.X(), fPosIn.Y(), fPosIn.Z()),
               TVector3(fPosOut.X(), fPosOut.Y(), fPosOut.Z()),
               TVector3(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz()),
               TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
               fTime,
               fLength,
               fELoss);

        // Increment number of TofdPoints for this track
        auto stack = (R3BStack*)gMC->GetStack();
        stack->AddPoint(kTOFD);

        ResetParameters();
    }

    return kTRUE;
}

// -----   Public method EndOfEvent   -----------------------------------------
void R3BTofD::EndOfEvent()
{
    if (fVerboseLevel)
        Print();
    fTofdCollection->Clear();

    ResetParameters();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void R3BTofD::Register()
{
    R3BLOG(DEBUG, "");
    FairRootManager::Instance()->Register("TofDPoint", GetName(), fTofdCollection, kTRUE);
}
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BTofD::GetCollection(Int_t iColl) const
{
    if (iColl == 0)
        return fTofdCollection;
    else
        return NULL;
}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void R3BTofD::Print(Option_t* option) const
{
    Int_t nHits = fTofdCollection->GetEntriesFast();
    LOG(INFO) << "R3BTofD: " << nHits << " points registered in this event";
}
// ----------------------------------------------------------------------------

// -----   Public method Reset   ----------------------------------------------
void R3BTofD::Reset()
{
    fTofdCollection->Clear();
    ResetParameters();
}
// ----------------------------------------------------------------------------

// -----   Public method CopyClones   -----------------------------------------
void R3BTofD::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
    Int_t nEntries = cl1->GetEntriesFast();
    R3BLOG(INFO, nEntries << " entries to add");
    TClonesArray& clref = *cl2;
    R3BTofdPoint* oldpoint = NULL;
    for (Int_t i = 0; i < nEntries; i++)
    {
        oldpoint = (R3BTofdPoint*)cl1->At(i);
        Int_t index = oldpoint->GetTrackID() + offset;
        oldpoint->SetTrackID(index);
        new (clref[fPosIndex]) R3BTofdPoint(*oldpoint);
        fPosIndex++;
    }
    R3BLOG(INFO, cl2->GetEntriesFast() << " merged entries");
}

// -----   Private method AddHit   --------------------------------------------
R3BTofdPoint* R3BTofD::AddHit(Int_t trackID,
                              Int_t detID,
                              TVector3 posIn,
                              TVector3 posOut,
                              TVector3 momIn,
                              TVector3 momOut,
                              Double_t time,
                              Double_t length,
                              Double_t eLoss)
{
    TClonesArray& clref = *fTofdCollection;
    Int_t size = clref.GetEntriesFast();
    if (fVerboseLevel > 1)
    {
        R3BLOG(INFO,
               "at (" << posIn.X() << ", " << posIn.Y() << ", " << posIn.Z() << ") cm,  detector " << detID
                      << ", track " << trackID << ", energy loss " << eLoss * 1e06 << " keV");
    }
    return new (clref[size]) R3BTofdPoint(trackID, detID, posIn, posOut, momIn, momOut, time, length, eLoss);
}

Bool_t R3BTofD::CheckIfSensitive(std::string name)
{
    if (TString(name).Contains("Paddle"))
    {
        LOG(DEBUG) << "Found TofD geometry from ROOT file: " << name;
        return kTRUE;
    }
    return kFALSE;
}

ClassImp(R3BTofD);
