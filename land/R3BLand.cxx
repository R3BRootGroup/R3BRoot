// -------------------------------------------------------------------------
// -----                        R3BLand source file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------
#include "R3BLand.h"

#include "R3BGeoLand.h"
#include "R3BLandPoint.h"
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
R3BLand::R3BLand() : FairDetector("R3BLand", kTRUE, kSTS) {
  ResetParameters();
  fLandCollection = new TClonesArray("R3BLandPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BLand::R3BLand(const char* name, Bool_t active) 
  : FairDetector(name, active, kSTS) {
  ResetParameters();
  fLandCollection = new TClonesArray("R3BLandPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BLand::~R3BLand() {

  if ( flGeoPar ) delete flGeoPar;
  if (fLandCollection) {
    fLandCollection->Delete();
    delete fLandCollection;
  }
}
// -------------------------------------------------------------------------



// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BLand::ProcessHits(FairVolume* vol) {


/*     cout << " -I process hit called for:" <<  vol->GetName() << endl;
//  Set parameters at entrance of volume. Reset ELoss.

     if ( vol ) {
         cout << " -I- FairVolume name Id:copy "
              << vol->getName() << " : " << vol->getMCid() << " : " << vol->getCopyNo() << endl;
         Int_t copyNo=0;
	 cout << "-I- Corresponding Geant: " << gMC->CurrentVolID(copyNo) << ":" << copyNo << endl;
	 cout << "-I- Looking @ hiearchy ... " << endl;

         FairGeoNode* node = vol->getGeoNode();
         FairGeoNode* mother = node->getMotherNode();

	 cout << "  Mother Node (id: copy) " << mother->getMCid() << " : "
	 << mother->getCopyNo() << endl;
}
*/

  // --- get Geometry hiearchical Information
  Int_t cp1=-1;
  Int_t cp2=-1;
  Int_t volId1=-1;
  Int_t volId2=-1;
  volId1 =  gMC->CurrentVolID(cp1);
  volId2 =  gMC->CurrentVolOffID(1, cp2);

    if ( gMC->IsTrackEntering() ) {
    fELoss  = 0.;
    fTime   = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPosIn);
    gMC->TrackMomentum(fMomIn);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Set additional parameters at exit of active volume. Create R3BLandPoint.
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
      
//       cout << "1st direction: " << olddirection[0] << "," << olddirection[1] << "," << olddirection[2] << endl;

      for (Int_t i=0; i<3; i++){
	newdirection[i] = -1*olddirection[i];
      }
      
      gGeoManager->SetCurrentDirection(newdirection);
      TGeoNode *bla = gGeoManager->FindNextBoundary(2);
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

    AddHit(fTrackID, fVolumeID, cp2, cp1,
	   TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
	   TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
	   TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
	   TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
	   fTime, fLength, fELoss);
    
    // Increment number of LandPoints for this track
    FairStack* stack = (FairStack*) gMC->GetStack();
    stack->AddPoint(kSTS);
    
    ResetParameters();
  }

  return kTRUE;
}
// ----------------------------------------------------------------------------
//void R3BLand::SaveGeoParams(){
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
void R3BLand::BeginEvent() {

//  if (! kGeoSaved ) {
//      SaveGeoParams();
//  cout << "-I- LAND geometry parameters saved " << endl;
//  kGeoSaved = kTRUE;
//  }

}
// -----   Public method EndOfEvent   -----------------------------------------
void R3BLand::EndOfEvent() {

  if (fVerboseLevel) Print();
  fLandCollection->Clear();

  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method Register   -------------------------------------------
void R3BLand::Register() {
  FairRootManager::Instance()->Register("LandPoint", GetName(), fLandCollection, kTRUE);
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BLand::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fLandCollection;
  else return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void R3BLand::Print() const {
  Int_t nHits = fLandCollection->GetEntriesFast();
  cout << "-I- R3BLand: " << nHits << " points registered in this event." 
       << endl;
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void R3BLand::Reset() {
  fLandCollection->Clear();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void R3BLand::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- R3BLand: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  R3BLandPoint* oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++) {
   oldpoint = (R3BLandPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) R3BLandPoint(*oldpoint);
    fPosIndex++;
  }
   cout << " -I- R3BLand: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}

// -----   Private method AddHit   --------------------------------------------
R3BLandPoint* R3BLand::AddHit(Int_t trackID, Int_t detID, Int_t id1, Int_t id2,
			     TVector3 posIn,
			     TVector3 posOut, TVector3 momIn,
			     TVector3 momOut, Double_t time,
			     Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fLandCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1) 
    cout << "-I- R3BLand: Adding Point at (" << posIn.X() << ", " << posIn.Y() 
	 << ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
	 << trackID << ", energy loss " << eLoss*1e06 << " keV" << endl;
  return new(clref[size]) R3BLandPoint(trackID, detID, id1, id2,  posIn, posOut,
				      momIn, momOut, time, length, eLoss*1e06);
}
// -----   Public method ConstructGeometry   ----------------------------------
void R3BLand::ConstructGeometry() {

  // out-of-file geometry definition
   Double_t dx,dy,dz;
   Double_t dx1, dx2, dy1, dy2;
   Double_t vert[20], par[20];
   Double_t theta, phi, h1, bl1, tl1, alpha1, h2, bl2, tl2, alpha2;
   Double_t twist;
   Double_t origin[3];
   Double_t rmin, rmax, rmin1, rmax1, rmin2, rmax2;
   Double_t r, rlo, rhi;
   Double_t a,b;
   Double_t point[3], norm[3];
   Double_t rin, stin, rout, stout;
   Double_t thx, phx, thy, phy, thz, phz;
   Double_t alpha, theta1, theta2, phi1, phi2, dphi;
   Double_t tr[3], rot[9];
   Double_t z, density, radl, absl, w;
   Double_t lx,ly,lz,tx,ty,tz;
   Double_t xvert[50], yvert[50];
   Double_t zsect,x0,y0,scale0;
   Int_t nel, numed, nz, nedges, nvert;

   TGeoBoolNode *pBoolNode = 0;


/****************************************************************************/
// Material definition

 // Vacuum
  TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0,0,0);
  TGeoMedium *pMed1 = new TGeoMedium("Vacuum",1, matVacuum);
  pMed1->Print();

// Mixture: Air
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
  TGeoMedium*
  pMed2 = new TGeoMedium("Air", numed,pMat2);

  TGeoMaterial *vacuum = new TGeoMaterial("vacuum",0,0,0);
  TGeoMaterial *Fe = new TGeoMaterial("Fe",55.84,26,7.87);
  TGeoMaterial *Cu = new TGeoMaterial("Cu",63.549,29,8.92);

//------------------Creat media----------------------------------
  TGeoMedium *Iron = new TGeoMedium("Iron",800,Fe);

 // BC408 plastic medium
 // Mixture: BC408
   nel     = 2;
   density = 1.032000;
   TGeoMixture*
   pMat37 = new TGeoMixture("BC408", nel,density);
      a = 1.007940;   z = 1.000000;   w = 0.520000;  // H
   pMat37->DefineElement(0,a,z,w);
      a = 12.010700;   z = 6.000000;   w = 0.480000;  // C
   pMat37->DefineElement(1,a,z,w);
   pMat37->SetIndex(36);
// Medium: BC408
   numed   = 36;  // medium number
   TGeoMedium*
   pMed37 = new TGeoMedium("BC408", numed,pMat37);

//
// <DB> Attempt to use TGeo Assemblies
// to model neutron detectors 25.03.09
//

   TGeoVolume* vWorld = gGeoManager->GetTopVolume();
   vWorld->SetVisLeaves(kTRUE);

    // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
  Double_t padle_h_dim1x = 100.1;
  Double_t padle_h_dim1y = 5.1;
  Double_t padle_h_dim1z = 5.1;
  TGeoShape* padle_h_box1 = new TGeoBBox("padle_h_box1",
					 padle_h_dim1x,
					 padle_h_dim1y,
					 padle_h_dim1z);
  Double_t padle_h_dim2x = 100.0;
  Double_t padle_h_dim2y = 5.0;
  Double_t padle_h_dim2z = 5.0;
  TGeoShape* padle_h_box2 = new TGeoBBox("padle_h_box2",
					 padle_h_dim2x,
					 padle_h_dim2y,
					 padle_h_dim2z);

  // Create a composite shape
  TGeoCompositeShape *sheetbox
      = new TGeoCompositeShape("diffbox", "padle_h_box1 - padle_h_box2");
  // Corresponding boolean volume
  TGeoVolume *bvol = new TGeoVolume("sheetbox",sheetbox,Iron);

  //end-cap list
  dx1 = 5.100000;
  dx2 = 2.510000;
  dy1 = 5.100000;
  dy2 = 2.510000;
  dz  = 5.000000;

  TGeoVolume *trap = gGeoManager->MakeTrd2("atrap",pMed37,dx1,dx2,dy1,dy2,dz);

  // sheet Iron list
  //------------------ Iron sheets -----------------------------------------
  Double_t padle_h_dim3x = 100.0;
  Double_t padle_h_dim3y = 5.0;
  Double_t padle_h_dim3z = 0.125;

  TGeoVolume *padle_h_box3
      =gGeoManager->MakeBox("padle_h_box3",Iron,
                                    padle_h_dim3x,
				    padle_h_dim3y,
				    padle_h_dim3z);

  Double_t padle_h_dim4x = 100.0;
  Double_t padle_h_dim4y = 5.0;
  Double_t padle_h_dim4z = 0.25;

  TGeoVolume *padle_h_box4
      =gGeoManager->MakeBox("padle_h_box4",Iron,
                                    padle_h_dim4x,
				    padle_h_dim4y,
				    padle_h_dim4z);

   //------------------ BC408 sheets -----------------------------------------
  Double_t padle_h_dim5x = 100.0;
  Double_t padle_h_dim5y = 5.0;
  Double_t padle_h_dim5z = 0.25;

  TGeoVolume *padle_h_box5
      =gGeoManager->MakeBox("padle_h_box5",pMed37,
                                    padle_h_dim5x,
				    padle_h_dim5y,
				    padle_h_dim5z);




 // Make the elementary assembly of the whole structure
   TGeoVolume *aLand = new TGeoVolumeAssembly("ALAND");

   TGeoRotation *rot0 = new TGeoRotation();
   rot0->RotateX(0.);
   rot0->RotateY(0.);
   rot0->RotateZ(0.);
   Double_t xx = 0.;
   Double_t yy = -95.;
   Double_t zz = -45.;

   aLand->AddNode(bvol,1,new TGeoCombiTrans(xx,yy,zz,rot0));

   TGeoRotation *rot1 = new TGeoRotation();
   rot1->RotateX(0.);
   rot1->RotateY(90.);
   rot1->RotateZ(90.);
   xx = 105.1;
   yy = -95.;
   zz = -45.;

   aLand->AddNode(trap,2,new TGeoCombiTrans(xx,yy,zz,rot1));
 

   TGeoRotation *rot2 = new TGeoRotation();
   rot2->RotateX(0.);
   rot2->RotateY(90.);
   rot2->RotateZ(270.);
   xx = -105.1;
   yy = -95.;
   zz = -45.;
   aLand->AddNode(trap,3,new TGeoCombiTrans(xx,yy,zz,rot2));


   
   AddSensitiveVolume(padle_h_box3); //Fe
   AddSensitiveVolume(padle_h_box4); //fe
   AddSensitiveVolume(padle_h_box5); //Scint.

   fNbOfSensitiveVol+=3; //? FIXME


   //paddles
   TGeoRotation *rot3 = new TGeoRotation();
   rot3->RotateX(0.);
   rot3->RotateY(0.);
   rot3->RotateZ(0.);
   xx = 0.;
   yy = -95.;
   
   zz = -49.875;
   aLand->AddNode(padle_h_box3,4,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -49.;
   aLand->AddNode(padle_h_box4,5,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -48.;
   aLand->AddNode(padle_h_box4,6,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -47.;
   aLand->AddNode(padle_h_box4,7,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -46.;
   aLand->AddNode(padle_h_box4,8,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -45.;
   aLand->AddNode(padle_h_box4,9,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -44.;
   aLand->AddNode(padle_h_box4,10,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -43.;
   aLand->AddNode(padle_h_box4,11,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -42.;
   aLand->AddNode(padle_h_box4,12,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -41.;
   aLand->AddNode(padle_h_box4,13,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -40.125;
   aLand->AddNode(padle_h_box3,14,new TGeoCombiTrans(xx,yy,zz,rot3));


   //bc408 paddles

   xx = 0.;
   yy = -95.;

   zz = -49.5;
   aLand->AddNode(padle_h_box5,15,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -48.5;
   aLand->AddNode(padle_h_box5,16,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -47.5;
   aLand->AddNode(padle_h_box5,17,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -46.5;
   aLand->AddNode(padle_h_box5,18,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -45.5;
   aLand->AddNode(padle_h_box5,19,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -44.5;
   aLand->AddNode(padle_h_box5,20,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -43.5;
   aLand->AddNode(padle_h_box5,21,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -42.5;
   aLand->AddNode(padle_h_box5,22,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -41.5;
   aLand->AddNode(padle_h_box5,23,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -40.5;
   aLand->AddNode(padle_h_box5,24,new TGeoCombiTrans(xx,yy,zz,rot3));


 //------------------------- Horizontal Assembly Multiplication & Rotation -----------------------------------------------------------------------  


   // cout << " -I- Assembly: aLand serial nb: " << aLand->GetNumber() << endl;
   // cout << " -I- now couting subnodes ... " << aLand->GetNdaughters() << endl;
   //aLand->CountNodes(1000,1);


  TGeoVolume *cell = new TGeoVolumeAssembly("CELL");

  // cout << " -I- Assembly: cell serial nb: " << cell->GetNumber() << endl;


   TGeoRotation *rot4 = new TGeoRotation();
   rot4->RotateX(0.);
   rot4->RotateY(0.);
   rot4->RotateZ(0.);

   tx=0.;
   ty=0.;
   tz=0.;

 

 for ( Int_t i = 0; i < 5; i++ )
 {
  for( Int_t j = 0; j < 20; j++ )
  {
     tx=0.;
     ty=j*10.2;
     tz=i*20.;
     Int_t nindex = (j+20*i)+1; //1-100
     cell->AddNode(aLand,nindex,new TGeoCombiTrans(tx,ty,tz,rot4));
  }
 }


   cout << " -I- Assembly: cell count nodes (1) "  <<   cell->GetNdaughters() << endl;
   cout << " -I- total : " << cell->CountNodes(5000,0) << endl;


//------------------------- Vertical Assembly Multiplication & Rotation -----------------------------------------------------------------------  

   TGeoRotation *rot5 = new TGeoRotation();
   rot5->RotateX(0.);
   rot5->RotateY(0.);
   rot5->RotateZ(90.);

  for ( Int_t i = 0; i < 5; i++ )
 {
  for( Int_t j = 0; j < 20; j++ )
  {
     tx=j*10.2-192.;
     ty=1.9;
     tz=(2*i+1)*10.;
     Int_t nindex2 = (j+20*i)+101; //101 200
     cell->AddNode(aLand,nindex2,new TGeoCombiTrans(tx,ty,tz,rot5));
  }
 }

   cout << " -I- Assembly: cell count nodes (2) "  <<  cell->GetNdaughters() << endl;
   cout << " -I- total " << cell->CountNodes(5000,0) << endl;



//
   TGeoRotation *rotg = new TGeoRotation();
   rotg->RotateX(0.);
   rotg->RotateY(0.);
   rotg->RotateZ(0.);
   tx = 0.0;
   ty = 0.0;
   tz = 1000.0;

  vWorld->AddNode(cell,1,new TGeoCombiTrans(tx,ty,tz,rotg) );

}



/*
void R3BLand::ConstructGeometry() {
  
  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  R3BGeoLand*       stsGeo  = new R3BGeoLand();
  stsGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(stsGeo);

  Bool_t rc = geoFace->readSet(stsGeo);
  if (rc) stsGeo->create(geoLoad->getGeoBuilder());
  TList* volList = stsGeo->getListOfVolumes();
  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  R3BGeoLandPar* par=(R3BGeoLandPar*)(rtdb->getContainer("R3BGeoLandPar"));
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

ClassImp(R3BLand)
