// -------------------------------------------------------------------------
// -----                        ELILuMon source file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------
#include "ELILuMon.h"

#include "ELIGeoLuMon.h"
#include "ELILuMonPoint.h"
#include "ELIGeoLuMonPar.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
#include "FairStack.h"
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
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;



// -----   Default constructor   -------------------------------------------
ELILuMon::ELILuMon() : R3BDetector("ELILuMon", kTRUE, kTOF) {
  ResetParameters();
  fLuMonCollection = new TClonesArray("ELILuMonPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fCutE=1.0e-03; // 1 MeV default
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
ELILuMon::ELILuMon(const char* name, Bool_t active) 
  : R3BDetector(name, active, kTOF) {
  ResetParameters();
  fLuMonCollection = new TClonesArray("ELILuMonPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fCutE=1.0e-03; // 1MeV default
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
ELILuMon::~ELILuMon() {

  if ( flGeoPar ) delete flGeoPar;
  if (fLuMonCollection) {
    fLuMonCollection->Delete();
    delete fLuMonCollection;
  }
}
// -------------------------------------------------------------------------


void ELILuMon::Initialize()
{
  FairDetector::Initialize();

   cout << endl;
   cout << "-I- ELILuMon: initialisation" << endl;
   cout << "-I- ELILuMon: Sci. Vol. (McId) " << gMC->VolId("LuMonLog")<< endl;

}


void ELILuMon::SetSpecialPhysicsCuts(){

   cout << endl;

   cout << "-I- ELILuMon: Adding customized Physics cut ... " << endl;

   if (gGeoManager) {
     TGeoMedium* pSi = gGeoManager->GetMedium("plasticForTOF");
     if ( pSi ) {
      // Setting processes for Si only
	 gMC->Gstpar(pSi->GetId()  ,"LOSS",3);
         gMC->Gstpar(pSi->GetId()  ,"STRA",1.0);
         gMC->Gstpar(pSi->GetId()  ,"PAIR",1.0);
	 gMC->Gstpar(pSi->GetId()  ,"COMP",1.0);
	 gMC->Gstpar(pSi->GetId()  ,"PHOT",1.0);
         gMC->Gstpar(pSi->GetId()  ,"ANNI",1.0);
	 gMC->Gstpar(pSi->GetId()  ,"BREM",1.0);
	 gMC->Gstpar(pSi->GetId()  ,"HADR",1.0);
         gMC->Gstpar(pSi->GetId()  ,"DRAY",1.0);
         gMC->Gstpar(pSi->GetId()  ,"DCAY",1.0);
         gMC->Gstpar(pSi->GetId()  ,"MULS",1.0);
	 gMC->Gstpar(pSi->GetId()  ,"RAYL",1.0);

	 // Setting Energy-CutOff for Si Only
	Double_t cutE = fCutE; // GeV-> 1 keV

	cout << "-I- ELILuMon Scintillator Medium Id " << pSi->GetId()
	    << " Energy Cut-Off : " << cutE
	    << endl;
        cout << endl;
        //Si
	gMC->Gstpar(pSi->GetId(),"CUTGAM",cutE);   /** gammas (GeV)*/
        gMC->Gstpar(pSi->GetId(),"CUTELE",cutE);   /** electrons (GeV)*/
	gMC->Gstpar(pSi->GetId(),"CUTNEU",cutE);   /** neutral hadrons (GeV)*/
	gMC->Gstpar(pSi->GetId(),"CUTHAD",cutE);   /** charged hadrons (GeV)*/
	gMC->Gstpar(pSi->GetId(),"CUTMUO",cutE);   /** muons (GeV)*/
	gMC->Gstpar(pSi->GetId(),"BCUTE",cutE);    /** electron bremsstrahlung (GeV)*/
	gMC->Gstpar(pSi->GetId(),"BCUTM",cutE);    /** muon and hadron bremsstrahlung(GeV)*/
	gMC->Gstpar(pSi->GetId(),"DCUTE",cutE);    /** delta-rays by electrons (GeV)*/
	gMC->Gstpar(pSi->GetId(),"DCUTM",cutE);    /** delta-rays by muons (GeV)*/
        gMC->Gstpar(pSi->GetId(),"PPCUTM",-1.);   /** direct pair production by muons (GeV)*/

     }

 } //!gGeoManager


}



// -----   Public method ProcessHits  --------------------------------------
Bool_t ELILuMon::ProcessHits(FairVolume* vol) {

  // Simple Det PLane

    if ( gMC->IsTrackEntering() ) {
    fELoss  = 0.;
    fTime   = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPosIn);
    gMC->TrackMomentum(fMomIn);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Set additional parameters at exit of active volume. Create ELILuMonPoint.
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) {
    fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
    fVolumeID = vol->getMCid();
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
     // TGeoNode *bla = gGeoManager->FindNextBoundary(2);
      safety = gGeoManager->GetSafeDistance();


      gGeoManager->SetCurrentDirection(-newdirection[0],-newdirection[1],-newdirection[2]);
      
      for (Int_t i=0; i<3; i++){
	newpos[i] = oldpos[i] - (3*safety*olddirection[i]);
      }

      if ( fPosIn.Z() < 30. && newpos[2] > 30.02 ) {
	cerr << "2nd direction: " << olddirection[0] << "," << olddirection[1] << "," << olddirection[2] 
	     << " with safety = " << safety << endl;
	cerr << "oldpos = " << oldpos[0] << "," << oldpos[1] << "," << oldpos[2] << endl;
	cerr << "newpos = " << newpos[0] << "," << newpos[1] << "," << newpos[2] << endl;
      }

      fPosOut.SetX(newpos[0]);
      fPosOut.SetY(newpos[1]);
      fPosOut.SetZ(newpos[2]);
    }

    AddHit(fTrackID, fVolumeID,
	   TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
	   TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
	   TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
	   TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
	   fTime, fLength, fELoss);
    
    // Increment number of LuMonPoints for this track
    FairStack* stack = (FairStack*) gMC->GetStack();
    stack->AddPoint(kTOF);
    
    ResetParameters();
  }

  return kTRUE;
}
// ----------------------------------------------------------------------------
//void ELILuMon::SaveGeoParams(){
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
void ELILuMon::BeginEvent() {

//  if (! kGeoSaved ) {
//      SaveGeoParams();
//  cout << "-I STS geometry parameters saved " << endl;
//  kGeoSaved = kTRUE;
//  }

}
// -----   Public method EndOfEvent   -----------------------------------------
void ELILuMon::EndOfEvent() {

  if (fVerboseLevel) Print();
  fLuMonCollection->Clear();

  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method Register   -------------------------------------------
void ELILuMon::Register() {
  FairRootManager::Instance()->Register("LuMonPoint", GetName(), fLuMonCollection, kTRUE);
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* ELILuMon::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fLuMonCollection;
  else return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void ELILuMon::Print() const {
  Int_t nHits = fLuMonCollection->GetEntriesFast();
  cout << "-I- ELILuMon: " << nHits << " points registered in this event." 
       << endl;
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void ELILuMon::Reset() {
  fLuMonCollection->Clear();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void ELILuMon::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- ELILuMon: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  ELILuMonPoint* oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++) {
   oldpoint = (ELILuMonPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) ELILuMonPoint(*oldpoint);
    fPosIndex++;
  }
   cout << " -I- ELILuMon: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}

// -----   Private method AddHit   --------------------------------------------
ELILuMonPoint* ELILuMon::AddHit(Int_t trackID, Int_t detID, TVector3 posIn,
			    TVector3 posOut, TVector3 momIn, 
			    TVector3 momOut, Double_t time, 
			    Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fLuMonCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1) 
    cout << "-I- ELILuMon: Adding Point at (" << posIn.X() << ", " << posIn.Y() 
	 << ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
	 << trackID << ", energy loss " << eLoss*1e06 << " keV" << endl;
  return new(clref[size]) ELILuMonPoint(trackID, detID, posIn, posOut,
				      momIn, momOut, time, length, eLoss);
}
// -----   Public method ConstructGeometry   ----------------------------------

void ELILuMon::ConstructGeometry() {
   ConstructGeometry1();
}

void ELILuMon::ConstructGeometry1() {

   //-  Some Material definition
   TGeoMaterial *material = 0;
   TGeoMedium   *medium   = 0;
   Float_t *buf = 0;
   Float_t sumWeight;
   Int_t i;
   
   // material Lead
   Int_t kMatLead=601;
   material = gGeoManager->Material("Pb", 207.2, 82., 11.35,kMatLead);
   material->Print();
   
   // mixtrure Scintillator (CH)
   Int_t kMatPoly=602;
   Float_t aP[2] = {12.011, 1.00794} ;
   Float_t zP[2] = {6.0, 1.0} ;
   Float_t wP[2] = {9.0, 8.0} ;
   Float_t dP = 1.032 ;
   Int_t   nP = 2;
   sumWeight = 0;
   for (i=0; i<nP; i++) sumWeight += aP[i]*wP[i];
   for (i=0; i<nP; i++) wP[i] *= aP[i]/sumWeight;
   material = gGeoManager->Mixture("Scintillator",aP,zP,dP,nP,wP,kMatPoly);
   material->Print();
   
   // Iron 
   Int_t kMatFe=603;
   material = gGeoManager->Material("Fe", 55.8, 26., 7.87, kMatFe);
   material->Print();
   
   
   // Mixtrure Scintillator (PbWO)
   Int_t kMatPbWO=604;
   Float_t aPP[3] = {207.19, 183.85, 16.0} ;
   Float_t zPP[3] = {82.0, 74.0, 8.0} ;
   Float_t wPP[3] = {1.0, 1.0, 4.0} ;
   Float_t dPP = 8.28 ;
   Int_t   nPP = 3;
   sumWeight = 0;
   for (i=0; i<nPP; i++) sumWeight += aPP[i]*wPP[i];
   for (i=0; i<nPP; i++) wPP[i] *= aPP[i]/sumWeight;
   material = gGeoManager->Mixture("PbWO",aPP,zPP,dPP,nPP,wPP,kMatPbWO);
   material->Print();
   
   
   //- Media Definition
   Int_t kMedLead=601, kMedScin=602, kMedFe=603, kMedPbWO=604;
   TGeoMedium*
   pMed1 = gGeoManager->Medium("Lead"        , kMedLead, kMatLead, 0, 0, 0., 10.0, 0.1, 0.1, 0.1, 0.1);
   pMed1->Print();
   TGeoMedium*
   pMed2 = gGeoManager->Medium("Scintillator", kMedScin, kMatPoly, 1, 0, 0., 10.0, 0.1, 0.1, 0.1, 0.1);
   pMed2->Print();
   TGeoMedium*
   pMed3 = gGeoManager->Medium("Iron", kMedFe, kMatFe, 0, 0, 0., 10.0, 0.1, 0.1, 0.1, 0.1);
   pMed3->Print();
   TGeoMedium*
   pMed4 = gGeoManager->Medium("PbWO", kMedPbWO, kMatPbWO, 1, 0, 0., 10.0, 0.1, 0.1, 0.1, 0.1);
   pMed4->Print(); 
   
   //- Volumes 

   //Top Volume
   TGeoVolume* pWorld = gGeoManager->GetTopVolume();
   pWorld->SetVisLeaves(kTRUE);

   
   Double_t dx = 2.1/2.;
   Double_t dy = 2.1/2.;
   Double_t dz = 22.5/2.;
   TGeoShape *pLuMonBox = new TGeoBBox("LUMONBox", dx,dy,dz);
   // Volume: LuMon
   TGeoVolume*
   pLuMonLog = new TGeoVolume("LUMONLog",pLuMonBox, pMed4);
   pLuMonLog->SetVisLeaves(kTRUE);

   // Rotation (Unity) 
   TGeoRotation *pRot = new TGeoRotation();

   Double_t step = 2.1;  // step size of 2.1 cm

   // Single Crystal Module Size
   dx = -2.1;
   dy = -2.1;
   dz = +200.0;

   // Numbering for the copies
   Int_t nb=0;

   for (Int_t iCol = 0; iCol<3 ;iCol++){
      for (Int_t iRow = 0; iRow<3; iRow++) {
	 pWorld->AddNode( pLuMonLog,nb,  new TGeoCombiTrans("", dx+iCol*step,dy+iRow*step,dz,pRot));
	 nb++;
      }
   }
   

   // Declare all modules as sensitive
   AddSensitiveVolume(pLuMonLog);
   fNbOfSensitiveVol+=1;

}


void ELILuMon::ConstructGeometry2() {
// Dummy function
}



ClassImp(ELILuMon)
