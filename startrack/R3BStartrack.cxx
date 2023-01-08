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

#include "R3BStartrack.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairVolume.h"
#include "R3BGeoStartrack.h"
#include "R3BGeoStartrackPar.h"
#include "R3BMCStack.h"
#include "R3BStartrackPoint.h"
#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TGeoManager.h"
#include "TObjArray.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include <stdlib.h>

R3BStartrack::R3BStartrack()
    : R3BStartrack("")
{
}

R3BStartrack::R3BStartrack(const TString& geoFile, const TGeoTranslation& trans, const TGeoRotation& rot)
    : R3BStartrack(geoFile, { trans, rot })
{
}

R3BStartrack::R3BStartrack(const TString& geoFile, const TGeoCombiTrans& combi)
    : R3BDetector("R3BStartrack", kSTARTRACK, geoFile, combi)
    , fTraCollection(new TClonesArray("R3BStartrackPoint"))
    , fPosIndex(0)
    , kGeoSaved(kFALSE)
    , flGeoPar(new TList())
{
    flGeoPar->SetName(GetName());
    ResetParameters();
}

R3BStartrack::~R3BStartrack()
{
    if (flGeoPar)
    {
        delete flGeoPar;
    }
    if (fTraCollection)
    {
        fTraCollection->Delete();
        delete fTraCollection;
    }
}

// ----   Initialize   -----------------------------------------------------
void R3BStartrack::Initialize()
{
    FairDetector::Initialize();

    LOG(info) << "R3BStartrack: initialisation";
    //    LOG(debug) << "R3BStartrack: Sens. Vol. (McId) " << gMC->VolId("StartrackLog1");
    LOG(debug) << "R3BStartrack: Sens. Vol. (McId) " << gMC->VolId("STaRTraLog1");
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BStartrack::ProcessHits(FairVolume* vol)
{
    //      cout << " -I process hit called for:" <<  vol->GetName() << endl;
    // Set parameters at entrance of volume. Reset ELoss.

    //    if ( vol ) {
    //        cout << " Name Id:copy "
    //            << vol->getName() << " : " << vol->getMCid() << " : " << vol->getCopyNo() << endl;
    //        Int_t copyNo=0;
    //        cout << " Geant: " << gMC->CurrentVolID(copyNo) << ":" << copyNo << endl;
    //    }

    if (gMC->IsTrackEntering())
    {
        fELoss = 0.;
        fTime = gMC->TrackTime() * 1.0e09;
        fLength = gMC->TrackLength();
        gMC->TrackPosition(fPosIn);
        gMC->TrackMomentum(fMomIn);
        //    cout << "X,Y,X tracker=" << fPosIn(0) << " " << fPosIn(1) << " " << fPosIn(2) << endl;
        //    cout << "track length=" << fLength << endl;
        //    cout << "Volume name=" <<  vol->getRealName()<< endl;
    }

    // Sum energy loss for all steps in the active volume
    fELoss += gMC->Edep();
    //    cout << "Tracker Eloss=" << fELoss << "  " << gMC->Edep() << endl;

    // Set additional parameters at exit of active volume. Create R3BStartrackPoint.
    if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared())
    {
        fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
        fVolumeID = vol->getMCid();    // getCopyNo(); //getMCid();
        fDetCopyID = vol->getCopyNo(); // added by Marc
        //     cout << "VolumeID=" <<  vol->getMCid()<< endl;
        //     cout << "DetCopyID=" <<  vol->getCopyNo()<< endl;
        //      cout << "DetMotherID=" <<  vol->getMotherId()<< endl;
        //      cout << "Volume name=" <<  vol->getRealName()<< endl;
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
            //   TGeoNode *bla = gGeoManager->FindNextBoundary(2);
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
               fDetCopyID, // fdetCopyID, added by Marc
               TVector3(fPosIn.X(), fPosIn.Y(), fPosIn.Z()),
               TVector3(fPosOut.X(), fPosOut.Y(), fPosOut.Z()),
               TVector3(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz()),
               TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
               fTime,
               fLength,
               fELoss);

        // Increment number of StartrackPoints for this track
        R3BStack* stack = (R3BStack*)gMC->GetStack();
        stack->AddPoint(kSTARTRACK);

        ResetParameters();
    }

    return kTRUE;
}

