// -------------------------------------------------------------------------
// -----                        R3BATof source file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------
#include "R3BATof.h"

#include "R3BGeoATof.h"
#include "R3BDetPlain.h"
#include "R3BATofPoint.h"
#include "R3BGeoATofPar.h"



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
#include "TMath.h"

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
R3BATof::R3BATof() : R3BDetector("R3BATof", kTRUE, kTOF) {
  ResetParameters();
  fTofCollection = new TClonesArray("R3BATofPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fCutE=1.0e-03; // 1 MeV default
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BATof::R3BATof(const char* name, Bool_t active) 
  : R3BDetector(name, active, kTOF) {
  ResetParameters();
  fTofCollection = new TClonesArray("R3BATofPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fCutE=1.0e-03; // 1MeV default
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BATof::~R3BATof() {

  if ( flGeoPar ) delete flGeoPar;
  if (fTofCollection) {
    fTofCollection->Delete();
    delete fTofCollection;
  }
}
// -------------------------------------------------------------------------


void R3BATof::Initialize()
{
  FairDetector::Initialize();

   fTofLandId   =  gMC->VolId("LandTOF");
   fTofAladinId =  gMC->VolId("AladinTOF");


   cout << endl;
   cout << "-I- R3BATof: initialisation" << endl;
   cout << "-I- R3BATof: Sci. Vol. (Land McId) " << fTofLandId << endl;
   cout << "-I- R3BATof: Sci. Vol. (Aladin McId) " << fTofAladinId << endl;

}


void R3BATof::SetSpecialPhysicsCuts(){

   cout << endl;

   cout << "-I- R3BATof: Adding customized Physics cut ... " << endl;

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

	cout << "-I- R3bTof Scintillator Medium Id " << pSi->GetId()
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
Bool_t R3BATof::ProcessHits(FairVolume* vol) {


   Int_t copy=-1; 
   Int_t volId = gMC->CurrentVolID(copy); 
   Int_t tofId=-1;

  // Land Sci. plane id=0
  // Aladin Sci. plane id=1
   if (volId == fTofLandId) tofId=0;
   else tofId =1;

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

  // Set additional parameters at exit of active volume. Create R3BATofPoint.
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

    AddHit(fTrackID, tofId, copy,
	   TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
	   TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
	   TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
	   TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
	   fTime, fLength, fELoss);
    
    // Increment number of TofPoints for this track
    FairStack* stack = (FairStack*) gMC->GetStack();
    stack->AddPoint(kTOF);
    
    ResetParameters();
  }

  return kTRUE;
}
// ----------------------------------------------------------------------------
//void R3BATof::SaveGeoParams(){
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
void R3BATof::BeginEvent() {

//  if (! kGeoSaved ) {
//      SaveGeoParams();
//  cout << "-I STS geometry parameters saved " << endl;
//  kGeoSaved = kTRUE;
//  }

}
// -----   Public method EndOfEvent   -----------------------------------------
void R3BATof::EndOfEvent() {

  if (fVerboseLevel) Print();
  fTofCollection->Clear();

  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method Register   -------------------------------------------
void R3BATof::Register() {
  FairRootManager::Instance()->Register("TOFPoint", GetName(), fTofCollection, kTRUE);
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BATof::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fTofCollection;
  else return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void R3BATof::Print() const {
  Int_t nHits = fTofCollection->GetEntriesFast();
  cout << "-I- R3BATof: " << nHits << " points registered in this event." 
       << endl;
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void R3BATof::Reset() {
  fTofCollection->Clear();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void R3BATof::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- R3BATof: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  R3BATofPoint* oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++) {
   oldpoint = (R3BATofPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) R3BATofPoint(*oldpoint);
    fPosIndex++;
  }
   cout << " -I- R3BATof: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}

// -----   Private method AddHit   --------------------------------------------
R3BATofPoint* R3BATof::AddHit(Int_t trackID, Int_t detID, Int_t mod, TVector3 posIn,
			    TVector3 posOut, TVector3 momIn, 
			    TVector3 momOut, Double_t time, 
			    Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fTofCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1) 
    cout << "-I- R3BATof: Adding Point at (" << posIn.X() << ", " << posIn.Y() 
	 << ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
	 << trackID << ", energy loss " << eLoss*1e06 << " keV" << endl;
  return new(clref[size]) R3BATofPoint(trackID, detID, mod, posIn, posOut,
				      momIn, momOut, time, length, eLoss);
}
// -----   Public method ConstructGeometry   ----------------------------------
void R3BATof::ConstructGeometry() {

    return ConstructGeometry1();

}


void R3BATof::ConstructGeometry1() {

   // out-of-file geometry definition
   Double_t dx,dy,dz;
   Double_t a;
   Double_t thx, phx, thy, phy, thz, phz;
   Double_t z, density, w;
   Int_t nel, numed;
   Double_t par[20];

   /****************************************************************************/
   // Material definition
   // Mixture: Air
   TGeoMedium * pMedAir=NULL;
   if (gGeoManager->GetMedium("Air") ){
       pMedAir=gGeoManager->GetMedium("Air");
   }else{
       nel     = 2;
       density = 0.001290;
       TGeoMixture*
	   pMatAir = new TGeoMixture("Air", nel,density);
       a = 14.006740;   z = 7.000000;   w = 0.700000;  // N
       pMatAir->DefineElement(0,a,z,w);
       a = 15.999400;   z = 8.000000;   w = 0.300000;  // O
       pMatAir->DefineElement(1,a,z,w);
       pMatAir->SetIndex(1);
       // Medium: Air
       numed   = 1;  // medium number
       pMedAir = new TGeoMedium("Air", numed,pMatAir);
   }

   // Mixture: plasticForTOF
   TGeoMedium * pMedSci=NULL;
   if (gGeoManager->GetMedium("Scintillator") ){
       pMedSci=gGeoManager->GetMedium("Scintillator");
   }else{
       // Mixture: Scintillator
       nel     = 2;
       density = 1.032000;
       TGeoMixture *
	   pMatSci = new TGeoMixture("Scintillator", nel,density);
       a = 12.010000;   z = 6.000000;   w = 0.922427;  // C
       pMatSci->DefineElement(0,a,z,w);
       a = 1.010000;   z = 1.000000;   w = 0.077573;  // H
       pMatSci->DefineElement(1,a,z,w);
       pMatSci->SetIndex(12);
       // Medium: Scintillator
       numed   = 12;  // medium number
       pMedSci = new TGeoMedium("Scintillator", numed,pMatSci);

   }

 
   // SET TOP VOLUME OF GEOMETRY
   TGeoVolume * pWorld = gGeoManager->GetTopVolume();
   pWorld->SetVisLeaves(kTRUE);

   // Aladin TOF
   Double_t X=200.;
   Double_t Y=200.;
   Double_t Z=50.;
   TGeoShape *pMAladinTOF = new TGeoBBox("MAladinBox", X/2.0,
					               Y/2.0,
					               Z/2.0);
   // Volume: MAladinTOF
   TGeoVolume*
   pMAladinTOFv = new TGeoVolume("MAladinTOF",pMAladinTOF, pMedAir);
   pMAladinTOFv->SetInvisible();
   pMAladinTOFv->SetVisLeaves(kTRUE);
   pMAladinTOFv->SetVisContainers(kFALSE);

   //<DB>  Global positioning of ATOF ----------------------------------------------------
   //      has to be changed later
   //      this numbers are just taken 
   //      as an example. 
   /*
   Double_t DX = -98.3;//[cm]
   Double_t DY =  0.00;//[cm]
   Double_t DZ = 489.00;//[cm]
   Double_t angle = 22.8;//[deg]/(0Z)axis
   */

   Double_t DX = 0.0;//[cm]
   Double_t DY = 0.0;//[cm]
   Double_t DZ = 0.0;//[cm]
   Double_t angle = 0.0;//[deg]/(0Z)axis
  
   //------------------------------------------------------------------------------------- 



   TGeoRotation *gRot = new TGeoRotation();
   gRot->RotateY(angle);
   TGeoCombiTrans *tr = GetGlobalPosition(new TGeoCombiTrans("global",DX,DY,DZ,gRot));

   pWorld->AddNode(pMAladinTOFv,1,tr);



   // All units in [cm]
   Double_t ATOFbar_width    = 2.5;
   Double_t ATOFbar_height   = 100.;
   Double_t ATOFbar_thikness = 2.;
   Double_t ATOFwall_width   = 240;

   TGeoShape *pAladinTOF = new TGeoBBox("AladinBox", ATOFbar_width/2.0,
					             ATOFbar_height/2.0,
					             ATOFbar_thikness/2.0);
   // Volume: AladinTOF
   TGeoVolume*
   pAladinTOFv = new TGeoVolume("AladinTOF",pAladinTOF, pMedSci);
   pAladinTOFv->SetVisLeaves(kTRUE);

   AddSensitiveVolume(pAladinTOFv);


   // Constructing TOF
   R3BDetPlain Aladin_Plain("AladinTOF");
   Aladin_Plain.SetZ(DZ);
   Aladin_Plain.SetXorY(DY);
 

   Double_t x    = -1.* ATOFwall_width/2.0;
   Double_t xmax = +1.* ATOFwall_width/2.0;


   TGeoRotation *rot = new TGeoRotation();
   rot->RotateY(0.0);
   Int_t id = 0;
   Char_t tname[40];

   while(x < xmax){
       Aladin_Plain.AddRaw(id,x);
       sprintf(tname,"AladinTOF id%3i",id);
       dx= x;
       //dy= Par->Get_Geometry_AladinTOF_PosY();
       //dz= Par->Get_Geometry_AladinTOF_PosZ();
       dy = 0.0;
       dz = 0.0;
       TGeoCombiTrans *t = new TGeoCombiTrans(tname,dx,dy,dz,rot);
       pMAladinTOFv->AddNode(pAladinTOFv, id, t);
       id++;
       x += ATOFbar_width;
   }
   // Aladin_Plain.Write();
}


void R3BATof::ConstructGeometry2() {

    // out-of-file geometry definition
   Double_t dx,dy,dz;
   Double_t a;
   Double_t thx, phx, thy, phy, thz, phz;
   Double_t z, density, w;
   Int_t nel, numed;
   Double_t par[20];

/****************************************************************************/
// Material definition


  // Mixture: Air
  TGeoMedium * pMedAir=NULL;
   if (gGeoManager->GetMedium("Air") ){
       pMedAir=gGeoManager->GetMedium("Air");
   }else{
     nel     = 2;
     density = 0.001290;
     TGeoMixture*
	 pMatAir = new TGeoMixture("Air", nel,density);
     a = 14.006740;   z = 7.000000;   w = 0.700000;  // N
     pMatAir->DefineElement(0,a,z,w);
     a = 15.999400;   z = 8.000000;   w = 0.300000;  // O
     pMatAir->DefineElement(1,a,z,w);
     pMatAir->SetIndex(1);
     // Medium: Air
     numed   = 1;  // medium number
     par[0]  = 0.000000; // isvol
     par[1]  = 1.000000; // ifield
     par[2]  = 0.000000; // fieldm
     par[3]  = 0.000000; // tmaxfd
     par[4]  = 0.000000; // stemax
     par[5]  = 0.000000; // deemax
     par[6]  = 0.000100; // epsil
     par[7]  = 0.000000; // stmin
     pMedAir = new TGeoMedium("Air", numed,pMatAir, par);
   }


  // Mixture: plasticForTOF
  TGeoMedium * pMedSci=NULL;
   if (gGeoManager->GetMedium("Scintillator") ){
       pMedSci=gGeoManager->GetMedium("Scintillator");
   }else{
   // Mixture: Scintillator
   nel     = 2;
   density = 1.032000;
   TGeoMixture *
   pMatSci = new TGeoMixture("Scintillator", nel,density);
      a = 12.010000;   z = 6.000000;   w = 0.922427;  // C
   pMatSci->DefineElement(0,a,z,w);
      a = 1.010000;   z = 1.000000;   w = 0.077573;  // H
   pMatSci->DefineElement(1,a,z,w);
   pMatSci->SetIndex(12);
// Medium: Scintillator
   numed   = 12;  // medium number
   par[0]  = 0.000000; // isvol
   par[1]  = 0.000000; // ifield
   par[2]  = 0.000000; // fieldm
   par[3]  = 0.000000; // tmaxfd
   par[4]  = 0.000000; // stemax
   par[5]  = 0.000000; // deemax
   par[6]  = 0.000100; // epsil
   par[7]  = 0.000000; // stmin

   pMedSci = new TGeoMedium("Scintillator", numed,pMatSci, par);

   }

   // TRANSFORMATION MATRICES
   // Combi transformation: 
   dx = -72.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix3 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix2 = new TGeoCombiTrans("", dx,dy,dz,pMatrix3);
   // Combi transformation: 
   dx = -70.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix5 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix4 = new TGeoCombiTrans("", dx,dy,dz,pMatrix5);
   // Combi transformation: 
   dx = -67.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix7 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix6 = new TGeoCombiTrans("", dx,dy,dz,pMatrix7);
   // Combi transformation: 
   dx = -65.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix9 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix8 = new TGeoCombiTrans("", dx,dy,dz,pMatrix9);
   // Combi transformation: 
   dx = -62.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix11 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix10 = new TGeoCombiTrans("", dx,dy,dz,pMatrix11);
   // Combi transformation: 
   dx = -60.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix13 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix12 = new TGeoCombiTrans("", dx,dy,dz,pMatrix13);
   // Combi transformation: 
   dx = -57.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix15 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix14 = new TGeoCombiTrans("", dx,dy,dz,pMatrix15);
   // Combi transformation: 
   dx = -55.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix17 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix16 = new TGeoCombiTrans("", dx,dy,dz,pMatrix17);
   // Combi transformation: 
   dx = -52.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix19 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix18 = new TGeoCombiTrans("", dx,dy,dz,pMatrix19);
   // Combi transformation: 
   dx = -50.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix21 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix20 = new TGeoCombiTrans("", dx,dy,dz,pMatrix21);
   // Combi transformation: 
   dx = -47.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix23 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix22 = new TGeoCombiTrans("", dx,dy,dz,pMatrix23);
   // Combi transformation: 
   dx = -45.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix25 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix24 = new TGeoCombiTrans("", dx,dy,dz,pMatrix25);
   // Combi transformation: 
   dx = -42.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix27 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix26 = new TGeoCombiTrans("", dx,dy,dz,pMatrix27);
   // Combi transformation: 
   dx = -40.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix29 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix28 = new TGeoCombiTrans("", dx,dy,dz,pMatrix29);
   // Combi transformation: 
   dx = -37.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix31 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix30 = new TGeoCombiTrans("", dx,dy,dz,pMatrix31);
   // Combi transformation: 
   dx = -35.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix33 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix32 = new TGeoCombiTrans("", dx,dy,dz,pMatrix33);
   // Combi transformation: 
   dx = -32.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix35 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix34 = new TGeoCombiTrans("", dx,dy,dz,pMatrix35);
   // Combi transformation: 
   dx = -30.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix37 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix36 = new TGeoCombiTrans("", dx,dy,dz,pMatrix37);
   // Combi transformation: 
   dx = -27.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix39 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix38 = new TGeoCombiTrans("", dx,dy,dz,pMatrix39);
   // Combi transformation: 
   dx = -25.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix41 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix40 = new TGeoCombiTrans("", dx,dy,dz,pMatrix41);
   // Combi transformation: 
   dx = -22.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix43 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix42 = new TGeoCombiTrans("", dx,dy,dz,pMatrix43);
   // Combi transformation: 
   dx = -20.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix45 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix44 = new TGeoCombiTrans("", dx,dy,dz,pMatrix45);
   // Combi transformation: 
   dx = -17.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix47 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix46 = new TGeoCombiTrans("", dx,dy,dz,pMatrix47);
   // Combi transformation: 
   dx = -15.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix49 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix48 = new TGeoCombiTrans("", dx,dy,dz,pMatrix49);
   // Combi transformation: 
   dx = -12.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix51 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix50 = new TGeoCombiTrans("", dx,dy,dz,pMatrix51);
   // Combi transformation: 
   dx = -10.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix53 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix52 = new TGeoCombiTrans("", dx,dy,dz,pMatrix53);
   // Combi transformation: 
   dx = -7.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix55 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix54 = new TGeoCombiTrans("", dx,dy,dz,pMatrix55);
   // Combi transformation: 
   dx = -5.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix57 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix56 = new TGeoCombiTrans("", dx,dy,dz,pMatrix57);
   // Combi transformation: 
   dx = -2.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix59 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix58 = new TGeoCombiTrans("", dx,dy,dz,pMatrix59);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix61 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix60 = new TGeoCombiTrans("", dx,dy,dz,pMatrix61);
   // Combi transformation: 
   dx = 2.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix63 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix62 = new TGeoCombiTrans("", dx,dy,dz,pMatrix63);
   // Combi transformation: 
   dx = 5.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix65 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix64 = new TGeoCombiTrans("", dx,dy,dz,pMatrix65);
   // Combi transformation: 
   dx = 7.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix67 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix66 = new TGeoCombiTrans("", dx,dy,dz,pMatrix67);
   // Combi transformation: 
   dx = 10.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix69 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix68 = new TGeoCombiTrans("", dx,dy,dz,pMatrix69);
   // Combi transformation: 
   dx = 12.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix71 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix70 = new TGeoCombiTrans("", dx,dy,dz,pMatrix71);
   // Combi transformation: 
   dx = 15.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix73 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix72 = new TGeoCombiTrans("", dx,dy,dz,pMatrix73);
   // Combi transformation: 
   dx = 17.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix75 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix74 = new TGeoCombiTrans("", dx,dy,dz,pMatrix75);
   // Combi transformation: 
   dx = 20.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix77 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix76 = new TGeoCombiTrans("", dx,dy,dz,pMatrix77);
   // Combi transformation: 
   dx = 22.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix79 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix78 = new TGeoCombiTrans("", dx,dy,dz,pMatrix79);
   // Combi transformation: 
   dx = 25.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix81 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix80 = new TGeoCombiTrans("", dx,dy,dz,pMatrix81);
   // Combi transformation: 
   dx = 27.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix83 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix82 = new TGeoCombiTrans("", dx,dy,dz,pMatrix83);
   // Combi transformation: 
   dx = 30.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix85 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix84 = new TGeoCombiTrans("", dx,dy,dz,pMatrix85);
   // Combi transformation: 
   dx = 32.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix87 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix86 = new TGeoCombiTrans("", dx,dy,dz,pMatrix87);
   // Combi transformation: 
   dx = 35.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix89 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix88 = new TGeoCombiTrans("", dx,dy,dz,pMatrix89);
   // Combi transformation: 
   dx = 37.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix91 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix90 = new TGeoCombiTrans("", dx,dy,dz,pMatrix91);
   // Combi transformation: 
   dx = 40.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix93 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix92 = new TGeoCombiTrans("", dx,dy,dz,pMatrix93);
   // Combi transformation: 
   dx = 42.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix95 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix94 = new TGeoCombiTrans("", dx,dy,dz,pMatrix95);
   // Combi transformation: 
   dx = 45.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix97 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix96 = new TGeoCombiTrans("", dx,dy,dz,pMatrix97);
   // Combi transformation: 
   dx = 47.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix99 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix98 = new TGeoCombiTrans("", dx,dy,dz,pMatrix99);
   // Combi transformation: 
   dx = 50.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix101 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix100 = new TGeoCombiTrans("", dx,dy,dz,pMatrix101);
   // Combi transformation: 
   dx = 52.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix103 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix102 = new TGeoCombiTrans("", dx,dy,dz,pMatrix103);
   // Combi transformation: 
   dx = 55.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix105 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix104 = new TGeoCombiTrans("", dx,dy,dz,pMatrix105);
   // Combi transformation: 
   dx = 57.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix107 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix106 = new TGeoCombiTrans("", dx,dy,dz,pMatrix107);
   // Combi transformation: 
   dx = 60.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix109 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix108 = new TGeoCombiTrans("", dx,dy,dz,pMatrix109);
   // Combi transformation: 
   dx = 62.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix111 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix110 = new TGeoCombiTrans("", dx,dy,dz,pMatrix111);
   // Combi transformation: 
   dx = 65.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix113 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix112 = new TGeoCombiTrans("", dx,dy,dz,pMatrix113);
   // Combi transformation: 
   dx = 67.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix115 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix114 = new TGeoCombiTrans("", dx,dy,dz,pMatrix115);
   // Combi transformation: 
   dx = 70.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix117 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix116 = new TGeoCombiTrans("", dx,dy,dz,pMatrix117);
   // Combi transformation: 
   dx = 72.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix119 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix118 = new TGeoCombiTrans("", dx,dy,dz,pMatrix119);
   // Combi transformation: 
   dx = 75.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix121 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix120 = new TGeoCombiTrans("", dx,dy,dz,pMatrix121);
   // Combi transformation: 
   dx = 77.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix123 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix122 = new TGeoCombiTrans("", dx,dy,dz,pMatrix123);
   // Combi transformation: 
   dx = 80.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix125 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix124 = new TGeoCombiTrans("", dx,dy,dz,pMatrix125);
   // Combi transformation: 
   dx = 82.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix127 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix126 = new TGeoCombiTrans("", dx,dy,dz,pMatrix127);
   // Combi transformation: 
   dx = 85.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix129 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix128 = new TGeoCombiTrans("", dx,dy,dz,pMatrix129);
   // Combi transformation: 
   dx = 87.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix131 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix130 = new TGeoCombiTrans("", dx,dy,dz,pMatrix131);
   // Combi transformation: 
   dx = 90.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix133 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix132 = new TGeoCombiTrans("", dx,dy,dz,pMatrix133);
   // Combi transformation: 
   dx = 92.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix135 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix134 = new TGeoCombiTrans("", dx,dy,dz,pMatrix135);
   // Combi transformation: 
   dx = 95.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix137 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix136 = new TGeoCombiTrans("", dx,dy,dz,pMatrix137);
   // Combi transformation: 
   dx = 97.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix139 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix138 = new TGeoCombiTrans("", dx,dy,dz,pMatrix139);
   // Combi transformation: 
   dx = 100.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix141 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix140 = new TGeoCombiTrans("", dx,dy,dz,pMatrix141);
   // Combi transformation: 
   dx = 102.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix143 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix142 = new TGeoCombiTrans("", dx,dy,dz,pMatrix143);
   // Combi transformation: 
   dx = 105.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix145 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix144 = new TGeoCombiTrans("", dx,dy,dz,pMatrix145);
   // Combi transformation: 
   dx = 107.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix147 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix146 = new TGeoCombiTrans("", dx,dy,dz,pMatrix147);
   // Combi transformation: 
   dx = 110.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix149 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix148 = new TGeoCombiTrans("", dx,dy,dz,pMatrix149);
   // Combi transformation: 
   dx = 112.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix151 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix150 = new TGeoCombiTrans("", dx,dy,dz,pMatrix151);
   // Combi transformation: 
   dx = 115.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix153 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix152 = new TGeoCombiTrans("", dx,dy,dz,pMatrix153);
   // Combi transformation: 
   dx = 117.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix155 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix154 = new TGeoCombiTrans("", dx,dy,dz,pMatrix155);
   // Combi transformation: 
   dx = 120.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix157 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix156 = new TGeoCombiTrans("", dx,dy,dz,pMatrix157);
   // Combi transformation: 
   dx = 122.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix159 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix158 = new TGeoCombiTrans("", dx,dy,dz,pMatrix159);
   // Combi transformation: 
   dx = 125.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix161 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix160 = new TGeoCombiTrans("", dx,dy,dz,pMatrix161);
   // Combi transformation: 
   dx = 127.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix163 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix162 = new TGeoCombiTrans("", dx,dy,dz,pMatrix163);
   // Combi transformation: 
   dx = 130.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix165 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix164 = new TGeoCombiTrans("", dx,dy,dz,pMatrix165);
   // Combi transformation: 
   dx = 132.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix167 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix166 = new TGeoCombiTrans("", dx,dy,dz,pMatrix167);
   // Combi transformation: 
   dx = 135.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix169 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix168 = new TGeoCombiTrans("", dx,dy,dz,pMatrix169);
   // Combi transformation: 
   dx = 137.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix171 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix170 = new TGeoCombiTrans("", dx,dy,dz,pMatrix171);
   // Combi transformation: 
   dx = 140.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix173 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix172 = new TGeoCombiTrans("", dx,dy,dz,pMatrix173);
   // Combi transformation: 
   dx = 142.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix175 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix174 = new TGeoCombiTrans("", dx,dy,dz,pMatrix175);
   // Combi transformation: 
   dx = 145.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix177 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix176 = new TGeoCombiTrans("", dx,dy,dz,pMatrix177);
   // Combi transformation: 
   dx = 147.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix179 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix178 = new TGeoCombiTrans("", dx,dy,dz,pMatrix179);
   // Combi transformation: 
   dx = 150.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix181 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix180 = new TGeoCombiTrans("", dx,dy,dz,pMatrix181);
   // Combi transformation: 
   dx = 152.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix183 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix182 = new TGeoCombiTrans("", dx,dy,dz,pMatrix183);
   // Combi transformation: 
   dx = 155.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix185 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix184 = new TGeoCombiTrans("", dx,dy,dz,pMatrix185);
   // Combi transformation: 
   dx = 157.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix187 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix186 = new TGeoCombiTrans("", dx,dy,dz,pMatrix187);
   // Combi transformation: 
   dx = 160.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix189 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix188 = new TGeoCombiTrans("", dx,dy,dz,pMatrix189);
   // Combi transformation: 
   dx = 162.500000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix191 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix190 = new TGeoCombiTrans("", dx,dy,dz,pMatrix191);
   // Combi transformation: 
   dx = 165.000000;
   dy = 0.000000;
   dz = 735.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix193 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix192 = new TGeoCombiTrans("", dx,dy,dz,pMatrix193);

   // SET TOP VOLUME OF GEOMETRY
   TGeoVolume * pWorld = gGeoManager->GetTopVolume();
   pWorld->SetVisLeaves(kTRUE);


   // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
   // Shape: AladinTOF type: TGeoBBox
   dx = 1.250000;
   dy = 50.000000;
   dz = 1.000000;
   TGeoShape *pAladinTOF = new TGeoBBox("AladinTOF", dx,dy,dz);
   // Volume: AladinTOF
   TGeoVolume*
   pAladinTOFv = new TGeoVolume("AladinTOF",pAladinTOF, pMedSci);
   pAladinTOFv->SetVisLeaves(kTRUE);



   pWorld->AddNode(pAladinTOFv, 0, pMatrix2);
   pWorld->AddNode(pAladinTOFv, 1, pMatrix4);
   pWorld->AddNode(pAladinTOFv, 2, pMatrix6);
   pWorld->AddNode(pAladinTOFv, 3, pMatrix8);
   pWorld->AddNode(pAladinTOFv, 4, pMatrix10);
   pWorld->AddNode(pAladinTOFv, 5, pMatrix12);
   pWorld->AddNode(pAladinTOFv, 6, pMatrix14);
   pWorld->AddNode(pAladinTOFv, 7, pMatrix16);
   pWorld->AddNode(pAladinTOFv, 8, pMatrix18);
   pWorld->AddNode(pAladinTOFv, 9, pMatrix20);
   pWorld->AddNode(pAladinTOFv, 10, pMatrix22);
   pWorld->AddNode(pAladinTOFv, 11, pMatrix24);
   pWorld->AddNode(pAladinTOFv, 12, pMatrix26);
   pWorld->AddNode(pAladinTOFv, 13, pMatrix28);
   pWorld->AddNode(pAladinTOFv, 14, pMatrix30);
   pWorld->AddNode(pAladinTOFv, 15, pMatrix32);
   pWorld->AddNode(pAladinTOFv, 16, pMatrix34);
   pWorld->AddNode(pAladinTOFv, 17, pMatrix36);
   pWorld->AddNode(pAladinTOFv, 18, pMatrix38);
   pWorld->AddNode(pAladinTOFv, 19, pMatrix40);
   pWorld->AddNode(pAladinTOFv, 20, pMatrix42);
   pWorld->AddNode(pAladinTOFv, 21, pMatrix44);
   pWorld->AddNode(pAladinTOFv, 22, pMatrix46);
   pWorld->AddNode(pAladinTOFv, 23, pMatrix48);
   pWorld->AddNode(pAladinTOFv, 24, pMatrix50);
   pWorld->AddNode(pAladinTOFv, 25, pMatrix52);
   pWorld->AddNode(pAladinTOFv, 26, pMatrix54);
   pWorld->AddNode(pAladinTOFv, 27, pMatrix56);
   pWorld->AddNode(pAladinTOFv, 28, pMatrix58);
   pWorld->AddNode(pAladinTOFv, 29, pMatrix60);
   pWorld->AddNode(pAladinTOFv, 30, pMatrix62);
   pWorld->AddNode(pAladinTOFv, 31, pMatrix64);
   pWorld->AddNode(pAladinTOFv, 32, pMatrix66);
   pWorld->AddNode(pAladinTOFv, 33, pMatrix68);
   pWorld->AddNode(pAladinTOFv, 34, pMatrix70);
   pWorld->AddNode(pAladinTOFv, 35, pMatrix72);
   pWorld->AddNode(pAladinTOFv, 36, pMatrix74);
   pWorld->AddNode(pAladinTOFv, 37, pMatrix76);
   pWorld->AddNode(pAladinTOFv, 38, pMatrix78);
   pWorld->AddNode(pAladinTOFv, 39, pMatrix80);
   pWorld->AddNode(pAladinTOFv, 40, pMatrix82);
   pWorld->AddNode(pAladinTOFv, 41, pMatrix84);
   pWorld->AddNode(pAladinTOFv, 42, pMatrix86);
   pWorld->AddNode(pAladinTOFv, 43, pMatrix88);
   pWorld->AddNode(pAladinTOFv, 44, pMatrix90);
   pWorld->AddNode(pAladinTOFv, 45, pMatrix92);
   pWorld->AddNode(pAladinTOFv, 46, pMatrix94);
   pWorld->AddNode(pAladinTOFv, 47, pMatrix96);
   pWorld->AddNode(pAladinTOFv, 48, pMatrix98);
   pWorld->AddNode(pAladinTOFv, 49, pMatrix100);
   pWorld->AddNode(pAladinTOFv, 50, pMatrix102);
   pWorld->AddNode(pAladinTOFv, 51, pMatrix104);
   pWorld->AddNode(pAladinTOFv, 52, pMatrix106);
   pWorld->AddNode(pAladinTOFv, 53, pMatrix108);
   pWorld->AddNode(pAladinTOFv, 54, pMatrix110);
   pWorld->AddNode(pAladinTOFv, 55, pMatrix112);
   pWorld->AddNode(pAladinTOFv, 56, pMatrix114);
   pWorld->AddNode(pAladinTOFv, 57, pMatrix116);
   pWorld->AddNode(pAladinTOFv, 58, pMatrix118);
   pWorld->AddNode(pAladinTOFv, 59, pMatrix120);
   pWorld->AddNode(pAladinTOFv, 60, pMatrix122);
   pWorld->AddNode(pAladinTOFv, 61, pMatrix124);
   pWorld->AddNode(pAladinTOFv, 62, pMatrix126);
   pWorld->AddNode(pAladinTOFv, 63, pMatrix128);
   pWorld->AddNode(pAladinTOFv, 64, pMatrix130);
   pWorld->AddNode(pAladinTOFv, 65, pMatrix132);
   pWorld->AddNode(pAladinTOFv, 66, pMatrix134);
   pWorld->AddNode(pAladinTOFv, 67, pMatrix136);
   pWorld->AddNode(pAladinTOFv, 68, pMatrix138);
   pWorld->AddNode(pAladinTOFv, 69, pMatrix140);
   pWorld->AddNode(pAladinTOFv, 70, pMatrix142);
   pWorld->AddNode(pAladinTOFv, 71, pMatrix144);
   pWorld->AddNode(pAladinTOFv, 72, pMatrix146);
   pWorld->AddNode(pAladinTOFv, 73, pMatrix148);
   pWorld->AddNode(pAladinTOFv, 74, pMatrix150);
   pWorld->AddNode(pAladinTOFv, 75, pMatrix152);
   pWorld->AddNode(pAladinTOFv, 76, pMatrix154);
   pWorld->AddNode(pAladinTOFv, 77, pMatrix156);
   pWorld->AddNode(pAladinTOFv, 78, pMatrix158);
   pWorld->AddNode(pAladinTOFv, 79, pMatrix160);
   pWorld->AddNode(pAladinTOFv, 80, pMatrix162);
   pWorld->AddNode(pAladinTOFv, 81, pMatrix164);
   pWorld->AddNode(pAladinTOFv, 82, pMatrix166);
   pWorld->AddNode(pAladinTOFv, 83, pMatrix168);
   pWorld->AddNode(pAladinTOFv, 84, pMatrix170);
   pWorld->AddNode(pAladinTOFv, 85, pMatrix172);
   pWorld->AddNode(pAladinTOFv, 86, pMatrix174);
   pWorld->AddNode(pAladinTOFv, 87, pMatrix176);
   pWorld->AddNode(pAladinTOFv, 88, pMatrix178);
   pWorld->AddNode(pAladinTOFv, 89, pMatrix180);
   pWorld->AddNode(pAladinTOFv, 90, pMatrix182);
   pWorld->AddNode(pAladinTOFv, 91, pMatrix184);
   pWorld->AddNode(pAladinTOFv, 92, pMatrix186);
   pWorld->AddNode(pAladinTOFv, 93, pMatrix188);
   pWorld->AddNode(pAladinTOFv, 94, pMatrix190);
   pWorld->AddNode(pAladinTOFv, 95, pMatrix192);


   AddSensitiveVolume(pAladinTOFv);
   fNbOfSensitiveVol+=96;

}



/*
void R3BATof::ConstructGeometry() {
  
  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  R3BGeoTof*       stsGeo  = new R3BGeoTof();
  stsGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(stsGeo);

  Bool_t rc = geoFace->readSet(stsGeo);
  if (rc) stsGeo->create(geoLoad->getGeoBuilder());
  TList* volList = stsGeo->getListOfVolumes();
  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  R3BGeoTofPar* par=(R3BGeoTofPar*)(rtdb->getContainer("R3BGeoTofPar"));
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

ClassImp(R3BATof)
