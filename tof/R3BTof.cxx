// -------------------------------------------------------------------------
// -----                        R3BTof source file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------
#include <stdlib.h>

#include "R3BTof.h"

#include "R3BGeoTof.h"
#include "R3BTofPoint.h"
#include "R3BGeoTofPar.h"
#include "R3BMCStack.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairVolume.h"

#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TObjArray.h"

// includes for modeling
#include "TGeoManager.h"

// -----   Default constructor   -------------------------------------------
R3BTof::R3BTof()
    : R3BDetector("R3BTof", kTRUE, kTOF)
{
    ResetParameters();
    fTofCollection = new TClonesArray("R3BTofPoint");
    fPosIndex = 0;
    kGeoSaved = kFALSE;
    flGeoPar = new TList();
    flGeoPar->SetName(GetName());
    fVerboseLevel = 1;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BTof::R3BTof(const char* name, Bool_t active)
: R3BDetector(name, active, kTOF)
{
    ResetParameters();
    fTofCollection = new TClonesArray("R3BTofPoint");
    fPosIndex = 0;
    kGeoSaved = kFALSE;
    flGeoPar = new TList();
    flGeoPar->SetName(GetName());
    fVerboseLevel = 1;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BTof::R3BTof(const char* name,
               TString geoFile,
               Bool_t active,
               Float_t x,
               Float_t y,
               Float_t z,
               Float_t rot_x,
               Float_t rot_y,
               Float_t rot_z)
    : R3BDetector(name, active, kTOF)
{
    ResetParameters();
    SetGeometryFileName(geoFile);
    SetPosition(x, y, z);
    SetRotation(rot_x, rot_y, rot_z);
    fTofCollection = new TClonesArray("R3BTofPoint");
    fPosIndex = 0;
    kGeoSaved = kFALSE;
    flGeoPar = new TList();
    flGeoPar->SetName(GetName());
    fVerboseLevel = 1;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
R3BTof::~R3BTof()
{
    if (flGeoPar)
        delete flGeoPar;
    if (fTofCollection)
    {
        fTofCollection->Delete();
        delete fTofCollection;
    }
}
// -------------------------------------------------------------------------

void R3BTof::Initialize()
{
    FairDetector::Initialize();

    LOG(INFO) << "R3BTof: initialisation" << FairLogger::endl;
    LOG(DEBUG) << "R3BTof: Sci. Vol. (McId) " << gMC->VolId("TOFLog") << FairLogger::endl;
}

void R3BTof::SetSpecialPhysicsCuts()
{
    LOG(INFO) << "-I- R3BTof: Adding customized Physics cut ... " << FairLogger::endl;

    if (gGeoManager)
    {
        TGeoMedium* pSi = gGeoManager->GetMedium("plasticForTOF");
        if (pSi)
        {
            // Setting processes for Si only
            gMC->Gstpar(pSi->GetId(), "LOSS", 3);
            gMC->Gstpar(pSi->GetId(), "STRA", 1.0);
            gMC->Gstpar(pSi->GetId(), "PAIR", 1.0);
            gMC->Gstpar(pSi->GetId(), "COMP", 1.0);
            gMC->Gstpar(pSi->GetId(), "PHOT", 1.0);
            gMC->Gstpar(pSi->GetId(), "ANNI", 1.0);
            gMC->Gstpar(pSi->GetId(), "BREM", 1.0);
            gMC->Gstpar(pSi->GetId(), "HADR", 1.0);
            gMC->Gstpar(pSi->GetId(), "DRAY", 1.0);
            gMC->Gstpar(pSi->GetId(), "DCAY", 1.0);
            gMC->Gstpar(pSi->GetId(), "MULS", 1.0);
            gMC->Gstpar(pSi->GetId(), "RAYL", 1.0);

            // Setting Energy-CutOff for Si Only
            Double_t cutE = fCutE; // GeV-> 1 keV

            LOG(INFO) << "-I- R3BTof: plasticForTOF Medium Id " << pSi->GetId() << " Energy Cut-Off : " << cutE << " GeV" << FairLogger::endl;

            // Si
            gMC->Gstpar(pSi->GetId(), "CUTGAM", cutE); /** gammas (GeV)*/
            gMC->Gstpar(pSi->GetId(), "CUTELE", cutE); /** electrons (GeV)*/
            gMC->Gstpar(pSi->GetId(), "CUTNEU", cutE); /** neutral hadrons (GeV)*/
            gMC->Gstpar(pSi->GetId(), "CUTHAD", cutE); /** charged hadrons (GeV)*/
            gMC->Gstpar(pSi->GetId(), "CUTMUO", cutE); /** muons (GeV)*/
            gMC->Gstpar(pSi->GetId(), "BCUTE", cutE);  /** electron bremsstrahlung (GeV)*/
            gMC->Gstpar(pSi->GetId(), "BCUTM", cutE);  /** muon and hadron bremsstrahlung(GeV)*/
            gMC->Gstpar(pSi->GetId(), "DCUTE", cutE);  /** delta-rays by electrons (GeV)*/
            gMC->Gstpar(pSi->GetId(), "DCUTM", cutE);  /** delta-rays by muons (GeV)*/
            gMC->Gstpar(pSi->GetId(), "PPCUTM", -1.);  /** direct pair production by muons (GeV)*/
        }

    } //!gGeoManager
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BTof::ProcessHits(FairVolume* vol)
{
    // debug
    //    static Int_t evt;

    // Simple Det PLane
    if (gMC->IsTrackEntering())
    {
        fELoss = 0.;
        // fTime   = gMC->TrackTime() * 1.0e09;
        // fLength = gMC->TrackLength();
        fTime_in = gMC->TrackTime() * 1.0e09;
        fLength_in = gMC->TrackLength();
        gMC->TrackPosition(fPosIn);
        gMC->TrackMomentum(fMomIn);
        //    cout << "-I- evt: " << evt++ << " track entering " << endl;
    }

    // Sum energy loss for all steps in the active volume
    fELoss += gMC->Edep();

    // Set additional parameters at exit of active volume. Create R3BTofPoint.
    if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared())
    {
        fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
        fVolumeID = vol->getMCid();
        gMC->TrackPosition(fPosOut);
        gMC->TrackMomentum(fMomOut);
//        if (fELoss == 0.)
//            return kFALSE;

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

            //       cout << "1st direction: " << olddirection[0] << "," << olddirection[1] << "," << olddirection[2] << endl;

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
               TVector3(fPosIn.X(), fPosIn.Y(), fPosIn.Z()),
               TVector3(fPosOut.X(), fPosOut.Y(), fPosOut.Z()),
               TVector3(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz()),
               TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
               fTime,
               fLength,
               fELoss);

        // Increment number of TofPoints for this track
        R3BStack* stack = (R3BStack*)gMC->GetStack();
        stack->AddPoint(kTOF);

        ResetParameters();
    }

    return kTRUE;
}

// ----------------------------------------------------------------------------
// void R3BTof::SaveGeoParams(){
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
void R3BTof::BeginEvent()
{
    //  if (! kGeoSaved ) {
    //      SaveGeoParams();
    //  cout << "-I STS geometry parameters saved " << endl;
    //  kGeoSaved = kTRUE;
    //  }
}

// -----   Public method EndOfEvent   -----------------------------------------
void R3BTof::EndOfEvent()
{
    if (fVerboseLevel)
        Print();
    fTofCollection->Clear();

    ResetParameters();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void R3BTof::Register()
{
    FairRootManager::Instance()->Register("TOFPoint", GetName(), fTofCollection, kTRUE);
}
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BTof::GetCollection(Int_t iColl) const
{
    if (iColl == 0)
        return fTofCollection;
    else
        return NULL;
}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void R3BTof::Print(Option_t* option) const
{
    Int_t nHits = fTofCollection->GetEntriesFast();
    LOG(INFO) << "R3BTof: " << nHits << " points registered in this event" << FairLogger::endl;
}
// ----------------------------------------------------------------------------

// -----   Public method Reset   ----------------------------------------------
void R3BTof::Reset()
{
    fTofCollection->Clear();
    ResetParameters();
}
// ----------------------------------------------------------------------------

// -----   Public method CopyClones   -----------------------------------------
void R3BTof::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
    Int_t nEntries = cl1->GetEntriesFast();
    LOG(INFO) << "R3BTof: " << nEntries << " entries to add" << FairLogger::endl;
    TClonesArray& clref = *cl2;
    R3BTofPoint* oldpoint = NULL;
    for (Int_t i = 0; i < nEntries; i++)
    {
        oldpoint = (R3BTofPoint*)cl1->At(i);
        Int_t index = oldpoint->GetTrackID() + offset;
        oldpoint->SetTrackID(index);
        new (clref[fPosIndex]) R3BTofPoint(*oldpoint);
        fPosIndex++;
    }
    LOG(INFO) << "R3BTof: " << cl2->GetEntriesFast() << " merged entries" << FairLogger::endl;
}

// -----   Private method AddHit   --------------------------------------------
R3BTofPoint*
    R3BTof::AddHit(Int_t trackID, Int_t detID, TVector3 posIn, TVector3 posOut, TVector3 momIn, TVector3 momOut, Double_t time, Double_t length, Double_t eLoss)
{
    TClonesArray& clref = *fTofCollection;
    Int_t size = clref.GetEntriesFast();
    if (fVerboseLevel > 1)
        LOG(INFO) << "R3BTof: Adding Point at (" << posIn.X() << ", " << posIn.Y() << ", " << posIn.Z() << ") cm,  detector " << detID << ", track " << trackID
                  << ", energy loss " << eLoss * 1e06 << " keV" << FairLogger::endl;
    return new (clref[size]) R3BTofPoint(trackID, detID, posIn, posOut, momIn, momOut, time, length, eLoss);
}

// -----   Public method ConstructGeometry   ----------------------------------
void R3BTof::ConstructGeometry()
{
    TString fileName = GetGeometryFileName();
    if (fileName.EndsWith(".root"))
    {
        LOG(INFO) << "Constructing TOF geometry from ROOT file " << fileName.Data() << FairLogger::endl;
        ConstructRootGeometry();
    }
    else
    {
        LOG(FATAL) << "TOF geometry file is not specified" << FairLogger::endl;
        exit(1);
    }
}

Bool_t R3BTof::CheckIfSensitive(std::string name)
{
    if (TString(name).Contains("TOFLog"))
    {
        return kTRUE;
    }
    return kFALSE;
}

ClassImp(R3BTof)
