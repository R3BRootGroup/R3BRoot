// -------------------------------------------------------------------------
// -----                        R3BMfi source file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------
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
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;



// -----   Default constructor   -------------------------------------------
R3BMfi::R3BMfi() : R3BDetector("R3BMfi", kTRUE, kMFI) {
// R3BMfi::R3BMfi() : R3BDetector("R3BMfi", kTRUE, 1) {
  ResetParameters();
  fMfiCollection = new TClonesArray("R3BMfiPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fCutE=1.e-03;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BMfi::R3BMfi(const char* name, Bool_t active) 
  : R3BDetector(name, active, kMFI) {
//   : R3BDetector(name, active, 1) {
  ResetParameters();
  fMfiCollection = new TClonesArray("R3BMfiPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fCutE=1.e-03;
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

   cout << endl;
   cout << "-I- R3BMfi: initialisation" << endl;
   cout << "-I- R3BMfi: Sci. Vol. (McId) " << gMC->VolId("MFILog")<< endl;

}


void R3BMfi::SetSpecialPhysicsCuts(){

   cout << endl;

   cout << "-I- R3BMfi: Adding customized Physics cut ... " << endl;

   if (gGeoManager) {
     TGeoMedium* pSi = gGeoManager->GetMedium("plasticForGFI");
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

	cout << "-I- R3bMfi Scintillator Medium Id " << pSi->GetId()
	    << " Energy Cut-Off : " << cutE
	     << " GeV " << endl;
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
Bool_t R3BMfi::ProcessHits(FairVolume* vol) {

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
    
    // Increment number of MfiPoints for this track
    R3BStack* stack = (R3BStack*) gMC->GetStack();
    stack->AddPoint(kMFI);
    
    ResetParameters();
  }

  return kTRUE;
}

// -----   Public method EndOfEvent   -----------------------------------------
void R3BMfi::BeginEvent() {

//  if (! kGeoSaved ) {
//      SaveGeoParams();
//  cout << "-I STS geometry parameters saved " << endl;
//  kGeoSaved = kTRUE;
//  }

}
// -----   Public method EndOfEvent   -----------------------------------------
void R3BMfi::EndOfEvent() {

  if (fVerboseLevel) Print();
  fMfiCollection->Clear();

  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method Register   -------------------------------------------
void R3BMfi::Register() {
  FairRootManager::Instance()->Register("MFIPoint", GetName(), fMfiCollection, kTRUE);
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BMfi::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fMfiCollection;
  else return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void R3BMfi::Print(Option_t *option) const
{
  Int_t nHits = fMfiCollection->GetEntriesFast();
  cout << "-I- R3BMfi: " << nHits << " points registered in this event." 
       << endl;
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void R3BMfi::Reset() {
  fMfiCollection->Clear();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void R3BMfi::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- R3BMfi: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  R3BMfiPoint* oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++) {
   oldpoint = (R3BMfiPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) R3BMfiPoint(*oldpoint);
    fPosIndex++;
  }
   cout << " -I- R3BMfi: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}

// -----   Private method AddHit   --------------------------------------------
R3BMfiPoint* R3BMfi::AddHit(Int_t trackID, Int_t detID, Int_t plane , TVector3 posIn,
			    TVector3 posOut, TVector3 momIn, 
			    TVector3 momOut, Double_t time, 
			    Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fMfiCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1) 
    cout << "-I- R3BMfi: Adding Point at (" << posIn.X() << ", " << posIn.Y() 
	 << ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
	 << trackID << ", energy loss " << eLoss*1e06 << " keV" << endl;
  return new(clref[size]) R3BMfiPoint(trackID, detID, plane, posIn, posOut,
				      momIn, momOut, time, length, eLoss);
}
// -----   Public method ConstructGeometry   ----------------------------------
void R3BMfi::ConstructGeometry() {

  // out-of-file geometry definition
   Double_t dx,dy,dz;
   Double_t a;
   Double_t thx, phx, thy, phy, thz, phz;
  Double_t z, density, w;//, radl, absl;
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

   // Mixture: plasticForGFI
   TGeoMedium * pMed35=NULL;
   if (gGeoManager->GetMedium("plasticForGFI") ){
       pMed35=gGeoManager->GetMedium("plasticForGFI");
   }else{
       nel     = 2;
       density = 1.032000;
       TGeoMixture*
	   pMat35 = new TGeoMixture("plasticForGFI", nel,density);
       a = 12.010700;   z = 6.000000;   w = 0.914708;  // C
       pMat35->DefineElement(0,a,z,w);
       a = 1.007940;   z = 1.000000;   w = 0.085292;  // H
       pMat35->DefineElement(1,a,z,w);
       pMat35->SetIndex(34);
       // Medium: plasticForGFI
       numed   = 34;  // medium number
       Double_t par[8];
       par[0]  = 0.000000; // isvol
       par[1]  = 0.000000; // ifield
       par[2]  = 0.000000; // fieldm
       par[3]  = 0.000000; // tmaxfd
       par[4]  = 0.000000; // stemax
       par[5]  = 0.000000; // deemax
       par[6]  = 0.000100; // epsil
       par[7]  = 0.000000; // stmin
       pMed35 = new TGeoMedium("plasticForGFI", numed,pMat35,par);
   }




   // TRANSFORMATION MATRICES
   
   //global position
   
   // Combi transformation:
   
   //from Ralf's tracker: without offsets
   //LABPOS(MFI1,-63.82 , 0.00 , 520.25)
   //ROTATE(MFI1,13.5);
   dx = -63.82; //s412 pschrock
   dy = 0.00;
   dz = 520.25;
   

   // Rotation: 
   
   //pschrock: its easier to use 'Rotate' functions
   TGeoRotation *gRot = new TGeoRotation();
   gRot->RotateX(0.);
   gRot->RotateY(-13.5000000); // s412
   gRot->RotateZ(0.);
   
   
   TGeoCombiTrans*
//    pMatrix2 = new TGeoCombiTrans("", dx,dy,dz,pMatrix3);
   pMatrix2 = new TGeoCombiTrans("", dx,dy,dz,gRot);
   
   
   //translation and rotation of active volume
   
   // Combi transformation: 
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
   
   



   
   //create wrapping for mfi
   //use plastic for the moment (like in ggland)
   
   //front
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = -0.01300; //center of cladding
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix9 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
    TGeoCombiTrans*
   pMatrix8 = new TGeoCombiTrans("", dx,dy,dz,pMatrix9);
   
   //back
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 0.013000; //center of cladding
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix11 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
    TGeoCombiTrans*
   pMatrix10 = new TGeoCombiTrans("", dx,dy,dz,pMatrix11);
   





   // World definition
   TGeoVolume* pWorld = gGeoManager->GetTopVolume();
   pWorld->SetVisLeaves(kTRUE);

 


   // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
   
   //box for mfi volumes
   
   // Shape: GFIBoxWorld type: TGeoBBox
   dx = 15.000;	//half width of box for mfi volumes
   dy = 10.00000;
   dz = 0.050000;
   TGeoShape *pMFIBoxWorld = new TGeoBBox("MFIBoxWorld", dx,dy,dz);
   // Volume: MFILogWorld
   TGeoVolume*
   pMFILogWorld = new TGeoVolume("MFILogWorld",pMFIBoxWorld, pMed1); //vacuum filled
   pMFILogWorld->SetVisLeaves(kTRUE);
   
   
   
    // Shape: FrontCladding type: TGeoBBox
   dx = 15.000000;
   dy = 10.000000;
   dz = 0.0015000; //half thickness of cladding
   TGeoShape *pFrontCladding = new TGeoBBox("FrontClading", dx,dy,dz);
   // Volume: logicUpFrame
   TGeoVolume* plogicFrontClading = new TGeoVolume("logicFrontCladding",pFrontCladding, pMed35);
   plogicFrontClading->SetVisLeaves(kTRUE);
   pMFILogWorld->AddNode(plogicFrontClading, 0, pMatrix8);
   
   
    // Shape: BackCladding type: TGeoBBox
   dx = 15.000000;
   dy = 10.000000;
   dz = 0.0015000; //half thickness of cladding
   TGeoShape *pBackCladding = new TGeoBBox("BackCladding", dx,dy,dz);
   // Volume: logicUpFrame
   TGeoVolume* plogicBackCladding = new TGeoVolume("logicBackCladding",pBackCladding, pMed35);
   plogicBackCladding->SetVisLeaves(kTRUE);
   pMFILogWorld->AddNode(plogicBackCladding, 0, pMatrix10);
   
   
   
   
   


   // Global positioning

   TGeoCombiTrans *pGlobal1 = GetGlobalPosition(pMatrix2); //position and rotation is in pMatrix2
//    TGeoCombiTrans *pGlobal2 = GetGlobalPosition(pMatrix4); //from Gfi2

   pWorld->AddNode( pMFILogWorld, 0, pGlobal1 );


   //active volume of mfi

   // Shape: MFIBox type: TGeoBBox
   dx = 15.000000; //half width of active volume
   dy = 10.000000; //half width of active volume
   dz = 0.01250000; //250 um full thickness of fibers
   TGeoShape *pMFIBox = new TGeoBBox("MFIBox", dx,dy,dz);
   // Volume: MFILog
   TGeoVolume*
   pMFILog = new TGeoVolume("MFILog",pMFIBox, pMed35);
   pMFILog->SetVisLeaves(kTRUE);
   pMFILogWorld->AddNode(pMFILog, 0, pMatrix6); //put here the active volume of mfi, pMatrix6 has no more moving parameters




   // Add the sensitive part
   AddSensitiveVolume(pMFILog);
   fNbOfSensitiveVol+=1;
}




ClassImp(R3BMfi)
