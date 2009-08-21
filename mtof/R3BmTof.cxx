// -------------------------------------------------------------------------
// -----                        R3BmTof source file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------
#include "R3BmTof.h"

#include "R3BGeomTof.h"
#include "R3BmTofPoint.h"
#include "R3BGeomTofPar.h"

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
R3BmTof::R3BmTof() : R3BDetector("R3BmTof", kTRUE, kMTOF) {
  ResetParameters();
  fmTofCollection = new TClonesArray("R3BmTofPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fCutE=1.0e-03; // 1MeV default
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BmTof::R3BmTof(const char* name, Bool_t active) 
  : R3BDetector(name, active, kMTOF) {
  ResetParameters();
  fmTofCollection = new TClonesArray("R3BmTofPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fCutE=1.0e-03; // 1MeV default
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BmTof::~R3BmTof() {

  if ( flGeoPar ) delete flGeoPar;
  if (fmTofCollection) {
    fmTofCollection->Delete();
    delete fmTofCollection;
  }
}
// -------------------------------------------------------------------------
void R3BmTof::Initialize()
{
  FairDetector::Initialize();

   cout << endl;
   cout << "-I- R3BmTof: initialisation" << endl;
   cout << "-I- R3BmTof: Sci. Vol. (McId) " << gMC->VolId("mTOFLog")<< endl;

}


void R3BmTof::SetSpecialPhysicsCuts(){

   cout << endl;

   cout << "-I- R3BmTof: Adding customized Physics cut ... " << endl;

   if (gGeoManager) {
     TGeoMedium* pSi = gGeoManager->GetMedium("plasticFormTOF");
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

	cout << "-I- R3bmTof Scintillator Medium Id " << pSi->GetId()
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
Bool_t R3BmTof::ProcessHits(FairVolume* vol) {

  // Simple Det plane


   
    if ( gMC->IsTrackEntering() ) {
    fELoss  = 0.;
    fTime   = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPosIn);
    gMC->TrackMomentum(fMomIn);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

 
  // Set additional parameters at exit of active volume. Create R3BmTofPoint.
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
    
    // Increment number of mTofPoints for this track
    FairStack* stack = (FairStack*) gMC->GetStack();
    stack->AddPoint(kMTOF);
    
    ResetParameters();
  }

  return kTRUE;
}
// ----------------------------------------------------------------------------
//void R3BmTof::SaveGeoParams(){
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
void R3BmTof::BeginEvent() {

//  if (! kGeoSaved ) {
//      SaveGeoParams();
//  cout << "-I STS geometry parameters saved " << endl;
//  kGeoSaved = kTRUE;
//  }

}
// -----   Public method EndOfEvent   -----------------------------------------
void R3BmTof::EndOfEvent() {

  if (fVerboseLevel) Print();
  fmTofCollection->Clear();

  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method Register   -------------------------------------------
void R3BmTof::Register() {
  FairRootManager::Instance()->Register("mTOFPoint", GetName(), fmTofCollection, kTRUE);
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BmTof::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fmTofCollection;
  else return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void R3BmTof::Print() const {
  Int_t nHits = fmTofCollection->GetEntriesFast();
  cout << "-I- R3BmTof: " << nHits << " points registered in this event." 
       << endl;
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void R3BmTof::Reset() {
  fmTofCollection->Clear();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void R3BmTof::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- R3BmTof: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  R3BmTofPoint* oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++) {
   oldpoint = (R3BmTofPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) R3BmTofPoint(*oldpoint);
    fPosIndex++;
  }
   cout << " -I- R3BmTof: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}

// -----   Private method AddHit   --------------------------------------------
R3BmTofPoint* R3BmTof::AddHit(Int_t trackID, Int_t detID, TVector3 posIn,
			    TVector3 posOut, TVector3 momIn, 
			    TVector3 momOut, Double_t time, 
			    Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fmTofCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1) 
    cout << "-I- R3BmTof: Adding Point at (" << posIn.X() << ", " << posIn.Y() 
	 << ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
	 << trackID << ", energy loss " << eLoss*1e06 << " keV" << endl;
  return new(clref[size]) R3BmTofPoint(trackID, detID, posIn, posOut,
				      momIn, momOut, time, length, eLoss);
}
// -----   Public method ConstructGeometry   ----------------------------------
void R3BmTof::ConstructGeometry() {

  // out-of-file geometry definition
   Double_t dx,dy,dz;
   Double_t  par[20];
   Double_t a;
   Double_t thx, phx, thy, phy, thz, phz;
   Double_t z, density, w;
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


  // Mixture: plasticForTOF
  TGeoMedium * pMed34=NULL;
   if (gGeoManager->GetMedium("plasticFormTOF") ){
       pMed34=gGeoManager->GetMedium("plasticFormTOF");
   }else{
     nel     = 2;
     density = 1.032000;
     TGeoMixture*
	 pMat34 = new TGeoMixture("plasticFormTOF", nel,density);
     a = 12.010700;   z = 6.000000;   w = 0.914708;  // C
     pMat34->DefineElement(0,a,z,w);
     a = 1.007940;   z = 1.000000;   w = 0.085292;  // H
     pMat34->DefineElement(1,a,z,w);
     pMat34->SetIndex(33);
     // Medium: plasticForTOF
     numed   = 33;  // medium number
     Double_t par[8];
     par[0]  = 0.000000; // isvol
     par[1]  = 0.000000; // ifield
     par[2]  = 0.000000; // fieldm
     par[3]  = 0.000000; // tmaxfd
     par[4]  = 0.000000; // stemax
     par[5]  = 0.000000; // deemax
     par[6]  = 0.000100; // epsil
     par[7]  = 0.000000; // stmin
     pMed34 = new TGeoMedium("plasticFormTOF", numed,pMat34,par);
   }


   // TRANSFORMATION MATRICES
   // Combi transformation: 
   dx = 151.000000;
   dy = 0.000000;
   dz = 758.000000;
   // Rotation: 
   thx = 106.700000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 16.700000;    phz = 0.000000;
   TGeoRotation *pMatrix3 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix2 = new TGeoCombiTrans("", dx,dy,dz,pMatrix3);


   // Shape: World type: TGeoBBox
   TGeoVolume* pWorld = gGeoManager->GetTopVolume();
   pWorld->SetVisLeaves(kTRUE);

   // Create a global Mother Volume
   /*
   dx = 200.000000;
   dy = 200.000000;
   dz = 200.000000;
   TGeoShape *pBoxWorld = new TGeoBBox("mTofBoxWorld", dx,dy,dz);
   TGeoVolume*
   pWorld  = new TGeoVolume("mTofBoxLogWorld",pBoxWorld, pMed2);
   pWorld->SetVisLeaves(kTRUE);
   TGeoCombiTrans *pGlobalc = GetGlobalPosition();

   // add the sphere as Mother Volume
   pAWorld->AddNode(pWorld, 0, pGlobalc);
   */

 
   // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
   // Shape: mTOFBox type: TGeoBBox
   dx = 24.000000;
   dy = 24.000000;
   dz = 0.250000;
   TGeoShape *pmTOFBox = new TGeoBBox("mTOFBox", dx,dy,dz);
   // Volume: mTOFLog
   TGeoVolume *
   pmTOFLog = new TGeoVolume("mTOFLog",pmTOFBox, pMed34);
   pmTOFLog->SetVisLeaves(kTRUE);

   TGeoCombiTrans *pGlobal = GetGlobalPosition(pMatrix2);

   if (pGlobal){
       pWorld->AddNode(pmTOFLog, 0, pGlobal);
   }else{
       pWorld->AddNode(pmTOFLog, 0, pMatrix2);
   }


  AddSensitiveVolume(pmTOFLog);
  fNbOfSensitiveVol+=1;
}



/*
void R3BmTof::ConstructGeometry() {
  
  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  R3BGeomTof*       stsGeo  = new R3BGeomTof();
  stsGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(stsGeo);

  Bool_t rc = geoFace->readSet(stsGeo);
  if (rc) stsGeo->create(geoLoad->getGeoBuilder());
  TList* volList = stsGeo->getListOfVolumes();
  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  R3BGeomTofPar* par=(R3BGeomTofPar*)(rtdb->getContainer("R3BGeomTofPar"));
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

ClassImp(R3BmTof)
