// -------------------------------------------------------------------------
// -----                  R3BXBall source file                           -----
// -----            Created 26/03/09  by D.Bertini                     -----
// -----        new Version: 20/07/09 <D.Bertini@gsi.de>               -----
// -----        new Version: 08/04/10 <wranne@student.chalmers.se>     -----
// -----        new Version: 25/11/10 <marc.labiche@stfc.ac.uk>        -----
// -----        new Version: 08/06/12 <pablo.cabanelas@usc.es>         -----
// -------------------------------------------------------------------------
#include <stdlib.h>

#include "R3BXBall.h"

#include "R3BGeoXBall.h"
#include "R3BXBallPoint.h"
#include "R3BXBallCrystalHit.h"
#include "R3BXBallCrystalHitSim.h"
#include "R3BGeoXBallPar.h"
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
#include "TRotation.h"

// includes for modeling
#include "TGeoManager.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TVector3.h"
#include "TGeoMatrix.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoArb8.h"
#include "TGeoBBox.h"
#include "TGeoPgon.h"
#include "TGeoCone.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"
#include "TGeoTube.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"
// try the mathematical ROOT package
#include "Math/Rotation3D.h"
#include "Math/Point3D.h"



// -----   Default constructor   -------------------------------------------
R3BXBall::R3BXBall() : R3BDetector("R3BXBall", kTRUE, kCAL) {
  ResetParameters();
  fXBallCollection = new TClonesArray("R3BXBallPoint");
  fXBallCrystalHitCollection = new TClonesArray("R3BXBallCrystalHitSim");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  kDrawCrystals = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fGeoVersion = 1;
  fCollectionOption = 0;
  fNonUniformity = 0.;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BXBall::R3BXBall(const char* name, Bool_t active)
  : R3BDetector(name, active, kCAL) {
  ResetParameters();
  fXBallCollection = new TClonesArray("R3BXBallPoint");
  fXBallCrystalHitCollection = new TClonesArray("R3BXBallCrystalHitSim");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  kDrawCrystals = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fGeoVersion = 1;
  fCollectionOption = 0;
  fNonUniformity = 0.;
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
R3BXBall::R3BXBall(const char* name,
                   TString geoFile,
                   Bool_t active,
                   Float_t x,
                   Float_t y,
                   Float_t z,
                   Float_t rot_x,
                   Float_t rot_y,
                   Float_t rot_z)
: R3BDetector(name, active, kCAL) {
    SetPosition(x, y, z);
    SetRotation(rot_x, rot_y, rot_z);
    SetGeometryFileName(geoFile);
    ResetParameters();
    fXBallCollection = new TClonesArray("R3BXBallPoint");
    fXBallCrystalHitCollection = new TClonesArray("R3BXBallCrystalHitSim");
    fPosIndex = 0;
    kGeoSaved = kFALSE;
    kDrawCrystals = kFALSE;
    flGeoPar = new TList();
    flGeoPar->SetName( GetName());
    fVerboseLevel = 1;
    fGeoVersion = 1;
    fCollectionOption = 0;
    fNonUniformity = 0.;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BXBall::~R3BXBall() {

  if ( flGeoPar ) delete flGeoPar;
  if (fXBallCollection) {
   fXBallCollection->Delete();
   delete fXBallCollection;
  }
  if (fXBallCrystalHitCollection) {
   fXBallCrystalHitCollection->Delete();
   delete fXBallCrystalHitCollection;
  }
}
// -------------------------------------------------------------------------

void R3BXBall::Initialize()
{
  FairDetector::Initialize();

  LOG(INFO) << "R3BXBall initialisation" << FairLogger::endl;
  LOG(DEBUG) << "R3BXBall: Vol. (McId)" << FairLogger::endl;
  LOG(DEBUG) << "R3BXBall: Crystal A : " << gMC->VolId("crystalLogNAJA")<< FairLogger::endl;
  LOG(DEBUG) << "R3BXBall: Crystal B : " << gMC->VolId("crystalLogNAJB")<< FairLogger::endl;
  LOG(DEBUG) << "R3BXBall: Crystal C : " << gMC->VolId("crystalLogNAJC")<< FairLogger::endl;
  LOG(DEBUG) << "R3BXBall: Crystal D : " << gMC->VolId("crystalLogNAJD")<< FairLogger::endl;

  // Crystals type ID
  // type  ID
  // A    1
  // B    2
  // C    3
  // D    4

  fTypeA = gMC->VolId("crystalLogNAJA");
  fTypeB = gMC->VolId("crystalLogNAJB");
  fTypeC = gMC->VolId("crystalLogNAJC");
  fTypeD = gMC->VolId("crystalLogNAJD");

  TGeoVolume *vol = gGeoManager->GetVolume("CBLogWorld");
  vol->SetVisibility(kFALSE);
}



// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BXBall::ProcessHits(FairVolume* vol) {

  Int_t copyNo=0;

  // Gets the right crystal type
  Int_t fVolID = gMC->CurrentVolID(copyNo);
  Int_t fCrystalType = GetCrystalType(fVolID);

  // Gets the right crystal number from its mother wrapping
  fVolID = gMC->CurrentVolOffID(1,copyNo);

  if ( gMC->IsTrackEntering() ) {
    fELoss = 0.;
    fNSteps  = 0;
    fTime = gMC->TrackTime() * 1.0e09;    //[ns]
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPosIn);
    gMC->TrackMomentum(fMomIn);
    fEinc   = gMC->Etot();
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();
  fNSteps++;

  // Set additional parameters at exit of active volume. Create R3BXBallPoint.
  if ( gMC->IsTrackExiting() ||
     gMC->IsTrackStop() ||
     gMC->IsTrackDisappeared() ) {

    fTrackID        = gMC->GetStack()->GetCurrentTrackNumber();
    fVolumeID       = vol->getMCid();
    fParentTrackID  = gMC->GetStack()->GetCurrentParentTrackNumber();
    fTrackPID       = gMC->TrackPid();
    fUniqueID       = gMC->GetStack()->GetCurrentTrack()->GetUniqueID();

    gMC->TrackPosition(fPosOut);
    gMC->TrackMomentum(fMomOut);

    if (fELoss == 0. ) return kFALSE;

    if (gMC->IsTrackExiting()) {
      const Double_t* oldpos;
      const Double_t* olddirection;
      Double_t newpos[3];
      Double_t newdirection[3];
      Double_t safety;

      gGeoManager->FindNode(fPosOut.X(),fPosOut.Y(),fPosOut.Z());
      oldpos = gGeoManager->GetCurrentPoint();
      olddirection = gGeoManager->GetCurrentDirection();

      for (Int_t i=0; i<3; i++){
        newdirection[i] = -1*olddirection[i];
      }

      gGeoManager->SetCurrentDirection(newdirection);
      safety = gGeoManager->GetSafeDistance();


      gGeoManager->SetCurrentDirection(-newdirection[0],-newdirection[1],-newdirection[2]);

      for (Int_t i=0; i<3; i++){
        newpos[i] = oldpos[i] - (3*safety*olddirection[i]);
      }

      fPosOut.SetX(newpos[0]);
      fPosOut.SetY(newpos[1]);
      fPosOut.SetZ(newpos[2]);
    }

    if(fCollectionOption == 0 || fCollectionOption == 2) {
      AddHit(fTrackID, fVolumeID, fCrystalType, copyNo ,
        TVector3(fPosIn.X(), fPosIn.Y(), fPosIn.Z()),
        TVector3(fPosOut.X(), fPosOut.Y(), fPosOut.Z()),
        TVector3(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz()),
        TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
        fTime, fLength, fELoss);

      // Increment number of XBallPoints for this track
      R3BStack* stack = (R3BStack*) gMC->GetStack();
      stack->AddPoint(kCAL);
    }

    //Adding a crystalHit support
    if(fCollectionOption == 1 || fCollectionOption == 2) {
      Int_t nCrystalHits = fXBallCrystalHitCollection->GetEntriesFast();
      Bool_t existHit = 0;

      if (nCrystalHits==0) AddCrystalHit(fCrystalType, copyNo, NUSmearing(fELoss), fTime, fNSteps, fEinc,
					fTrackID, fVolumeID, fParentTrackID, fTrackPID, fUniqueID);
      else {
        for (Int_t i=0; i<nCrystalHits; i++) {
          if ( ((R3BXBallCrystalHitSim *)(fXBallCrystalHitCollection->At(i)))->GetCrystalNumber() == copyNo ) {
            ((R3BXBallCrystalHitSim *)(fXBallCrystalHitCollection->At(i)))->AddMoreEnergy(NUSmearing(fELoss));
            if ( ((R3BXBallCrystalHitSim *)(fXBallCrystalHitCollection->At(i)))->GetTime() > fTime ) {
              ((R3BXBallCrystalHitSim *)(fXBallCrystalHitCollection->At(i)))->SetTime(fTime);
            }
            existHit=1; //to avoid the creation of a new CrystalHit
            break;
          }
        }
        if (!existHit) AddCrystalHit(fCrystalType, copyNo, NUSmearing(fELoss), fTime, fNSteps, fEinc,
					fTrackID, fVolumeID, fParentTrackID, fTrackPID, fUniqueID);
      }

      existHit=0;
    }


    ResetParameters();
  }
  return kTRUE;
}


// ----------------------------------------------------------------------------
/*
void R3BXBall::SaveGeoParams()
{
}
*/


// -----   Public method EndOfEvent   -----------------------------------------
void R3BXBall::BeginEvent()
{
}


// -----   Public method EndOfEvent   -----------------------------------------
void R3BXBall::EndOfEvent() {

  if (fVerboseLevel) Print();

  if(fCollectionOption == 0) {
    fXBallCollection->Clear();
  } else if(fCollectionOption == 1) {
    fXBallCrystalHitCollection->Clear();
  } else if(fCollectionOption == 2) {
    fXBallCollection->Clear();
    fXBallCrystalHitCollection->Clear();
  }

  ResetParameters();
}
// ----------------------------------------------------------------------------



void R3BXBall::SetSpecialPhysicsCuts()
{
    LOG(INFO) << "-I- R3BXBall: Adding customized Physics cut ... " << FairLogger::endl;

    if (gGeoManager)
    {
        TGeoMedium* pSi = gGeoManager->GetMedium("NaI");
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
            gMC->Gstpar(pSi->GetId(), "HADR", 5.0);
            gMC->Gstpar(pSi->GetId(), "DRAY", 1.0);
            gMC->Gstpar(pSi->GetId(), "DCAY", 1.0);
            gMC->Gstpar(pSi->GetId(), "MULS", 1.0);
            gMC->Gstpar(pSi->GetId(), "RAYL", 1.0);

            // Setting Energy-CutOff for Si Only
            Double_t cutE = 0.00001; // GeV-> 10 keV

            LOG(INFO) << "-I- R3BXBall: NaI Medium Id " << pSi->GetId() << " Energy Cut-Off : " << cutE << " GeV" << FairLogger::endl;

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
        // <DB> trick to remove too much internal
        // tracking in the Aladin magnet yoke
        TGeoMedium* pFe = gGeoManager->GetMedium("iron");

        if (pFe)
        {
            Double_t cutM = 1.e-01; // 100 MeV
            gMC->Gstpar(pFe->GetId(), "CUTELE", cutM);
            gMC->Gstpar(pFe->GetId(), "DRAY", 0.0);
        }

    } //!gGeoManager
}



// -----   Public method Register   -------------------------------------------
void R3BXBall::Register() {
  if(fCollectionOption == 0) {
    FairRootManager::Instance()->Register("XBCrystalPoint", GetName(), fXBallCollection, kTRUE);
  } else if(fCollectionOption == 1) {
    FairRootManager::Instance()->Register("XBCrystalHitSim", GetName(), fXBallCrystalHitCollection, kTRUE);
  } else if(fCollectionOption == 2) {
    FairRootManager::Instance()->Register("XBCrystalPoint", GetName(), fXBallCollection, kTRUE);
    FairRootManager::Instance()->Register("XBCrystalHitSim", GetName(), fXBallCrystalHitCollection, kTRUE);
  }
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BXBall::GetCollection(Int_t iColl) const {
  if(fCollectionOption == 0) {
    if (iColl == 0) return fXBallCollection;
    else return NULL;
  } else if(fCollectionOption == 1) {
    if (iColl == 0) return fXBallCrystalHitCollection;
    else return NULL;
  } else if(fCollectionOption == 2) {
    if (iColl == 0) return fXBallCollection;
    //if (iColl == 1) return fXBallCrystalHitCollection;
    else return NULL;
  } else return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void R3BXBall::Print(Option_t* option) const
{
  if(fCollectionOption == 0) {
    Int_t nHits = fXBallCollection->GetEntriesFast();
    LOG(INFO) << "R3BXBall: " << nHits << " points registered in this event" << FairLogger::endl;
  } else if(fCollectionOption == 1) {
    Int_t nHits = fXBallCrystalHitCollection->GetEntriesFast();
    LOG(INFO) << "R3BXBall: " << nHits << " hits registered in this event" << FairLogger::endl;
  } else if(fCollectionOption == 2) {
    Int_t nHits = fXBallCollection->GetEntriesFast();
    LOG(INFO) << "R3BXBall: " << nHits << " points registered in this event" << FairLogger::endl;
    nHits = fXBallCrystalHitCollection->GetEntriesFast();
    LOG(INFO) << "R3BXBall: " << nHits << " hits registered in this event" << FairLogger::endl;
  }
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void R3BXBall::Reset() {
  fXBallCollection->Clear();
  fXBallCrystalHitCollection->Clear();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void R3BXBall::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
  Int_t nEntries = cl1->GetEntriesFast();
  LOG(INFO) << "R3BXBall: " << nEntries << " entries to add" << FairLogger::endl;
  TClonesArray& clref = *cl2;
  R3BXBallPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++)
  {
    oldpoint = (R3BXBallPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) R3BXBallPoint(*oldpoint);
    fPosIndex++;
  }
  LOG(INFO) << "R3BXBall: " << cl2->GetEntriesFast() << " merged entries." << FairLogger::endl;
}

// -----   Private method AddHit   --------------------------------------------
R3BXBallPoint* R3BXBall::AddHit(Int_t trackID, Int_t detID, Int_t type, Int_t cp, TVector3 posIn,
        TVector3 posOut, TVector3 momIn,
        TVector3 momOut, Double_t time,
        Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fXBallCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1)
    LOG(INFO) << "R3BXBall: Adding Point at (" << posIn.X() << ", " << posIn.Y()
    << ", " << posIn.Z() << ") cm, detector " << detID << ", track "
    << trackID << ", energy loss " << eLoss*1e06 << " keV" << FairLogger::endl;
  return new(clref[size]) R3BXBallPoint(trackID, detID, type, cp , posIn, posOut,
            momIn, momOut, time, length, eLoss);
}

// -----   Private method AddCrystalHit   --------------------------------------------
R3BXBallCrystalHitSim* R3BXBall::AddCrystalHit(Int_t type, Int_t copy,
    Double_t energy, Double_t time, Int_t steps, Double_t einc,
    Int_t trackid, Int_t volid, Int_t partrackid, Int_t pdgtype, Int_t uniqueid)
{
  TClonesArray& clref = *fXBallCrystalHitCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1)
    LOG(INFO) << "R3BXBall: Adding Hit in detector type " << type << ", and number " << copy
    << " entering with " << einc*1e06 << " keV, depositing " << energy*1e06 << " keV" << FairLogger::endl
    << " trackid: " << trackid << " volume id: " << volid << " partrackid : " << partrackid << " type: " << pdgtype
    << " unique id: " << uniqueid << FairLogger::endl;
  return new(clref[size]) R3BXBallCrystalHitSim(type, copy, energy, time, steps, einc, trackid, volid, partrackid, pdgtype, uniqueid);
}

// -----   Private method NUSmearing  --------------------------------------------
Double_t R3BXBall::NUSmearing(Double_t inputEnergy)
{
  // Very simple preliminary scheme where the NU is introduced as a flat random
  // distribution with limits fNonUniformity (%) of the energy value.
  //
  return gRandom->Uniform(inputEnergy-inputEnergy*fNonUniformity/100,inputEnergy+inputEnergy*fNonUniformity/100);
}

// -----  Public method SetNonUniformity  ----------------------------------
void R3BXBall::SetNonUniformity(Double_t nonU)
{
  fNonUniformity = nonU;
  LOG(INFO) << "R3BXBall::SetNonUniformity to " << fNonUniformity << " %" << FairLogger::endl;
}



void R3BXBall::ConstructGeometry()
{
  TString fileName = GetGeometryFileName();
  if(fileName.EndsWith(".root")) {
    LOG(INFO) << "Constructing Crystal Ball geometry from ROOT file " << fileName.Data() << FairLogger::endl;
    ConstructRootGeometry();
  } else {
    LOG(FATAL) << "Crystal Ball geometry file name is not specified" << FairLogger::endl;
    exit(1);
  }
}



Bool_t R3BXBall::CheckIfSensitive(std::string name)
{
  TString volName = name;
  if(volName.Contains("crystalLogNAJ")) {
    return kTRUE;
  }
  return kFALSE;
}



ClassImp(R3BXBall)
