// -------------------------------------------------------------------------
// -----                        R3BMfi source file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------
#include <stdlib.h>

#include "R3BMfi.h"

#include "R3BGeoMfi.h"
#include "R3BMfiPoint.h"
// #include "R3BGeoMfiPar.h"
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
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoMatrix.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoBBox.h"
#include "TGeoPara.h"
#include "TGeoPgon.h"
#include "TGeoSphere.h"
#include "TGeoArb8.h"
#include "TGeoCone.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"



// -----   Default constructor   -------------------------------------------
R3BMfi::R3BMfi() : R3BDetector("R3BMfi", kTRUE, kMFI)
{
// R3BMfi::R3BMfi() : R3BDetector("R3BMfi", kTRUE, 1) {
  ResetParameters();
  fMfiCollection = new TClonesArray("R3BMfiPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BMfi::R3BMfi(const char* name, Bool_t active)
: R3BDetector(name, active, kMFI)
{
    //   : R3BDetector(name, active, 1) {
    ResetParameters();
    fMfiCollection = new TClonesArray("R3BMfiPoint");
    fPosIndex = 0;
    kGeoSaved = kFALSE;
    flGeoPar = new TList();
    flGeoPar->SetName( GetName());
    fVerboseLevel = 1;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BMfi::R3BMfi(const char* name,
               TString geoFile,
               Bool_t active,
               Float_t x,
               Float_t y,
               Float_t z,
               Float_t rot_x,
               Float_t rot_y,
               Float_t rot_z)
  : R3BDetector(name, active, kMFI)
{
//   : R3BDetector(name, active, 1) {
  ResetParameters();
  SetGeometryFileName(geoFile);
  SetPosition(x, y, z);
  SetRotation(rot_x, rot_y, rot_z);
  fMfiCollection = new TClonesArray("R3BMfiPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BMfi::~R3BMfi() {

  if ( flGeoPar ) delete flGeoPar;
  if (fMfiCollection) {
    fMfiCollection->Delete();
    delete fMfiCollection;
  }
}
// -------------------------------------------------------------------------



void R3BMfi::Initialize()
{
  FairDetector::Initialize();

  LOG(INFO) << "R3BMfi: initialisation" << FairLogger::endl;
  LOG(DEBUG) << "R3BMfi: Sci. Vol. (McId) " << gMC->VolId("MFILog")<< FairLogger::endl;
}



// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BMfi::ProcessHits(FairVolume* vol)
{
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


  
  // Set additional parameters at exit of active volume. Create R3BMfiPoint.
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
    
    // Increment number of MfiPoints for this track
    R3BStack* stack = (R3BStack*) gMC->GetStack();
    stack->AddPoint(kMFI);
    
    ResetParameters();
  }

  return kTRUE;
}



// -----   Public method EndOfEvent   -----------------------------------------
void R3BMfi::BeginEvent()
{
}



// -----   Public method EndOfEvent   -----------------------------------------
void R3BMfi::EndOfEvent()
{
  if (fVerboseLevel) Print();
  fMfiCollection->Clear();

  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method Register   -------------------------------------------
void R3BMfi::Register()
{
  FairRootManager::Instance()->Register("MFIPoint", GetName(), fMfiCollection, kTRUE);
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BMfi::GetCollection(Int_t iColl) const
{
  if (iColl == 0) return fMfiCollection;
  else return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void R3BMfi::Print(Option_t *option) const
{
  Int_t nHits = fMfiCollection->GetEntriesFast();
  LOG(INFO) << "R3BMfi: " << nHits << " points registered in this event" << FairLogger::endl;
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void R3BMfi::Reset()
{
  fMfiCollection->Clear();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void R3BMfi::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
  Int_t nEntries = cl1->GetEntriesFast();
  LOG(INFO) << "R3BMfi: " << nEntries << " entries to add" << FairLogger::endl;
  TClonesArray& clref = *cl2;
  R3BMfiPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++) {
    oldpoint = (R3BMfiPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) R3BMfiPoint(*oldpoint);
    fPosIndex++;
  }
  LOG(INFO) << "R3BMfi: " << cl2->GetEntriesFast() << " merged entries" << FairLogger::endl;
}



// -----   Private method AddHit   --------------------------------------------
R3BMfiPoint* R3BMfi::AddHit(Int_t trackID, Int_t detID, Int_t plane , TVector3 posIn,
			    TVector3 posOut, TVector3 momIn, 
			    TVector3 momOut, Double_t time, 
			    Double_t length, Double_t eLoss)
{
  TClonesArray& clref = *fMfiCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel > 1) {
    LOG(INFO) << "R3BMfi: Adding Point at (" << posIn.X() << ", " << posIn.Y()
    << ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
    << trackID << ", energy loss " << eLoss*1e06 << " keV" << FairLogger::endl;
  }
  return new(clref[size]) R3BMfiPoint(trackID, detID, plane, posIn, posOut,
                                      momIn, momOut, time, length, eLoss);
}



// -----   Public method ConstructGeometry   ----------------------------------
void R3BMfi::ConstructGeometry()
{
  TString fileName = GetGeometryFileName();
  if(fileName.EndsWith(".root")) {
    LOG(INFO) << "Constructing MFI geometry from ROOT file " << fileName.Data() << FairLogger::endl;
    ConstructRootGeometry();
  } else {
    LOG(FATAL) << "MFI geometry file is not specified" << FairLogger::endl;
    exit(1);
  }
}



Bool_t R3BMfi::CheckIfSensitive(std::string name)
{
  if(TString(name).Contains("MFILog")) {
    return kTRUE;
  }
  return kFALSE;
}



ClassImp(R3BMfi)
