// -------------------------------------------------------------------------
// -----                        R3BSTaRTra source file                 -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------
#include <stdlib.h>

#include "R3BSTaRTra.h"

#include "R3BGeoSTaRTra.h"
#include "R3BSTaRTraPoint.h"
#include "R3BGeoSTaRTraPar.h"
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
R3BSTaRTra::R3BSTaRTra() : R3BDetector("R3BSTaRTra", kTRUE, kSTaRTrack)
{
  ResetParameters();
  fTraCollection = new TClonesArray("R3BSTaRTraPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BSTaRTra::R3BSTaRTra(const char* name, Bool_t active)
: R3BDetector(name, active, kSTaRTrack) {
    ResetParameters();
    fTraCollection = new TClonesArray("R3BSTaRTraPoint");
    fPosIndex = 0;
    kGeoSaved = kFALSE;
    flGeoPar = new TList();
    flGeoPar->SetName( GetName());
    fVerboseLevel = 1;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BSTaRTra::R3BSTaRTra(const char* name,
                       TString geoFile,
                       Bool_t active,
                       Float_t x,
                       Float_t y,
                       Float_t z,
                       Float_t rot_x,
                       Float_t rot_y,
                       Float_t rot_z)
: R3BDetector(name, active, kSTaRTrack) {
  ResetParameters();
  SetGeometryFileName(geoFile);
  SetPosition(x, y, z);
  SetRotation(rot_x, rot_y, rot_z);
  fTraCollection = new TClonesArray("R3BSTaRTraPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BSTaRTra::~R3BSTaRTra()
{
  if ( flGeoPar ) delete flGeoPar;
  if (fTraCollection) {
    fTraCollection->Delete();
    delete fTraCollection;
  }
}
// -------------------------------------------------------------------------



// ----   Initialize   -----------------------------------------------------
void R3BSTaRTra::Initialize()
{
  FairDetector::Initialize();
  
  LOG(INFO) << "R3BSTaRTra: initialisation" << FairLogger::endl;
  LOG(DEBUG) << "R3BSTaRTra: Sens. Vol. (McId) " << gMC->VolId("STaRTraLog1") << FairLogger::endl;
}



// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BSTaRTra::ProcessHits(FairVolume* vol)
{
  //      cout << " -I process hit called for:" <<  vol->GetName() << endl;
  // Set parameters at entrance of volume. Reset ELoss.
  
  //    if ( vol ) {
  //        cout << " Name Id:copy "
  //            << vol->getName() << " : " << vol->getMCid() << " : " << vol->getCopyNo() << endl;
  //        Int_t copyNo=0;
  //        cout << " Geant: " << gMC->CurrentVolID(copyNo) << ":" << copyNo << endl;
  //    }
  
  if ( gMC->IsTrackEntering() ) {
    fELoss  = 0.;
    fTime   = gMC->TrackTime() * 1.0e09;
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
  
  // Set additional parameters at exit of active volume. Create R3BSTaRTraPoint.
  if ( gMC->IsTrackExiting()    ||
      gMC->IsTrackStop()       ||
      gMC->IsTrackDisappeared()   ) {
    fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
    fVolumeID = vol->getMCid(); //getCopyNo(); //getMCid();
    fDetCopyID = vol->getCopyNo();  // added by Marc
    //     cout << "VolumeID=" <<  vol->getMCid()<< endl;
    //     cout << "DetCopyID=" <<  vol->getCopyNo()<< endl;
    //      cout << "DetMotherID=" <<  vol->getMotherId()<< endl;
    //      cout << "Volume name=" <<  vol->getRealName()<< endl;
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
      
      //       cout << "1st direction: " << olddirection[0] << "," << olddirection[1] << "," << olddirection[2] << endl;
      
      for (Int_t i=0; i<3; i++){
        newdirection[i] = -1*olddirection[i];
      }
      
      gGeoManager->SetCurrentDirection(newdirection);
      //   TGeoNode *bla = gGeoManager->FindNextBoundary(2);
      safety = gGeoManager->GetSafeDistance();
      
      
      gGeoManager->SetCurrentDirection(-newdirection[0],-newdirection[1],-newdirection[2]);
      
      for (Int_t i=0; i<3; i++){
        newpos[i] = oldpos[i] - (3*safety*olddirection[i]);
      }
      
      fPosOut.SetX(newpos[0]);
      fPosOut.SetY(newpos[1]);
      fPosOut.SetZ(newpos[2]);
    }
    
    AddHit(fTrackID, fVolumeID, fDetCopyID,   // fdetCopyID, added by Marc
           TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
           TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
           TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
           TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
           fTime, fLength, fELoss);
    
    // Increment number of STaRTraPoints for this track
    R3BStack* stack = (R3BStack*) gMC->GetStack();
    stack->AddPoint(kSTaRTrack);
    
    ResetParameters();
  }
  
  return kTRUE;
}



// ----------------------------------------------------------------------------
//void R3BSTaRTra::SaveGeoParams(){
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
void R3BSTaRTra::BeginEvent()
{
  //  cout << "-I- begin tracker event called ##########" << endl;
  
  //  if(gGeoManager){
  //    TGeoVolume * vol=gGeoManager->FindVolumeFast("STaRTraLog1");
  
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
void R3BSTaRTra::EndOfEvent()
{
  if (fVerboseLevel) Print();
  fTraCollection->Clear();
  
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method Register   -------------------------------------------
void R3BSTaRTra::Register()
{
  FairRootManager::Instance()->Register("STaRTraPoint", GetName(), fTraCollection, kTRUE);
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BSTaRTra::GetCollection(Int_t iColl) const
{
  if (iColl == 0) return fTraCollection;
  else return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void R3BSTaRTra::Print(Option_t *option) const
{
  Int_t nHits = fTraCollection->GetEntriesFast();
  LOG(INFO) << "R3BSTaRTra: " << nHits << " points registered in this event" << FairLogger::endl;
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void R3BSTaRTra::Reset()
{
  fTraCollection->Clear();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void R3BSTaRTra::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
  Int_t nEntries = cl1->GetEntriesFast();
  LOG(INFO) << "R3BSTaRTra: " << nEntries << " entries to add" << FairLogger::endl;
  TClonesArray& clref = *cl2;
  R3BSTaRTraPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++) {
    oldpoint = (R3BSTaRTraPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) R3BSTaRTraPoint(*oldpoint);
    fPosIndex++;
  }
  LOG(INFO) << "R3BSTaRTra: " << cl2->GetEntriesFast() << " merged entries" << FairLogger::endl;
}



// -----   Private method AddHit   --------------------------------------------
R3BSTaRTraPoint* R3BSTaRTra::AddHit(Int_t trackID, Int_t detID, Int_t detCopyID, TVector3 posIn, // Int_t detCopyID added by Marc
                                    TVector3 posOut, TVector3 momIn,
                                    TVector3 momOut, Double_t time,
                                    Double_t length, Double_t eLoss)
{
  TClonesArray& clref = *fTraCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1)
    LOG(INFO) << "R3BSTaRTra: Adding Point at (" << posIn.X() << ", " << posIn.Y()
    << ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
    << trackID << ", energy loss " << eLoss*1e06 << " keV" << FairLogger::endl;
  return new(clref[size]) R3BSTaRTraPoint(trackID, detID, detCopyID, posIn, posOut,
                                          momIn, momOut, time, length, eLoss); // detCopy added by Marc
}


// -----   Public method ConstructGeometry   ----------------------------------
void R3BSTaRTra::ConstructGeometry()
{
  TString fileName = GetGeometryFileName();
  if(fileName.EndsWith(".root")) {
    LOG(INFO) << "Constructing STaR Tracker geometry from ROOT file " << fileName.Data() << FairLogger::endl;
    ConstructRootGeometry();
  } else {
    LOG(FATAL) << "STaR Tracker geometry file is not specified" << FairLogger::endl;
    exit(1);
  }
}


Bool_t R3BSTaRTra::CheckIfSensitive(std::string name)
{
  if(TString(name).Contains("STaRTraLog")) {
    return kTRUE;
  }
  return kFALSE;
}


ClassImp(R3BSTaRTra)
