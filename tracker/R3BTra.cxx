// -------------------------------------------------------------------------
// -----                        R3BTra source file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------
#include "R3BTra.h"

#include "R3BGeoTra.h"
#include "R3BTraPoint.h"
#include "R3BGeoTraPar.h"

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
#include "TGeoTube.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"
#include "TGeoShapeAssembly.h"
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;



// -----   Default constructor   -------------------------------------------
R3BTra::R3BTra() : FairDetector("R3BTra", kTRUE, kTRA) {
  ResetParameters();
  fTraCollection = new TClonesArray("R3BTraPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fCutE=1.0e-06; // 1keV default
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BTra::R3BTra(const char* name, Bool_t active) 
  : FairDetector(name, active, kTRA) {
  ResetParameters();
  fTraCollection = new TClonesArray("R3BTraPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fCutE=1.0e-06; // 1keV default
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BTra::~R3BTra() {

  if ( flGeoPar ) delete flGeoPar;
  if (fTraCollection) {
    fTraCollection->Delete();
    delete fTraCollection;
  }
}
// -------------------------------------------------------------------------

// ----   Initialize   -----------------------------------------------------

void R3BTra::Initialize()
{
  FairDetector::Initialize();

   cout << endl;
   cout << "-I- R3BTra: initialisation" << endl;
   cout << "-I- R3BTra: Sens. Vol. (McId) " << gMC->VolId("TraLog")<< endl;

}


void R3BTra::SetSpecialPhysicsCuts(){

   cout << endl;

   cout << "-I- R3BTra: Adding customized Physics cut ... " << endl;

   if (gGeoManager) {

     TGeoMedium* pSi = gGeoManager->GetMedium("Silicon");
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
   // <DB> trick to remove too much internal
   // tracking in the Aladin magnet yoke
   TGeoMedium* pFe = gGeoManager->GetMedium("Iron");

     if ( pFe ) {
     Double_t cutM= 1.e-01; // 100 MeV
      gMC->Gstpar(pFe->GetId(),"CUTELE",cutM);
      gMC->Gstpar(pFe->GetId()  ,"DRAY",0.0);
     }

 } //!gGeoManager


}

// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BTra::ProcessHits(FairVolume* vol) {

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
    //cout << "X,Y,X tracker=" << fPosIn(0) << " " << fPosIn(1) << " " << fPosIn(2) << endl;
    //cout << "track length=" << fLength << endl;
  }

  // Sum energy loss for all steps in the active volume
      fELoss += gMC->Edep();
    //cout << "Tracker Eloss=" << fELoss << "  " << gMC->Edep() << endl;

  // Set additional parameters at exit of active volume. Create R3BTraPoint.
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
   //   TGeoNode *bla = gGeoManager->FindNextBoundary(2);
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
    
    // Increment number of TraPoints for this track
    FairStack* stack = (FairStack*) gMC->GetStack();
    stack->AddPoint(kTRA);
    
    ResetParameters();
  }

  return kTRUE;
}
// ----------------------------------------------------------------------------
//void R3BTra::SaveGeoParams(){
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
void R3BTra::BeginEvent() {

//  cout << "-I- begin tracker event called ##########" << endl;

  if(gGeoManager){
    TGeoVolume * vol=gGeoManager->FindVolumeFast("TraLog");
  
    if(vol){
       //    cout << "id tracker serial number : " << vol->GetNumber() << endl;
    }
  }

//  if (! kGeoSaved ) {
//      SaveGeoParams();
//  cout << "-I STS geometry parameters saved " << endl;
//  kGeoSaved = kTRUE;
//  }

}
// -----   Public method EndOfEvent   -----------------------------------------
void R3BTra::EndOfEvent() {

  if (fVerboseLevel) Print();
  fTraCollection->Clear();

  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method Register   -------------------------------------------
void R3BTra::Register() {
  FairRootManager::Instance()->Register("TraPoint", GetName(), fTraCollection, kTRUE);
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BTra::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fTraCollection;
  else return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void R3BTra::Print() const {
  Int_t nHits = fTraCollection->GetEntriesFast();
  cout << "-I- R3BTra: " << nHits << " points registered in this event." 
       << endl;
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void R3BTra::Reset() {
  fTraCollection->Clear();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void R3BTra::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- R3BTra: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  R3BTraPoint* oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++) {
   oldpoint = (R3BTraPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) R3BTraPoint(*oldpoint);
    fPosIndex++;
  }
   cout << " -I- R3BTra: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}

// -----   Private method AddHit   --------------------------------------------
R3BTraPoint* R3BTra::AddHit(Int_t trackID, Int_t detID, TVector3 posIn,
			    TVector3 posOut, TVector3 momIn, 
			    TVector3 momOut, Double_t time, 
			    Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fTraCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1) 
    cout << "-I- R3BTra: Adding Point at (" << posIn.X() << ", " << posIn.Y() 
	 << ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
	 << trackID << ", energy loss " << eLoss*1e06 << " keV" << endl;
  return new(clref[size]) R3BTraPoint(trackID, detID, posIn, posOut,
				      momIn, momOut, time, length, eLoss);
}


// -----   Public method ConstructGeometry   ----------------------------------
void R3BTra::ConstructGeometry() {

  // out-of-file geometry definition
   Double_t dx,dy,dz;
   Double_t par[20];
   Double_t rmin, rmax;
   Double_t a;
   Double_t thx, phx, thy, phy, thz, phz;
   Double_t  phi1, phi2;
   Double_t z, density, radl, absl, w;
   Double_t tx,ty,tz;
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

 // Material: Silicon
   TGeoMedium * pMed22=NULL;
   if (gGeoManager->GetMedium("Silicon") ){
       pMed22=gGeoManager->GetMedium("Silicon");
   }else{
     a       = 28.090000;
     z       = 14.000000;
     density = 2.330000;
     radl    = 9.351106;
     absl    = 456.628489;
     TGeoMaterial*
	 pMat22 = new TGeoMaterial("Silicon", a,z,density);

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
     pMed22 = new TGeoMedium("Silicon", numed,pMat22,par);
   }


   // Material: Copper
   TGeoMedium * pMed25=NULL;
   if (gGeoManager->GetMedium("Copper") ){
       pMed25=gGeoManager->GetMedium("Copper");
   }else{
     a       = 63.540000;
     z       = 29.000000;
     density = 8.960000;
     radl    = 1.435029;
     absl    = 155.874854;
     TGeoMaterial*
	 pMat25 = new TGeoMaterial("Copper", a,z,density,radl,absl);
     pMat25->SetIndex(24);
     // Medium: Copper
     numed   = 24;  // medium number
     Double_t par[8];
     par[0]  = 0.000000; // isvol
     par[1]  = 0.000000; // ifield
     par[2]  = 0.000000; // fieldm
     par[3]  = 0.000000; // tmaxfd
     par[4]  = 0.000000; // stemax
     par[5]  = 0.000000; // deemax
     par[6]  = 0.000100; // epsil
     par[7]  = 0.000000; // stmin
     pMed25 = new TGeoMedium("Copper", numed,pMat25, par);
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
     TGeoMaterial*
	 pMat21 = new TGeoMaterial("Aluminum", a,z,density,radl,absl);
     pMat21->SetIndex(20);
     // Medium: Aluminum
     numed   = 20;  // medium number
     Double_t par[8];
     par[0]  = 0.000000; // isvol
     par[1]  = 0.000000; // ifield
     par[2]  = 0.000000; // fieldm
     par[3]  = 0.000000; // tmaxfd
     par[4]  = 0.000000; // stemax
     par[5]  = 0.000000; // deemax
     par[6]  = 0.000100; // epsil
     par[7]  = 0.000000; // stmin
     pMed21 = new TGeoMedium("Aluminum", numed,pMat21, par);
   }



 // TRANSFORMATION MATRICES
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   //TGeoRotation *pMatrix3 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  // TGeoCombiTrans*
  // pMatrix2 = new TGeoCombiTrans("", dx,dy,dz,pMatrix3);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 10.820000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix5 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix4 = new TGeoCombiTrans("", dx,dy,dz,pMatrix5);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
  // TGeoRotation *pMatrix33 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  // TGeoCombiTrans*
  // pMatrix32 = new TGeoCombiTrans("", dx,dy,dz,pMatrix33);
   // Combi transformation: 
   dx = 0.000000;
   dy = -2.100000;
   dz = 4.470000;
   // Rotation: 
   thx = 0.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 90.000000;
   TGeoRotation *pMatrix7 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix6 = new TGeoCombiTrans("", dx,dy,dz,pMatrix7);
   // Combi transformation: 
   dx = 0.000000;
   dy = 2.100000;
   dz = 4.470000;
   // Rotation: 
   thx = 180.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 270.000000;
   TGeoRotation *pMatrix9 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix8 = new TGeoCombiTrans("", dx,dy,dz,pMatrix9);
   // Combi transformation: 
   dx = 2.100000;
   dy = 0.000000;
   dz = 4.470000;
   // Rotation: 
   thx = 180.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 90.000000;    phz = 0.000000;
   TGeoRotation *pMatrix11 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix10 = new TGeoCombiTrans("", dx,dy,dz,pMatrix11);
   // Combi transformation: 
   dx = -2.100000;
   dy = 0.000000;
   dz = 4.470000;
   // Rotation: 
   thx = 0.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 90.000000;    phz = 180.000000;
   TGeoRotation *pMatrix13 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix12 = new TGeoCombiTrans("", dx,dy,dz,pMatrix13);
   // Combi transformation: 
   dx = 0.000000;
   dy = -14.090000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 0.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 270.000000;
   TGeoRotation *pMatrix15 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix14 = new TGeoCombiTrans("", dx,dy,dz,pMatrix15);
   // Combi transformation: 
   dx = 0.000000;
   dy = -17.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 0.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 270.000000;
   TGeoRotation *pMatrix17 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix16 = new TGeoCombiTrans("", dx,dy,dz,pMatrix17);
   // Combi transformation: 
   dx = 0.000000;
   dy = -8.000000;
   dz = 10.820000;
   // Rotation: 
   thx = 90.000000;    phx = 270.000000;
   thy = 90.000000;    phy = 0.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix19 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix18 = new TGeoCombiTrans("", dx,dy,dz,pMatrix19);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
  // TGeoRotation *pMatrix35 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
 //  TGeoCombiTrans*
 //  pMatrix34 = new TGeoCombiTrans("", dx,dy,dz,pMatrix35);
   // Combi transformation: 
   dx = -2.100000;
   dy = -8.000000;
   dz = 4.470000;
   // Rotation: 
   thx = 90.000000;    phx = 90.000000;
   thy = 0.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 360.000000;
   TGeoRotation *pMatrix21 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix20 = new TGeoCombiTrans("", dx,dy,dz,pMatrix21);
   // Combi transformation: 
   dx = 2.100000;
   dy = 8.000000;
   dz = 4.470000;
   // Rotation: 
   thx = 90.000000;    phx = 90.000000;
   thy = 180.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 180.000000;
   TGeoRotation *pMatrix23 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix22 = new TGeoCombiTrans("", dx,dy,dz,pMatrix23);
   // Combi transformation: 
   dx = 8.000000;
   dy = -2.100000;
   dz = 4.470000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 180.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 90.000000;
   TGeoRotation *pMatrix25 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix24 = new TGeoCombiTrans("", dx,dy,dz,pMatrix25);
   // Combi transformation: 
   dx = -8.000000;
   dy = 2.100000;
   dz = 4.470000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 180.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 90.000000;
   TGeoRotation *pMatrix27 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix26 = new TGeoCombiTrans("", dx,dy,dz,pMatrix27);
   // Combi transformation: 
   dx = -6.240000;
   dy = -3.900000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix29 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix28 = new TGeoCombiTrans("", dx,dy,dz,pMatrix29);
   // Combi transformation: 
   dx = -6.240000;
   dy = -3.900000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix31 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix30 = new TGeoCombiTrans("", dx,dy,dz,pMatrix31);


   // Shape: World type: TGeoBBox
   TGeoVolume* pWorld = gGeoManager->GetTopVolume();
   pWorld->SetVisLeaves(kTRUE);


   // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
   // Shape: SiVacuumSphereWorld type: TGeoSphere

   // Si Shape & volume: TraBox type: TGeoBBox
   dx = 3.600000;
   dy = 2.070000;
   dz = 0.015000;
   // Volume: TraLog
   TGeoVolume *TraLog = gGeoManager->MakeBox("TraLog",pMed22,dx,dy,dz);


   // Shape: MontagePlatform type: TGeoTubeSeg
   rmin = 2.750000;
   rmax = 18.000000;
   dz   = 0.150000;
   phi1 = 0.000000;
   phi2 = 360.000000;
   TGeoShape *pMontagePlatform = new TGeoTubeSeg("MontagePlatform",rmin,rmax,dz,phi1,phi2);
   // Volume: MontagePlatformLog
   TGeoVolume*
   pMontagePlatformLog = new TGeoVolume("MontagePlatformLog",pMontagePlatform, pMed25);


   // Shape: MontageRing type: TGeoTubeSeg
   rmin = 12.000000;
   rmax = 15.000000;
   dz   = 0.250000;
   phi1 = 0.000000;
   phi2 = 360.000000;
   TGeoShape *pMontageRing = new TGeoTubeSeg("MontageRing",rmin,rmax,dz,phi1,phi2);
   // Volume: MontageRingLog
   TGeoVolume*
   pMontageRingLog = new TGeoVolume("MontageRingLog",pMontageRing, pMed25);


   // Shape: targetWheel type: TGeoTubeSeg
   rmin = 0.410000;
   rmax = 5.300000;
   dz   = 0.100000;
   phi1 = 0.000000;
   phi2 = 360.000000;
   TGeoShape *ptargetWheel = new TGeoTubeSeg("targetWheel",rmin,rmax,dz,phi1,phi2);
   // Volume: targetWheelLog
   TGeoVolume*
   ptargetWheelLog = new TGeoVolume("targetWheelLog",ptargetWheel, pMed21);


   // Shape: targetWheel2 type: TGeoTubeSeg
   rmin = 9.500000;
   rmax = 10.000000;
   dz   = 0.100000;
   phi1 = 0.000000;
   phi2 = 360.000000;
   TGeoShape *ptargetWheel2 = new TGeoTubeSeg("targetWheel2",rmin,rmax,dz,phi1,phi2);
   // Volume: targetWheel2Log
   TGeoVolume*
   ptargetWheel2Log = new TGeoVolume("targetWheel2Log",ptargetWheel2, pMed21);


   // Shape: innerElectronicBox type: TGeoBBox
   dx = 3.800000;
   dy = 3.300000;
   dz = 0.800000;
   TGeoShape *pinnerElectronicBox = new TGeoBBox("innerElectronicBox", dx,dy,dz);
   // Volume: innerElectronicsLog
   TGeoVolume*
   pinnerElectronicsLog = new TGeoVolume("innerElectronicsLog",pinnerElectronicBox, pMed2);

   //
   // Make elementary assembly of the whole structure.
   //

   TGeoVolume *aTra = new TGeoVolumeAssembly("ATRA");

    aTra->AddNode(ptargetWheel2Log,1, pMatrix30);
 
    aTra->AddNode(pinnerElectronicsLog ,1, pMatrix18);
    aTra->AddNode(pinnerElectronicsLog ,2, pMatrix20);
    aTra->AddNode(pinnerElectronicsLog ,3, pMatrix22);
    aTra->AddNode(pinnerElectronicsLog ,4, pMatrix24);
    aTra->AddNode(pinnerElectronicsLog ,5, pMatrix26);

    aTra->AddNode(ptargetWheelLog ,1, pMatrix28);

    aTra->AddNode(pMontageRingLog ,1, pMatrix16);

    aTra->AddNode(pMontagePlatformLog ,1, pMatrix14);
   
    AddSensitiveVolume(TraLog);
    fNbOfSensitiveVol+=1;

    aTra->AddNode(TraLog,1, pMatrix4);
    aTra->AddNode(TraLog,2, pMatrix6);
    aTra->AddNode(TraLog,3, pMatrix8);
    aTra->AddNode(TraLog,4, pMatrix10);
    aTra->AddNode(TraLog,5, pMatrix12);

    //

    TGeoRotation *rotg = new TGeoRotation();
    rotg->RotateX(0.);
    rotg->RotateY(0.);
    rotg->RotateZ(0.);
    dx=tx=0.0;
    dy=ty=0.0;
    dz=tz=0.0;

    pWorld->AddNode(aTra,1, new TGeoCombiTrans(tx,ty,tz,rotg));


}

/*
void R3BTra::ConstructGeometry() {
  
  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  R3BGeoTra*       stsGeo  = new R3BGeoTra();
  stsGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(stsGeo);

  Bool_t rc = geoFace->readSet(stsGeo);
  if (rc) stsGeo->create(geoLoad->getGeoBuilder());
  TList* volList = stsGeo->getListOfVolumes();
  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  R3BGeoTraPar* par=(R3BGeoTraPar*)(rtdb->getContainer("R3BGeoTraPar"));
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

ClassImp(R3BTra)
