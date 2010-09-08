// -------------------------------------------------------------------------
// -----                        R3BTof source file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------
#include "R3BVeto.h"

#include "R3BGeoVeto.h"
#include "R3BVetoPoint.h"
#include "R3BGeoVetoPar.h"

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
R3BVeto::R3BVeto() : R3BDetector("R3BVETO", kTRUE, kVETO) {
  ResetParameters();
  fVetoCollection = new TClonesArray("R3BVetoPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fCutE=1.0e-03; // 1 MeV default
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BVeto::R3BVeto(const char* name, Bool_t active) 
  : R3BDetector(name, active, kVETO) {
  ResetParameters();
  fVetoCollection = new TClonesArray("R3BVetoPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fCutE=1.0e-03; // 1MeV default
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BVeto::~R3BVeto() {

  if ( flGeoPar ) delete flGeoPar;
  if (fVetoCollection) {
    fVetoCollection->Delete();
    delete fVetoCollection;
  }
}
// -------------------------------------------------------------------------


void R3BVeto::Initialize()
{
  FairDetector::Initialize();

   cout << endl;
   cout << "-I- R3BVeto: initialisation" << endl;
   cout << "-I- R3BVeto: Sci. Vol. (McId) " << gMC->VolId("VETOLogpad")<< endl;
   Int_t id1 = gMC->VolId("VETOLogpad");
   //fMapMcId[id1]=1;
}


void R3BVeto::SetSpecialPhysicsCuts(){

   cout << endl;

   cout << "-I- R3BVeto: Adding customized Physics cut ... " << endl;

   if (gGeoManager) {
     TGeoMedium* pSi = gGeoManager->GetMedium("plasticForVETO");
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

	cout << "-I- R3bVeto Scintillator Medium Id " << pSi->GetId()
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
Bool_t R3BVeto::ProcessHits(FairVolume* vol) {

  //--- get Geometry hierarhical Information

  Int_t cp1=-1;
  Int_t cp2=-1;
  Int_t id1=-1;
  Int_t id2=-1;

  //modules ids
  id1= gMC->CurrentVolID(cp1);
  //numbering scheme
  //fPaddleTyp=fMapMcId[volId1];
  //mother assembly def
  id2=gMC->CurrentVolOffID(1,cp2);

    if ( gMC->IsTrackEntering() ) {
    fELoss  = 0.;
    fTime   = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPosIn);
    gMC->TrackMomentum(fMomIn);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Set additional parameters at exit of active volume. Create R3BVETOPoint.
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

    AddHit(fTrackID, fVolumeID, 1, cp2,cp1,
	   TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
	   TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
	   TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
	   TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
	   fTime, fLength, fELoss);
    
    // Increment number of VetoPoints for this track
    FairStack* stack = (FairStack*) gMC->GetStack();
    stack->AddPoint(kVETO);
    
    ResetParameters();
  }

  return kTRUE;
}
// ----------------------------------------------------------------------------
//  void R3BVeto::SaveGeoParams(){
 
//  cout << " -I Save STS geo params " << endl;

//  TFolder *mf = (TFolder*) gDirectory->FindObjectAny("cbmroot");
//  cout << " mf: " << mf << endl;
//  TFolder *stsf = NULL;
//  if (mf ) stsf = (TFolder*) mf->FindObjectAny(GetName());
//  cout << " stsf: " << stsf << endl;
//  if (stsf) stsf->Add( flGeoPar ) ;
//  FairRootManager::Instance()->WriteFolder();
//  mf->Write("cbmroot",TObject::kWriteDelete);
//  }


// -----   Public method EndOfEvent   -----------------------------------------
void R3BVeto::BeginEvent() {

//  if (! kGeoSaved ) {
//      SaveGeoParams();
//  cout << "-I STS geometry parameters saved " << endl;
//  kGeoSaved = kTRUE;
//  }

}
// -----   Public method EndOfEvent   -----------------------------------------
void R3BVeto::EndOfEvent() {

  if (fVerboseLevel) Print();
  fVetoCollection->Clear();

  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method Register   -------------------------------------------
void R3BVeto::Register() {
  FairRootManager::Instance()->Register("VETOPoint", GetName(), fVetoCollection, kTRUE);
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BVeto::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fVetoCollection;
  else return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void R3BVeto::Print() const {
  Int_t nHits = fVetoCollection->GetEntriesFast();
  cout << "-I- R3BVeto: " << nHits << " points registered in this event." 
       << endl;
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void R3BVeto::Reset() {
  fVetoCollection->Clear();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void R3BVeto::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- R3BVeto: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  R3BVetoPoint* oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++) {
   oldpoint = (R3BVetoPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) R3BVetoPoint(*oldpoint);
    fPosIndex++;
  }
   cout << " -I- R3BVeto: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}

// -----   Private method AddHit   --------------------------------------------
R3BVetoPoint* R3BVeto::AddHit(Int_t trackID, Int_t detID, Int_t box, Int_t sector, Int_t paddle, TVector3 posIn,
			    TVector3 posOut, TVector3 momIn, 
			    TVector3 momOut, Double_t time, 
			    Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fVetoCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1) 
    cout << "-I- R3BVeto: Adding Point at (" << posIn.X() << ", " << posIn.Y() 
	 << ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
	 << trackID << ", energy loss " << eLoss*1e06 << " keV" << endl;
  return new(clref[size]) R3BVetoPoint(trackID, detID, box, sector,paddle, posIn, posOut,
				      momIn, momOut, time, length, eLoss);
}
// -----   Public method ConstructGeometry   ----------------------------------
void R3BVeto::ConstructGeometry() {

  // out-of-file geometry definition
   Double_t dx,dy,dz;
   Double_t w1,w2,w3;
   Double_t dxp,dyp,dzp;
   Double_t dxpad,dypad,dzpad;
   Double_t a;
   Double_t thx, phx, thy, phy, thz, phz;
   Double_t thx1, phx1, thy1, phy1, thz1, phz1;
   Double_t thx2, phx2, thy2, phy2, thz2, phz2;
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


  // Mixture: plasticForVETO
  TGeoMedium * pMed34=NULL;
   if (gGeoManager->GetMedium("plasticForVETO") ){
       pMed34=gGeoManager->GetMedium("plasticForVETO");
   }else{
     nel     = 2;
     density = 1.032000;
     TGeoMixture*
	 pMat34 = new TGeoMixture("plasticForVETO", nel,density);
     a = 12.010700;   z = 6.000000;   w = 0.914708;  // C
     pMat34->DefineElement(0,a,z,w);
     a = 1.007940;   z = 1.000000;   w = 0.085292;  // H
     pMat34->DefineElement(1,a,z,w);
     pMat34->SetIndex(33);
     // Medium: plasticForVETO
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
     pMed34 = new TGeoMedium("plasticForVETO", numed,pMat34,par);
   }


  // TRANSFORMATION MATRICES
   // Combi transformation: 
    w1 = 0.000;
    w2 = 0.000000;
    w3 = 0.00000;
   // dz = 0.;
   // Rotation for whole veto:
   thx = 121.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 31.000000;    phz = 0.000000;

   TGeoRotation *pMatrix3 = new TGeoRotation();
   pMatrix3->RotateX(0.);
   pMatrix3->RotateY(0.);
   pMatrix3->RotateZ(0.);
   
   TGeoCombiTrans*
   pMatrix2 = new TGeoCombiTrans("", w1,w2,w3,pMatrix3);

   //Top Volume
   TGeoVolume* pWorld = gGeoManager->GetTopVolume();
   pWorld->SetVisLeaves(kTRUE);



   // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
   // Shape: VETOBox type: TGeoBBox
   dx = 100.000000;
   dy = 100.000000;
   dz = 0.50000;

   dxp = 100.00;
   dyp = 100.00;
   dzp = 0.25;
   

   dxpad = 5.00;
   dypad = 100.00;
   dzpad = 0.25;

    TGeoRotation *pMatrix51 = new TGeoRotation();
   pMatrix51->RotateX(0.);
   pMatrix51->RotateY(0.);
   pMatrix51->RotateZ(0.);
   TGeoCombiTrans* pMatrix41=new TGeoCombiTrans("",0.,0.,-dzp,pMatrix51);

   TGeoRotation *pMatrix52 = new TGeoRotation();
   pMatrix52->RotateX(0.);
   pMatrix52->RotateY(0.);
   pMatrix52->RotateZ(90.);
   
   TGeoCombiTrans* pMatrix42=new TGeoCombiTrans("",0.,0.,dzp,pMatrix52);

   TGeoVolume* pVETOLog= new TGeoVolumeAssembly("VETOLog");

   
   // Volume: VETOLog
   TGeoVolume*pVETOLogp = new TGeoVolumeAssembly("VETOLogp");
   pVETOLog->SetVisLeaves(kTRUE);
  
   TGeoVolume* pVETOLogpad= gGeoManager->MakeBox("VETOLogpad",pMed34,dxpad,dypad,dzpad);

   AddSensitiveVolume(pVETOLogpad);
   fNbOfSensitiveVol+=1;
   Double_t x=-95.0;
   for(Int_t i=1;i<21;i++){
     pVETOLogp->AddNode(pVETOLogpad,i,new TGeoCombiTrans(x,0.,0.,pMatrix51));
     x=x+2*5.0;
   }
  
  
   pVETOLog->AddNode(pVETOLogp,1,pMatrix41);
   pVETOLog->AddNode(pVETOLogp,2,pMatrix42);
  

   
   TGeoCombiTrans *pGlobal = GetGlobalPosition(pMatrix2);
   if (pGlobal){
     pWorld->AddNode(pVETOLog, 1, pGlobal);
   }else{
     pWorld->AddNode(pVETOLog, 1, pMatrix2);
   }
   

   // AddSensitiveVolume(pVETOLogp);
//    fNbOfSensitiveVol+=2;

//    AddSensitiveVolume(pVETOLog);
//    fNbOfSensitiveVol+=1;
   

}



/*
void R3BVeto::ConstructGeometry() {
  
  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  R3BGeoVeto*       stsGeo  = new R3BGeoVeto();
  stsGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(stsGeo);

  Bool_t rc = geoFace->readSet(stsGeo);
  if (rc) stsGeo->create(geoLoad->getGeoBuilder());
  TList* volList = stsGeo->getListOfVolumes();
  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  R3BGeoVetoPar* par=(R3BGeoVetoPar*)(rtdb->getContainer("R3BGeoVetoPar"));
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

ClassImp(R3BVeto)
