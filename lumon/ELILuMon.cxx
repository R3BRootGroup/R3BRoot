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

#include "ELILuMon.h"
#include "ELIGeoLuMon.h"
#include "ELIGeoLuMonPar.h"
#include "ELILuMonPoint.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairVolume.h"
#include "R3BMCStack.h"
#include "TClonesArray.h"
#include "TGeoArb8.h"
#include "TGeoBBox.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"
#include "TGeoCone.h"
#include "TGeoMCGeometry.h"
#include "TGeoManager.h"
#include "TGeoMaterial.h"
#include "TGeoMatrix.h"
#include "TGeoMedium.h"
#include "TGeoPara.h"
#include "TGeoPgon.h"
#include "TGeoShapeAssembly.h"
#include "TGeoSphere.h"
#include "TObjArray.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include <iostream>
#include <stdlib.h>

using std::cerr;
using std::cout;
using std::endl;

ELILuMon::ELILuMon()
    : ELILuMon("")
{
}

ELILuMon::ELILuMon(const TString& geoFile, const TGeoTranslation& trans, const TGeoRotation& rot)
    : ELILuMon(geoFile, { trans, rot })
{
}

ELILuMon::ELILuMon(const TString& geoFile, const TGeoCombiTrans& combi)
    : R3BDetector("ELILuMon", kLUMON, geoFile, combi)
    , fLuMonCollection(new TClonesArray("ELILuMonPoint"))
    , fPosIndex(0)
    , kGeoSaved(kFALSE)
    , flGeoPar(new TList())
{
    flGeoPar->SetName(GetName());
    ResetParameters();
}

ELILuMon::~ELILuMon()
{
    if (flGeoPar)
    {
        delete flGeoPar;
    }
    if (fLuMonCollection)
    {
        fLuMonCollection->Delete();
        delete fLuMonCollection;
    }
}

void ELILuMon::Initialize()
{
    FairDetector::Initialize();

    cout << endl;
    cout << "-I- ELILuMon: initialisation" << endl;
    cout << "-I- ELILuMon: Sci. Vol. (McId) " << gMC->VolId("LUMONLog") << endl;
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t ELILuMon::ProcessHits(FairVolume* vol)
{
    Int_t copyNo = -1;
    gMC->CurrentVolID(copyNo);

    // Simple Det PLane

    if (gMC->IsTrackEntering())
    {
        fELoss = 0.;
        fTime = gMC->TrackTime() * 1.0e09;
        fLength = gMC->TrackLength();
        gMC->TrackPosition(fPosIn);
        gMC->TrackMomentum(fMomIn);
    }

    // Sum energy loss for all steps in the active volume
    fELoss += gMC->Edep();

    // Set additional parameters at exit of active volume. Create ELILuMonPoint.
    if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared())
    {
        fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
        fVolumeID = vol->getMCid();
        gMC->TrackPosition(fPosOut);
        gMC->TrackMomentum(fMomOut);
        if (fELoss == 0.)
            return kFALSE;

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

            //       cout << "1st direction: " << olddirection[0] << "," << olddirection[1] << "," << olddirection[2] <<
            //       endl;

            for (Int_t i = 0; i < 3; i++)
            {
                newdirection[i] = -1 * olddirection[i];
            }

            gGeoManager->SetCurrentDirection(newdirection);
            // TGeoNode *bla = gGeoManager->FindNextBoundary(2);
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
               copyNo,
               TVector3(fPosIn.X(), fPosIn.Y(), fPosIn.Z()),
               TVector3(fPosOut.X(), fPosOut.Y(), fPosOut.Z()),
               TVector3(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz()),
               TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
               fTime,
               fLength,
               fELoss);

        // Increment number of LuMonPoints for this track
        R3BStack* stack = (R3BStack*)gMC->GetStack();
        stack->AddPoint(kLUMON);

        ResetParameters();
    }

    return kTRUE;
}
// ----------------------------------------------------------------------------
// void ELILuMon::SaveGeoParams(){
//
//  cout << " -I Save STS geo params " << endl;
//
//  TFolder *mf = (TFolder*) gDirectory->FindObjectAny("cbmroot");
//  cout << " mf: " << mf << endl;
//  TFolder *stsf = NULL;
//  if (mf ) stsf = (TFolder*) mf->FindObjectAny(GetName());
//  cout << " stsf: " << stsf << endl;
//  if (stsf) stsf->Add( flGeoPar0 ) ;
//  FairRootManager::Instance()->WriteFolder();
//  mf->Write("cbmroot",TObject::kWriteDelete);
//}