// ----------------------------------------------------------------------------
// void R3BStartrack::SaveGeoParams(){
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
void R3BStartrack::BeginEvent()
{
    //  cout << "-I- begin tracker event called ##########" << endl;

    //  if(gGeoManager){
    //    TGeoVolume * vol=gGeoManager->FindVolumeFast("StartrackLog1");

    //    if(vol){
    //           cout << "id tracker serial number : " << vol->GetNumber() << endl;
    //    }
    //  }

    //  if (! kGeoSaved ) {
    //      SaveGeoParams();
    //  cout << "-I STS geometry parameters saved " << endl;
    //  kGeoSaved = kTRUE;
    //  }
}

// -----   Public method EndOfEvent   -----------------------------------------
void R3BStartrack::EndOfEvent()
{
    if (fVerboseLevel)
        Print();
    fTraCollection->Clear();

    ResetParameters();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void R3BStartrack::Register()
{
    FairRootManager::Instance()->Register("StartrackPoint", GetName(), fTraCollection, kTRUE);
}
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BStartrack::GetCollection(Int_t iColl) const
{
    if (iColl == 0)
        return fTraCollection;
    else
        return NULL;
}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void R3BStartrack::Print(Option_t* option) const
{
    Int_t nHits = fTraCollection->GetEntriesFast();
    LOG(info) << "R3BStartrack: " << nHits << " points registered in this event";
}
// ----------------------------------------------------------------------------

// -----   Public method Reset   ----------------------------------------------
void R3BStartrack::Reset()
{
    fTraCollection->Clear();
    ResetParameters();
}
// ----------------------------------------------------------------------------

// -----   Public method CopyClones   -----------------------------------------
void R3BStartrack::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
    Int_t nEntries = cl1->GetEntriesFast();
    LOG(info) << "R3BStartrack: " << nEntries << " entries to add";
    TClonesArray& clref = *cl2;
    R3BStartrackPoint* oldpoint = NULL;
    for (Int_t i = 0; i < nEntries; i++)
    {
        oldpoint = (R3BStartrackPoint*)cl1->At(i);
        Int_t index = oldpoint->GetTrackID() + offset;
        oldpoint->SetTrackID(index);
        new (clref[fPosIndex]) R3BStartrackPoint(*oldpoint);
        fPosIndex++;
    }
    LOG(info) << "R3BStartrack: " << cl2->GetEntriesFast() << " merged entries";
}

// -----   Private method AddHit   --------------------------------------------
R3BStartrackPoint* R3BStartrack::AddHit(Int_t trackID,
                                        Int_t detID,
                                        Int_t detCopyID,
                                        TVector3 posIn, // Int_t detCopyID added by Marc
                                        TVector3 posOut,
                                        TVector3 momIn,
                                        TVector3 momOut,
                                        Double_t time,
                                        Double_t length,
                                        Double_t eLoss)
{
    TClonesArray& clref = *fTraCollection;
    Int_t size = clref.GetEntriesFast();
    if (fVerboseLevel > 1)
        LOG(info) << "R3BStartrack: Adding Point at (" << posIn.X() << ", " << posIn.Y() << ", " << posIn.Z()
                  << ") cm,  detector " << detID << ", track " << trackID << ", energy loss " << eLoss * 1e06 << " keV";
    return new (clref[size]) R3BStartrackPoint(
        trackID, detID, detCopyID, posIn, posOut, momIn, momOut, time, length, eLoss); // detCopy added by Marc
}

Bool_t R3BStartrack::CheckIfSensitive(std::string name)
{
    //    if (TString(name).Contains("StartrackLog"))
    if (TString(name).Contains("STaRTraLog"))
    {
        return kTRUE;
    }
    return kFALSE;
}

ClassImp(R3BStartrack)
