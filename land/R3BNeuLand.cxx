// -------------------------------------------------------------------------
// -----                        R3BNeuLand source file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------
#include "R3BNeuLand.h"

#include "R3BGeoLand.h"
#include "R3BNeuLandPoint.h"
#include "R3BGeoLandPar.h"

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
#include "TGeoShapeAssembly.h"
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;



// -----   Default constructor   -------------------------------------------
R3BNeuLand::R3BNeuLand() : R3BDetector("R3BNeuLand", kTRUE, kLAND) {
  ResetParameters();
  fLandCollection = new TClonesArray("R3BNeuLandPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fVersion =1;

  fIdMedFe = -1;
  fIdMedGlass = -1;
  fIdMedGas = -1;


}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BNeuLand::R3BNeuLand(const char* name, Bool_t active) 
  : R3BDetector(name, active, kLAND) {
  ResetParameters();
  fLandCollection = new TClonesArray("R3BNeuLandPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fVersion = 1;

  fIdMedFe = -1;
  fIdMedGlass = -1;
  fIdMedGas = -1;

}

R3BNeuLand::~R3BNeuLand() {

  if ( flGeoPar ) delete flGeoPar;
  if (fLandCollection) {
    fLandCollection->Delete();
    delete fLandCollection;
  }
}

void R3BNeuLand::Initialize()
{
  FairDetector::Initialize();

}


Bool_t R3BNeuLand::ProcessHits(FairVolume* vol) {

  // --- get Geometry hiearchical Information
  Int_t cp1=-1;
  Int_t cp2=-1;
  Int_t cp3=-1;

  Int_t volId1=-1;
  Int_t volId2=-1;
  Int_t volId3=-1;

  Int_t iMed = gMC->CurrentMedium();   
  //  cout << " iMed " << iMed << " iMedGas:  " << fIdMedGas << endl; 

  if ( iMed == fIdMedFe ) {
    cout << " in Fe: Track Pid " << gMC->TrackPid() << endl;
   }

  if (iMed == fIdMedGas){

    volId1 =  gMC->CurrentVolID(cp1);
    volId2 =  gMC->CurrentVolOffID(1, cp2);
    volId3 =  gMC->CurrentVolOffID(2, cp3);
    
    //  cout << " ProcessHit Volume: " << vol->getName() << " Id2: " << volId2 
    //	 << " copyNr2: " << cp2  
    //	 << " Id3: " << volId3  << " copyNr3 " << cp3 
    //	 << " Id1: " << volId1  << " copyNr1 " << cp1 << endl; 
    
    if ( gMC->IsTrackEntering() ) {
      fELoss  = 0.;
      fTime   = gMC->TrackTime() * 1.0e09;
      fLength = gMC->TrackLength();
      gMC->TrackPosition(fPosIn);
      gMC->TrackMomentum(fMomIn);
    }
    
    // Sum energy loss for all steps in the active volume
    fELoss += gMC->Edep();
    
    // Set additional parameters at exit of active volume. Create R3BNeuLandPoint.
    if ( gMC->IsTrackExiting()    ||
	 gMC->IsTrackStop()       ||
	 gMC->IsTrackDisappeared()   ) {
      fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
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
	//  TGeoNode *bla = gGeoManager->FindNextBoundary(2);
	safety = gGeoManager->GetSafeDistance();
	
	
	gGeoManager->SetCurrentDirection(-newdirection[0],-newdirection[1],-newdirection[2]);
	
	for (Int_t i=0; i<3; i++){
	  newpos[i] = oldpos[i] - (3*safety*olddirection[i]);
	}
	
	if ( fPosIn.Z() < 30. && newpos[2] > 30.02 ) {
	  cerr << "-I- R3BNeuLand : 2nd direction: " << olddirection[0] << "," << olddirection[1] << "," << olddirection[2] 
	       << " with safety = " << safety << endl;
	  cerr << "-I- R3BNeuLand oldpos = " << oldpos[0] << "," << oldpos[1] << "," << oldpos[2] << endl;
	  cerr << "-I- R3BNeuLand newpos = " << newpos[0] << "," << newpos[1] << "," << newpos[2] << endl;
	}
	
	fPosOut.SetX(newpos[0]);
	fPosOut.SetY(newpos[1]);
	fPosOut.SetZ(newpos[2]);
      }
      
      AddHit(fTrackID, fVolumeID,   cp3,   cp2, cp1,
	     TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
	     TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
	     TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
	     TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
	     fTime, fLength, fELoss);
      
      // Increment number of NeuLandPoints for this track
      FairStack* stack = (FairStack*) gMC->GetStack();
      stack->AddPoint(kLAND);
      
      ResetParameters();
    }
  }// ! iMed == Gas  
    return kTRUE;
}




// -----   Public method EndOfEvent   -----------------------------------------
void R3BNeuLand::BeginEvent() {

    if (gGeoManager) {
    TGeoVolume * vol = gGeoManager->FindVolumeFast("padle_h_box3");
    // cout << "UID box3 : " << gGeoManager->GetUID("padle_h_box3") << endl;
    //cout << "UID box4 : " << gGeoManager->GetUID("padle_h_box4") << endl;
    //cout << "UID box5 : " << gGeoManager->GetUID("padle_h_box5") << endl;
	    if (vol) {
	       //	cout << "id box3 serial number: " << vol->GetNumber() << endl;
	    }
    }

    // Store the Materials Id
    fIdMedFe    = pMedFe->GetId();
    fIdMedGlass = pMed_glas->GetId();
    fIdMedGas   = pMed_gas->GetId();

}


void R3BNeuLand::EndOfEvent() {

  if (fVerboseLevel) Print();
  fLandCollection->Clear();

  ResetParameters();
}


// -----   Public method Register   -------------------------------------------
void R3BNeuLand::Register() {
  FairRootManager::Instance()->Register("NeuLandPoint", GetName(), fLandCollection, kTRUE);
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BNeuLand::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fLandCollection;
  else return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void R3BNeuLand::Print() const {
  Int_t nHits = fLandCollection->GetEntriesFast();
  cout << "-I- R3BNeuLand: " << nHits << " points registered in this event." 
       << endl;
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void R3BNeuLand::Reset() {
  fLandCollection->Clear();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void R3BNeuLand::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- R3BNeuLand: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  R3BNeuLandPoint* oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++) {
   oldpoint = (R3BNeuLandPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) R3BNeuLandPoint(*oldpoint);
    fPosIndex++;
  }
   cout << " -I- R3BNeuLand: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}

// -----   Private method AddHit   --------------------------------------------
R3BNeuLandPoint* R3BNeuLand::AddHit(Int_t trackID, Int_t detID, Int_t box, Int_t id1, Int_t id2,
			     TVector3 posIn,
			     TVector3 posOut, TVector3 momIn,
			     TVector3 momOut, Double_t time,
			     Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fLandCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1) 
    cout << "-I- R3BNeuLand: Adding Point at (" << posIn.X() << ", " << posIn.Y() 
	 << ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
	 << trackID << ", energy loss " << eLoss  << " GeV" << endl;
  return new(clref[size]) R3BNeuLandPoint(trackID, detID, box, id1, id2,  posIn, posOut,
				      momIn, momOut, time, length, eLoss);
}
// -----   Public method ConstructGeometry   ----------------------------------
void R3BNeuLand::ConstructGeometry() {
   if ((fVersion == 1 ) || (fVersion == 2 )) return ConstructGeometry1();

   // New Design for RPC
   if ((fVersion == 3 )) return ConstructGeometry2();

   cout << "-I- R3BNeuLand  ConstruGeometry() :unknown Geometry version  !!! " << endl;
}


void R3BNeuLand::ConstructGeometry1() {
  // This is the  defintion for RPC based Land
  // Detector
  // out-of-file geometry definition
   Double_t dx,dy,dz;
   Double_t dx1, dx2, dy1, dy2;
   Double_t a;
   Double_t z, density, w;
   Int_t nel, numed;
   Double_t radl,absl;

   Double_t  sumWeight = 0.0;

/****************************************************************************/
// Material definition
// User tracking media parameters   

    Double_t par[8];
    par[0]  = 0.000000; // isvol
    par[1]  = 0.000000; // ifield
    par[2]  = 0.000000; // fieldm
    par[3]  = 10.00000; // tmaxfd
    par[4]  = 0.100000; // stemax
    par[5]  = 0.050000; // deemax
    par[6]  = 0.001000; // epsil
    par[7]  = 0.001000; // stmin
   
//-- Mixture: Air
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
    numed   = 22;  // medium number
    pMed2 = new TGeoMedium("Air", numed,pMat2, par);
   }

//--  Material: Iron
   TGeoMedium * pMedFe=NULL;
   if (gGeoManager->GetMedium("Iron") ){
       pMedFe=gGeoManager->GetMedium("Iron");
   }else{
    w       =        0.;
    a       = 55.850000;
    z       = 26.000000;
    density = 7.870000;
    radl    = 1.757717;
    absl    = 169.994418;
    TGeoMaterial*
	pMatFe = new TGeoMaterial("Iron", a,z,density,radl,absl);
    pMatFe->SetIndex(701);
    numed   = 23;  // medium number
    pMedFe = new TGeoMedium("Iron", numed,pMatFe, par);
   }

//-- Mixture: PolyStyrene
    nel     = 2;
    density = 1.032;
    Double_t aps[2] = {12.011,1.008};
    Double_t zps[2] = {6.,1.};
    Double_t wps[2] = {1.,1.104};

    // renormalized
    sumWeight = 0.0;
    for (Int_t i=0; i<nel; i++) sumWeight += aps[i]*wps[i];
    for (Int_t i=0; i<nel; i++) wps[i] *= aps[i]/sumWeight;



    TGeoMixture*
    pMat_ps = new TGeoMixture("PS", nel,density);

    pMat_ps->DefineElement(0,aps[0],zps[0],wps[0]);
    pMat_ps->DefineElement(1,aps[1],zps[1],wps[1]);
    pMat_ps->SetIndex(24);
    // Medium: Air
    numed   = 24;  // medium number
    TGeoMedium*
    pMed_ps = new TGeoMedium("PS", numed,pMat_ps, par);

//-- Mixture: Quartz Glas
    nel     = 2;
    density = 2.5;
    Double_t aglas[2] = {16.,28.};
    Double_t zglas[2] = {8.,14.};
    Double_t wglas[2] = {2.,1.};
    // renormalized
    sumWeight = 0.0;
    for (Int_t i=0; i<nel; i++) sumWeight += aglas[i]*wglas[i];
    for (Int_t i=0; i<nel; i++) wglas[i] *= aglas[i]/sumWeight;
    
    TGeoMixture*
    pMat_glas = new TGeoMixture("QGLASS", nel,density);

    pMat_glas->DefineElement(0,aglas[0],zglas[0],wglas[0]);
    pMat_glas->DefineElement(1,aglas[1],zglas[1],wglas[1]);
    pMat_glas->SetIndex(24);
    // Medium: Quartz glas
    numed   = 25;  // medium number
    TGeoMedium*
    pMed_glas = new TGeoMedium("QGLAS", numed,pMat_glas, par);

//

    

//-- Geometry defintion

//-- Mixture: RPC Gas
    nel     = 4;
    density = 0.0053;
    Double_t agas[4] = {1.,12.,19.,32.};
    Double_t zgas[4] = {1.,6.,9.,16.};
    Double_t wgas[4] = {2.2,1.9,4.,0.85};

    // renormalized
    sumWeight = 0.0;
    for (Int_t i=0; i<nel; i++) sumWeight += agas[i]*wgas[i];
    for (Int_t i=0; i<nel; i++) wgas[i] *= agas[i]/sumWeight;


    TGeoMixture*
    pMat_gas = new TGeoMixture("RPCGAS", nel,density);

    pMat_gas->DefineElement(0,agas[0],zgas[0],wgas[0]);
    pMat_gas->DefineElement(1,agas[1],zgas[1],wgas[1]);
    pMat_gas->DefineElement(2,agas[2],zgas[2],wgas[2]);
    pMat_gas->DefineElement(3,agas[3],zgas[3],wgas[3]);
    pMat_gas->SetIndex(25);
    // Medium: RPCgas
    numed   = 26;  // medium number
    // More fine steping def. in Gas
    par[0]  = 0.000000; // isvol
    par[1]  = 0.000000; // ifield
    par[2]  = 0.000000; // fieldm
    par[3]  = 10.00000; // tmaxfd
    par[4]  = 0.010000; // stemax
    par[5]  = 0.005000; // deemax
    par[6]  = 0.001000; // epsil
    par[7]  = 0.001000; // stmin
    
    TGeoMedium*
    pMed_gas = new TGeoMedium("RPCGAS", numed,pMat_gas, par);


//-- Main Geometry definition
//-- Get the top volume (CAVE) from the TGeoManager class
    
    TGeoVolume* vWorld = gGeoManager->GetTopVolume();
    vWorld->SetVisLeaves(kTRUE);
    
// -- Define paddle
    // Volume : GAS
    Double_t padx = 99.8;
    Double_t pady = 24.8;
    Double_t padz = 0.015;
    TGeoShape* pad_gas = new TGeoBBox("paddle_gas1",
				       padx,
				       pady,
				       padz);

    TGeoVolume*
       pGas = new TGeoVolume("GAS",pad_gas, pMed_gas);
    pGas->SetVisLeaves(kTRUE);

    // Add Gas as a sensitive volume
    AddSensitiveVolume(pGas);


    // Store the Materials Id
    fIdMedFe    = pMedFe->GetId();
    fIdMedGlass = pMed_glas->GetId();
    fIdMedGas   = pMed_gas->GetId();




    // Volume GLASS
    padx = 99.8;
    pady = 24.8;
    padz = 0.05;
    TGeoShape* pad_glas = new TGeoBBox("paddle_glas",
				       padx,
				       pady,
				       padz);
    TGeoVolume*
       pGlas = new TGeoVolume("GLAS",pad_glas, pMed_glas);
    pGlas->SetVisLeaves(kTRUE);

    // Volume : IRON
    padx = 100.0;
    pady = 25.0;
    padz = 1.02;
    TGeoShape* pad_mod1 = new TGeoBBox("paddle_mod1",
				       padx,
				       pady,
				       padz);    
  // Volume: MOD1
    TGeoVolume*
       pMod1 = new TGeoVolume("MOD1",pad_mod1, pMedFe);
    pMod1->SetVisLeaves(kTRUE);
    
    
    
    TGeoRotation *rot = new TGeoRotation();
    rot->RotateX(0.);
    rot->RotateY(0.);
    rot->RotateZ(0.);
    
    Double_t tx = 0.0;
    Double_t ty = 0.0;
    Double_t tz = 0.0;
    
  //- Create a Module 
    
    tz = 0.77;
    pMod1->AddNode(pGlas,0, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = 0.705;
    pMod1->AddNode(pGas,1, new TGeoCombiTrans(tx,ty,tz,rot));

    tz = 0.640;
    pMod1->AddNode(pGlas,2, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = 0.575;
    pMod1->AddNode(pGas,3, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = 0.510; 
    pMod1->AddNode(pGlas,4, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = 0.445;
    pMod1->AddNode(pGas,5, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = 0.38;  
    pMod1->AddNode(pGlas,6, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = 0.315;
    pMod1->AddNode(pGas,7, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = 0.25; 
    pMod1->AddNode(pGlas,8, new TGeoCombiTrans(tx,ty,tz,rot));

    tz = -0.250;
    pMod1->AddNode(pGlas,9, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = -0.315;
    pMod1->AddNode(pGas,10, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = -0.380;
    pMod1->AddNode(pGlas,11, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = -0.445;
    pMod1->AddNode(pGas,12, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = -0.510; 
    pMod1->AddNode(pGlas,13, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = -0.575;
    pMod1->AddNode(pGas,14, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = -0.640;  
    pMod1->AddNode(pGlas,15, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = -0.705;
    pMod1->AddNode(pGas,16, new TGeoCombiTrans(tx,ty,tz,rot));
    tz = -0.770; 
    pMod1->AddNode(pGlas,17, new TGeoCombiTrans(tx,ty,tz,rot));      

    
   // Full Geometry Definition
     Double_t posZ =1000.;

    if ( fVersion == 1 ){
       Double_t thx,thy,thz;
       Double_t phx,phy,phz;
       thx = 90.0;    phx = 270.0;
       thy = 90.0;    phy = 0.0;
       thz = 0.0;     phz = 0.0;
       TGeoRotation *pRot2 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
       
       
       for ( Int_t i=0;i<50;i+=2){
	  
	  for (Int_t j=0;j<4;j++){
	     tx = 0.0;
	     ty = -75.3+(j)*50.2;
	     tz = posZ +(i)*2.14;
	     vWorld->AddNode(pMod1,(i)*4+j, new TGeoCombiTrans(tx,ty,tz,rot) );
             ty = 0.0;
	     tx = -75.3+(j)*50.2;
	     tz = posZ +(i+1)*2.14;
	     vWorld->AddNode(pMod1,(i+1)*4+j, new TGeoCombiTrans(tx,ty,tz,pRot2) );
	  }
	  
       } 
       
    }//! version 1
    
    // Module Only 
    if ( fVersion == 2 ){
       tz = posZ;
       vWorld->AddNode(pMod1,0, new TGeoCombiTrans(tx,ty,tz,rot));             
    }//! version 2
    
    
//!end of construction
}




void R3BNeuLand::ConstructGeometry2() {
  // This is the  defintion for RPC based Land
  // Detector

   Double_t dx,dy,dz;
   Double_t dx1, dx2, dy1, dy2;
   Double_t a;
   Double_t z, density, w;
   Int_t nel, numed;
   Double_t radl,absl;

   Double_t  sumWeight = 0.0;

/****************************************************************************/
// Material definition
// User tracking media parameters   

    Double_t par[8];
    par[0]  = 0.000000; // isvol
    par[1]  = 0.000000; // ifield
    par[2]  = 0.000000; // fieldm
    par[3]  = 10.00000; // tmaxfd
    par[4]  = 0.100000; // stemax
    par[5]  = 0.050000; // deemax
    par[6]  = 0.001000; // epsil
    par[7]  = 0.001000; // stmin
   
//-- Mixture: Air
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
    numed   = 22;  // medium number
    pMed2 = new TGeoMedium("Air", numed,pMat2, par);
   }

//--  Material: Iron
   pMedFe=NULL;
   if (gGeoManager->GetMedium("Iron") ){
       pMedFe=gGeoManager->GetMedium("Iron");
   }else{
    w       =        0.;
    a       = 55.850000;
    z       = 26.000000;
    density = 7.870000;
    radl    = 1.757717;
    absl    = 169.994418;
    TGeoMaterial*
	pMatFe = new TGeoMaterial("Iron", a,z,density,radl,absl);
    pMatFe->SetIndex(701);
    numed   = 23;  // medium number
    pMedFe = new TGeoMedium("Iron", numed,pMatFe, par);
   }

//-- Mixture: PolyStyrene
    nel     = 2;
    density = 1.032;
    Double_t aps[2] = {12.011,1.008};
    Double_t zps[2] = {6.,1.};
    Double_t wps[2] = {1.,1.104};
    // renormalized
    sumWeight = 0.0;
    for (Int_t i=0; i<nel; i++) sumWeight += aps[i]*wps[i];
    for (Int_t i=0; i<nel; i++) wps[i] *= aps[i]/sumWeight;

    TGeoMixture*
    pMat_ps = new TGeoMixture("PS", nel,density);
    pMat_ps->DefineElement(0,aps[0],zps[0],wps[0]);
    pMat_ps->DefineElement(1,aps[1],zps[1],wps[1]);
    pMat_ps->SetIndex(24);
    // Medium: Air
    numed   = 24;  // medium number
    TGeoMedium*
    pMed_ps = new TGeoMedium("PS", numed,pMat_ps, par);

//-- Mixture: Quartz Glas
    nel     = 2;
    density = 2.5;
    Double_t aglas[2] = {16.,28.};
    Double_t zglas[2] = {8.,14.};
    Double_t wglas[2] = {2.,1.};
    // renormalized
    sumWeight = 0.0;
    for (Int_t i=0; i<nel; i++) sumWeight += aglas[i]*wglas[i];
    for (Int_t i=0; i<nel; i++) wglas[i] *= aglas[i]/sumWeight;
    
    TGeoMixture*
    pMat_glas = new TGeoMixture("QGLASS", nel,density);

    pMat_glas->DefineElement(0,aglas[0],zglas[0],wglas[0]);
    pMat_glas->DefineElement(1,aglas[1],zglas[1],wglas[1]);
    pMat_glas->SetIndex(24);
    // Medium: Quartz glas
    numed   = 25;  // medium number
    pMed_glas = new TGeoMedium("QGLAS", numed,pMat_glas, par);

//-- Mixture: RPC Gas
    nel     = 4;
    density = 0.0053;
    Double_t agas[4] = {1.,12.,19.,32.};
    Double_t zgas[4] = {1.,6.,9.,16.};
    Double_t wgas[4] = {2.2,1.9,4.,0.85};

    // renormalized
    sumWeight = 0.0;
    for (Int_t i=0; i<nel; i++) sumWeight += agas[i]*wgas[i];
    for (Int_t i=0; i<nel; i++) wgas[i] *= agas[i]/sumWeight;

    TGeoMixture*
    pMat_gas = new TGeoMixture("RPCGAS", nel,density);
    pMat_gas->DefineElement(0,agas[0],zgas[0],wgas[0]);
    pMat_gas->DefineElement(1,agas[1],zgas[1],wgas[1]);
    pMat_gas->DefineElement(2,agas[2],zgas[2],wgas[2]);
    pMat_gas->DefineElement(3,agas[3],zgas[3],wgas[3]);
    pMat_gas->SetIndex(25);
    // Medium: RPCgas
    numed   = 26;  // medium number
    // More fine steping def. in Gas
    par[0]  = 0.000000; // isvol
    par[1]  = 0.000000; // ifield
    par[2]  = 0.000000; // fieldm
    par[3]  = 10.00000; // tmaxfd
    par[4]  = 0.0010000; // stemax
    par[5]  = 0.005000; // deemax
    par[6]  = 0.001000; // epsil
    par[7]  = 0.001000; // stmin
    
    pMed_gas = new TGeoMedium("RPCGAS", numed,pMat_gas, par);


//-- Main Geometry definition
//-- Get the top volume (CAVE) from the TGeoManager class
    
    TGeoVolume* vWorld = gGeoManager->GetTopVolume();
    vWorld->SetVisLeaves(kTRUE);

   // Define RPC with volume division
    Double_t padx = 100.0;
    Double_t pady = 25.0;
    Double_t padz = 1.02;
    TGeoShape* pad_mod1 = new TGeoBBox("paddle_mod1",
				       padx,
				       pady,
				       padz);    
  // Volume: MOD1
    TGeoVolume*
       pMod1 = new TGeoVolume("IRONVOL",pad_mod1, pMedFe);
   
    Double_t delta_Fe = 0.2; //[cm]
    Double_t delta_Glass = 0.05; 
    Double_t delta_Gas = 0.03;


    Int_t ndiv = ( 2*padz - delta_Fe ) / (delta_Glass+delta_Gas);
    Double_t start_z = -padz + delta_Fe ;
    Double_t step = (delta_Glass+delta_Gas);

    cout << "ndiv: " << ndiv << " start_z: " << start_z << " step: " << step << endl;
 

    TGeoVolume *divVolGlass =
     gGeoManager->Division("FeSLICE","IRONVOL",3, ndiv , start_z , step, pMed_glas->GetId()); 

    TGeoVolume *divVolGas =
     gGeoManager->Division("CELL","FeSLICE",3, 1 ,-step/2+delta_Glass , 0, pMed_gas->GetId());     
    AddSensitiveVolume(divVolGas);

   // Make the elementary assembly of the whole structure
    TGeoVolume *aRPCModule = new TGeoVolumeAssembly("RPCMODULE"); 
   
    // Anode 
    Double_t apadx = 100.0;
    Double_t apady = 25.0;
    Double_t apadz = delta_Fe;
    TGeoShape* pAnode = new TGeoBBox("anode",
				       apadx,
				       apady,
				       apadz);    
    TGeoVolume*
    pAnode_v = new TGeoVolume("ANODE",pAnode, pMedFe);
   
    TGeoRotation *rot = new TGeoRotation();
    rot->RotateX(0.);
    rot->RotateY(0.);
    rot->RotateZ(0.);
    
    Double_t tx = 0.0;
    Double_t ty = 0.0;
    Double_t tz = 0.0;

    // add divided volume to top Volume
    aRPCModule->AddNode(pMod1,    1, new TGeoCombiTrans(tx,ty,tz,rot) );
    tz = padz+apadz;
    aRPCModule->AddNode(pAnode_v, 2, new TGeoCombiTrans(tx,ty,tz,rot) );
    tz = padz+ 2*apadz + padz;
    rot->RotateY(360.);
    aRPCModule->AddNode(pMod1,    3, new TGeoCombiTrans(tx,ty,tz,rot) );

    rot->RotateX(0.);
    rot->RotateY(0.);
    rot->RotateZ(0.);
    tx = 0.0;
    ty = 0.0;
    tz = 0.0;

    // add divided volume to top Volume
    vWorld->AddNode(aRPCModule,1, new TGeoCombiTrans(tx,ty,tz,rot) );

}




ClassImp(R3BNeuLand)