// -----   Public method EndOfEvent   -----------------------------------------
void ELILuMon::BeginEvent()
{

    //  if (! kGeoSaved ) {
    //      SaveGeoParams();
    //  cout << "-I STS geometry parameters saved " << endl;
    //  kGeoSaved = kTRUE;
    //  }
}
// -----   Public method EndOfEvent   -----------------------------------------
void ELILuMon::EndOfEvent()
{

    if (fVerboseLevel)
        Print();
    fLuMonCollection->Clear();

    ResetParameters();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void ELILuMon::Register() { FairRootManager::Instance()->Register("LuMonPoint", GetName(), fLuMonCollection, kTRUE); }
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray* ELILuMon::GetCollection(Int_t iColl) const
{
    if (iColl == 0)
        return fLuMonCollection;
    else
        return NULL;
}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void ELILuMon::Print(Option_t* option) const
{
    Int_t nHits = fLuMonCollection->GetEntriesFast();
    cout << "-I- ELILuMon: " << nHits << " points registered in this event." << endl;
}
// ----------------------------------------------------------------------------

// -----   Public method Reset   ----------------------------------------------
void ELILuMon::Reset()
{
    fLuMonCollection->Clear();
    ResetParameters();
}
// ----------------------------------------------------------------------------

// -----   Public method CopyClones   -----------------------------------------
void ELILuMon::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
    Int_t nEntries = cl1->GetEntriesFast();
    cout << "-I- ELILuMon: " << nEntries << " entries to add." << endl;
    TClonesArray& clref = *cl2;
    ELILuMonPoint* oldpoint = NULL;
    for (Int_t i = 0; i < nEntries; i++)
    {
        oldpoint = (ELILuMonPoint*)cl1->At(i);
        Int_t index = oldpoint->GetTrackID() + offset;
        oldpoint->SetTrackID(index);
        new (clref[fPosIndex]) ELILuMonPoint(*oldpoint);
        fPosIndex++;
    }
    cout << " -I- ELILuMon: " << cl2->GetEntriesFast() << " merged entries." << endl;
}

// -----   Private method AddHit   --------------------------------------------
ELILuMonPoint* ELILuMon::AddHit(Int_t trackID,
                                Int_t detID,
                                Int_t copy,
                                TVector3 posIn,
                                TVector3 posOut,
                                TVector3 momIn,
                                TVector3 momOut,
                                Double_t time,
                                Double_t length,
                                Double_t eLoss)
{
    TClonesArray& clref = *fLuMonCollection;
    Int_t size = clref.GetEntriesFast();
    if (fVerboseLevel > 1)
        cout << "-I- ELILuMon: Adding Point at (" << posIn.X() << ", " << posIn.Y() << ", " << posIn.Z()
             << ") cm,  detector " << detID << ", track " << trackID << ", energy loss " << eLoss * 1e06 << " keV"
             << endl;
    return new (clref[size]) ELILuMonPoint(trackID, detID, copy, posIn, posOut, momIn, momOut, time, length, eLoss);
}

Bool_t ELILuMon::CheckIfSensitive(std::string name)
{
    if (TString(name).Contains("LUMONLog"))
    {
        cout << "!!!!!!!!!!!!! Works!" << endl;
        return kTRUE;
    }
    return kFALSE;
}

ClassImp(ELILuMon)
