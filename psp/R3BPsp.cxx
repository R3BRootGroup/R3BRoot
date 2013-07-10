// -------------------------------------------------------------------------
// -----                        R3BPsp source file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------
#include "R3BPsp.h"

#include "R3BGeoPsp.h"
#include "R3BPspPoint.h"
// #include "R3BGeoPspPar.h"
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
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;



// -----   Default constructor   -------------------------------------------
R3BPsp::R3BPsp() : R3BDetector("R3BPsp", kTRUE, kPSP) {
  ResetParameters();
  fPspCollection = new TClonesArray("R3BPspPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fCutE=1.e-03;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BPsp::R3BPsp(const char* name, Bool_t active) 
  : R3BDetector(name, active, kPSP) {
  ResetParameters();
  fPspCollection = new TClonesArray("R3BPspPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fCutE=1.e-03;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BPsp::~R3BPsp() {

  if ( flGeoPar ) delete flGeoPar;
  if (fPspCollection) {
    fPspCollection->Delete();
    delete fPspCollection;
  }
}
// -------------------------------------------------------------------------

void R3BPsp::Initialize()
{
  FairDetector::Initialize();

   cout << endl;
   cout << "-I- R3BPsp: initialisation" << endl;
   cout << "-I- R3BPsp: Vol. (McId) " << gMC->VolId("PSPLog")<< endl;

}


void R3BPsp::SetSpecialPhysicsCuts(){

   //pschrock 2013-06-07
   //this lines are fron R3BTra.cxx
   cout << endl;

	cout << "-I- R3BTra: Adding customized Physics cut ... " << endl;

	if (gGeoManager) {

		TGeoMedium* pSi = gGeoManager->GetMedium("Silicon");
		if ( pSi ) {
			// Setting processes for Si only
			gMC->Gstpar(pSi->GetId()  ,"LOSS",1);
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
			Double_t cutE = fCutE; // GeV-> 100 keV

			cout << "-I- R3bTra Silicon Medium Id " << pSi->GetId()
				<< " Energy Cut-Off : " << cutE
				<< endl;
			cout << endl;

			//Si
			gMC->Gstpar(pSi->GetId(),"CUTGAM",cutE);   /** gammas (GeV)*/
			gMC->Gstpar(pSi->GetId(),"CUTELE",cutE);   /** electrons (GeV)*/
			gMC->Gstpar(pSi->GetId(),"CUTNEU",cutE);   /** neutral hadrons (GeV)*/
			gMC->Gstpar(pSi->GetId(),"CUTHAD",cutE);   /** charged hadrons (GeV)*/
			gMC->Gstpar(pSi->GetId(),"CUTMUO",cutE);   /** muons (GeV)*/
			gMC->Gstpar(pSi->GetId(),"BCUTE" ,cutE);    /** electron bremsstrahlung (GeV)*/
			gMC->Gstpar(pSi->GetId(),"BCUTM" ,cutE);    /** muon and hadron bremsstrahlung(GeV)*/
			gMC->Gstpar(pSi->GetId(),"DCUTE" ,cutE);    /** delta-rays by electrons (GeV)*/
			gMC->Gstpar(pSi->GetId(),"DCUTM" ,cutE);    /** delta-rays by muons (GeV)*/
			gMC->Gstpar(pSi->GetId(),"PPCUTM",-1.);   /** direct pair production by muons (GeV)*/

		}

 } //!gGeoManager


}


// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BPsp::ProcessHits(FairVolume* vol) {

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


  
  // Set additional parameters at exit of active volume. Create R3BPspPoint.
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
    
    // Increment number of PspPoints for this track
    R3BStack* stack = (R3BStack*) gMC->GetStack();
    stack->AddPoint(kPSP);
    
    ResetParameters();
  }

  return kTRUE;
}

// -----   Public method EndOfEvent   -----------------------------------------
void R3BPsp::BeginEvent() {

//  if (! kGeoSaved ) {
//      SaveGeoParams();
//  cout << "-I STS geometry parameters saved " << endl;
//  kGeoSaved = kTRUE;
//  }

}
// -----   Public method EndOfEvent   -----------------------------------------
void R3BPsp::EndOfEvent() {

  if (fVerboseLevel) Print();
  fPspCollection->Clear();

  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method Register   -------------------------------------------
void R3BPsp::Register() {
  FairRootManager::Instance()->Register("PSPPoint", GetName(), fPspCollection, kTRUE);
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BPsp::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fPspCollection;
  else return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void R3BPsp::Print() const {
  Int_t nHits = fPspCollection->GetEntriesFast();
  cout << "-I- R3BPsp: " << nHits << " points registered in this event." 
       << endl;
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void R3BPsp::Reset() {
  fPspCollection->Clear();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void R3BPsp::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- R3BPsp: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  R3BPspPoint* oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++) {
   oldpoint = (R3BPspPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) R3BPspPoint(*oldpoint);
    fPosIndex++;
  }
   cout << " -I- R3BPsp: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}

// -----   Private method AddHit   --------------------------------------------
R3BPspPoint* R3BPsp::AddHit(Int_t trackID, Int_t detID, Int_t plane , TVector3 posIn,
			    TVector3 posOut, TVector3 momIn, 
			    TVector3 momOut, Double_t time, 
			    Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fPspCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1) 
    cout << "-I- R3BPsp: Adding Point at (" << posIn.X() << ", " << posIn.Y() 
	 << ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
	 << trackID << ", energy loss " << eLoss*1e06 << " keV" << endl;
  return new(clref[size]) R3BPspPoint(trackID, detID, plane, posIn, posOut,
				      momIn, momOut, time, length, eLoss);
}
// -----   Public method ConstructGeometry   ----------------------------------
void R3BPsp::ConstructGeometry() {

  // out-of-file geometry definition
   Double_t dx,dy,dz;
   Double_t a;
   Double_t thx, phx, thy, phy, thz, phz;
   Double_t z, density, radl, absl, w;
   Int_t nel, numed;


/****************************************************************************/
// Material definition

 // Vacuum

   TGeoMedium * pMed1=NULL;
   if (gGeoManager->GetMedium("Vacuum") ){
       pMed1=gGeoManager->GetMedium("Vacuum");
   }else{
       TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0,0,0);
       Double_t par[8];
       par[0]  = 0.000000; // isvol
       par[1]  = 0.000000; // ifield
       par[2]  = 0.000000; // fieldm
       par[3]  = 0.000000; // tmaxfd
       par[4]  = 0.000000; // stemax
       par[5]  = 0.000000; // deemax
       par[6]  = 0.000100; // epsil
       par[7]  = 0.000000; // stmin
       pMed1 = new TGeoMedium("Vacuum",1, matVacuum,par);
   }

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

   
   
   //pschrock 2013-06-07
   //this lines are from R3BTra.cxx
   
   // Material: Silicon
	TGeoMedium * pMedSi=NULL;
	if (gGeoManager->GetMedium("Silicon") ){
		pMedSi=gGeoManager->GetMedium("Silicon");
	}else{
		a       = 28.090000;
		z       = 14.000000;
		density = 2.330000;
		radl    = 9.351106;
		absl    = 456.628489;
		TGeoMaterial*
			pMat22 = new TGeoMaterial("Silicon", a,z,density,radl,absl);

		pMat22->SetIndex(21);
		// Medium: Silicon
		numed   = 21;  // medium number
		
		Double_t par[8];
		par[0]  = 0.000000; // isvol
	    par[1]  = 0.000000; // ifield
	    par[2]  = 0.000000; // fieldm
	    par[3]  = 0.000000; // tmaxfd
	    par[4]  = 0.000000; // stemax
	    par[5]  = 0.000000; // deemax
	    par[6]  = 0.000100; // epsil
	    par[7]  = 0.000000; // stmin

		pMedSi = new TGeoMedium("Silicon", numed,pMat22,par);
	}
   
   
   
   
   
   
   
   
   
   
   //psp1
   dx = 0.0;  //pschrock s412
   dy = 0.0;
   dz = -221.0;
   
   TGeoRotation *gRot1 = new TGeoRotation();
   gRot1->RotateX(0.);
   gRot1->RotateY(0.); // s412
   gRot1->RotateZ(0.);
   
   TGeoCombiTrans* pMatrix1 = new TGeoCombiTrans("", dx,dy,dz,gRot1);
   
   
   
   //psp2
   dx = 0.0;  //pschrock s412
   dy = 0.0;
   dz = -89.0;
   
   TGeoRotation *gRot2 = new TGeoRotation();
   gRot2->RotateX(0.);
   gRot2->RotateY(0.); // s412
   gRot2->RotateZ(0.);
   
   TGeoCombiTrans* pMatrix2 = new TGeoCombiTrans("", dx,dy,dz,gRot2);
   
   
   
   
   
   
   //psp3
   dx = 0.00; //s412 pschrock
   dy = 0.00;
   dz = 94.1;
   

   TGeoRotation *gRot3 = new TGeoRotation();
   gRot3->RotateX(0.);
   gRot3->RotateY(0.); // s412
   gRot3->RotateZ(0.);

   TGeoCombiTrans* pMatrix3 = new TGeoCombiTrans("", dx,dy,dz,gRot3);
   
   
   
   
   


   // World definition
   TGeoVolume* pWorld = gGeoManager->GetTopVolume();
   pWorld->SetVisLeaves(kTRUE);

 

   // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
   // Shape: PSPBoxWorld type: TGeoBBox
   dx = 2.5000;
   dy = 2.5000;
   dz = 0.050000;
   TGeoShape *pPSPBoxWorld = new TGeoBBox("PSPBoxWorld", dx,dy,dz);
   // Volume: PSPLogWorld
//    TGeoVolume* pPSPLogWorld = new TGeoVolume("PSPLogWorld",pPSPBoxWorld, pMed1); //vacuum filled
   TGeoVolume* pPSP1LogWorld = new TGeoVolume("PSP1LogWorld",pPSPBoxWorld, pMed1); //vacuum filled
   TGeoVolume* pPSP2LogWorld = new TGeoVolume("PSP1LogWorld",pPSPBoxWorld, pMed1); //vacuum filled
   TGeoVolume* pPSP3LogWorld = new TGeoVolume("PSP1LogWorld",pPSPBoxWorld, pMed1); //vacuum filled
//    pPSPLogWorld->SetVisLeaves(kTRUE);
   pPSP1LogWorld->SetVisLeaves(kTRUE);
   pPSP2LogWorld->SetVisLeaves(kTRUE);
   pPSP3LogWorld->SetVisLeaves(kTRUE);

   // Global positioning
   TGeoCombiTrans *pGlobal1 = GetGlobalPosition(pMatrix1); //psp1
   TGeoCombiTrans *pGlobal2 = GetGlobalPosition(pMatrix2); //psp2
   TGeoCombiTrans *pGlobal3 = GetGlobalPosition(pMatrix3); //psp3

   //put vacuum boxes for psp's
//    pWorld->AddNode( pPSPLogWorld, 1, pGlobal1 ); //psp1
//    pWorld->AddNode( pPSPLogWorld, 2, pGlobal2 ); //psp2
//    pWorld->AddNode( pPSPLogWorld, 3, pGlobal3 ); //psp3
   pWorld->AddNode( pPSP1LogWorld, 1, pGlobal1 ); //psp1
   pWorld->AddNode( pPSP2LogWorld, 2, pGlobal2 ); //psp2
   pWorld->AddNode( pPSP3LogWorld, 3, pGlobal3 ); //psp3

   // Shape: PSPBox type: TGeoBBox
   dx = 2.25000;
   dy = 2.25000;
   dz = 0.01005; //psp1 201 um, s412
   TGeoShape *pPSP1Box = new TGeoBBox("PSP1Box", dx,dy,dz);
   
   dx = 2.25000;
   dy = 2.25000;
   dz = 0.0102; //psp2 204 um, s412
   TGeoShape *pPSP2Box = new TGeoBBox("PSP2Box", dx,dy,dz);
   
   dx = 2.25000;
   dy = 2.25000;
   dz = 0.00520; //psp3 104 um, s412
   TGeoShape *pPSP3Box = new TGeoBBox("PSP3Box", dx,dy,dz);
   
   
   // Volume: PSPLog
   //create active volumes of psp's with specific thicknesses
   TGeoVolume* pPSP1Log = new TGeoVolume("PSP1Log",pPSP1Box, pMedSi);
   TGeoVolume* pPSP2Log = new TGeoVolume("PSP2Log",pPSP2Box, pMedSi);
   TGeoVolume* pPSP3Log = new TGeoVolume("PSP3Log",pPSP3Box, pMedSi);
   pPSP1Log->SetVisLeaves(kTRUE);
   pPSP2Log->SetVisLeaves(kTRUE);
   pPSP3Log->SetVisLeaves(kTRUE);
   
   
   
   
   
   // Combi transformation: 
   //"empty" matrix for placing active volumes inside vacuum box
   dx = 0.000000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix7 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix6 = new TGeoCombiTrans("", dx,dy,dz,pMatrix7);
   
   
   
//    pPSPLogWorld->AddNode(pPSP1Log, 1, pMatrix6); 
//    pPSPLogWorld->AddNode(pPSP2Log, 2, pMatrix6); 
//    pPSPLogWorld->AddNode(pPSP3Log, 3, pMatrix6); 
   pPSP1LogWorld->AddNode(pPSP1Log, 1, pMatrix6); 
   pPSP2LogWorld->AddNode(pPSP2Log, 2, pMatrix6); 
   pPSP3LogWorld->AddNode(pPSP3Log, 3, pMatrix6); 
   
   
   
   
   // Add the sensitive part
   AddSensitiveVolume(pPSP1Log);
   AddSensitiveVolume(pPSP2Log);
   AddSensitiveVolume(pPSP3Log);
   fNbOfSensitiveVol+=1;
}



ClassImp(R3BPsp)
