// -------------------------------------------------------------------------
// -----                        R3BGfi source file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------
#include <stdlib.h>

#include "R3BGfi.h"

#include "R3BGeoGfi.h"
#include "R3BGfiPoint.h"
#include "R3BGeoGfiPar.h"
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
R3BGfi::R3BGfi() : R3BDetector("R3BGfi", kTRUE, kGFI) {
  ResetParameters();
  fGfiCollection = new TClonesArray("R3BGfiPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BGfi::R3BGfi(const char* name, Bool_t active) 
  : R3BDetector(name, active, kGFI) {
  ResetParameters();
  fGfiCollection = new TClonesArray("R3BGfiPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BGfi::~R3BGfi() {

  if ( flGeoPar ) delete flGeoPar;
  if (fGfiCollection) {
    fGfiCollection->Delete();
    delete fGfiCollection;
  }
}
// -------------------------------------------------------------------------



void R3BGfi::Initialize()
{
  FairDetector::Initialize();

  LOG(INFO) << "R3BGfi: initialisation" << FairLogger::endl;
  LOG(DEBUG) << "R3BGfi: Sci. Vol. (McId) " << gMC->VolId("GFILog") << FairLogger::endl;
}



// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BGfi::ProcessHits(FairVolume* vol) {

   // 2 Simple Det PLane
   // get Info from DCH planes
    Int_t copyNo  = -1;
    Int_t planeNr = -1;
   // Get the Geo info from MC Point
    gMC->CurrentVolID(copyNo);
    gMC->CurrentVolOffID(1,planeNr);

   if ( gMC->IsTrackEntering() ) {
    fELoss  = 0.;
    //fTime   = gMC->TrackTime() * 1.0e09;
    //fLength = gMC->TrackLength();
    fTime_in   = gMC->TrackTime() * 1.0e09;
    fLength_in = gMC->TrackLength();
    gMC->TrackPosition(fPosIn);
    gMC->TrackMomentum(fMomIn);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();


  
  // Set additional parameters at exit of active volume. Create R3BGfiPoint.
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) {
    fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
    fVolumeID = vol->getMCid();
    gMC->TrackPosition(fPosOut);
    gMC->TrackMomentum(fMomOut);
    if (fELoss == 0. ) return kFALSE;
    
    fTime_out   = gMC->TrackTime() * 1.0e09;	//also in case particle is stopped in detector, or decays...
    fLength_out = gMC->TrackLength();
    fTime = (fTime_out+fTime_in)/2.;
    fLength = (fLength_out+fLength_in)/2.;

    if (gMC->IsTrackExiting()) {
      const Double_t* oldpos;
      const Double_t* olddirection;
      Double_t newpos[3];
      Double_t newdirection[3];
      Double_t safety;
      
      gGeoManager->FindNode(fPosOut.X(),fPosOut.Y(),fPosOut.Z());
      oldpos = gGeoManager->GetCurrentPoint();
      olddirection = gGeoManager->GetCurrentDirection();
      
//       cout << "1st direction: " << olddirection[0] << "," << olddirection[1] << "," << olddirection[2] << endl;

      for (Int_t i=0; i<3; i++){
	newdirection[i] = -1*olddirection[i];
      }
      
      gGeoManager->SetCurrentDirection(newdirection);
     // TGeoNode *bla = gGeoManager->FindNextBoundary(2);
      safety = gGeoManager->GetSafeDistance();


      gGeoManager->SetCurrentDirection(-newdirection[0],-newdirection[1],-newdirection[2]);
      
      for (Int_t i=0; i<3; i++){
	newpos[i] = oldpos[i] - (3*safety*olddirection[i]);
      }

      if ( fPosIn.Z() < 30. && newpos[2] > 30.02 ) {
        LOG(ERROR) << "2nd direction: " << olddirection[0] << "," << olddirection[1] << "," << olddirection[2]
        << " with safety = " << safety << FairLogger::endl;
        LOG(ERROR) << "oldpos = " << oldpos[0] << "," << oldpos[1] << "," << oldpos[2] << FairLogger::endl;
        LOG(ERROR) << "newpos = " << newpos[0] << "," << newpos[1] << "," << newpos[2] << FairLogger::endl;
      }

      fPosOut.SetX(newpos[0]);
      fPosOut.SetY(newpos[1]);
      fPosOut.SetZ(newpos[2]);
    }

    AddHit(fTrackID, fVolumeID, planeNr ,
	   TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
	   TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
	   TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
	   TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
	   fTime, fLength, fELoss);
    
    // Increment number of GfiPoints for this track
    R3BStack* stack = (R3BStack*) gMC->GetStack();
    stack->AddPoint(kGFI);
    
    ResetParameters();
  }

  return kTRUE;
}

// -----   Public method EndOfEvent   -----------------------------------------
void R3BGfi::BeginEvent() {

//  if (! kGeoSaved ) {
//      SaveGeoParams();
//  cout << "-I STS geometry parameters saved " << endl;
//  kGeoSaved = kTRUE;
//  }

}
// -----   Public method EndOfEvent   -----------------------------------------
void R3BGfi::EndOfEvent() {

  if (fVerboseLevel) Print();
  fGfiCollection->Clear();

  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method Register   -------------------------------------------
void R3BGfi::Register() {
  FairRootManager::Instance()->Register("GFIPoint", GetName(), fGfiCollection, kTRUE);
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BGfi::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fGfiCollection;
  else return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void R3BGfi::Print(Option_t *option) const
{
  Int_t nHits = fGfiCollection->GetEntriesFast();
  LOG(INFO) << "R3BGfi: " << nHits << " points registered in this event" << FairLogger::endl;
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void R3BGfi::Reset() {
  fGfiCollection->Clear();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void R3BGfi::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
  Int_t nEntries = cl1->GetEntriesFast();
  LOG(INFO) << "R3BGfi: " << nEntries << " entries to add" << FairLogger::endl;
  TClonesArray& clref = *cl2;
  R3BGfiPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++) {
    oldpoint = (R3BGfiPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) R3BGfiPoint(*oldpoint);
    fPosIndex++;
  }
  LOG(INFO) << "R3BGfi: " << cl2->GetEntriesFast() << " merged entries" << FairLogger::endl;
}


// -----   Private method AddHit   --------------------------------------------
R3BGfiPoint* R3BGfi::AddHit(Int_t trackID, Int_t detID, Int_t plane , TVector3 posIn,
			    TVector3 posOut, TVector3 momIn, 
			    TVector3 momOut, Double_t time, 
			    Double_t length, Double_t eLoss)
{
  TClonesArray& clref = *fGfiCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1)
    LOG(INFO) << "R3BGfi: Adding Point at (" << posIn.X() << ", " << posIn.Y()
    << ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
    << trackID << ", energy loss " << eLoss*1e06 << " keV" << FairLogger::endl;
  return new(clref[size]) R3BGfiPoint(trackID, detID, plane, posIn, posOut,
                                      momIn, momOut, time, length, eLoss);
}



// -----   Public method ConstructGeometry   ----------------------------------
void R3BGfi::ConstructGeometry()
{
  TString fileName = GetGeometryFileName();
  if(fileName.EndsWith(".root")) {
    LOG(INFO) << "Constructing GFI geometry from ROOT file " << fileName.Data() << FairLogger::endl;
    ConstructRootGeometry();
  } else {
    LOG(FATAL) << "GFI geometry file is not specified" << FairLogger::endl;
    exit(1);
  }
}



Bool_t R3BGfi::CheckIfSensitive(std::string name)
{
  if(TString(name).Contains("GFILog")) {
    return kTRUE;
  }
  return kFALSE;
}



ClassImp(R3BGfi)
