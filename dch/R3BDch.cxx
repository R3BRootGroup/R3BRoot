// -------------------------------------------------------------------------
// -----                        R3BDch source file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------
#include "R3BDch.h"

#include "R3BGeoDch.h"
#include "R3BDchPoint.h"
#include "R3BGeoDchPar.h"

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
R3BDch::R3BDch() : R3BDetector("R3BDch", kTRUE, kDCH) {
  ResetParameters();
  fDchCollection = new TClonesArray("R3BDchPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BDch::R3BDch(const char* name, Bool_t active) 
  : R3BDetector(name, active, kDCH) {
  ResetParameters();
  fDchCollection = new TClonesArray("R3BDchPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BDch::~R3BDch() {

  if ( flGeoPar ) delete flGeoPar;
  if (fDchCollection) {
    fDchCollection->Delete();
    delete fDchCollection;
  }
}
// -------------------------------------------------------------------------

void R3BDch::Initialize()
{
  FairDetector::Initialize();

   cout << endl;
   cout << "-I- R3BDch: initialisation" << endl;
   cout << "-I- R3BDch: Sci. Vol: (McId) " << gMC->VolId("DCHLog")<< endl;

}

void R3BDch::SetSpecialPhysicsCuts(){

   cout << endl;
   cout << "-I- R3Dch: Adding customized Physics cut: " << endl;
   cout << "-I- R3Dch: not yet implemented ... " << endl;
   cout << endl;
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BDch::ProcessHits(FairVolume* vol) {
// Set parameters at entrance of volume. Reset ELoss.
// get Info from DCH planes
    Int_t copyNo  = -1;
    Int_t planeNr = -1;
// Get the Geo info from MC Point
    gMC->CurrentVolID(copyNo);
    gMC->CurrentVolOffID(1,planeNr);


    if ( gMC->IsTrackEntering() ) {
    fELoss  = 0.;
    fTime   = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPosIn);
    gMC->TrackMomentum(fMomIn);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Set additional parameters at exit of active volume. Create R3BDchPoint.
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) {
    fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
    fVolumeID = vol->getMCid();
    gMC->TrackPosition(fPosOut);
    gMC->TrackMomentum(fMomOut);

  // <D.B> remove this test  here or not ????
 //   if (fELoss == 0. ) return kFALSE;
    
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
    //  TGeoNode *bla = gGeoManager->FindNextBoundary(2);
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


    AddHit(fTrackID, fVolumeID, planeNr ,
	   TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
	   TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
	   TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
	   TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
	   fTime, fLength, fELoss);
    
    // Increment number of DchPoints for this track
    FairStack* stack = (FairStack*) gMC->GetStack();
    stack->AddPoint(kDCH);
    
    ResetParameters();
  }

  return kTRUE;
}
// ----------------------------------------------------------------------------
//void R3BDch::SaveGeoParams(){
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
void R3BDch::BeginEvent() {

//  if (! kGeoSaved ) {
//      SaveGeoParams();
//  cout << "-I STS geometry parameters saved " << endl;
//  kGeoSaved = kTRUE;
//  }

}
// -----   Public method EndOfEvent   -----------------------------------------
void R3BDch::EndOfEvent() {

  if (fVerboseLevel) Print();
  fDchCollection->Clear();

  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method Register   -------------------------------------------
void R3BDch::Register() {
  FairRootManager::Instance()->Register("DCHPoint", GetName(), fDchCollection, kTRUE);
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BDch::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fDchCollection;
  else return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void R3BDch::Print() const {
  Int_t nHits = fDchCollection->GetEntriesFast();
  cout << "-I- R3BDch: " << nHits << " points registered in this event." 
       << endl;
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void R3BDch::Reset() {
  fDchCollection->Clear();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void R3BDch::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- R3BDch: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  R3BDchPoint* oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++) {
   oldpoint = (R3BDchPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) R3BDchPoint(*oldpoint);
    fPosIndex++;
  }
   cout << " -I- R3BDch: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}

// -----   Private method AddHit   --------------------------------------------
R3BDchPoint* R3BDch::AddHit(Int_t trackID, Int_t detID, Int_t plane, TVector3 posIn,
			    TVector3 posOut, TVector3 momIn, 
			    TVector3 momOut, Double_t time, 
			    Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fDchCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1) 
    cout << "-I- R3BDch: Adding Point at (" << posIn.X() << ", " << posIn.Y() 
	 << ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
	 << trackID << ", energy loss " << eLoss*1e06 << " keV" << endl;
  return new(clref[size]) R3BDchPoint(trackID, detID, plane, posIn, posOut,
				      momIn, momOut, time, length, eLoss);
}
// -----   Public method ConstructGeometry   ----------------------------------
void R3BDch::ConstructGeometry() {

  // out-of-file geometry definition
   Double_t dx,dy,dz;
   Double_t theta, phi;
   Double_t a;
   Double_t thx, phx, thy, phy, thz, phz;
   Double_t alpha;
   Double_t z, density, radl, absl, w;
   Int_t nel, numed;


/****************************************************************************/
// Material definition

   // Mixture: Air
   TGeoMedium * pMed2=NULL;
   if (gGeoManager->GetMedium("Air") ){
       pMed2=gGeoManager->GetMedium("Air");
   }else{
    nel     = 2;
    density = 0.001290;
    TGeoMixture*
	pMat2 = new TGeoMixture("Air", nel,density);
    a = 14.006740;   z = 7.000000;   w = 0.700000;  // N
    pMat2->DefineElement(0,a,z,w);
    a = 15.999400;   z = 8.000000;   w = 0.300000;  // O
    pMat2->DefineElement(1,a,z,w);
    pMat2->SetIndex(1);
    // Medium: Air
    numed   = 1;  // medium number
    Double_t par[8];
    par[0]  = 0.000000; // isvol
    par[1]  = 0.000000; // ifield
    par[2]  = 0.000000; // fieldm
    par[3]  = 0.000000; // tmaxfd
    par[4]  = 0.000000; // stemax
    par[5]  = 0.000000; // deemax
    par[6]  = 0.000100; // epsil
    par[7]  = 0.000000; // stmin
    pMed2 = new TGeoMedium("Air", numed,pMat2, par);
   }

// Mixture: mixtureForDCH
   TGeoMedium * pMed33=NULL;
   if (gGeoManager->GetMedium("mixtureForDCH") ){
       pMed33=gGeoManager->GetMedium("mixtureForDCH");
   }else{
      nel     = 3;
      density = 0.001017;
      TGeoMixture*
	  pMat33 = new TGeoMixture("mixtureForDCH", nel,density);
      a = 39.948000;   z = 18.000000;   w = 0.800000;  // AR
      pMat33->DefineElement(0,a,z,w);
      a = 12.010700;   z = 6.000000;   w = 0.054582;  // C
      pMat33->DefineElement(1,a,z,w);
      a = 15.999400;   z = 8.000000;   w = 0.145418;  // O
      pMat33->DefineElement(2,a,z,w);
      pMat33->SetIndex(32);
      // Medium: mixtureForDCH
      numed   = 32;  // medium number
      Double_t par[8];
      par[0]  = 0.000000; // isvol
      par[1]  = 0.000000; // ifield
      par[2]  = 0.000000; // fieldm
      par[3]  = 0.000000; // tmaxfd
      par[4]  = 0.000000; // stemax
      par[5]  = 0.000000; // deemax
      par[6]  = 0.000100; // epsil
      par[7]  = 0.000000; // stmin
      pMed33 = new TGeoMedium("mixtureForDCH", numed,pMat33, par);
   }

   // Material: Aluminum
   TGeoMedium * pMed21=NULL;
   if (gGeoManager->GetMedium("Aluminum") ){
       pMed21=gGeoManager->GetMedium("Aluminum");
   }else{
      a       = 26.980000;
      z       = 13.000000;
      density = 2.700000;
      radl    = 8.875105;
      absl    = 388.793113;

      TGeoMaterial *matAl
	   = new TGeoMaterial("Aluminum", a,z,density,radl,absl);

      Double_t par[8];
      par[0]  = 0.000000; // isvol
      par[1]  = 0.000000; // ifield
      par[2]  = 0.000000; // fieldm
      par[3]  = 0.000000; // tmaxfd
      par[4]  = 0.000000; // stemax
      par[5]  = 0.000000; // deemax
      par[6]  = 0.000100; // epsil
      par[7]  = 0.000000; // stmin

      pMed21 = new TGeoMedium("Aluminum",3, matAl, par);
   }


// Mixture: Mylar
   TGeoMedium * pMed15=NULL;
   if (gGeoManager->GetMedium("Mylar") ){
       pMed15=gGeoManager->GetMedium("Mylar");
   }else{
    nel     = 3;
    density = 1.397000;
    TGeoMixture*
	pMat15 = new TGeoMixture("Mylar", nel,density);
    a = 12.010700;   z = 6.000000;   w = 0.625010;  // C
    pMat15->DefineElement(0,a,z,w);
    a = 1.007940;   z = 1.000000;   w = 0.041961;  // H
    pMat15->DefineElement(1,a,z,w);
    a = 15.999400;   z = 8.000000;   w = 0.333029;  // O
    pMat15->DefineElement(2,a,z,w);
    pMat15->SetIndex(14);
    // Medium: Mylar
    numed   = 14;  // medium number
    Double_t par[8];
    par[0]  = 0.000000; // isvol
    par[1]  = 0.000000; // ifield
    par[2]  = 0.000000; // fieldm
    par[3]  = 0.000000; // tmaxfd
    par[4]  = 0.000000; // stemax
    par[5]  = 0.000000; // deemax
    par[6]  = 0.000100; // epsil
    par[7]  = 0.000000; // stmin
    pMed15 = new TGeoMedium("Mylar", numed,pMat15,par);
   }


// Material: HeliumGas
   TGeoMedium * pMed4=NULL;
   if (gGeoManager->GetMedium("HeliumGas") ){
       pMed4=gGeoManager->GetMedium("HeliumGas");
   }else{
     a       = 4.000000;
     z       = 2.000000;
     density = 0.000125;
     radl    = 683475.828563;
     absl    = 4444726.310227;
     TGeoMaterial*
	 pMat4 = new TGeoMaterial("HeliumGas", a,z,density,radl,absl);
     pMat4->SetIndex(50);
     // Medium: HeliumGas
     numed   = 50;  // medium number
     Double_t par[8];
     par[0]  = 0.000000; // isvol
     par[1]  = 0.000000; // ifield
     par[2]  = 0.000000; // fieldm
     par[3]  = 0.000000; // tmaxfd
     par[4]  = 0.000000; // stemax
     par[5]  = 0.000000; // deemax
     par[6]  = 0.000100; // epsil
     par[7]  = 0.000000; // stmin
     pMed4 = new TGeoMedium("HeliumGas", numed,pMat4, par);
   }

    // TRANSFORMATION MATRICES
   // Combi transformation: 
   dx = 128.700000;
   dy = 0.000000;
   dz = 443.900000;
   // Rotation: 
   thx = 121.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 31.000000;    phz = 0.000000;
   TGeoRotation *pMatrix3 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix2 = new TGeoCombiTrans("", dx,dy,dz,pMatrix3);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix9 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
    TGeoCombiTrans*
   pMatrix8 = new TGeoCombiTrans("", dx,dy,dz,pMatrix9);
   // Combi transformation: 
   dx = 0.000000;
   dy = 42.200000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix11 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
    TGeoCombiTrans*
   pMatrix10 = new TGeoCombiTrans("", dx,dy,dz,pMatrix11);
   // Combi transformation: 
   dx = 0.000000;
   dy = -42.200000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix13 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
    TGeoCombiTrans*
   pMatrix12 = new TGeoCombiTrans("", dx,dy,dz,pMatrix13);
   // Combi transformation: 
   dx = 53.400000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix15 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix14 = new TGeoCombiTrans("", dx,dy,dz,pMatrix15);
   // Combi transformation: 
   dx = -53.400000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix17 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix16 = new TGeoCombiTrans("", dx,dy,dz,pMatrix17);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 4.060600;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix19 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix18 = new TGeoCombiTrans("", dx,dy,dz,pMatrix19);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = -4.060600;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix21 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
    TGeoCombiTrans*
   pMatrix20 = new TGeoCombiTrans("", dx,dy,dz,pMatrix21);
   // Combi transformation: 
   dx = 169.100000;
   dy = 0.000000;
   dz = 535.800000;
   // Rotation: 
   thx = 121.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 31.000000;    phz = 0.000000;
   TGeoRotation *pMatrix5 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix4 = new TGeoCombiTrans("", dx,dy,dz,pMatrix5);
   // Combi transformation: 
   dx = 148.900000;
   dy = 0.000000;
   dz = 489.850000;
   // Rotation: 
   thx = 121.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 31.000000;    phz = 0.000000;
   TGeoRotation *pMatrix7 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix6 = new TGeoCombiTrans("", dx,dy,dz,pMatrix7);

   // SET TOP VOLUME OF GEOMETRY
   TGeoVolume * pWorld = gGeoManager->GetTopVolume();
   pWorld->SetVisLeaves(kTRUE);

   //Create a global Mother Volume
   /*
   dx = 100.000000;
   dy = 100.000000;
   dz = 100.000000;
   TGeoShape *pBoxWorld = new TGeoBBox("DCHBoxWorld", dx,dy,dz);
   TGeoVolume*
   pWorld  = new TGeoVolume("DCHBoxLogWorld",pBoxWorld, pMed2);
   pWorld->SetVisLeaves(kTRUE);
   TGeoCombiTrans * t0 = new TGeoCombiTrans();
   TGeoCombiTrans *pGlobalc = GetGlobalPosition(t0);
   // add the sphere as Mother Volume
   pAWorld->AddNodeOverlap(pWorld, 0, pGlobalc);
   */

   // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
   // Shape: DCHBoxWorld type: TGeoBBox
   dx = 55.400000;
   dy = 44.200000;
   dz = 4.061200;
   TGeoShape *pDCHBoxWorld = new TGeoBBox("DCHBoxWorld", dx,dy,dz);
   // Volume: DCHLogWorld
   TGeoVolume*
   pDCHLogWorld = new TGeoVolume("DCHLogWorld",pDCHBoxWorld, pMed2);
   pDCHLogWorld->SetVisLeaves(kTRUE);


   pWorld->AddNode(pDCHLogWorld, 0, GetGlobalPosition(pMatrix2));
   pWorld->AddNode(pDCHLogWorld, 1, GetGlobalPosition(pMatrix4));

   // Shape: heliumBag type: TGeoPara
   dx    = 55.400000;
   dy    = 44.200000;
   dz    = 43.830510;
   alpha = 0.000000;
   theta = 0.000000;
   phi   = 0.000000;

   TGeoShape *pheliumBagv =
       new TGeoPara("heliumBag",dx,dy,dz,alpha,theta,phi);
   // Volume: heliumBag
   TGeoVolume*
   pheliumBag = new TGeoVolume("heliumBag",pheliumBagv, pMed4);
   pheliumBag->SetVisLeaves(kTRUE);
   pWorld->AddNode(pheliumBag, 0, GetGlobalPosition(pMatrix6));

   // Shape: DCHBox type: TGeoBBox
   dx = 51.400000;
   dy = 40.200000;
   dz = 4.060000;
   TGeoShape *pDCHBox = new TGeoBBox("DCHBox", dx,dy,dz);
   // Volume: DCHLog
   TGeoVolume*
   pDCHLog = new TGeoVolume("DCHLog",pDCHBox, pMed33);
   pDCHLog->SetVisLeaves(kTRUE);
   pDCHLogWorld->AddNode(pDCHLog, 0, pMatrix8);



   // Shape: UpFrame type: TGeoBBox
   dx = 55.400000;
   dy = 2.000000;
   dz = 4.061200;
   TGeoShape *pUpFrame = new TGeoBBox("UpFrame", dx,dy,dz);
   // Volume: logicUpFrame
   TGeoVolume*
   plogicUpFrame = new TGeoVolume("logicUpFrame",pUpFrame, pMed21);
   plogicUpFrame->SetVisLeaves(kTRUE);
   pDCHLogWorld->AddNode(plogicUpFrame, 0, pMatrix10);
   // Shape: DownFrame type: TGeoBBox
   dx = 55.400000;
   dy = 2.000000;
   dz = 4.061200;
   TGeoShape *pDownFrame = new TGeoBBox("DownFrame", dx,dy,dz);
   // Volume: logicDownFrame
   TGeoVolume*
   plogicDownFrame = new TGeoVolume("logicDownFrame",pDownFrame, pMed21);
   plogicDownFrame->SetVisLeaves(kTRUE);
   pDCHLogWorld->AddNode(plogicDownFrame, 0, pMatrix12);
   // Shape: RightFrame type: TGeoBBox
   dx = 2.000000;
   dy = 40.200000;
   dz = 4.060000;
   TGeoShape *pRightFrame = new TGeoBBox("RightFrame", dx,dy,dz);
   // Volume: logicRightFrame
   TGeoVolume*
   plogicRightFrame = new TGeoVolume("logicRightFrame",pRightFrame, pMed21);
   plogicRightFrame->SetVisLeaves(kTRUE);
   pDCHLogWorld->AddNode(plogicRightFrame, 0, pMatrix14);
   // Shape: LeftFrame type: TGeoBBox
   dx = 2.000000;
   dy = 40.200000;
   dz = 4.060000;
   TGeoShape *pLeftFrame = new TGeoBBox("LeftFrame", dx,dy,dz);
   // Volume: logicLeftFrame
   TGeoVolume*
   plogicLeftFrame = new TGeoVolume("logicLeftFrame",pLeftFrame, pMed21);
   plogicLeftFrame->SetVisLeaves(kTRUE);
   pDCHLogWorld->AddNode(plogicLeftFrame, 0, pMatrix16);
   // Shape: FrontFrame type: TGeoBBox
   dx = 51.400000;
   dy = 40.200000;
   dz = 0.000600;
   TGeoShape *pFrontFrame = new TGeoBBox("FrontFrame", dx,dy,dz);
   // Volume: logicFrontFrame
   TGeoVolume*
   plogicFrontFrame = new TGeoVolume("logicFrontFrame",pFrontFrame, pMed15);
   plogicFrontFrame->SetVisLeaves(kTRUE);
   pDCHLogWorld->AddNode(plogicFrontFrame, 0, pMatrix18);
   // Shape: BackFrame type: TGeoBBox
   dx = 51.400000;
   dy = 40.200000;
   dz = 0.000600;
   TGeoShape *pBackFrame = new TGeoBBox("BackFrame", dx,dy,dz);
   // Volume: logicBackFrame
   TGeoVolume*
   plogicBackFrame = new TGeoVolume("logicBackFrame",pBackFrame, pMed15);
   plogicBackFrame->SetVisLeaves(kTRUE);
   pDCHLogWorld->AddNode(plogicBackFrame, 0, pMatrix20);


   // add sensitive volume to DCH
   AddSensitiveVolume(pDCHLog);
   fNbOfSensitiveVol+=1;

}



/*
void R3BDch::ConstructGeometry() {
  
  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  R3BGeoDch*       stsGeo  = new R3BGeoDch();
  stsGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(stsGeo);

  Bool_t rc = geoFace->readSet(stsGeo);
  if (rc) stsGeo->create(geoLoad->getGeoBuilder());
  TList* volList = stsGeo->getListOfVolumes();
  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  R3BGeoDchPar* par=(R3BGeoDchPar*)(rtdb->getContainer("R3BGeoDchPar"));
  TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
  TObjArray *fPassNodes = par->GetGeoPassiveNodes();

  TListIter iter(volList);
  FairGeoNode* node   = NULL;
  FairGeoVolume *aVol=NULL;

  while( (node = (FairGeoNode*)iter.Next()) ) {
      aVol = dynamic_cast<FairGeoVolume*> ( node );
       if ( node->isSensitive()  ) {
           fSensNodes->AddLast( aVol );
       }else{
           fPassNodes->AddLast( aVol );
       }
  }
  par->setChanged();
  par->setInputVersion(fRun->GetRunId(),1);
  ProcessNodes( volList );

}
*/

ClassImp(R3BDch)
