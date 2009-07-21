// -------------------------------------------------------------------------
// -----                        R3BCal source file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------
#include "R3BCal.h"

#include "R3BGeoCal.h"
#include "R3BCalPoint.h"
#include "R3BGeoCalPar.h"

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
#include "TRotation.h"

// includes for modeling
#include "TGeoManager.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TVector3.h"
#include "TGeoMatrix.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoBBox.h"
#include "TGeoPgon.h"
#include "TGeoSphere.h"
#include "TGeoArb8.h"
#include "TGeoCone.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"
#include "TGeoTube.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"
#include <iostream>
// try the mathematical ROOT package
#include "Math/Rotation3D.h"
#include "Math/Point3D.h"

using std::cout;
using std::cerr;
using std::endl;




// -----   Default constructor   -------------------------------------------
R3BCal::R3BCal() : FairDetector("R3BCal", kTRUE, kSTS) {
  ResetParameters();
  fCalCollection = new TClonesArray("R3BCalPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fGeoVersion = 1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
R3BCal::R3BCal(const char* name, Bool_t active) 
  : FairDetector(name, active, kSTS) {
  ResetParameters();
  fCalCollection = new TClonesArray("R3BCalPoint");
  fPosIndex = 0;
  kGeoSaved = kFALSE;
  flGeoPar = new TList();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
  fGeoVersion = 1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
R3BCal::~R3BCal() {

  if ( flGeoPar ) delete flGeoPar;
  if (fCalCollection) {
    fCalCollection->Delete();
    delete fCalCollection;
  }
}
// -------------------------------------------------------------------------



// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BCal::ProcessHits(FairVolume* vol) {
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
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Set additional parameters at exit of active volume. Create R3BCalPoint.
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

    AddHit(fTrackID, fVolumeID,
	   TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
	   TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
	   TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
	   TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
	   fTime, fLength, fELoss);
    
    // Increment number of CalPoints for this track
    FairStack* stack = (FairStack*) gMC->GetStack();
    stack->AddPoint(kSTS);
    
    ResetParameters();
  }

  return kTRUE;
}
// ----------------------------------------------------------------------------
//void R3BCal::SaveGeoParams(){
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
void R3BCal::BeginEvent() {

//  if (! kGeoSaved ) {
//      SaveGeoParams();
//  cout << "-I STS geometry parameters saved " << endl;
//  kGeoSaved = kTRUE;
//  }

}
// -----   Public method EndOfEvent   -----------------------------------------
void R3BCal::EndOfEvent() {

  if (fVerboseLevel) Print();
  fCalCollection->Clear();

  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method Register   -------------------------------------------
void R3BCal::Register() {
  FairRootManager::Instance()->Register("CrystalPoint", GetName(), fCalCollection, kTRUE);
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BCal::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fCalCollection;
  else return NULL;
}
// ----------------------------------------------------------------------------



// -----   Public method Print   ----------------------------------------------
void R3BCal::Print() const {
  Int_t nHits = fCalCollection->GetEntriesFast();
  cout << "-I- R3BCal: " << nHits << " points registered in this event." 
       << endl;
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void R3BCal::Reset() {
  fCalCollection->Clear();
  ResetParameters();
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void R3BCal::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- R3BCal: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  R3BCalPoint* oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++) {
   oldpoint = (R3BCalPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) R3BCalPoint(*oldpoint);
    fPosIndex++;
  }
   cout << " -I- R3BCal: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}

// -----   Private method AddHit   --------------------------------------------
R3BCalPoint* R3BCal::AddHit(Int_t trackID, Int_t detID, TVector3 posIn,
			    TVector3 posOut, TVector3 momIn, 
			    TVector3 momOut, Double_t time, 
			    Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fCalCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1) 
    cout << "-I- R3BCal: Adding Point at (" << posIn.X() << ", " << posIn.Y() 
	 << ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
	 << trackID << ", energy loss " << eLoss*1e06 << " keV" << endl;
  return new(clref[size]) R3BCalPoint(trackID, detID, posIn, posOut,
				      momIn, momOut, time, length, eLoss);
}


void R3BCal::ConstructGeometry() {

   if (fGeoVersion == 1 ) return ConstructGeometry1();
   else
   return ConstructGeometry2();    
}


void R3BCal::ConstructGeometry2(){
   cout << endl;
   cout << "-I- R3bCal ConstructGeometry2() called " << endl;
   cout << endl;


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

// --- Local Material definition

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
  
 // Mixture: CsI
  nel     = 2;
  density = 4.510000;
  TGeoMixture*
      pMat9 = new TGeoMixture("CsIn", nel,density);
  a = 132.905450;   z = 55.000000;   w = 0.511549;  // CS
  pMat9->DefineElement(0,a,z,w);
  a = 126.904470;   z = 53.000000;   w = 0.488451;  // I
  pMat9->DefineElement(1,a,z,w);
  pMat9->SetIndex(601);

  TGeoMedium* pMed9 = new TGeoMedium("CsIn", 2,pMat9);

 // Material: Aluminum
  a       = 26.980000;
  z       = 13.000000;
  density = 2.700000;
  radl    = 8.875105;
  absl    = 388.793113;

  TGeoMaterial *matAl = new TGeoMaterial("Aluminum", a,z,density,radl,absl);
  TGeoMedium* pMed21 = new TGeoMedium("Aluminum",3, matAl);

// ---- Geometry Definition 
   // Get the top Volume
    TGeoVolume *top =  gGeoManager->GetTopVolume();

   // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
   // Shape: CBSphereWorld type: TGeoSphere
   rmin   = 24.250000;
   rmax   = 70.000000;
   theta1 = 0.000000;
   theta2 = 180.000000;
   phi1   = 0.000000;
   phi2   = 360.000000;
   TGeoShape *pCBSphereWorld  = new TGeoSphere("CBSphereWorld",rmin,rmax,theta1, theta2,phi1,phi2);
   // Volume: CBLogWorld
   TGeoVolume*
   pCBLogWorld  = new TGeoVolume("CBLogWorld",pCBSphereWorld, pMed2);
   pCBLogWorld->SetVisLeaves(kTRUE);

   TGeoRotation	*pGlobalRot = 
      createMatrix(TMath::Pi()*TMath::RadToDeg()/2.,
		   0.0,
		   0.0);
   TGeoTranslation *pGlobalt = new TGeoTranslation(0.0,0.0,0.0);
   TGeoCombiTrans *pGlobalc = new TGeoCombiTrans(*pGlobalt,*pGlobalRot);  


   // add the sphere as Mother Volume
   top->AddNode(pCBLogWorld, 0, pGlobalc);



   
   // Shape: PGON type: TGeoPgon
   phi1    = 0.000000;
   dphi    = 360.000000;
   nedges = 5;
   nz      = 2;
   Double_t zPlane[2]={-10.0,10.0};
   Double_t rInner[2]={0.,0.};
   Double_t rOuter[2]={3.688,6.638};
   TGeoPgon *pgon = new TGeoPgon("PGON",phi1,dphi,nedges,nz);
   pgon->DefineSection(0, zPlane[0],rInner[0],rOuter[0]);
   pgon->DefineSection(1, zPlane[1],rInner[1],rOuter[1]);
   TGeoShape *pPGON = pgon;

Double_t NAJA_X[12] = {-184.0195,184.0195,0,0,-297.7205,-297.7205,297.7205,297.7205,0,0,-184.0195,184.0195};
Double_t NAJA_Y[12] = {297.7205,297.7205,184.0195,184.0195,0,0,0,0,-184.0195,-184.0195,-297.7205,-297.7205};
Double_t NAJA_Z[12] = {0,0,297.7205,-297.7205,-184.0195,184.0195,-184.0195,184.0195,297.7205,-297.7205,0,0};

TVector3 NAJA_Rot_col_X[12];
NAJA_Rot_col_X[0].SetXYZ(TMath::Sin(18.000000*TMath::Pi()/180)*TMath::Cos(211.720000*TMath::Pi()/180),
                       TMath::Sin(72.000000*TMath::Pi()/180)*TMath::Cos(31.720000*TMath::Pi()/180),
                       TMath::Sin(90.000000*TMath::Pi()/180)*TMath::Cos(121.720000*TMath::Pi()/180));

NAJA_Rot_col_X[1].SetXYZ(TMath::Sin(18.000000*TMath::Pi()/180)*TMath::Cos(328.280000*TMath::Pi()/180),
                       TMath::Sin(108.000000*TMath::Pi()/180)*TMath::Cos(328.280000*TMath::Pi()/180),
                       TMath::Sin(90.000000*TMath::Pi()/180)*TMath::Cos(58.280000*TMath::Pi()/180));

NAJA_Rot_col_X[2].SetXYZ(TMath::Sin(99.350000*TMath::Pi()/180)*TMath::Cos(15.450000*TMath::Pi()/180),
                       TMath::Sin(120.000000*TMath::Pi()/180)*TMath::Cos(110.905000*TMath::Pi()/180),
                       TMath::Sin(31.718000*TMath::Pi()/180)*TMath::Cos(90.000000*TMath::Pi()/180));

NAJA_Rot_col_X[3].SetXYZ(TMath::Sin(80.650000*TMath::Pi()/180)*TMath::Cos(15.450000*TMath::Pi()/180),
                       TMath::Sin(120.000000*TMath::Pi()/180)*TMath::Cos(290.905000*TMath::Pi()/180),
                       TMath::Sin(148.282000*TMath::Pi()/180)*TMath::Cos(90.000000*TMath::Pi()/180));

NAJA_Rot_col_X[4].SetXYZ(TMath::Sin(105.24000*TMath::Pi()/180)*TMath::Cos(80.310000*TMath::Pi()/180),
                       TMath::Sin(144.00000*TMath::Pi()/180)*TMath::Cos(328.28620*TMath::Pi()/180),
                       TMath::Sin(121.717186*TMath::Pi()/180)*TMath::Cos(180.0035095*TMath::Pi()/180));

NAJA_Rot_col_X[5].SetXYZ(TMath::Sin(74.760000*TMath::Pi()/180)*TMath::Cos(80.310000*TMath::Pi()/180),
                       TMath::Sin(144.000000*TMath::Pi()/180)*TMath::Cos(148.286600*TMath::Pi()/180),
                       TMath::Sin(58.2822647*TMath::Pi()/180)*TMath::Cos(180.0037079*TMath::Pi()/180));

NAJA_Rot_col_X[6].SetXYZ(TMath::Sin(105.240000*TMath::Pi()/180)*TMath::Cos(99.690000*TMath::Pi()/180),
                       TMath::Sin(36.000000*TMath::Pi()/180)*TMath::Cos(31.713800*TMath::Pi()/180),
                       TMath::Sin(121.717000*TMath::Pi()/180)*TMath::Cos(359.9964905*TMath::Pi()/180));

NAJA_Rot_col_X[7].SetXYZ(TMath::Sin(74.760000*TMath::Pi()/180)*TMath::Cos(99.690000*TMath::Pi()/180),
                       TMath::Sin(36.000000*TMath::Pi()/180)*TMath::Cos(211.713560*TMath::Pi()/180),
                       TMath::Sin(58.282450*TMath::Pi()/180)*TMath::Cos(359.996300*TMath::Pi()/180));

NAJA_Rot_col_X[8].SetXYZ(TMath::Sin(99.350000*TMath::Pi()/180)*TMath::Cos(344.550000*TMath::Pi()/180),
                       TMath::Sin(60.000000*TMath::Pi()/180)*TMath::Cos(69.095000*TMath::Pi()/180),
                       TMath::Sin(31.717600*TMath::Pi()/180)*TMath::Cos(270.000000*TMath::Pi()/180));

NAJA_Rot_col_X[9].SetXYZ(TMath::Sin(80.650000*TMath::Pi()/180)*TMath::Cos(344.550000*TMath::Pi()/180),
                       TMath::Sin(60.000000*TMath::Pi()/180)*TMath::Cos(249.095000*TMath::Pi()/180),
                       TMath::Sin(148.282400*TMath::Pi()/180)*TMath::Cos(270.000000*TMath::Pi()/180));

NAJA_Rot_col_X[10].SetXYZ(TMath::Sin(18.000000*TMath::Pi()/180)*TMath::Cos(148.280000*TMath::Pi()/180),
                       TMath::Sin(108.000000*TMath::Pi()/180)*TMath::Cos(328.280000*TMath::Pi()/180),
                       TMath::Sin(90.000000*TMath::Pi()/180)*TMath::Cos(238.280000*TMath::Pi()/180));

NAJA_Rot_col_X[11].SetXYZ(TMath::Sin(18.000000*TMath::Pi()/180)*TMath::Cos(31.720000*TMath::Pi()/180),
                       TMath::Sin(72.000000*TMath::Pi()/180)*TMath::Cos(211.720000*TMath::Pi()/180),
                       TMath::Sin(90.000000*TMath::Pi()/180)*TMath::Cos(301.720000*TMath::Pi()/180));


TVector3 NAJA_Rot_col_Y[12];
NAJA_Rot_col_Y[0].SetXYZ(TMath::Sin(18.000000*TMath::Pi()/180)*TMath::Sin(211.720000*TMath::Pi()/180),
                    TMath::Sin(72.000000*TMath::Pi()/180)*TMath::Sin(31.720000*TMath::Pi()/180),
                    TMath::Sin(90.000000*TMath::Pi()/180)*TMath::Sin(121.720000*TMath::Pi()/180));

NAJA_Rot_col_Y[1].SetXYZ(TMath::Sin(18.000000*TMath::Pi()/180)*TMath::Sin(328.280000*TMath::Pi()/180),
                    TMath::Sin(108.000000*TMath::Pi()/180)*TMath::Sin(328.280000*TMath::Pi()/180),
                    TMath::Sin(90.000000*TMath::Pi()/180)*TMath::Sin(58.280000*TMath::Pi()/180));

NAJA_Rot_col_Y[2].SetXYZ(TMath::Sin(99.350000*TMath::Pi()/180)*TMath::Sin(15.450000*TMath::Pi()/180),
                    TMath::Sin(120.000000*TMath::Pi()/180)*TMath::Sin(110.905000*TMath::Pi()/180),
                    TMath::Sin(31.718000*TMath::Pi()/180)*TMath::Sin(90.000000*TMath::Pi()/180));

NAJA_Rot_col_Y[3].SetXYZ(TMath::Sin(80.650000*TMath::Pi()/180)*TMath::Sin(15.450000*TMath::Pi()/180),
                    TMath::Sin(120.000000*TMath::Pi()/180)*TMath::Sin(290.905000*TMath::Pi()/180),
                    TMath::Sin(148.282000*TMath::Pi()/180)*TMath::Sin(90.000000*TMath::Pi()/180));

NAJA_Rot_col_Y[4].SetXYZ(TMath::Sin(105.24000*TMath::Pi()/180)*TMath::Sin(80.310000*TMath::Pi()/180),
                    TMath::Sin(144.00000*TMath::Pi()/180)*TMath::Sin(328.28620*TMath::Pi()/180),
                    TMath::Sin(121.717186*TMath::Pi()/180)*TMath::Sin(180.0035095*TMath::Pi()/180));

NAJA_Rot_col_Y[5].SetXYZ(TMath::Sin(74.760000*TMath::Pi()/180)*TMath::Sin(80.310000*TMath::Pi()/180),
                    TMath::Sin(144.000000*TMath::Pi()/180)*TMath::Sin(148.286600*TMath::Pi()/180),
                    TMath::Sin(58.2822647*TMath::Pi()/180)*TMath::Sin(180.0037079*TMath::Pi()/180));

NAJA_Rot_col_Y[6].SetXYZ(TMath::Sin(105.240000*TMath::Pi()/180)*TMath::Sin(99.690000*TMath::Pi()/180),
                    TMath::Sin(36.000000*TMath::Pi()/180)*TMath::Sin(31.713800*TMath::Pi()/180),
                    TMath::Sin(121.717000*TMath::Pi()/180)*TMath::Sin(359.9964905*TMath::Pi()/180));

NAJA_Rot_col_Y[7].SetXYZ(TMath::Sin(74.760000*TMath::Pi()/180)*TMath::Sin(99.690000*TMath::Pi()/180),
                    TMath::Sin(36.000000*TMath::Pi()/180)*TMath::Sin(211.713560*TMath::Pi()/180),
                    TMath::Sin(58.282450*TMath::Pi()/180)*TMath::Sin(359.996300*TMath::Pi()/180));

NAJA_Rot_col_Y[8].SetXYZ(TMath::Sin(99.350000*TMath::Pi()/180)*TMath::Sin(344.550000*TMath::Pi()/180),
                    TMath::Sin(60.000000*TMath::Pi()/180)*TMath::Sin(69.095000*TMath::Pi()/180),
                    TMath::Sin(31.717600*TMath::Pi()/180)*TMath::Sin(270.000000*TMath::Pi()/180));

NAJA_Rot_col_Y[9].SetXYZ(TMath::Sin(80.650000*TMath::Pi()/180)*TMath::Sin(344.550000*TMath::Pi()/180),
                    TMath::Sin(60.000000*TMath::Pi()/180)*TMath::Sin(249.095000*TMath::Pi()/180),
                    TMath::Sin(148.282400*TMath::Pi()/180)*TMath::Sin(270.000000*TMath::Pi()/180));

NAJA_Rot_col_Y[10].SetXYZ(TMath::Sin(18.000000*TMath::Pi()/180)*TMath::Sin(148.280000*TMath::Pi()/180),
                    TMath::Sin(108.000000*TMath::Pi()/180)*TMath::Sin(328.280000*TMath::Pi()/180),
                    TMath::Sin(90.000000*TMath::Pi()/180)*TMath::Sin(238.280000*TMath::Pi()/180));

NAJA_Rot_col_Y[11].SetXYZ(TMath::Sin(18.000000*TMath::Pi()/180)*TMath::Sin(31.720000*TMath::Pi()/180),
                    TMath::Sin(72.000000*TMath::Pi()/180)*TMath::Sin(211.720000*TMath::Pi()/180),
                    TMath::Sin(90.000000*TMath::Pi()/180)*TMath::Sin(301.720000*TMath::Pi()/180));


TVector3 NAJA_Rot_col_Z[12];
NAJA_Rot_col_Z[0].SetXYZ(TMath::Cos(18.000000*TMath::Pi()/180),
                 TMath::Cos(72.000000*TMath::Pi()/180),
                 TMath::Cos(90.000000*TMath::Pi()/180));
NAJA_Rot_col_Z[1].SetXYZ(TMath::Cos(18.000000*TMath::Pi()/180),
                 TMath::Cos(108.000000*TMath::Pi()/180),
                 TMath::Cos(90.000000*TMath::Pi()/180));
NAJA_Rot_col_Z[2].SetXYZ(TMath::Cos(99.350000*TMath::Pi()/180),
                 TMath::Cos(120.000000*TMath::Pi()/180),
                 TMath::Cos(31.718000*TMath::Pi()/180));
NAJA_Rot_col_Z[3].SetXYZ(TMath::Cos(80.650000*TMath::Pi()/180),
                 TMath::Cos(120.000000*TMath::Pi()/180),
                 TMath::Cos(148.282000*TMath::Pi()/180));
NAJA_Rot_col_Z[4].SetXYZ(TMath::Cos(105.24000*TMath::Pi()/180),
                 TMath::Cos(144.00000*TMath::Pi()/180),
                 TMath::Cos(121.717186*TMath::Pi()/180));
NAJA_Rot_col_Z[5].SetXYZ(TMath::Cos(74.760000*TMath::Pi()/180),
                 TMath::Cos(144.000000*TMath::Pi()/180),
                 TMath::Cos(58.2822647*TMath::Pi()/180));
NAJA_Rot_col_Z[6].SetXYZ(TMath::Cos(105.240000*TMath::Pi()/180),
                 TMath::Cos(36.000000*TMath::Pi()/180),
                 TMath::Cos(121.717000*TMath::Pi()/180));
NAJA_Rot_col_Z[7].SetXYZ(TMath::Cos(74.760000*TMath::Pi()/180),
                 TMath::Cos(36.000000*TMath::Pi()/180),
                 TMath::Cos(58.282450*TMath::Pi()/180));
NAJA_Rot_col_Z[8].SetXYZ(TMath::Cos(99.350000*TMath::Pi()/180),
                 TMath::Cos(60.000000*TMath::Pi()/180),
                 TMath::Cos(31.717600*TMath::Pi()/180));
NAJA_Rot_col_Z[9].SetXYZ(TMath::Cos(80.650000*TMath::Pi()/180),
                 TMath::Cos(60.000000*TMath::Pi()/180),
                 TMath::Cos(148.282400*TMath::Pi()/180));
NAJA_Rot_col_Z[10].SetXYZ(TMath::Cos(18.000000*TMath::Pi()/180),
                 TMath::Cos(108.000000*TMath::Pi()/180),
                 TMath::Cos(90.000000*TMath::Pi()/180));
NAJA_Rot_col_Z[11].SetXYZ(TMath::Cos(18.000000*TMath::Pi()/180),
                 TMath::Cos(72.000000*TMath::Pi()/180),
                 TMath::Cos(90.000000*TMath::Pi()/180));


  // Volume: crystalLogNAJA
   TGeoVolume *
   pcrystalLogNAJA  = new TGeoVolume("crystalLogNAJA",pPGON, pMed9);
   AddSensitiveVolume(pcrystalLogNAJA);
   pcrystalLogNAJA->SetVisLeaves(kTRUE);


   // -- First part defined 
   TGeoRotation *pRot=NULL;
      for(Int_t iter1=0 ; iter1<12 ; iter1++) {

	 Double_t fRotation[9] = {
	    NAJA_Rot_col_X[iter1].X(),NAJA_Rot_col_X[iter1].Y(),NAJA_Rot_col_X[iter1].Z(),
	    NAJA_Rot_col_Y[iter1].X(),NAJA_Rot_col_Y[iter1].Y(),NAJA_Rot_col_Y[iter1].Z(),
	    NAJA_Rot_col_Z[iter1].X(),NAJA_Rot_col_Z[iter1].Y(),NAJA_Rot_col_Z[iter1].Z(),                               
	 };

	 pRot = new TGeoRotation();
	 pRot->SetMatrix(fRotation);
	   
	 dx =  NAJA_X[iter1]/10.;
	 dy =  NAJA_Y[iter1]/10.;
	 dz =  NAJA_Z[iter1]/10.;

	 TGeoTranslation *trans = new TGeoTranslation(dx,dy,dz);
	 TGeoCombiTrans *combi = new TGeoCombiTrans(*trans,*pRot);
	 
	 pCBLogWorld->AddNode(pcrystalLogNAJA, iter1, combi); 
	 
   }//! for iter1  

   //-- Adding single crystal

 // Shape: TRB1 type: TGeoTrap
   dz     = 10.000000;
   theta  = 0.000000;
   phi    = 0.000000;
   h1     = 1.542000;
   bl1    = 4.848000;
   tl1    = 2.658000;
   alpha1 = 0.000000;
   h2     = 2.778000;
   bl2    = 8.726000;
   tl2    = 4.780000;
   alpha2 = 0.000000;
   TGeoShape *pTRB1 = new TGeoTrap("TRB1", dz,theta,phi,h1,bl1,tl1,alpha1,h2,bl2,tl2,alpha2);
   // Shape: TRB2 type: TGeoTrap
   dz     = 10.000000;
   theta  = 0.000000;
   phi    = 0.000000;
   h1     = 1.863000;
   bl1    = 4.850000;
   tl1    = 2.162000;
   alpha1 = 0.000000;
   h2     = 3.353000;
   bl2    = 8.730000;
   tl2    = 3.892000;
   alpha2 = 0.000000;
   TGeoShape *pTRB2 = new TGeoTrap("TRB2", dz,theta,phi,h1,bl1,tl1,alpha1,h2,bl2,tl2,alpha2);
   // Combi transformation: TRB
   dx = 0.000000;
   dy = -4.748000;
   dz = -0.292480;
   // Rotation: 
   thx = 90.000000;    phx = 180.000000;
   thy = 97.775000;    phy = 270.000000;
   thz = 7.775000;    phz = 270.000000;
   TGeoRotation *pMatrix3 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix2 = new TGeoCombiTrans("TRB", dx,dy,dz,pMatrix3);
   // Union for TRB1 and TRB2 
   pBoolNode = new TGeoUnion(pTRB1,pTRB2,0,pMatrix2);
   // Shape: TRB type: TGeoCompositeShape
   TGeoShape *pTRB = new TGeoCompositeShape("TRB", pBoolNode);
   pTRB->SetTitle("TRB1+TRB2:TRB");
   // Volume: crystalLogNAJB
   TGeoVolume*
   pcrystalLogNAJB  = new TGeoVolume("crystalLogNAJB",pTRB, pMed9);


Double_t NAJB_X[60] = {-93.1805,93.1805,-152.88,-152.88,152.88,152.88,-241.4999,-241.4999,-54.7505,54.7505,241.4999,241.4999,54.7505,-54.7505,-88.6375,88.6375,88.6375,-88.6375,-247.3589,-302.1269,-302.1269,-247.3589,0,247.3589,302.1269,302.1269,247.3589,0,-337.3685,-337.3685,337.3685,337.3685,0,-247.3589,-302.1269,-302.1269,-247.3589,0,247.3589,302.1269,302.1269,247.3589,88.6375,-88.6375,-88.6375,88.6375,54.7505,-54.7505,-241.4999,-241.4999,-54.7505,54.7505,241.4999,241.4999,-152.88,-152.88,152.88,152.88,-93.1805,93.1805};
Double_t NAJB_Y[60] = {337.3685,337.3685,302.1129,302.1129,302.1129,302.1129,247.3274,247.3274,241.4789,241.4789,247.3274,247.3274,241.4789,241.4789,152.873,152.873,152.873,152.873,54.775,88.6375,88.6375,54.775,98.056,54.775,88.6375,88.6375,54.775,98.056,0,0,0,0,-98.056,-54.775,-88.6375,-88.6375,-54.775,-98.056,-54.775,-88.6375,-88.6375,-54.775,-152.873,-152.873,-152.873,-152.873,-241.4789,-241.4789,-247.3274,-247.3274,-241.4789,-241.4789,-247.3274,-247.3274,-302.1129,-302.1129,-302.1129,-302.1129,-337.3685,-337.3685};
Double_t NAJB_Z[60] = {0,0,-88.6305,88.6305,88.6305,-88.6305,-54.831,54.831,247.3659,247.3659,54.831,-54.831,-247.3659,-247.3659,302.1129,302.1129,-302.1129,-302.1129,-241.4824,-152.8485,152.8485,241.4824,335.9824,241.4824,152.8485,-152.8485,-241.4824,-335.9824,-93.1805,93.1805,-93.1805,93.1805,335.9824,241.4824,152.8485,-152.8485,-241.4824,-335.9824,-241.4824,-152.8485,152.8485,241.4824,302.1129,302.1129,-302.1129,-302.1129,247.3659,247.3659,54.831,-54.831,-247.3659,-247.3659,-54.831,54.831,88.6305,-88.6305,-88.6305,88.6305,0,0};

TVector3 NAJB_Rot_col_X[60];
NAJB_Rot_col_X[0].SetXYZ(TMath::Sin(179.999939*TMath::Pi()/180)*TMath::Cos(-121.714996*TMath::Pi()/180),
                       TMath::Sin(89.999969*TMath::Pi()/180)*TMath::Cos(-163.721970*TMath::Pi()/180),
                       TMath::Sin(90.000076*TMath::Pi()/180)*TMath::Cos(106.278061*TMath::Pi()/180));

NAJB_Rot_col_X[1].SetXYZ(TMath::Sin(0.000003*TMath::Pi()/180)*TMath::Cos(-121.720016*TMath::Pi()/180),
                       TMath::Sin(90.000076*TMath::Pi()/180)*TMath::Cos(-16.278015*TMath::Pi()/180),
                       TMath::Sin(89.999969*TMath::Pi()/180)*TMath::Cos(73.721939*TMath::Pi()/180));

NAJB_Rot_col_X[2].SetXYZ(TMath::Sin(108.000031*TMath::Pi()/180)*TMath::Cos(-148.279999*TMath::Pi()/180),
                       TMath::Sin(23.548264*TMath::Pi()/180)*TMath::Cos(169.926025*TMath::Pi()/180),
                       TMath::Sin(104.668808*TMath::Pi()/180)*TMath::Cos(116.841034*TMath::Pi()/180));

NAJB_Rot_col_X[3].SetXYZ(TMath::Sin(108.000092*TMath::Pi()/180)*TMath::Cos(31.719986*TMath::Pi()/180),
                       TMath::Sin(156.451691*TMath::Pi()/180)*TMath::Cos(169.926025*TMath::Pi()/180),
                       TMath::Sin(75.331177*TMath::Pi()/180)*TMath::Cos(116.841034*TMath::Pi()/180));

NAJB_Rot_col_X[4].SetXYZ(TMath::Sin(71.999969*TMath::Pi()/180)*TMath::Cos(-31.720016*TMath::Pi()/180),
                       TMath::Sin(156.451691*TMath::Pi()/180)*TMath::Cos(10.073961*TMath::Pi()/180),
                       TMath::Sin(75.331177*TMath::Pi()/180)*TMath::Cos(63.158966*TMath::Pi()/180));

NAJB_Rot_col_X[5].SetXYZ(TMath::Sin(71.999908*TMath::Pi()/180)*TMath::Cos(148.279953*TMath::Pi()/180),
                       TMath::Sin(23.548248*TMath::Pi()/180)*TMath::Cos(10.073951*TMath::Pi()/180),
                       TMath::Sin(104.668808*TMath::Pi()/180)*TMath::Cos(63.158966*TMath::Pi()/180));

NAJB_Rot_col_X[6].SetXYZ(TMath::Sin(35.999985*TMath::Pi()/180)*TMath::Cos(-148.279999*TMath::Pi()/180),
                       TMath::Sin(55.488815*TMath::Pi()/180)*TMath::Cos(50.572174*TMath::Pi()/180),
                       TMath::Sin(99.004181*TMath::Pi()/180)*TMath::Cos(134.317352*TMath::Pi()/180));

NAJB_Rot_col_X[7].SetXYZ(TMath::Sin(35.999985*TMath::Pi()/180)*TMath::Cos(31.719986*TMath::Pi()/180),
                       TMath::Sin(124.511108*TMath::Pi()/180)*TMath::Cos(50.572174*TMath::Pi()/180),
                       TMath::Sin(80.995865*TMath::Pi()/180)*TMath::Cos(134.317398*TMath::Pi()/180));

NAJB_Rot_col_X[8].SetXYZ(TMath::Sin(71.998657*TMath::Pi()/180)*TMath::Cos(-148.283234*TMath::Pi()/180),
                       TMath::Sin(50.469315*TMath::Pi()/180)*TMath::Cos(-42.728638*TMath::Pi()/180),
                       TMath::Sin(45.035828*TMath::Pi()/180)*TMath::Cos(102.779205*TMath::Pi()/180));

NAJB_Rot_col_X[9].SetXYZ(TMath::Sin(108.001343*TMath::Pi()/180)*TMath::Cos(148.283234*TMath::Pi()/180),
                       TMath::Sin(50.469315*TMath::Pi()/180)*TMath::Cos(-137.271378*TMath::Pi()/180),
                       TMath::Sin(45.035828*TMath::Pi()/180)*TMath::Cos(77.220795*TMath::Pi()/180));

NAJB_Rot_col_X[10].SetXYZ(TMath::Sin(143.999985*TMath::Pi()/180)*TMath::Cos(-31.720016*TMath::Pi()/180),
                       TMath::Sin(124.511169*TMath::Pi()/180)*TMath::Cos(129.427750*TMath::Pi()/180),
                       TMath::Sin(80.995819*TMath::Pi()/180)*TMath::Cos(45.682571*TMath::Pi()/180));

NAJB_Rot_col_X[11].SetXYZ(TMath::Sin(143.999985*TMath::Pi()/180)*TMath::Cos(148.279953*TMath::Pi()/180),
                       TMath::Sin(55.488846*TMath::Pi()/180)*TMath::Cos(129.427811*TMath::Pi()/180),
                       TMath::Sin(99.004181*TMath::Pi()/180)*TMath::Cos(45.682571*TMath::Pi()/180));

NAJB_Rot_col_X[12].SetXYZ(TMath::Sin(108.001343*TMath::Pi()/180)*TMath::Cos(-31.716766*TMath::Pi()/180),
                       TMath::Sin(129.530640*TMath::Pi()/180)*TMath::Cos(-137.271317*TMath::Pi()/180),
                       TMath::Sin(134.964142*TMath::Pi()/180)*TMath::Cos(77.220795*TMath::Pi()/180));

NAJB_Rot_col_X[13].SetXYZ(TMath::Sin(71.998657*TMath::Pi()/180)*TMath::Cos(31.716766*TMath::Pi()/180),
                       TMath::Sin(129.530640*TMath::Pi()/180)*TMath::Cos(-42.728607*TMath::Pi()/180),
                       TMath::Sin(134.964142*TMath::Pi()/180)*TMath::Cos(102.779205*TMath::Pi()/180));

NAJB_Rot_col_X[14].SetXYZ(TMath::Sin(59.997574*TMath::Pi()/180)*TMath::Cos(-69.094238*TMath::Pi()/180),
                       TMath::Sin(85.992615*TMath::Pi()/180)*TMath::Cos(23.223938*TMath::Pi()/180),
                       TMath::Sin(30.324402*TMath::Pi()/180)*TMath::Cos(120.102737*TMath::Pi()/180));

NAJB_Rot_col_X[15].SetXYZ(TMath::Sin(120.002426*TMath::Pi()/180)*TMath::Cos(69.094284*TMath::Pi()/180),
                       TMath::Sin(85.992615*TMath::Pi()/180)*TMath::Cos(156.776047*TMath::Pi()/180),
                       TMath::Sin(30.324402*TMath::Pi()/180)*TMath::Cos(59.897247*TMath::Pi()/180));

NAJB_Rot_col_X[16].SetXYZ(TMath::Sin(120.002426*TMath::Pi()/180)*TMath::Cos(-110.905762*TMath::Pi()/180),
                       TMath::Sin(94.007385*TMath::Pi()/180)*TMath::Cos(156.776047*TMath::Pi()/180),
                       TMath::Sin(149.675552*TMath::Pi()/180)*TMath::Cos(59.897308*TMath::Pi()/180));

NAJB_Rot_col_X[17].SetXYZ(TMath::Sin(59.997574*TMath::Pi()/180)*TMath::Cos(110.905701*TMath::Pi()/180),
                       TMath::Sin(94.007385*TMath::Pi()/180)*TMath::Cos(23.223938*TMath::Pi()/180),
                       TMath::Sin(149.675552*TMath::Pi()/180)*TMath::Cos(120.102692*TMath::Pi()/180));

NAJB_Rot_col_X[18].SetXYZ(TMath::Sin(119.999146*TMath::Pi()/180)*TMath::Cos(-69.093414*TMath::Pi()/180),
                       TMath::Sin(58.443619*TMath::Pi()/180)*TMath::Cos(-138.326401*TMath::Pi()/180),
                       TMath::Sin(133.631973*TMath::Pi()/180)*TMath::Cos(167.512238*TMath::Pi()/180));

NAJB_Rot_col_X[19].SetXYZ(TMath::Sin(143.997803*TMath::Pi()/180)*TMath::Cos(31.715622*TMath::Pi()/180),
                       TMath::Sin(113.163864*TMath::Pi()/180)*TMath::Cos(-94.359375*TMath::Pi()/180),
                       TMath::Sin(115.899887*TMath::Pi()/180)*TMath::Cos(163.649902*TMath::Pi()/180));

NAJB_Rot_col_X[20].SetXYZ(TMath::Sin(143.997803*TMath::Pi()/180)*TMath::Cos(-148.284317*TMath::Pi()/180),
                       TMath::Sin(66.836121*TMath::Pi()/180)*TMath::Cos(-94.359436*TMath::Pi()/180),
                       TMath::Sin(64.100113*TMath::Pi()/180)*TMath::Cos(163.649902*TMath::Pi()/180));

NAJB_Rot_col_X[21].SetXYZ(TMath::Sin(119.999146*TMath::Pi()/180)*TMath::Cos(110.906570*TMath::Pi()/180),
                       TMath::Sin(121.556427*TMath::Pi()/180)*TMath::Cos(-138.326401*TMath::Pi()/180),
                       TMath::Sin(46.368011*TMath::Pi()/180)*TMath::Cos(167.512238*TMath::Pi()/180));

NAJB_Rot_col_X[22].SetXYZ(TMath::Sin(90.000076*TMath::Pi()/180)*TMath::Cos(-0.000001*TMath::Pi()/180),
                       TMath::Sin(106.278061*TMath::Pi()/180)*TMath::Cos(90.000076*TMath::Pi()/180),
                       TMath::Sin(16.277985*TMath::Pi()/180)*TMath::Cos(89.999969*TMath::Pi()/180));

NAJB_Rot_col_X[23].SetXYZ(TMath::Sin(60.000854*TMath::Pi()/180)*TMath::Cos(-110.906570*TMath::Pi()/180),
                       TMath::Sin(121.556366*TMath::Pi()/180)*TMath::Cos(-41.673569*TMath::Pi()/180),
                       TMath::Sin(46.368011*TMath::Pi()/180)*TMath::Cos(12.487767*TMath::Pi()/180));

NAJB_Rot_col_X[24].SetXYZ(TMath::Sin(36.002151*TMath::Pi()/180)*TMath::Cos(148.284378*TMath::Pi()/180),
                       TMath::Sin(66.836121*TMath::Pi()/180)*TMath::Cos(-85.640564*TMath::Pi()/180),
                       TMath::Sin(64.100113*TMath::Pi()/180)*TMath::Cos(16.350067*TMath::Pi()/180));

NAJB_Rot_col_X[25].SetXYZ(TMath::Sin(36.002136*TMath::Pi()/180)*TMath::Cos(-31.715652*TMath::Pi()/180),
                       TMath::Sin(113.163864*TMath::Pi()/180)*TMath::Cos(-85.640564*TMath::Pi()/180),
                       TMath::Sin(115.899887*TMath::Pi()/180)*TMath::Cos(16.350067*TMath::Pi()/180));

NAJB_Rot_col_X[26].SetXYZ(TMath::Sin(60.000854*TMath::Pi()/180)*TMath::Cos(69.093414*TMath::Pi()/180),
                       TMath::Sin(58.443573*TMath::Pi()/180)*TMath::Cos(-41.673553*TMath::Pi()/180),
                       TMath::Sin(133.631973*TMath::Pi()/180)*TMath::Cos(12.487760*TMath::Pi()/180));

NAJB_Rot_col_X[27].SetXYZ(TMath::Sin(89.999908*TMath::Pi()/180)*TMath::Cos(180.000000*TMath::Pi()/180),
                       TMath::Sin(73.721939*TMath::Pi()/180)*TMath::Cos(89.999969*TMath::Pi()/180),
                       TMath::Sin(163.721970*TMath::Pi()/180)*TMath::Cos(90.000076*TMath::Pi()/180));

NAJB_Rot_col_X[28].SetXYZ(TMath::Sin(89.999969*TMath::Pi()/180)*TMath::Cos(89.999969*TMath::Pi()/180),
                       TMath::Sin(163.721970*TMath::Pi()/180)*TMath::Cos(-0.000009*TMath::Pi()/180),
                       TMath::Sin(106.278061*TMath::Pi()/180)*TMath::Cos(180.000000*TMath::Pi()/180));

NAJB_Rot_col_X[29].SetXYZ(TMath::Sin(89.999908*TMath::Pi()/180)*TMath::Cos(-89.999908*TMath::Pi()/180),
                       TMath::Sin(16.278000*TMath::Pi()/180)*TMath::Cos(0.000002*TMath::Pi()/180),
                       TMath::Sin(73.721939*TMath::Pi()/180)*TMath::Cos(-180.000000*TMath::Pi()/180));

NAJB_Rot_col_X[30].SetXYZ(TMath::Sin(89.999969*TMath::Pi()/180)*TMath::Cos(-89.999969*TMath::Pi()/180),
                       TMath::Sin(163.721970*TMath::Pi()/180)*TMath::Cos(180.000000*TMath::Pi()/180),
                       TMath::Sin(106.278061*TMath::Pi()/180)*TMath::Cos(-0.000001*TMath::Pi()/180));

NAJB_Rot_col_X[31].SetXYZ(TMath::Sin(90.000076*TMath::Pi()/180)*TMath::Cos(90.000076*TMath::Pi()/180),
                       TMath::Sin(16.278015*TMath::Pi()/180)*TMath::Cos(-179.999939*TMath::Pi()/180),
                       TMath::Sin(73.721939*TMath::Pi()/180)*TMath::Cos(0.000000*TMath::Pi()/180));

NAJB_Rot_col_X[32].SetXYZ(TMath::Sin(89.999969*TMath::Pi()/180)*TMath::Cos(-180.000000*TMath::Pi()/180),
                       TMath::Sin(106.278061*TMath::Pi()/180)*TMath::Cos(-89.999969*TMath::Pi()/180),
                       TMath::Sin(16.277985*TMath::Pi()/180)*TMath::Cos(-89.999969*TMath::Pi()/180));

NAJB_Rot_col_X[33].SetXYZ(TMath::Sin(60.000854*TMath::Pi()/180)*TMath::Cos(69.093414*TMath::Pi()/180),
                       TMath::Sin(121.556366*TMath::Pi()/180)*TMath::Cos(138.326401*TMath::Pi()/180),
                       TMath::Sin(46.368011*TMath::Pi()/180)*TMath::Cos(-167.512238*TMath::Pi()/180));

NAJB_Rot_col_X[34].SetXYZ(TMath::Sin(36.002136*TMath::Pi()/180)*TMath::Cos(-31.715622*TMath::Pi()/180),
                       TMath::Sin(66.836060*TMath::Pi()/180)*TMath::Cos(94.359436*TMath::Pi()/180),
                       TMath::Sin(64.100113*TMath::Pi()/180)*TMath::Cos(-163.649902*TMath::Pi()/180));

NAJB_Rot_col_X[35].SetXYZ(TMath::Sin(36.002136*TMath::Pi()/180)*TMath::Cos(148.284317*TMath::Pi()/180),
                       TMath::Sin(113.163864*TMath::Pi()/180)*TMath::Cos(94.359436*TMath::Pi()/180),
                       TMath::Sin(115.899887*TMath::Pi()/180)*TMath::Cos(-163.649902*TMath::Pi()/180));

NAJB_Rot_col_X[36].SetXYZ(TMath::Sin(60.000854*TMath::Pi()/180)*TMath::Cos(-110.906570*TMath::Pi()/180),
                       TMath::Sin(58.443573*TMath::Pi()/180)*TMath::Cos(138.326401*TMath::Pi()/180),
                       TMath::Sin(133.631973*TMath::Pi()/180)*TMath::Cos(-167.512238*TMath::Pi()/180));

NAJB_Rot_col_X[37].SetXYZ(TMath::Sin(89.999969*TMath::Pi()/180)*TMath::Cos(0.000001*TMath::Pi()/180),
                       TMath::Sin(73.721939*TMath::Pi()/180)*TMath::Cos(-89.999969*TMath::Pi()/180),
                       TMath::Sin(163.721970*TMath::Pi()/180)*TMath::Cos(-89.999969*TMath::Pi()/180));

NAJB_Rot_col_X[38].SetXYZ(TMath::Sin(119.999146*TMath::Pi()/180)*TMath::Cos(110.906570*TMath::Pi()/180),
                       TMath::Sin(58.443573*TMath::Pi()/180)*TMath::Cos(41.673584*TMath::Pi()/180),
                       TMath::Sin(133.631973*TMath::Pi()/180)*TMath::Cos(-12.487763*TMath::Pi()/180));

NAJB_Rot_col_X[39].SetXYZ(TMath::Sin(143.997864*TMath::Pi()/180)*TMath::Cos(-148.284317*TMath::Pi()/180),
                       TMath::Sin(113.163925*TMath::Pi()/180)*TMath::Cos(85.640564*TMath::Pi()/180),
                       TMath::Sin(115.899887*TMath::Pi()/180)*TMath::Cos(-16.350067*TMath::Pi()/180));

NAJB_Rot_col_X[40].SetXYZ(TMath::Sin(143.997803*TMath::Pi()/180)*TMath::Cos(31.715637*TMath::Pi()/180),
                       TMath::Sin(66.836121*TMath::Pi()/180)*TMath::Cos(85.640610*TMath::Pi()/180),
                       TMath::Sin(64.100113*TMath::Pi()/180)*TMath::Cos(-16.350067*TMath::Pi()/180));

NAJB_Rot_col_X[41].SetXYZ(TMath::Sin(119.999146*TMath::Pi()/180)*TMath::Cos(-69.093414*TMath::Pi()/180),
                       TMath::Sin(121.556427*TMath::Pi()/180)*TMath::Cos(41.673553*TMath::Pi()/180),
                       TMath::Sin(46.368011*TMath::Pi()/180)*TMath::Cos(-12.487760*TMath::Pi()/180));

NAJB_Rot_col_X[42].SetXYZ(TMath::Sin(59.997574*TMath::Pi()/180)*TMath::Cos(110.905762*TMath::Pi()/180),
                       TMath::Sin(85.992615*TMath::Pi()/180)*TMath::Cos(-156.776047*TMath::Pi()/180),
                       TMath::Sin(30.324402*TMath::Pi()/180)*TMath::Cos(-59.897247*TMath::Pi()/180));

NAJB_Rot_col_X[43].SetXYZ(TMath::Sin(120.002426*TMath::Pi()/180)*TMath::Cos(-110.905701*TMath::Pi()/180),
                       TMath::Sin(85.992615*TMath::Pi()/180)*TMath::Cos(-23.223938*TMath::Pi()/180),
                       TMath::Sin(30.324402*TMath::Pi()/180)*TMath::Cos(-120.102737*TMath::Pi()/180));

NAJB_Rot_col_X[44].SetXYZ(TMath::Sin(120.002426*TMath::Pi()/180)*TMath::Cos(69.094238*TMath::Pi()/180),
                       TMath::Sin(94.007385*TMath::Pi()/180)*TMath::Cos(-23.223938*TMath::Pi()/180),
                       TMath::Sin(149.675552*TMath::Pi()/180)*TMath::Cos(-120.102692*TMath::Pi()/180));

NAJB_Rot_col_X[45].SetXYZ(TMath::Sin(59.997574*TMath::Pi()/180)*TMath::Cos(-69.094284*TMath::Pi()/180),
                       TMath::Sin(94.007385*TMath::Pi()/180)*TMath::Cos(-156.776047*TMath::Pi()/180),
                       TMath::Sin(149.675552*TMath::Pi()/180)*TMath::Cos(-59.897308*TMath::Pi()/180));

NAJB_Rot_col_X[46].SetXYZ(TMath::Sin(71.998657*TMath::Pi()/180)*TMath::Cos(31.716766*TMath::Pi()/180),
                       TMath::Sin(50.469315*TMath::Pi()/180)*TMath::Cos(137.271317*TMath::Pi()/180),
                       TMath::Sin(45.035828*TMath::Pi()/180)*TMath::Cos(-77.220795*TMath::Pi()/180));

NAJB_Rot_col_X[47].SetXYZ(TMath::Sin(108.001343*TMath::Pi()/180)*TMath::Cos(-31.716766*TMath::Pi()/180),
                       TMath::Sin(50.469315*TMath::Pi()/180)*TMath::Cos(42.728607*TMath::Pi()/180),
                       TMath::Sin(45.035828*TMath::Pi()/180)*TMath::Cos(-102.779205*TMath::Pi()/180));

NAJB_Rot_col_X[48].SetXYZ(TMath::Sin(143.999985*TMath::Pi()/180)*TMath::Cos(148.279953*TMath::Pi()/180),
                       TMath::Sin(124.511169*TMath::Pi()/180)*TMath::Cos(-50.572174*TMath::Pi()/180),
                       TMath::Sin(80.995819*TMath::Pi()/180)*TMath::Cos(-134.317398*TMath::Pi()/180));

NAJB_Rot_col_X[49].SetXYZ(TMath::Sin(143.999985*TMath::Pi()/180)*TMath::Cos(-31.719986*TMath::Pi()/180),
                       TMath::Sin(55.488846*TMath::Pi()/180)*TMath::Cos(-50.572174*TMath::Pi()/180),
                       TMath::Sin(99.004181*TMath::Pi()/180)*TMath::Cos(-134.317398*TMath::Pi()/180));

NAJB_Rot_col_X[50].SetXYZ(TMath::Sin(108.001343*TMath::Pi()/180)*TMath::Cos(148.283234*TMath::Pi()/180),
                       TMath::Sin(129.530640*TMath::Pi()/180)*TMath::Cos(42.728638*TMath::Pi()/180),
                       TMath::Sin(134.964142*TMath::Pi()/180)*TMath::Cos(-102.779205*TMath::Pi()/180));

NAJB_Rot_col_X[51].SetXYZ(TMath::Sin(71.998657*TMath::Pi()/180)*TMath::Cos(-148.283234*TMath::Pi()/180),
                       TMath::Sin(129.530640*TMath::Pi()/180)*TMath::Cos(137.271378*TMath::Pi()/180),
                       TMath::Sin(134.964142*TMath::Pi()/180)*TMath::Cos(-77.220795*TMath::Pi()/180));

NAJB_Rot_col_X[52].SetXYZ(TMath::Sin(35.999969*TMath::Pi()/180)*TMath::Cos(31.720001*TMath::Pi()/180),
                       TMath::Sin(55.488815*TMath::Pi()/180)*TMath::Cos(-129.427811*TMath::Pi()/180),
                       TMath::Sin(99.004181*TMath::Pi()/180)*TMath::Cos(-45.682571*TMath::Pi()/180));

NAJB_Rot_col_X[53].SetXYZ(TMath::Sin(35.999985*TMath::Pi()/180)*TMath::Cos(-148.279953*TMath::Pi()/180),
                       TMath::Sin(124.511108*TMath::Pi()/180)*TMath::Cos(-129.427811*TMath::Pi()/180),
                       TMath::Sin(80.995865*TMath::Pi()/180)*TMath::Cos(-45.682571*TMath::Pi()/180));

NAJB_Rot_col_X[54].SetXYZ(TMath::Sin(71.999969*TMath::Pi()/180)*TMath::Cos(148.279953*TMath::Pi()/180),
                       TMath::Sin(156.451691*TMath::Pi()/180)*TMath::Cos(-169.926025*TMath::Pi()/180),
                       TMath::Sin(75.331177*TMath::Pi()/180)*TMath::Cos(-116.841034*TMath::Pi()/180));

NAJB_Rot_col_X[55].SetXYZ(TMath::Sin(71.999908*TMath::Pi()/180)*TMath::Cos(-31.719986*TMath::Pi()/180),
                       TMath::Sin(23.548248*TMath::Pi()/180)*TMath::Cos(-169.926025*TMath::Pi()/180),
                       TMath::Sin(104.668808*TMath::Pi()/180)*TMath::Cos(-116.841034*TMath::Pi()/180));

NAJB_Rot_col_X[56].SetXYZ(TMath::Sin(108.000031*TMath::Pi()/180)*TMath::Cos(31.720001*TMath::Pi()/180),
                       TMath::Sin(23.548264*TMath::Pi()/180)*TMath::Cos(-10.073964*TMath::Pi()/180),
                       TMath::Sin(104.668808*TMath::Pi()/180)*TMath::Cos(-63.158966*TMath::Pi()/180));

NAJB_Rot_col_X[57].SetXYZ(TMath::Sin(108.000092*TMath::Pi()/180)*TMath::Cos(-148.279999*TMath::Pi()/180),
                       TMath::Sin(156.451691*TMath::Pi()/180)*TMath::Cos(-10.073964*TMath::Pi()/180),
                       TMath::Sin(75.331177*TMath::Pi()/180)*TMath::Cos(-63.158966*TMath::Pi()/180));

NAJB_Rot_col_X[58].SetXYZ(TMath::Sin(0.000003*TMath::Pi()/180)*TMath::Cos(58.279968*TMath::Pi()/180),
                       TMath::Sin(90.000076*TMath::Pi()/180)*TMath::Cos(163.721970*TMath::Pi()/180),
                       TMath::Sin(89.999969*TMath::Pi()/180)*TMath::Cos(-106.278061*TMath::Pi()/180));

NAJB_Rot_col_X[59].SetXYZ(TMath::Sin(179.999939*TMath::Pi()/180)*TMath::Cos(58.285004*TMath::Pi()/180),
                       TMath::Sin(89.999969*TMath::Pi()/180)*TMath::Cos(16.278000*TMath::Pi()/180),
                       TMath::Sin(90.000076*TMath::Pi()/180)*TMath::Cos(-73.721939*TMath::Pi()/180));


TVector3 NAJB_Rot_col_Y[60];
NAJB_Rot_col_Y[0].SetXYZ(TMath::Sin(179.999939*TMath::Pi()/180)*TMath::Sin(-121.714996*TMath::Pi()/180),
                    TMath::Sin(89.999969*TMath::Pi()/180)*TMath::Sin(-163.721970*TMath::Pi()/180),
                    TMath::Sin(90.000076*TMath::Pi()/180)*TMath::Sin(106.278061*TMath::Pi()/180));

NAJB_Rot_col_Y[1].SetXYZ(TMath::Sin(0.000003*TMath::Pi()/180)*TMath::Sin(-121.720016*TMath::Pi()/180),
                    TMath::Sin(90.000076*TMath::Pi()/180)*TMath::Sin(-16.278015*TMath::Pi()/180),
                    TMath::Sin(89.999969*TMath::Pi()/180)*TMath::Sin(73.721939*TMath::Pi()/180));

NAJB_Rot_col_Y[2].SetXYZ(TMath::Sin(108.000031*TMath::Pi()/180)*TMath::Sin(-148.279999*TMath::Pi()/180),
                    TMath::Sin(23.548264*TMath::Pi()/180)*TMath::Sin(169.926025*TMath::Pi()/180),
                    TMath::Sin(104.668808*TMath::Pi()/180)*TMath::Sin(116.841034*TMath::Pi()/180));

NAJB_Rot_col_Y[3].SetXYZ(TMath::Sin(108.000092*TMath::Pi()/180)*TMath::Sin(31.719986*TMath::Pi()/180),
                    TMath::Sin(156.451691*TMath::Pi()/180)*TMath::Sin(169.926025*TMath::Pi()/180),
                    TMath::Sin(75.331177*TMath::Pi()/180)*TMath::Sin(116.841034*TMath::Pi()/180));

NAJB_Rot_col_Y[4].SetXYZ(TMath::Sin(71.999969*TMath::Pi()/180)*TMath::Sin(-31.720016*TMath::Pi()/180),
                    TMath::Sin(156.451691*TMath::Pi()/180)*TMath::Sin(10.073961*TMath::Pi()/180),
                    TMath::Sin(75.331177*TMath::Pi()/180)*TMath::Sin(63.158966*TMath::Pi()/180));

NAJB_Rot_col_Y[5].SetXYZ(TMath::Sin(71.999908*TMath::Pi()/180)*TMath::Sin(148.279953*TMath::Pi()/180),
                    TMath::Sin(23.548248*TMath::Pi()/180)*TMath::Sin(10.073951*TMath::Pi()/180),
                    TMath::Sin(104.668808*TMath::Pi()/180)*TMath::Sin(63.158966*TMath::Pi()/180));

NAJB_Rot_col_Y[6].SetXYZ(TMath::Sin(35.999985*TMath::Pi()/180)*TMath::Sin(-148.279999*TMath::Pi()/180),
                    TMath::Sin(55.488815*TMath::Pi()/180)*TMath::Sin(50.572174*TMath::Pi()/180),
                    TMath::Sin(99.004181*TMath::Pi()/180)*TMath::Sin(134.317352*TMath::Pi()/180));

NAJB_Rot_col_Y[7].SetXYZ(TMath::Sin(35.999985*TMath::Pi()/180)*TMath::Sin(31.719986*TMath::Pi()/180),
                    TMath::Sin(124.511108*TMath::Pi()/180)*TMath::Sin(50.572174*TMath::Pi()/180),
                    TMath::Sin(80.995865*TMath::Pi()/180)*TMath::Sin(134.317398*TMath::Pi()/180));

NAJB_Rot_col_Y[8].SetXYZ(TMath::Sin(71.998657*TMath::Pi()/180)*TMath::Sin(-148.283234*TMath::Pi()/180),
                    TMath::Sin(50.469315*TMath::Pi()/180)*TMath::Sin(-42.728638*TMath::Pi()/180),
                    TMath::Sin(45.035828*TMath::Pi()/180)*TMath::Sin(102.779205*TMath::Pi()/180));

NAJB_Rot_col_Y[9].SetXYZ(TMath::Sin(108.001343*TMath::Pi()/180)*TMath::Sin(148.283234*TMath::Pi()/180),
                    TMath::Sin(50.469315*TMath::Pi()/180)*TMath::Sin(-137.271378*TMath::Pi()/180),
                    TMath::Sin(45.035828*TMath::Pi()/180)*TMath::Sin(77.220795*TMath::Pi()/180));

NAJB_Rot_col_Y[10].SetXYZ(TMath::Sin(143.999985*TMath::Pi()/180)*TMath::Sin(-31.720016*TMath::Pi()/180),
                    TMath::Sin(124.511169*TMath::Pi()/180)*TMath::Sin(129.427750*TMath::Pi()/180),
                    TMath::Sin(80.995819*TMath::Pi()/180)*TMath::Sin(45.682571*TMath::Pi()/180));

NAJB_Rot_col_Y[11].SetXYZ(TMath::Sin(143.999985*TMath::Pi()/180)*TMath::Sin(148.279953*TMath::Pi()/180),
                    TMath::Sin(55.488846*TMath::Pi()/180)*TMath::Sin(129.427811*TMath::Pi()/180),
                    TMath::Sin(99.004181*TMath::Pi()/180)*TMath::Sin(45.682571*TMath::Pi()/180));

NAJB_Rot_col_Y[12].SetXYZ(TMath::Sin(108.001343*TMath::Pi()/180)*TMath::Sin(-31.716766*TMath::Pi()/180),
                    TMath::Sin(129.530640*TMath::Pi()/180)*TMath::Sin(-137.271317*TMath::Pi()/180),
                    TMath::Sin(134.964142*TMath::Pi()/180)*TMath::Sin(77.220795*TMath::Pi()/180));

NAJB_Rot_col_Y[13].SetXYZ(TMath::Sin(71.998657*TMath::Pi()/180)*TMath::Sin(31.716766*TMath::Pi()/180),
                    TMath::Sin(129.530640*TMath::Pi()/180)*TMath::Sin(-42.728607*TMath::Pi()/180),
                    TMath::Sin(134.964142*TMath::Pi()/180)*TMath::Sin(102.779205*TMath::Pi()/180));

NAJB_Rot_col_Y[14].SetXYZ(TMath::Sin(59.997574*TMath::Pi()/180)*TMath::Sin(-69.094238*TMath::Pi()/180),
                    TMath::Sin(85.992615*TMath::Pi()/180)*TMath::Sin(23.223938*TMath::Pi()/180),
                    TMath::Sin(30.324402*TMath::Pi()/180)*TMath::Sin(120.102737*TMath::Pi()/180));

NAJB_Rot_col_Y[15].SetXYZ(TMath::Sin(120.002426*TMath::Pi()/180)*TMath::Sin(69.094284*TMath::Pi()/180),
                    TMath::Sin(85.992615*TMath::Pi()/180)*TMath::Sin(156.776047*TMath::Pi()/180),
                    TMath::Sin(30.324402*TMath::Pi()/180)*TMath::Sin(59.897247*TMath::Pi()/180));

NAJB_Rot_col_Y[16].SetXYZ(TMath::Sin(120.002426*TMath::Pi()/180)*TMath::Sin(-110.905762*TMath::Pi()/180),
                    TMath::Sin(94.007385*TMath::Pi()/180)*TMath::Sin(156.776047*TMath::Pi()/180),
                    TMath::Sin(149.675552*TMath::Pi()/180)*TMath::Sin(59.897308*TMath::Pi()/180));

NAJB_Rot_col_Y[17].SetXYZ(TMath::Sin(59.997574*TMath::Pi()/180)*TMath::Sin(110.905701*TMath::Pi()/180),
                    TMath::Sin(94.007385*TMath::Pi()/180)*TMath::Sin(23.223938*TMath::Pi()/180),
                    TMath::Sin(149.675552*TMath::Pi()/180)*TMath::Sin(120.102692*TMath::Pi()/180));

NAJB_Rot_col_Y[18].SetXYZ(TMath::Sin(119.999146*TMath::Pi()/180)*TMath::Sin(-69.093414*TMath::Pi()/180),
                    TMath::Sin(58.443619*TMath::Pi()/180)*TMath::Sin(-138.326401*TMath::Pi()/180),
                    TMath::Sin(133.631973*TMath::Pi()/180)*TMath::Sin(167.512238*TMath::Pi()/180));

NAJB_Rot_col_Y[19].SetXYZ(TMath::Sin(143.997803*TMath::Pi()/180)*TMath::Sin(31.715622*TMath::Pi()/180),
                    TMath::Sin(113.163864*TMath::Pi()/180)*TMath::Sin(-94.359375*TMath::Pi()/180),
                    TMath::Sin(115.899887*TMath::Pi()/180)*TMath::Sin(163.649902*TMath::Pi()/180));

NAJB_Rot_col_Y[20].SetXYZ(TMath::Sin(143.997803*TMath::Pi()/180)*TMath::Sin(-148.284317*TMath::Pi()/180),
                    TMath::Sin(66.836121*TMath::Pi()/180)*TMath::Sin(-94.359436*TMath::Pi()/180),
                    TMath::Sin(64.100113*TMath::Pi()/180)*TMath::Sin(163.649902*TMath::Pi()/180));

NAJB_Rot_col_Y[21].SetXYZ(TMath::Sin(119.999146*TMath::Pi()/180)*TMath::Sin(110.906570*TMath::Pi()/180),
                    TMath::Sin(121.556427*TMath::Pi()/180)*TMath::Sin(-138.326401*TMath::Pi()/180),
                    TMath::Sin(46.368011*TMath::Pi()/180)*TMath::Sin(167.512238*TMath::Pi()/180));

NAJB_Rot_col_Y[22].SetXYZ(TMath::Sin(90.000076*TMath::Pi()/180)*TMath::Sin(-0.000001*TMath::Pi()/180),
                    TMath::Sin(106.278061*TMath::Pi()/180)*TMath::Sin(90.000076*TMath::Pi()/180),
                    TMath::Sin(16.277985*TMath::Pi()/180)*TMath::Sin(89.999969*TMath::Pi()/180));

NAJB_Rot_col_Y[23].SetXYZ(TMath::Sin(60.000854*TMath::Pi()/180)*TMath::Sin(-110.906570*TMath::Pi()/180),
                    TMath::Sin(121.556366*TMath::Pi()/180)*TMath::Sin(-41.673569*TMath::Pi()/180),
                    TMath::Sin(46.368011*TMath::Pi()/180)*TMath::Sin(12.487767*TMath::Pi()/180));

NAJB_Rot_col_Y[24].SetXYZ(TMath::Sin(36.002151*TMath::Pi()/180)*TMath::Sin(148.284378*TMath::Pi()/180),
                    TMath::Sin(66.836121*TMath::Pi()/180)*TMath::Sin(-85.640564*TMath::Pi()/180),
                    TMath::Sin(64.100113*TMath::Pi()/180)*TMath::Sin(16.350067*TMath::Pi()/180));

NAJB_Rot_col_Y[25].SetXYZ(TMath::Sin(36.002136*TMath::Pi()/180)*TMath::Sin(-31.715652*TMath::Pi()/180),
                    TMath::Sin(113.163864*TMath::Pi()/180)*TMath::Sin(-85.640564*TMath::Pi()/180),
                    TMath::Sin(115.899887*TMath::Pi()/180)*TMath::Sin(16.350067*TMath::Pi()/180));

NAJB_Rot_col_Y[26].SetXYZ(TMath::Sin(60.000854*TMath::Pi()/180)*TMath::Sin(69.093414*TMath::Pi()/180),
                    TMath::Sin(58.443573*TMath::Pi()/180)*TMath::Sin(-41.673553*TMath::Pi()/180),
                    TMath::Sin(133.631973*TMath::Pi()/180)*TMath::Sin(12.487760*TMath::Pi()/180));

NAJB_Rot_col_Y[27].SetXYZ(TMath::Sin(89.999908*TMath::Pi()/180)*TMath::Sin(180.000000*TMath::Pi()/180),
                    TMath::Sin(73.721939*TMath::Pi()/180)*TMath::Sin(89.999969*TMath::Pi()/180),
                    TMath::Sin(163.721970*TMath::Pi()/180)*TMath::Sin(90.000076*TMath::Pi()/180));

NAJB_Rot_col_Y[28].SetXYZ(TMath::Sin(89.999969*TMath::Pi()/180)*TMath::Sin(89.999969*TMath::Pi()/180),
                    TMath::Sin(163.721970*TMath::Pi()/180)*TMath::Sin(-0.000009*TMath::Pi()/180),
                    TMath::Sin(106.278061*TMath::Pi()/180)*TMath::Sin(180.000000*TMath::Pi()/180));

NAJB_Rot_col_Y[29].SetXYZ(TMath::Sin(89.999908*TMath::Pi()/180)*TMath::Sin(-89.999908*TMath::Pi()/180),
                    TMath::Sin(16.278000*TMath::Pi()/180)*TMath::Sin(0.000002*TMath::Pi()/180),
                    TMath::Sin(73.721939*TMath::Pi()/180)*TMath::Sin(-180.000000*TMath::Pi()/180));

NAJB_Rot_col_Y[30].SetXYZ(TMath::Sin(89.999969*TMath::Pi()/180)*TMath::Sin(-89.999969*TMath::Pi()/180),
                    TMath::Sin(163.721970*TMath::Pi()/180)*TMath::Sin(180.000000*TMath::Pi()/180),
                    TMath::Sin(106.278061*TMath::Pi()/180)*TMath::Sin(-0.000001*TMath::Pi()/180));

NAJB_Rot_col_Y[31].SetXYZ(TMath::Sin(90.000076*TMath::Pi()/180)*TMath::Sin(90.000076*TMath::Pi()/180),
                    TMath::Sin(16.278015*TMath::Pi()/180)*TMath::Sin(-179.999939*TMath::Pi()/180),
                    TMath::Sin(73.721939*TMath::Pi()/180)*TMath::Sin(0.000000*TMath::Pi()/180));

NAJB_Rot_col_Y[32].SetXYZ(TMath::Sin(89.999969*TMath::Pi()/180)*TMath::Sin(-180.000000*TMath::Pi()/180),
                    TMath::Sin(106.278061*TMath::Pi()/180)*TMath::Sin(-89.999969*TMath::Pi()/180),
                    TMath::Sin(16.277985*TMath::Pi()/180)*TMath::Sin(-89.999969*TMath::Pi()/180));

NAJB_Rot_col_Y[33].SetXYZ(TMath::Sin(60.000854*TMath::Pi()/180)*TMath::Sin(69.093414*TMath::Pi()/180),
                    TMath::Sin(121.556366*TMath::Pi()/180)*TMath::Sin(138.326401*TMath::Pi()/180),
                    TMath::Sin(46.368011*TMath::Pi()/180)*TMath::Sin(-167.512238*TMath::Pi()/180));

NAJB_Rot_col_Y[34].SetXYZ(TMath::Sin(36.002136*TMath::Pi()/180)*TMath::Sin(-31.715622*TMath::Pi()/180),
                    TMath::Sin(66.836060*TMath::Pi()/180)*TMath::Sin(94.359436*TMath::Pi()/180),
                    TMath::Sin(64.100113*TMath::Pi()/180)*TMath::Sin(-163.649902*TMath::Pi()/180));

NAJB_Rot_col_Y[35].SetXYZ(TMath::Sin(36.002136*TMath::Pi()/180)*TMath::Sin(148.284317*TMath::Pi()/180),
                    TMath::Sin(113.163864*TMath::Pi()/180)*TMath::Sin(94.359436*TMath::Pi()/180),
                    TMath::Sin(115.899887*TMath::Pi()/180)*TMath::Sin(-163.649902*TMath::Pi()/180));

NAJB_Rot_col_Y[36].SetXYZ(TMath::Sin(60.000854*TMath::Pi()/180)*TMath::Sin(-110.906570*TMath::Pi()/180),
                    TMath::Sin(58.443573*TMath::Pi()/180)*TMath::Sin(138.326401*TMath::Pi()/180),
                    TMath::Sin(133.631973*TMath::Pi()/180)*TMath::Sin(-167.512238*TMath::Pi()/180));

NAJB_Rot_col_Y[37].SetXYZ(TMath::Sin(89.999969*TMath::Pi()/180)*TMath::Sin(0.000001*TMath::Pi()/180),
                    TMath::Sin(73.721939*TMath::Pi()/180)*TMath::Sin(-89.999969*TMath::Pi()/180),
                    TMath::Sin(163.721970*TMath::Pi()/180)*TMath::Sin(-89.999969*TMath::Pi()/180));

NAJB_Rot_col_Y[38].SetXYZ(TMath::Sin(119.999146*TMath::Pi()/180)*TMath::Sin(110.906570*TMath::Pi()/180),
                    TMath::Sin(58.443573*TMath::Pi()/180)*TMath::Sin(41.673584*TMath::Pi()/180),
                    TMath::Sin(133.631973*TMath::Pi()/180)*TMath::Sin(-12.487763*TMath::Pi()/180));

NAJB_Rot_col_Y[39].SetXYZ(TMath::Sin(143.997864*TMath::Pi()/180)*TMath::Sin(-148.284317*TMath::Pi()/180),
                    TMath::Sin(113.163925*TMath::Pi()/180)*TMath::Sin(85.640564*TMath::Pi()/180),
                    TMath::Sin(115.899887*TMath::Pi()/180)*TMath::Sin(-16.350067*TMath::Pi()/180));

NAJB_Rot_col_Y[40].SetXYZ(TMath::Sin(143.997803*TMath::Pi()/180)*TMath::Sin(31.715637*TMath::Pi()/180),
                    TMath::Sin(66.836121*TMath::Pi()/180)*TMath::Sin(85.640610*TMath::Pi()/180),
                    TMath::Sin(64.100113*TMath::Pi()/180)*TMath::Sin(-16.350067*TMath::Pi()/180));

NAJB_Rot_col_Y[41].SetXYZ(TMath::Sin(119.999146*TMath::Pi()/180)*TMath::Sin(-69.093414*TMath::Pi()/180),
                    TMath::Sin(121.556427*TMath::Pi()/180)*TMath::Sin(41.673553*TMath::Pi()/180),
                    TMath::Sin(46.368011*TMath::Pi()/180)*TMath::Sin(-12.487760*TMath::Pi()/180));

NAJB_Rot_col_Y[42].SetXYZ(TMath::Sin(59.997574*TMath::Pi()/180)*TMath::Sin(110.905762*TMath::Pi()/180),
                    TMath::Sin(85.992615*TMath::Pi()/180)*TMath::Sin(-156.776047*TMath::Pi()/180),
                    TMath::Sin(30.324402*TMath::Pi()/180)*TMath::Sin(-59.897247*TMath::Pi()/180));

NAJB_Rot_col_Y[43].SetXYZ(TMath::Sin(120.002426*TMath::Pi()/180)*TMath::Sin(-110.905701*TMath::Pi()/180),
                    TMath::Sin(85.992615*TMath::Pi()/180)*TMath::Sin(-23.223938*TMath::Pi()/180),
                    TMath::Sin(30.324402*TMath::Pi()/180)*TMath::Sin(-120.102737*TMath::Pi()/180));

NAJB_Rot_col_Y[44].SetXYZ(TMath::Sin(120.002426*TMath::Pi()/180)*TMath::Sin(69.094238*TMath::Pi()/180),
                    TMath::Sin(94.007385*TMath::Pi()/180)*TMath::Sin(-23.223938*TMath::Pi()/180),
                    TMath::Sin(149.675552*TMath::Pi()/180)*TMath::Sin(-120.102692*TMath::Pi()/180));

NAJB_Rot_col_Y[45].SetXYZ(TMath::Sin(59.997574*TMath::Pi()/180)*TMath::Sin(-69.094284*TMath::Pi()/180),
                    TMath::Sin(94.007385*TMath::Pi()/180)*TMath::Sin(-156.776047*TMath::Pi()/180),
                    TMath::Sin(149.675552*TMath::Pi()/180)*TMath::Sin(-59.897308*TMath::Pi()/180));

NAJB_Rot_col_Y[46].SetXYZ(TMath::Sin(71.998657*TMath::Pi()/180)*TMath::Sin(31.716766*TMath::Pi()/180),
                    TMath::Sin(50.469315*TMath::Pi()/180)*TMath::Sin(137.271317*TMath::Pi()/180),
                    TMath::Sin(45.035828*TMath::Pi()/180)*TMath::Sin(-77.220795*TMath::Pi()/180));

NAJB_Rot_col_Y[47].SetXYZ(TMath::Sin(108.001343*TMath::Pi()/180)*TMath::Sin(-31.716766*TMath::Pi()/180),
                    TMath::Sin(50.469315*TMath::Pi()/180)*TMath::Sin(42.728607*TMath::Pi()/180),
                    TMath::Sin(45.035828*TMath::Pi()/180)*TMath::Sin(-102.779205*TMath::Pi()/180));

NAJB_Rot_col_Y[48].SetXYZ(TMath::Sin(143.999985*TMath::Pi()/180)*TMath::Sin(148.279953*TMath::Pi()/180),
                    TMath::Sin(124.511169*TMath::Pi()/180)*TMath::Sin(-50.572174*TMath::Pi()/180),
                    TMath::Sin(80.995819*TMath::Pi()/180)*TMath::Sin(-134.317398*TMath::Pi()/180));

NAJB_Rot_col_Y[49].SetXYZ(TMath::Sin(143.999985*TMath::Pi()/180)*TMath::Sin(-31.719986*TMath::Pi()/180),
                    TMath::Sin(55.488846*TMath::Pi()/180)*TMath::Sin(-50.572174*TMath::Pi()/180),
                    TMath::Sin(99.004181*TMath::Pi()/180)*TMath::Sin(-134.317398*TMath::Pi()/180));

NAJB_Rot_col_Y[50].SetXYZ(TMath::Sin(108.001343*TMath::Pi()/180)*TMath::Sin(148.283234*TMath::Pi()/180),
                    TMath::Sin(129.530640*TMath::Pi()/180)*TMath::Sin(42.728638*TMath::Pi()/180),
                    TMath::Sin(134.964142*TMath::Pi()/180)*TMath::Sin(-102.779205*TMath::Pi()/180));

NAJB_Rot_col_Y[51].SetXYZ(TMath::Sin(71.998657*TMath::Pi()/180)*TMath::Sin(-148.283234*TMath::Pi()/180),
                    TMath::Sin(129.530640*TMath::Pi()/180)*TMath::Sin(137.271378*TMath::Pi()/180),
                    TMath::Sin(134.964142*TMath::Pi()/180)*TMath::Sin(-77.220795*TMath::Pi()/180));

NAJB_Rot_col_Y[52].SetXYZ(TMath::Sin(35.999969*TMath::Pi()/180)*TMath::Sin(31.720001*TMath::Pi()/180),
                    TMath::Sin(55.488815*TMath::Pi()/180)*TMath::Sin(-129.427811*TMath::Pi()/180),
                    TMath::Sin(99.004181*TMath::Pi()/180)*TMath::Sin(-45.682571*TMath::Pi()/180));

NAJB_Rot_col_Y[53].SetXYZ(TMath::Sin(35.999985*TMath::Pi()/180)*TMath::Sin(-148.279953*TMath::Pi()/180),
                    TMath::Sin(124.511108*TMath::Pi()/180)*TMath::Sin(-129.427811*TMath::Pi()/180),
                    TMath::Sin(80.995865*TMath::Pi()/180)*TMath::Sin(-45.682571*TMath::Pi()/180));

NAJB_Rot_col_Y[54].SetXYZ(TMath::Sin(71.999969*TMath::Pi()/180)*TMath::Sin(148.279953*TMath::Pi()/180),
                    TMath::Sin(156.451691*TMath::Pi()/180)*TMath::Sin(-169.926025*TMath::Pi()/180),
                    TMath::Sin(75.331177*TMath::Pi()/180)*TMath::Sin(-116.841034*TMath::Pi()/180));

NAJB_Rot_col_Y[55].SetXYZ(TMath::Sin(71.999908*TMath::Pi()/180)*TMath::Sin(-31.719986*TMath::Pi()/180),
                    TMath::Sin(23.548248*TMath::Pi()/180)*TMath::Sin(-169.926025*TMath::Pi()/180),
                    TMath::Sin(104.668808*TMath::Pi()/180)*TMath::Sin(-116.841034*TMath::Pi()/180));

NAJB_Rot_col_Y[56].SetXYZ(TMath::Sin(108.000031*TMath::Pi()/180)*TMath::Sin(31.720001*TMath::Pi()/180),
                    TMath::Sin(23.548264*TMath::Pi()/180)*TMath::Sin(-10.073964*TMath::Pi()/180),
                    TMath::Sin(104.668808*TMath::Pi()/180)*TMath::Sin(-63.158966*TMath::Pi()/180));

NAJB_Rot_col_Y[57].SetXYZ(TMath::Sin(108.000092*TMath::Pi()/180)*TMath::Sin(-148.279999*TMath::Pi()/180),
                    TMath::Sin(156.451691*TMath::Pi()/180)*TMath::Sin(-10.073964*TMath::Pi()/180),
                    TMath::Sin(75.331177*TMath::Pi()/180)*TMath::Sin(-63.158966*TMath::Pi()/180));

NAJB_Rot_col_Y[58].SetXYZ(TMath::Sin(0.000003*TMath::Pi()/180)*TMath::Sin(58.279968*TMath::Pi()/180),
                    TMath::Sin(90.000076*TMath::Pi()/180)*TMath::Sin(163.721970*TMath::Pi()/180),
                    TMath::Sin(89.999969*TMath::Pi()/180)*TMath::Sin(-106.278061*TMath::Pi()/180));

NAJB_Rot_col_Y[59].SetXYZ(TMath::Sin(179.999939*TMath::Pi()/180)*TMath::Sin(58.285004*TMath::Pi()/180),
                    TMath::Sin(89.999969*TMath::Pi()/180)*TMath::Sin(16.278000*TMath::Pi()/180),
                    TMath::Sin(90.000076*TMath::Pi()/180)*TMath::Sin(-73.721939*TMath::Pi()/180));


TVector3 NAJB_Rot_col_Z[60];
NAJB_Rot_col_Z[0].SetXYZ(TMath::Cos(179.999939*TMath::Pi()/180),
                 TMath::Cos(89.999969*TMath::Pi()/180),
                 TMath::Cos(90.000076*TMath::Pi()/180));
NAJB_Rot_col_Z[1].SetXYZ(TMath::Cos(0.000003*TMath::Pi()/180),
                 TMath::Cos(90.000076*TMath::Pi()/180),
                 TMath::Cos(89.999969*TMath::Pi()/180));
NAJB_Rot_col_Z[2].SetXYZ(TMath::Cos(108.000031*TMath::Pi()/180),
                 TMath::Cos(23.548264*TMath::Pi()/180),
                 TMath::Cos(104.668808*TMath::Pi()/180));
NAJB_Rot_col_Z[3].SetXYZ(TMath::Cos(108.000092*TMath::Pi()/180),
                 TMath::Cos(156.451691*TMath::Pi()/180),
                 TMath::Cos(75.331177*TMath::Pi()/180));
NAJB_Rot_col_Z[4].SetXYZ(TMath::Cos(71.999969*TMath::Pi()/180),
                 TMath::Cos(156.451691*TMath::Pi()/180),
                 TMath::Cos(75.331177*TMath::Pi()/180));
NAJB_Rot_col_Z[5].SetXYZ(TMath::Cos(71.999908*TMath::Pi()/180),
                 TMath::Cos(23.548248*TMath::Pi()/180),
                 TMath::Cos(104.668808*TMath::Pi()/180));
NAJB_Rot_col_Z[6].SetXYZ(TMath::Cos(35.999985*TMath::Pi()/180),
                 TMath::Cos(55.488815*TMath::Pi()/180),
                 TMath::Cos(99.004181*TMath::Pi()/180));
NAJB_Rot_col_Z[7].SetXYZ(TMath::Cos(35.999985*TMath::Pi()/180),
                 TMath::Cos(124.511108*TMath::Pi()/180),
                 TMath::Cos(80.995865*TMath::Pi()/180));
NAJB_Rot_col_Z[8].SetXYZ(TMath::Cos(71.998657*TMath::Pi()/180),
                 TMath::Cos(50.469315*TMath::Pi()/180),
                 TMath::Cos(45.035828*TMath::Pi()/180));
NAJB_Rot_col_Z[9].SetXYZ(TMath::Cos(108.001343*TMath::Pi()/180),
                 TMath::Cos(50.469315*TMath::Pi()/180),
                 TMath::Cos(45.035828*TMath::Pi()/180));
NAJB_Rot_col_Z[10].SetXYZ(TMath::Cos(143.999985*TMath::Pi()/180),
                 TMath::Cos(124.511169*TMath::Pi()/180),
                 TMath::Cos(80.995819*TMath::Pi()/180));
NAJB_Rot_col_Z[11].SetXYZ(TMath::Cos(143.999985*TMath::Pi()/180),
                 TMath::Cos(55.488846*TMath::Pi()/180),
                 TMath::Cos(99.004181*TMath::Pi()/180));
NAJB_Rot_col_Z[12].SetXYZ(TMath::Cos(108.001343*TMath::Pi()/180),
                 TMath::Cos(129.530640*TMath::Pi()/180),
                 TMath::Cos(134.964142*TMath::Pi()/180));
NAJB_Rot_col_Z[13].SetXYZ(TMath::Cos(71.998657*TMath::Pi()/180),
                 TMath::Cos(129.530640*TMath::Pi()/180),
                 TMath::Cos(134.964142*TMath::Pi()/180));
NAJB_Rot_col_Z[14].SetXYZ(TMath::Cos(59.997574*TMath::Pi()/180),
                 TMath::Cos(85.992615*TMath::Pi()/180),
                 TMath::Cos(30.324402*TMath::Pi()/180));
NAJB_Rot_col_Z[15].SetXYZ(TMath::Cos(120.002426*TMath::Pi()/180),
                 TMath::Cos(85.992615*TMath::Pi()/180),
                 TMath::Cos(30.324402*TMath::Pi()/180));
NAJB_Rot_col_Z[16].SetXYZ(TMath::Cos(120.002426*TMath::Pi()/180),
                 TMath::Cos(94.007385*TMath::Pi()/180),
                 TMath::Cos(149.675552*TMath::Pi()/180));
NAJB_Rot_col_Z[17].SetXYZ(TMath::Cos(59.997574*TMath::Pi()/180),
                 TMath::Cos(94.007385*TMath::Pi()/180),
                 TMath::Cos(149.675552*TMath::Pi()/180));
NAJB_Rot_col_Z[18].SetXYZ(TMath::Cos(119.999146*TMath::Pi()/180),
                 TMath::Cos(58.443619*TMath::Pi()/180),
                 TMath::Cos(133.631973*TMath::Pi()/180));
NAJB_Rot_col_Z[19].SetXYZ(TMath::Cos(143.997803*TMath::Pi()/180),
                 TMath::Cos(113.163864*TMath::Pi()/180),
                 TMath::Cos(115.899887*TMath::Pi()/180));
NAJB_Rot_col_Z[20].SetXYZ(TMath::Cos(143.997803*TMath::Pi()/180),
                 TMath::Cos(66.836121*TMath::Pi()/180),
                 TMath::Cos(64.100113*TMath::Pi()/180));
NAJB_Rot_col_Z[21].SetXYZ(TMath::Cos(119.999146*TMath::Pi()/180),
                 TMath::Cos(121.556427*TMath::Pi()/180),
                 TMath::Cos(46.368011*TMath::Pi()/180));
NAJB_Rot_col_Z[22].SetXYZ(TMath::Cos(90.000076*TMath::Pi()/180),
                 TMath::Cos(106.278061*TMath::Pi()/180),
                 TMath::Cos(16.277985*TMath::Pi()/180));
NAJB_Rot_col_Z[23].SetXYZ(TMath::Cos(60.000854*TMath::Pi()/180),
                 TMath::Cos(121.556366*TMath::Pi()/180),
                 TMath::Cos(46.368011*TMath::Pi()/180));
NAJB_Rot_col_Z[24].SetXYZ(TMath::Cos(36.002151*TMath::Pi()/180),
                 TMath::Cos(66.836121*TMath::Pi()/180),
                 TMath::Cos(64.100113*TMath::Pi()/180));
NAJB_Rot_col_Z[25].SetXYZ(TMath::Cos(36.002136*TMath::Pi()/180),
                 TMath::Cos(113.163864*TMath::Pi()/180),
                 TMath::Cos(115.899887*TMath::Pi()/180));
NAJB_Rot_col_Z[26].SetXYZ(TMath::Cos(60.000854*TMath::Pi()/180),
                 TMath::Cos(58.443573*TMath::Pi()/180),
                 TMath::Cos(133.631973*TMath::Pi()/180));
NAJB_Rot_col_Z[27].SetXYZ(TMath::Cos(89.999908*TMath::Pi()/180),
                 TMath::Cos(73.721939*TMath::Pi()/180),
                 TMath::Cos(163.721970*TMath::Pi()/180));
NAJB_Rot_col_Z[28].SetXYZ(TMath::Cos(89.999969*TMath::Pi()/180),
                 TMath::Cos(163.721970*TMath::Pi()/180),
                 TMath::Cos(106.278061*TMath::Pi()/180));
NAJB_Rot_col_Z[29].SetXYZ(TMath::Cos(89.999908*TMath::Pi()/180),
                 TMath::Cos(16.278000*TMath::Pi()/180),
                 TMath::Cos(73.721939*TMath::Pi()/180));
NAJB_Rot_col_Z[30].SetXYZ(TMath::Cos(89.999969*TMath::Pi()/180),
                 TMath::Cos(163.721970*TMath::Pi()/180),
                 TMath::Cos(106.278061*TMath::Pi()/180));
NAJB_Rot_col_Z[31].SetXYZ(TMath::Cos(90.000076*TMath::Pi()/180),
                 TMath::Cos(16.278015*TMath::Pi()/180),
                 TMath::Cos(73.721939*TMath::Pi()/180));
NAJB_Rot_col_Z[32].SetXYZ(TMath::Cos(89.999969*TMath::Pi()/180),
                 TMath::Cos(106.278061*TMath::Pi()/180),
                 TMath::Cos(16.277985*TMath::Pi()/180));
NAJB_Rot_col_Z[33].SetXYZ(TMath::Cos(60.000854*TMath::Pi()/180),
                 TMath::Cos(121.556366*TMath::Pi()/180),
                 TMath::Cos(46.368011*TMath::Pi()/180));
NAJB_Rot_col_Z[34].SetXYZ(TMath::Cos(36.002136*TMath::Pi()/180),
                 TMath::Cos(66.836060*TMath::Pi()/180),
                 TMath::Cos(64.100113*TMath::Pi()/180));
NAJB_Rot_col_Z[35].SetXYZ(TMath::Cos(36.002136*TMath::Pi()/180),
                 TMath::Cos(113.163864*TMath::Pi()/180),
                 TMath::Cos(115.899887*TMath::Pi()/180));
NAJB_Rot_col_Z[36].SetXYZ(TMath::Cos(60.000854*TMath::Pi()/180),
                 TMath::Cos(58.443573*TMath::Pi()/180),
                 TMath::Cos(133.631973*TMath::Pi()/180));
NAJB_Rot_col_Z[37].SetXYZ(TMath::Cos(89.999969*TMath::Pi()/180),
                 TMath::Cos(73.721939*TMath::Pi()/180),
                 TMath::Cos(163.721970*TMath::Pi()/180));
NAJB_Rot_col_Z[38].SetXYZ(TMath::Cos(119.999146*TMath::Pi()/180),
                 TMath::Cos(58.443573*TMath::Pi()/180),
                 TMath::Cos(133.631973*TMath::Pi()/180));
NAJB_Rot_col_Z[39].SetXYZ(TMath::Cos(143.997864*TMath::Pi()/180),
                 TMath::Cos(113.163925*TMath::Pi()/180),
                 TMath::Cos(115.899887*TMath::Pi()/180));
NAJB_Rot_col_Z[40].SetXYZ(TMath::Cos(143.997803*TMath::Pi()/180),
                 TMath::Cos(66.836121*TMath::Pi()/180),
                 TMath::Cos(64.100113*TMath::Pi()/180));
NAJB_Rot_col_Z[41].SetXYZ(TMath::Cos(119.999146*TMath::Pi()/180),
                 TMath::Cos(121.556427*TMath::Pi()/180),
                 TMath::Cos(46.368011*TMath::Pi()/180));
NAJB_Rot_col_Z[42].SetXYZ(TMath::Cos(59.997574*TMath::Pi()/180),
                 TMath::Cos(85.992615*TMath::Pi()/180),
                 TMath::Cos(30.324402*TMath::Pi()/180));
NAJB_Rot_col_Z[43].SetXYZ(TMath::Cos(120.002426*TMath::Pi()/180),
                 TMath::Cos(85.992615*TMath::Pi()/180),
                 TMath::Cos(30.324402*TMath::Pi()/180));
NAJB_Rot_col_Z[44].SetXYZ(TMath::Cos(120.002426*TMath::Pi()/180),
                 TMath::Cos(94.007385*TMath::Pi()/180),
                 TMath::Cos(149.675552*TMath::Pi()/180));
NAJB_Rot_col_Z[45].SetXYZ(TMath::Cos(59.997574*TMath::Pi()/180),
                 TMath::Cos(94.007385*TMath::Pi()/180),
                 TMath::Cos(149.675552*TMath::Pi()/180));
NAJB_Rot_col_Z[46].SetXYZ(TMath::Cos(71.998657*TMath::Pi()/180),
                 TMath::Cos(50.469315*TMath::Pi()/180),
                 TMath::Cos(45.035828*TMath::Pi()/180));
NAJB_Rot_col_Z[47].SetXYZ(TMath::Cos(108.001343*TMath::Pi()/180),
                 TMath::Cos(50.469315*TMath::Pi()/180),
                 TMath::Cos(45.035828*TMath::Pi()/180));
NAJB_Rot_col_Z[48].SetXYZ(TMath::Cos(143.999985*TMath::Pi()/180),
                 TMath::Cos(124.511169*TMath::Pi()/180),
                 TMath::Cos(80.995819*TMath::Pi()/180));
NAJB_Rot_col_Z[49].SetXYZ(TMath::Cos(143.999985*TMath::Pi()/180),
                 TMath::Cos(55.488846*TMath::Pi()/180),
                 TMath::Cos(99.004181*TMath::Pi()/180));
NAJB_Rot_col_Z[50].SetXYZ(TMath::Cos(108.001343*TMath::Pi()/180),
                 TMath::Cos(129.530640*TMath::Pi()/180),
                 TMath::Cos(134.964142*TMath::Pi()/180));
NAJB_Rot_col_Z[51].SetXYZ(TMath::Cos(71.998657*TMath::Pi()/180),
                 TMath::Cos(129.530640*TMath::Pi()/180),
                 TMath::Cos(134.964142*TMath::Pi()/180));
NAJB_Rot_col_Z[52].SetXYZ(TMath::Cos(35.999969*TMath::Pi()/180),
                 TMath::Cos(55.488815*TMath::Pi()/180),
                 TMath::Cos(99.004181*TMath::Pi()/180));
NAJB_Rot_col_Z[53].SetXYZ(TMath::Cos(35.999985*TMath::Pi()/180),
                 TMath::Cos(124.511108*TMath::Pi()/180),
                 TMath::Cos(80.995865*TMath::Pi()/180));
NAJB_Rot_col_Z[54].SetXYZ(TMath::Cos(71.999969*TMath::Pi()/180),
                 TMath::Cos(156.451691*TMath::Pi()/180),
                 TMath::Cos(75.331177*TMath::Pi()/180));
NAJB_Rot_col_Z[55].SetXYZ(TMath::Cos(71.999908*TMath::Pi()/180),
                 TMath::Cos(23.548248*TMath::Pi()/180),
                 TMath::Cos(104.668808*TMath::Pi()/180));
NAJB_Rot_col_Z[56].SetXYZ(TMath::Cos(108.000031*TMath::Pi()/180),
                 TMath::Cos(23.548264*TMath::Pi()/180),
                 TMath::Cos(104.668808*TMath::Pi()/180));
NAJB_Rot_col_Z[57].SetXYZ(TMath::Cos(108.000092*TMath::Pi()/180),
                 TMath::Cos(156.451691*TMath::Pi()/180),
                 TMath::Cos(75.331177*TMath::Pi()/180));
NAJB_Rot_col_Z[58].SetXYZ(TMath::Cos(0.000003*TMath::Pi()/180),
                 TMath::Cos(90.000076*TMath::Pi()/180),
                 TMath::Cos(89.999969*TMath::Pi()/180));
NAJB_Rot_col_Z[59].SetXYZ(TMath::Cos(179.999939*TMath::Pi()/180),
                 TMath::Cos(89.999969*TMath::Pi()/180),
                 TMath::Cos(90.000076*TMath::Pi()/180));


// m1_TRB1/2
TVector3 col_X_m1_TRB1(TMath::Sin(90.*TMath::Pi()/180)*TMath::Cos(0.*TMath::Pi()/180),
                       TMath::Sin(93.525*TMath::Pi()/180)*TMath::Cos(90.*TMath::Pi()/180),
                       TMath::Sin(3.525*TMath::Pi()/180)*TMath::Cos(90.*TMath::Pi()/180));
TVector3 col_Y_m1_TRB1(TMath::Sin(90.*TMath::Pi()/180)*TMath::Sin(0.*TMath::Pi()/180),
                       TMath::Sin(93.525*TMath::Pi()/180)*TMath::Sin(90.*TMath::Pi()/180),
                       TMath::Sin(3.525*TMath::Pi()/180)*TMath::Sin(90.*TMath::Pi()/180));
TVector3 col_Z_m1_TRB1(TMath::Cos(90.*TMath::Pi()/180),
                       TMath::Cos(93.525*TMath::Pi()/180),
                       TMath::Cos(3.525*TMath::Pi()/180));
TVector3 col_X_m2_TRB2(TMath::Sin(90.*TMath::Pi()/180)*TMath::Cos(180.*TMath::Pi()/180),
                       TMath::Sin(94.25*TMath::Pi()/180)*TMath::Cos(270.*TMath::Pi()/180),
                       TMath::Sin(4.25*TMath::Pi()/180)*TMath::Cos(270.*TMath::Pi()/180));
TVector3 col_Y_m2_TRB2(TMath::Sin(90.*TMath::Pi()/180)*TMath::Sin(180.*TMath::Pi()/180),
                       TMath::Sin(94.25*TMath::Pi()/180)*TMath::Sin(270.*TMath::Pi()/180),
                       TMath::Sin(4.25*TMath::Pi()/180)*TMath::Sin(270.*TMath::Pi()/180));
TVector3 col_Z_m2_TRB2(TMath::Cos(90.*TMath::Pi()/180),
                       TMath::Cos(94.25*TMath::Pi()/180),
                       TMath::Cos(4.25*TMath::Pi()/180));



   //--- Additional Ring  rotation 
    TGeoRotation *m1_TRB1 = new TGeoRotation();     
    Double_t fRotable0[9] = {
	    col_X_m1_TRB1.X(),col_X_m1_TRB1.Y(),col_X_m1_TRB1.Z(),
	    col_Y_m1_TRB1.X(),col_Y_m1_TRB1.Y(),col_Y_m1_TRB1.Z(),
	    col_Z_m1_TRB1.X(),col_Z_m1_TRB1.Y(),col_Z_m1_TRB1.Z(),
	 };
    m1_TRB1->SetMatrix(fRotable0);

   //--- First part defined 
   TGeoRotation *pIndividualRot=NULL;
   TGeoRotation *pRingRot = NULL;

   ROOT::Math::XYZPoint tra1_TRB1(0.,21.56/10.,0.);
      
      for(Int_t iter2=0 ; iter2<2 ; iter2++) {

	 Double_t fRotable1[9] = {
	    NAJB_Rot_col_X[iter2].X(),NAJB_Rot_col_X[iter2].Y(),NAJB_Rot_col_X[iter2].Z(),
	    NAJB_Rot_col_Y[iter2].X(),NAJB_Rot_col_Y[iter2].Y(),NAJB_Rot_col_Y[iter2].Z(),
	    NAJB_Rot_col_Z[iter2].X(),NAJB_Rot_col_Z[iter2].Y(),NAJB_Rot_col_Z[iter2].Z()
	 };
	 
	 pIndividualRot = new TGeoRotation();
	 pIndividualRot->SetMatrix(fRotable1);


	 ROOT::Math::Rotation3D *pIndRot = new ROOT::Math::Rotation3D(
					    fRotable1[0],fRotable1[1],fRotable1[2],
					    fRotable1[3],fRotable1[4],fRotable1[5],
					    fRotable1[6],fRotable1[7],fRotable1[8]
					    );
	 ROOT::Math::Rotation3D pIndRotInv = pIndRot->Inverse();
	 //G4ThreeVector(NAJB_X[iter1],NAJB_Y[iter1],NAJB_Z[iter1])
	 // + ((pIndividualRingRot->inverse())*tra1_TRB1)
	 dx =  NAJB_X[iter2]/10.;
	 dy =  NAJB_Y[iter2]/10.;
	 dz =  NAJB_Z[iter2]/10.;
	 ROOT::Math::XYZPoint tt(dx,dy,dz);
	 ROOT::Math::XYZPoint ttt(0.0,0.0,0.0);
         ttt = pIndRotInv * tt;  

	 // pRingRot = m1_TRB1 * pIndividualRot * pGlobalRot(=Unity)
         pRingRot = new TGeoRotation(*m1_TRB1);
         pRingRot->MultiplyBy(pIndividualRot);  


       	 TGeoTranslation *trans = new TGeoTranslation(ttt.X(),ttt.Y(),ttt.Z());
	 TGeoCombiTrans *combi = new TGeoCombiTrans(*trans,*pRingRot);
	 
	 pCBLogWorld->AddNode(pcrystalLogNAJA, iter2, combi); 
	 
   }//! for iter2  



/*
 for(G4int iter1=0 ; iter1<60 ; iter1++) {
     pIndividualRingRot = 
         new G4RotationMatrix(NAJB_Rot_col_X[iter1],NAJB_Rot_col_Y[iter1],NAJB_Rot_col_Z[iter1]);
     pRingRot =  
         new G4RotationMatrix(((*m1_TRB1)*(*pIndividualRingRot)*(*pGlobalRot)));
	 
	 crystalPhysNAJB = 
    new G4PVPlacement(pRingRot,G4ThreeVector(NAJB_X[iter1],NAJB_Y[iter1],NAJB_Z[iter1])
    + ((pIndividualRingRot->inverse())*tra1_TRB1),
    crystalLogNAJB,"crystalPhysNAJB",
    worldLog,false,iter1);
				  
}
*/


      
}   



// -----   Public method ConstructGeometry   ----------------------------------
void R3BCal::ConstructGeometry1() {

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


  
 // Mixture: CsI
  nel     = 2;
  density = 4.510000;
  TGeoMixture*
      pMat9 = new TGeoMixture("CsI", nel,density);
  a = 132.905450;   z = 55.000000;   w = 0.511549;  // CS
  pMat9->DefineElement(0,a,z,w);
  a = 126.904470;   z = 53.000000;   w = 0.488451;  // I
  pMat9->DefineElement(1,a,z,w);
  pMat9->SetIndex(601);

  TGeoMedium* pMed9 = new TGeoMedium("CsI", 2,pMat9);

 // Material: Aluminum
  a       = 26.980000;
  z       = 13.000000;
  density = 2.700000;
  radl    = 8.875105;
  absl    = 388.793113;

  TGeoMaterial *matAl = new TGeoMaterial("Aluminum", a,z,density,radl,absl);
  TGeoMedium* pMed21 = new TGeoMedium("Aluminum",3, matAl);
  pMed21->Print();

/****************************************************************************/

   // Transformation definition
   TGeoCombiTrans *pMatrix[337];

   // Combi transformation:
   dx = 0.000000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 90.000000;
   thy = 90.000000;    phy = 180.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix11 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix10 = new TGeoCombiTrans("", dx,dy,dz,pMatrix11);
   // Combi transformation: 
   dx = -18.401950;
   dy = 29.772050;
   dz = 0.000000;
   // Rotation: 
   thx = 18.000000;    phx = 211.720000;
   thy = 72.000000;    phy = 31.720000;
   thz = 90.000000;    phz = 121.720000;
   TGeoRotation *pMatrix13 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix12 = new TGeoCombiTrans("", dx,dy,dz,pMatrix13);
   // Combi transformation: 
   dx = 18.401950;
   dy = 29.772050;
   dz = 0.000000;
   // Rotation: 
   thx = 18.000000;    phx = 328.280000;
   thy = 108.000000;    phy = 328.280000;
   thz = 90.000000;    phz = 58.280000;
   TGeoRotation *pMatrix15 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix14 = new TGeoCombiTrans("", dx,dy,dz,pMatrix15);
   // Combi transformation: 
   dx = 0.000000;
   dy = 18.401950;
   dz = 29.772050;
   // Rotation: 
   thx = 99.350085;    phx = 15.449959;
   thy = 120.000298;    phy = 110.904973;
   thz = 31.717900;    phz = 89.999127;
   TGeoRotation *pMatrix17 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix16 = new TGeoCombiTrans("", dx,dy,dz,pMatrix17);
   // Combi transformation: 
   dx = 0.000000;
   dy = 18.401950;
   dz = -29.772050;
   // Rotation: 
   thx = 80.649915;    phx = 15.449959;
   thy = 120.000298;    phy = 290.904973;
   thz = 148.282100;    phz = 89.999127;
   TGeoRotation *pMatrix19 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix18 = new TGeoCombiTrans("", dx,dy,dz,pMatrix19);
   // Combi transformation: 
   dx = -29.772050;
   dy = 0.000000;
   dz = -18.401950;
   // Rotation: 
   thx = 105.240107;    phx = 80.309893;
   thy = 144.000202;    phy = 328.286058;
   thz = 121.717291;    phz = 180.003510;
   TGeoRotation *pMatrix21 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix20 = new TGeoCombiTrans("", dx,dy,dz,pMatrix21);
   // Combi transformation: 
   dx = -29.772050;
   dy = 0.000000;
   dz = 18.401950;
   // Rotation: 
   thx = 74.760031;    phx = 80.310033;
   thy = 143.999902;    phy = 148.286675;
   thz = 58.282311;    phz = 180.003708;
   TGeoRotation *pMatrix23 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix22 = new TGeoCombiTrans("", dx,dy,dz,pMatrix23);
   // Combi transformation: 
   dx = 29.772050;
   dy = 0.000000;
   dz = -18.401950;
   // Rotation: 
   thx = 105.240130;    phx = 99.690071;
   thy = 35.999684;    phy = 31.714039;
   thz = 121.717157;    phz = 359.996490;
   TGeoRotation *pMatrix25 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix24 = new TGeoCombiTrans("", dx,dy,dz,pMatrix25);
   // Combi transformation: 
   dx = 29.772050;
   dy = 0.000000;
   dz = 18.401950;
   // Rotation: 
   thx = 74.759998;    phx = 99.690003;
   thy = 35.999990;    phy = 211.713507;
   thz = 58.282445;    phz = 359.996357;
   TGeoRotation *pMatrix27 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix26 = new TGeoCombiTrans("", dx,dy,dz,pMatrix27);
   // Combi transformation: 
   dx = 0.000000;
   dy = -18.401950;
   dz = 29.772050;
   // Rotation: 
   thx = 99.350008;    phx = 344.549995;
   thy = 59.999972;    phy = 69.095087;
   thz = 31.717611;    phz = 270.000873;
   TGeoRotation *pMatrix29 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix28 = new TGeoCombiTrans("", dx,dy,dz,pMatrix29);
   // Combi transformation: 
   dx = 0.000000;
   dy = -18.401950;
   dz = -29.772050;
   // Rotation: 
   thx = 80.649992;    phx = 344.549995;
   thy = 59.999972;    phy = 249.095087;
   thz = 148.282389;    phz = 270.000873;
   TGeoRotation *pMatrix31 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix30 = new TGeoCombiTrans("", dx,dy,dz,pMatrix31);
   // Combi transformation: 
   dx = -18.401950;
   dy = -29.772050;
   dz = 0.000000;
   // Rotation: 
   thx = 162.000000;    phx = 148.280000;
   thy = 108.000000;    phy = 328.280000;
   thz = 90.000000;    phz = 238.280000;
   TGeoRotation *pMatrix33 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix32 = new TGeoCombiTrans("", dx,dy,dz,pMatrix33);
   // Combi transformation: 
   dx = 18.401950;
   dy = -29.772050;
   dz = 0.000000;
   // Rotation: 
   thx = 18.000000;    phx = 31.720000;
   thy = 72.000000;    phy = 211.720000;
   thz = 90.000000;    phz = 301.720000;
   TGeoRotation *pMatrix35 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix34 = new TGeoCombiTrans("", dx,dy,dz,pMatrix35);
   // Combi transformation: 
   dx = -11.387622;
   dy = 33.132525;
   dz = 0.000001;
   // Rotation: 
   thx = 179.999918;    phx = 0.000000;
   thy = 90.000077;    phy = 199.803059;
   thz = 90.000028;    phz = 109.803059;
   TGeoRotation *pMatrix37 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix36 = new TGeoCombiTrans("", dx,dy,dz,pMatrix37);
   // Combi transformation: 
   dx = 11.387622;
   dy = 33.132525;
   dz = 0.000000;
   // Rotation: 
   thx = 0.000003;    phx = 0.000000;
   thy = 90.000003;    phy = 340.196943;
   thz = 90.000001;    phz = 70.196943;
   TGeoRotation *pMatrix39 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix38 = new TGeoCombiTrans("", dx,dy,dz,pMatrix39);
   // Combi transformation: 
   dx = -16.136089;
   dy = 30.361960;
   dz = -6.886594;
   // Rotation: 
   thx = 108.000028;    phx = 211.720015;
   thy = 21.477893;    phy = 177.388913;
   thz = 101.325831;    phz = 117.988694;
   TGeoRotation *pMatrix41 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix40 = new TGeoCombiTrans("", dx,dy,dz,pMatrix41);
   // Combi transformation: 
   dx = -16.136092;
   dy = 30.361960;
   dz = 6.886595;
   // Rotation: 
   thx = 108.000079;    phx = 31.720006;
   thy = 158.522046;    phy = 177.388899;
   thz = 78.674140;    phz = 117.988664;
   TGeoRotation *pMatrix43 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix42 = new TGeoCombiTrans("", dx,dy,dz,pMatrix43);
   // Combi transformation: 
   dx = 16.136090;
   dy = 30.361960;
   dz = 6.886594;
   // Rotation: 
   thx = 71.999964;    phx = 328.279971;
   thy = 158.522093;    phy = 2.611090;
   thz = 78.674157;    phz = 62.011298;
   TGeoRotation *pMatrix45 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix44 = new TGeoCombiTrans("", dx,dy,dz,pMatrix45);
   // Combi transformation: 
   dx = 16.136088;
   dy = 30.361959;
   dz = -6.886593;
   // Rotation: 
   thx = 72.000006;    phx = 148.279948;
   thy = 21.477852;    phy = 2.611041;
   thz = 101.325810;    phz = 62.011255;
   TGeoRotation *pMatrix47 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix46 = new TGeoCombiTrans("", dx,dy,dz,pMatrix47);
   // Combi transformation: 
   dx = -23.021675;
   dy = 26.105014;
   dz = -4.261582;
   // Rotation: 
   thx = 36.000004;    phx = 211.720004;
   thy = 54.892154;    phy = 46.340510;
   thz = 96.971413;    phz = 131.409077;
   TGeoRotation *pMatrix49 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix48 = new TGeoCombiTrans("", dx,dy,dz,pMatrix49);
   // Combi transformation: 
   dx = -23.021675;
   dy = 26.105015;
   dz = 4.261583;
   // Rotation: 
   thx = 35.999973;    phx = 31.719990;
   thy = 125.107812;    phy = 46.340523;
   thz = 83.028577;    phz = 131.409088;
   TGeoRotation *pMatrix51 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix50 = new TGeoCombiTrans("", dx,dy,dz,pMatrix51);
   // Combi transformation: 
   dx = -4.253533;
   dy = 23.019575;
   dz = 26.108865;
   // Rotation: 
   thx = 71.998712;    phx = 211.716792;
   thy = 53.712420;    phy = 315.519955;
   thz = 41.887157;    phz = 100.474050;
   TGeoRotation *pMatrix53 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix52 = new TGeoCombiTrans("", dx,dy,dz,pMatrix53);
   // Combi transformation: 
   dx = 4.253532;
   dy = 23.019575;
   dz = 26.108864;
   // Rotation: 
   thx = 108.001326;    phx = 148.283214;
   thy = 53.712445;    phy = 224.480032;
   thz = 41.887155;    phz = 79.526007;
   TGeoRotation *pMatrix55 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix54 = new TGeoCombiTrans("", dx,dy,dz,pMatrix55);
   // Combi transformation: 
   dx = 23.021677;
   dy = 26.105015;
   dz = 4.261582;
   // Rotation: 
   thx = 143.999982;    phx = 328.279978;
   thy = 125.107852;    phy = 133.659405;
   thz = 83.028554;    phz = 48.590864;
   TGeoRotation *pMatrix57 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix56 = new TGeoCombiTrans("", dx,dy,dz,pMatrix57);
   // Combi transformation: 
   dx = 23.021676;
   dy = 26.105014;
   dz = -4.261582;
   // Rotation: 
   thx = 143.999995;    phx = 148.279948;
   thy = 54.892151;    phy = 133.659460;
   thz = 96.971404;    phz = 48.590888;
   TGeoRotation *pMatrix59 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix58 = new TGeoCombiTrans("", dx,dy,dz,pMatrix59);
   // Combi transformation: 
   dx = 4.253533;
   dy = 23.019574;
   dz = -26.108864;
   // Rotation: 
   thx = 108.001329;    phx = 328.283239;
   thy = 126.287529;    phy = 224.480068;
   thz = 138.112867;    phz = 79.526055;
   TGeoRotation *pMatrix61 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix60 = new TGeoCombiTrans("", dx,dy,dz,pMatrix61);
   // Combi transformation: 
   dx = -4.253532;
   dy = 23.019575;
   dz = -26.108864;
   // Rotation: 
   thx = 71.998656;    phx = 31.716777;
   thy = 126.287549;    phy = 315.519971;
   thz = 138.112840;    phz = 100.473964;
   TGeoRotation *pMatrix63 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix62 = new TGeoCombiTrans("", dx,dy,dz,pMatrix63);
   // Combi transformation: 
   dx = -6.887294;
   dy = 16.135389;
   dz = 30.361962;
   // Rotation: 
   thx = 59.997612;    phx = 290.905661;
   thy = 89.044200;    phy = 21.457606;
   thz = 30.020793;    phz = 113.112097;
   TGeoRotation *pMatrix65 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix64 = new TGeoCombiTrans("", dx,dy,dz,pMatrix65);
   // Combi transformation: 
   dx = 6.887294;
   dy = 16.135389;
   dz = 30.361961;
   // Rotation: 
   thx = 120.002362;    phx = 69.094301;
   thy = 89.044241;    phy = 158.542381;
   thz = 30.020765;    phz = 66.887960;
   TGeoRotation *pMatrix67 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix66 = new TGeoCombiTrans("", dx,dy,dz,pMatrix67);
   // Combi transformation: 
   dx = 6.887294;
   dy = 16.135389;
   dz = -30.361962;
   // Rotation: 
   thx = 120.002428;    phx = 249.094333;
   thy = 90.955791;    phy = 158.542393;
   thz = 149.979167;    phz = 66.887920;
   TGeoRotation *pMatrix69 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix68 = new TGeoCombiTrans("", dx,dy,dz,pMatrix69);
   // Combi transformation: 
   dx = -6.887294;
   dy = 16.135388;
   dz = -30.361962;
   // Rotation: 
   thx = 59.997582;    phx = 110.905647;
   thy = 90.955795;    phy = 21.457589;
   thz = 149.979177;    phz = 113.112069;
   TGeoRotation *pMatrix71 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix70 = new TGeoCombiTrans("", dx,dy,dz,pMatrix71);
   // Combi transformation: 
   dx = -26.108164;
   dy = 4.255982;
   dz = -23.019926;
   // Rotation: 
   thx = 119.999123;    phx = 290.906536;
   thy = 55.613527;    phy = 224.179111;
   thz = 131.036574;    phz = 170.740166;
   TGeoRotation *pMatrix73 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix72 = new TGeoCombiTrans("", dx,dy,dz,pMatrix73);
   // Combi transformation: 
   dx = -30.363359;
   dy = 6.887294;
   dz = -16.132939;
   // Rotation: 
   thx = 143.997800;    phx = 31.715620;
   thy = 111.454468;    phy = 268.973114;
   thz = 117.397374;    phz = 167.220569;
   TGeoRotation *pMatrix75 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix74 = new TGeoCombiTrans("", dx,dy,dz,pMatrix75);
   // Combi transformation: 
   dx = -30.363359;
   dy = 6.887294;
   dz = 16.132940;
   // Rotation: 
   thx = 143.997784;    phx = 211.715667;
   thy = 68.545504;    phy = 268.973129;
   thz = 62.602631;    phz = 167.220569;
   TGeoRotation *pMatrix77 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix76 = new TGeoCombiTrans("", dx,dy,dz,pMatrix77);
   // Combi transformation: 
   dx = -26.108164;
   dy = 4.255982;
   dz = 23.019925;
   // Rotation: 
   thx = 119.999118;    phx = 110.906514;
   thy = 124.386494;    phy = 224.179103;
   thz = 48.963441;    phz = 170.740169;
   TGeoRotation *pMatrix79 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix78 = new TGeoCombiTrans("", dx,dy,dz,pMatrix79);
   // Combi transformation: 
   dx = -0.000003;
   dy = 11.875172;
   dz = 32.993915;
   // Rotation: 
   thx = 90.000076;    phx = 0.000068;
   thy = 109.803055;    phy = 90.000095;
   thz = 19.803055;    phz = 89.999857;
   TGeoRotation *pMatrix81 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix80 = new TGeoCombiTrans("", dx,dy,dz,pMatrix81);
   // Combi transformation: 
   dx = 26.108165;
   dy = 4.255982;
   dz = 23.019926;
   // Rotation: 
   thx = 60.000891;    phx = 249.093468;
   thy = 124.386468;    phy = 315.820912;
   thz = 48.963409;    phz = 9.259839;
   TGeoRotation *pMatrix83 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix82 = new TGeoCombiTrans("", dx,dy,dz,pMatrix83);
   // Combi transformation: 
   dx = 30.363359;
   dy = 6.887293;
   dz = 16.132938;
   // Rotation: 
   thx = 36.002177;    phx = 148.284386;
   thy = 68.545556;    phy = 271.026878;
   thz = 62.602633;    phz = 12.779405;
   TGeoRotation *pMatrix85 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix84 = new TGeoCombiTrans("", dx,dy,dz,pMatrix85);
   // Combi transformation: 
   dx = 30.363359;
   dy = 6.887293;
   dz = -16.132938;
   // Rotation: 
   thx = 36.002175;    phx = 328.284372;
   thy = 111.454449;    phy = 271.026875;
   thz = 117.397361;    phz = 12.779402;
   TGeoRotation *pMatrix87 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix86 = new TGeoCombiTrans("", dx,dy,dz,pMatrix87);
   // Combi transformation: 
   dx = 26.108164;
   dy = 4.255983;
   dz = -23.019925;
   // Rotation: 
   thx = 60.000891;    phx = 69.093510;
   thy = 55.613492;    phy = 315.820917;
   thz = 131.036553;    phz = 9.259837;
   TGeoRotation *pMatrix89 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix88 = new TGeoCombiTrans("", dx,dy,dz,pMatrix89);
   // Combi transformation: 
   dx = 0.000001;
   dy = 11.875172;
   dz = -32.993916;
   // Rotation: 
   thx = 89.999971;    phx = 179.999977;
   thy = 70.196968;    phy = 89.999967;
   thz = 160.196968;    phz = 90.000057;
   TGeoRotation *pMatrix91 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix90 = new TGeoCombiTrans("", dx,dy,dz,pMatrix91);
   // Combi transformation: 
   dx = -33.132525;
   dy = 0.000001;
   dz = -11.387622;
   // Rotation: 
   thx = 89.999969;    phx = 89.999969;
   thy = 160.196941;    phy = 0.000055;
   thz = 109.803059;    phz = 179.999958;
   TGeoRotation *pMatrix93 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix92 = new TGeoCombiTrans("", dx,dy,dz,pMatrix93);
   // Combi transformation: 
   dx = -33.132525;
   dy = 0.000004;
   dz = 11.387622;
   // Rotation: 
   thx = 89.999908;    phx = 270.000092;
   thy = 19.803056;    phy = 0.000347;
   thz = 70.196944;    phz = 180.000059;
   TGeoRotation *pMatrix95 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix94 = new TGeoCombiTrans("", dx,dy,dz,pMatrix95);
   // Combi transformation: 
   dx = 33.132525;
   dy = -0.000001;
   dz = -11.387622;
   // Rotation: 
   thx = 89.999969;    phx = 270.000031;
   thy = 160.196941;    phy = 180.000117;
   thz = 109.803059;    phz = 0.000020;
   TGeoRotation *pMatrix97 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix96 = new TGeoCombiTrans("", dx,dy,dz,pMatrix97);
   // Combi transformation: 
   dx = 33.132525;
   dy = 0.000002;
   dz = 11.387622;
   // Rotation: 
   thx = 90.000076;    phx = 90.000076;
   thy = 19.803057;    phy = 179.999865;
   thz = 70.196943;    phz = 0.000103;
   TGeoRotation *pMatrix99 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix98 = new TGeoCombiTrans("", dx,dy,dz,pMatrix99);
   // Combi transformation: 
   dx = 0.000001;
   dy = -11.875173;
   dz = 32.993918;
   // Rotation: 
   thx = 90.000000;    phx = 180.000031;
   thy = 109.802991;    phy = 270.000031;
   thz = 19.802991;    phz = 270.000031;
   TGeoRotation *pMatrix101 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix100 = new TGeoCombiTrans("", dx,dy,dz,pMatrix101);
   // Combi transformation: 
   dx = -26.108164;
   dy = -4.255982;
   dz = 23.019926;
   // Rotation: 
   thx = 60.000881;    phx = 69.093450;
   thy = 124.386461;    phy = 135.820891;
   thz = 48.963410;    phz = 189.259832;
   TGeoRotation *pMatrix103 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix102 = new TGeoCombiTrans("", dx,dy,dz,pMatrix103);
   // Combi transformation: 
   dx = -30.363362;
   dy = -6.887294;
   dz = 16.132938;
   // Rotation: 
   thx = 36.002139;    phx = 328.284399;
   thy = 68.545541;    phy = 91.026971;
   thz = 62.602690;    phz = 192.779479;
   TGeoRotation *pMatrix105 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix104 = new TGeoCombiTrans("", dx,dy,dz,pMatrix105);
   // Combi transformation: 
   dx = -30.363360;
   dy = -6.887294;
   dz = -16.132939;
   // Rotation: 
   thx = 36.002188;    phx = 148.284359;
   thy = 111.454473;    phy = 91.026891;
   thz = 117.397355;    phz = 192.779430;
   TGeoRotation *pMatrix107 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix106 = new TGeoCombiTrans("", dx,dy,dz,pMatrix107);
   // Combi transformation: 
   dx = -26.108164;
   dy = -4.255982;
   dz = -23.019925;
   // Rotation: 
   thx = 60.000878;    phx = 249.093491;
   thy = 55.613501;    phy = 135.820894;
   thz = 131.036551;    phz = 189.259833;
   TGeoRotation *pMatrix109 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix108 = new TGeoCombiTrans("", dx,dy,dz,pMatrix109);
   // Combi transformation: 
   dx = -0.000000;
   dy = -11.875172;
   dz = -32.993916;
   // Rotation: 
   thx = 89.999969;    phx = 0.000001;
   thy = 70.196968;    phy = 269.999990;
   thz = 160.196968;    phz = 270.000087;
   TGeoRotation *pMatrix111 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix110 = new TGeoCombiTrans("", dx,dy,dz,pMatrix111);
   // Combi transformation: 
   dx = 26.108164;
   dy = -4.255982;
   dz = -23.019925;
   // Rotation: 
   thx = 119.999118;    phx = 110.906501;
   thy = 55.613498;    phy = 44.179097;
   thz = 131.036552;    phz = 350.740165;
   TGeoRotation *pMatrix113 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix112 = new TGeoCombiTrans("", dx,dy,dz,pMatrix113);
   // Combi transformation: 
   dx = 30.363359;
   dy = -6.887294;
   dz = -16.132939;
   // Rotation: 
   thx = 143.997810;    phx = 211.715657;
   thy = 111.454471;    phy = 88.973132;
   thz = 117.397360;    phz = 347.220592;
   TGeoRotation *pMatrix115 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix114 = new TGeoCombiTrans("", dx,dy,dz,pMatrix115);
   // Combi transformation: 
   dx = 30.363359;
   dy = -6.887293;
   dz = 16.132939;
   // Rotation: 
   thx = 143.997800;    phx = 31.715642;
   thy = 68.545535;    phy = 88.973141;
   thz = 62.602624;    phz = 347.220596;
   TGeoRotation *pMatrix117 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix116 = new TGeoCombiTrans("", dx,dy,dz,pMatrix117);
   // Combi transformation: 
   dx = 26.108164;
   dy = -4.255983;
   dz = 23.019925;
   // Rotation: 
   thx = 119.999105;    phx = 290.906496;
   thy = 124.386504;    phy = 44.179080;
   thz = 48.963439;    phz = 350.740165;
   TGeoRotation *pMatrix119 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix118 = new TGeoCombiTrans("", dx,dy,dz,pMatrix119);
   // Combi transformation: 
   dx = 6.887294;
   dy = -16.135389;
   dz = 30.361962;
   // Rotation: 
   thx = 59.997614;    phx = 110.905674;
   thy = 89.044199;    phy = 201.457619;
   thz = 30.020791;    phz = 293.112113;
   TGeoRotation *pMatrix121 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix120 = new TGeoCombiTrans("", dx,dy,dz,pMatrix121);
   // Combi transformation: 
   dx = -6.887294;
   dy = -16.135389;
   dz = 30.361961;
   // Rotation: 
   thx = 120.002362;    phx = 249.094316;
   thy = 89.044242;    phy = 338.542396;
   thz = 30.020765;    phz = 246.887976;
   TGeoRotation *pMatrix123 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix122 = new TGeoCombiTrans("", dx,dy,dz,pMatrix123);
   // Combi transformation: 
   dx = -6.887294;
   dy = -16.135389;
   dz = -30.361962;
   // Rotation: 
   thx = 120.002427;    phx = 69.094345;
   thy = 90.955791;    phy = 338.542405;
   thz = 149.979168;    phz = 246.887932;
   TGeoRotation *pMatrix125 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix124 = new TGeoCombiTrans("", dx,dy,dz,pMatrix125);
   // Combi transformation: 
   dx = 6.887294;
   dy = -16.135389;
   dz = -30.361962;
   // Rotation: 
   thx = 59.997579;    phx = 290.905662;
   thy = 90.955794;    phy = 201.457603;
   thz = 149.979174;    phz = 293.112081;
   TGeoRotation *pMatrix127 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix126 = new TGeoCombiTrans("", dx,dy,dz,pMatrix127);
   // Combi transformation: 
   dx = 4.253534;
   dy = -23.019575;
   dz = 26.108865;
   // Rotation: 
   thx = 71.998733;    phx = 31.716781;
   thy = 53.712421;    phy = 135.519927;
   thz = 41.887144;    phz = 280.474057;
   TGeoRotation *pMatrix129 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix128 = new TGeoCombiTrans("", dx,dy,dz,pMatrix129);
   // Combi transformation: 
   dx = -4.253532;
   dy = -23.019576;
   dz = 26.108864;
   // Rotation: 
   thx = 108.001329;    phx = 328.283210;
   thy = 53.712441;    phy = 44.480024;
   thz = 41.887160;    phz = 259.526004;
   TGeoRotation *pMatrix131 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix130 = new TGeoCombiTrans("", dx,dy,dz,pMatrix131);
   // Combi transformation: 
   dx = -23.021675;
   dy = -26.105014;
   dz = 4.261582;
   // Rotation: 
   thx = 143.999988;    phx = 148.279953;
   thy = 125.107858;    phy = 313.659484;
   thz = 83.028604;    phz = 228.590907;
   TGeoRotation *pMatrix133 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix132 = new TGeoCombiTrans("", dx,dy,dz,pMatrix133);
   // Combi transformation: 
   dx = -23.021675;
   dy = -26.105014;
   dz = -4.261582;
   // Rotation: 
   thx = 143.999993;    phx = 328.280004;
   thy = 54.892154;    phy = 313.659473;
   thz = 96.971425;    phz = 228.590915;
   TGeoRotation *pMatrix135 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix134 = new TGeoCombiTrans("", dx,dy,dz,pMatrix135);
   // Combi transformation: 
   dx = -4.253532;
   dy = -23.019575;
   dz = -26.108864;
   // Rotation: 
   thx = 108.001338;    phx = 148.283229;
   thy = 126.287541;    phy = 44.480045;
   thz = 138.112851;    phz = 259.526044;
   TGeoRotation *pMatrix137 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix136 = new TGeoCombiTrans("", dx,dy,dz,pMatrix137);
   // Combi transformation: 
   dx = 4.253532;
   dy = -23.019575;
   dz = -26.108864;
   // Rotation: 
   thx = 71.998659;    phx = 211.716774;
   thy = 126.287545;    phy = 135.519963;
   thz = 138.112846;    phz = 280.473960;
   TGeoRotation *pMatrix139 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix138 = new TGeoCombiTrans("", dx,dy,dz,pMatrix139);
   // Combi transformation: 
   dx = 23.021676;
   dy = -26.105015;
   dz = -4.261583;
   // Rotation: 
   thx = 35.999984;    phx = 31.720039;
   thy = 54.892176;    phy = 226.340561;
   thz = 96.971419;    phz = 311.409127;
   TGeoRotation *pMatrix141 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix140 = new TGeoCombiTrans("", dx,dy,dz,pMatrix141);
   // Combi transformation: 
   dx = 23.021675;
   dy = -26.105015;
   dz = 4.261583;
   // Rotation: 
   thx = 35.999971;    phx = 211.720047;
   thy = 125.107816;    phy = 226.340536;
   thz = 83.028598;    phz = 311.409115;
   TGeoRotation *pMatrix143 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix142 = new TGeoCombiTrans("", dx,dy,dz,pMatrix143);
   // Combi transformation: 
   dx = -16.136090;
   dy = -30.361960;
   dz = 6.886594;
   // Rotation: 
   thx = 71.999971;    phx = 148.279949;
   thy = 158.522092;    phy = 182.611105;
   thz = 78.674146;    phz = 242.011278;
   TGeoRotation *pMatrix145 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix144 = new TGeoCombiTrans("", dx,dy,dz,pMatrix145);
   // Combi transformation: 
   dx = -16.136091;
   dy = -30.361960;
   dz = -6.886595;
   // Rotation: 
   thx = 71.999930;    phx = 328.279997;
   thy = 21.477937;    phy = 182.611075;
   thz = 101.325844;    phz = 242.011332;
   TGeoRotation *pMatrix147 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix146 = new TGeoCombiTrans("", dx,dy,dz,pMatrix147);
   // Combi transformation: 
   dx = 16.136090;
   dy = -30.361960;
   dz = -6.886594;
   // Rotation: 
   thx = 108.000030;    phx = 31.720015;
   thy = 21.477893;    phy = 357.388925;
   thz = 101.325828;    phz = 297.988695;
   TGeoRotation *pMatrix149 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix148 = new TGeoCombiTrans("", dx,dy,dz,pMatrix149);
   // Combi transformation: 
   dx = 16.136092;
   dy = -30.361960;
   dz = 6.886595;
   // Rotation: 
   thx = 108.000079;    phx = 211.720018;
   thy = 158.522046;    phy = 357.388911;
   thz = 78.674140;    phz = 297.988675;
   TGeoRotation *pMatrix151 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix150 = new TGeoCombiTrans("", dx,dy,dz,pMatrix151);
   // Combi transformation: 
   dx = -11.387622;
   dy = -33.132525;
   dz = 0.000000;
   // Rotation: 
   thx = 0.000003;    phx = 0.000000;
   thy = 89.999997;    phy = 160.196941;
   thz = 89.999999;    phz = 250.196941;
   TGeoRotation *pMatrix153 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix152 = new TGeoCombiTrans("", dx,dy,dz,pMatrix153);
   // Combi transformation: 
   dx = 11.387622;
   dy = -33.132525;
   dz = 0.000001;
   // Rotation: 
   thx = 179.999918;    phx = 0.000000;
   thy = 89.999923;    phy = 19.803056;
   thz = 89.999972;    phz = 289.803056;
   TGeoRotation *pMatrix155 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix154 = new TGeoCombiTrans("", dx,dy,dz,pMatrix155);
   // Combi transformation: 
   dx = -5.186027;
   dy = 33.299502;
   dz = 9.455163;
   // Rotation: 
   thx = 144.001094;    phx = 31.715036;
   thy = 121.401971;    phy = 178.881126;
   thz = 74.215844;    phz = 98.817682;
   TGeoRotation *pMatrix157 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix156 = new TGeoCombiTrans("", dx,dy,dz,pMatrix157);
   // Combi transformation: 
   dx = 5.186027;
   dy = 33.299502;
   dz = 9.455163;
   // Rotation: 
   thx = 35.998906;    phx = 328.284964;
   thy = 121.401971;    phy = 1.118874;
   thz = 74.215844;    phz = 81.182318;
   TGeoRotation *pMatrix159 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix158 = new TGeoCombiTrans("", dx,dy,dz,pMatrix159);
   // Combi transformation: 
   dx = 5.186027;
   dy = 33.299494;
   dz = -9.455163;
   // Rotation: 
   thx = 36.000020;    phx = 148.279988;
   thy = 58.597955;    phy = 1.117269;
   thz = 105.786054;    phz = 81.179414;
   TGeoRotation *pMatrix161 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix160 = new TGeoCombiTrans("", dx,dy,dz,pMatrix161);
   // Combi transformation: 
   dx = -5.186026;
   dy = 33.299522;
   dz = -9.455164;
   // Rotation: 
   thx = 144.000133;    phx = 211.714525;
   thy = 58.598154;    phy = 178.877047;
   thz = 105.786115;    phz = 98.814864;
   TGeoRotation *pMatrix163 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix162 = new TGeoCombiTrans("", dx,dy,dz,pMatrix163);
   // Combi transformation: 
   dx = 0.000001;
   dy = 30.106960;
   dz = 17.851246;
   // Rotation: 
   thx = 89.999908;    phx = 180.000061;
   thy = 30.709993;    phy = 270.000216;
   thz = 59.290007;    phz = 90.000006;
   TGeoRotation *pMatrix165 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix164 = new TGeoCombiTrans("", dx,dy,dz,pMatrix165);
   // Combi transformation: 
   dx = -0.000007;
   dy = 30.106950;
   dz = -17.851240;
   // Rotation: 
   thx = 89.999802;    phx = 359.997167;
   thy = 149.288015;    phy = 269.997500;
   thz = 120.711985;    phz = 89.997049;
   TGeoRotation *pMatrix167 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix166 = new TGeoCombiTrans("", dx,dy,dz,pMatrix167);
   // Combi transformation: 
   dx = -20.572708;
   dy = 23.757896;
   dz = -15.408720;
   // Rotation: 
   thx = 72.001971;    phx = 211.717320;
   thy = 32.535865;    phy = 91.102879;
   thz = 116.117441;    phz = 130.882458;
   TGeoRotation *pMatrix169 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix168 = new TGeoCombiTrans("", dx,dy,dz,pMatrix169);
   // Combi transformation: 
   dx = -20.572697;
   dy = 23.760693;
   dz = 15.404511;
   // Rotation: 
   thx = 72.001559;    phx = 31.717921;
   thy = 147.466804;    phy = 91.099167;
   thz = 63.885927;    phz = 130.881911;
   TGeoRotation *pMatrix171 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix170 = new TGeoCombiTrans("", dx,dy,dz,pMatrix171);
   // Combi transformation: 
   dx = -15.408718;
   dy = 20.568492;
   dz = 23.761743;
   // Rotation: 
   thx = 59.997618;    phx = 249.094312;
   thy = 57.471456;    phy = 0.702199;
   thz = 47.247066;    phz = 126.831530;
   TGeoRotation *pMatrix173 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix172 = new TGeoCombiTrans("", dx,dy,dz,pMatrix173);
   // Combi transformation: 
   dx = 15.408718;
   dy = 20.568492;
   dz = 23.761743;
   // Rotation: 
   thx = 120.002355;    phx = 110.905652;
   thy = 57.471439;    phy = 179.297775;
   thz = 47.247058;    phz = 53.168406;
   TGeoRotation *pMatrix175 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix174 = new TGeoCombiTrans("", dx,dy,dz,pMatrix175);
   // Combi transformation: 
   dx = 20.572697;
   dy = 23.760693;
   dz = 15.404511;
   // Rotation: 
   thx = 107.998476;    phx = 328.282075;
   thy = 147.466790;    phy = 88.900881;
   thz = 63.885936;    phz = 49.118070;
   TGeoRotation *pMatrix177 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix176 = new TGeoCombiTrans("", dx,dy,dz,pMatrix177);
   // Combi transformation: 
   dx = 20.572703;
   dy = 23.757893;
   dz = -15.408718;
   // Rotation: 
   thx = 107.999974;    phx = 148.279969;
   thy = 32.535302;    phy = 88.899059;
   thz = 116.115350;    phz = 49.114617;
   TGeoRotation *pMatrix179 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix178 = new TGeoCombiTrans("", dx,dy,dz,pMatrix179);
   // Combi transformation: 
   dx = 15.408720;
   dy = 20.568498;
   dz = -23.761745;
   // Rotation: 
   thx = 120.000212;    phx = 290.902923;
   thy = 122.529079;    phy = 179.296555;
   thz = 132.754334;    phz = 53.164314;
   TGeoRotation *pMatrix181 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix180 = new TGeoCombiTrans("", dx,dy,dz,pMatrix181);
   // Combi transformation: 
   dx = -15.408718;
   dy = 20.568492;
   dz = -23.761743;
   // Rotation: 
   thx = 59.997618;    phx = 69.094312;
   thy = 122.528544;    phy = 0.702199;
   thz = 132.752934;    phz = 126.831530;
   TGeoRotation *pMatrix183 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix182 = new TGeoCombiTrans("", dx,dy,dz,pMatrix183);
   // Combi transformation: 
   dx = -23.760793;
   dy = 15.409768;
   dz = -20.570592;
   // Rotation: 
   thx = 143.997842;    phx = 328.284406;
   thy = 90.592063;    phy = 237.469509;
   thz = 125.995725;    phz = 147.039399;
   TGeoRotation *pMatrix185 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix184 = new TGeoCombiTrans("", dx,dy,dz,pMatrix185);
   // Combi transformation: 
   dx = -30.091560;
   dy = 17.877840;
   dz = 0.000008;
   // Rotation: 
   thx = 0.002820;    phx = 177.780669;
   thy = 89.998655;    phy = 59.288001;
   thz = 90.002478;    phz = 149.288001;
   TGeoRotation *pMatrix187 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix186 = new TGeoCombiTrans("", dx,dy,dz,pMatrix187);
   // Combi transformation: 
   dx = -23.759295;
   dy = 15.409770;
   dz = 20.570598;
   // Rotation: 
   thx = 144.001005;    phx = 148.285348;
   thy = 89.406930;    phy = 237.468970;
   thz = 54.007459;    phz = 147.038178;
   TGeoRotation *pMatrix189 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix188 = new TGeoCombiTrans("", dx,dy,dz,pMatrix189);
   // Combi transformation: 
   dx = 23.759296;
   dy = 15.409770;
   dz = 20.570598;
   // Rotation: 
   thx = 35.998994;    phx = 211.714691;
   thy = 89.406937;    phy = 302.531059;
   thz = 54.007460;    phz = 32.961846;
   TGeoRotation *pMatrix191 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix190 = new TGeoCombiTrans("", dx,dy,dz,pMatrix191);
   // Combi transformation: 
   dx = 30.091570;
   dy = 17.877846;
   dz = 0.000001;
   // Rotation: 
   thx = 179.999880;    phx = 159.168602;
   thy = 89.999906;    phy = 120.710005;
   thz = 90.000074;    phz = 30.710005;
   TGeoRotation *pMatrix193 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix192 = new TGeoCombiTrans("", dx,dy,dz,pMatrix193);
   // Combi transformation: 
   dx = 23.759283;
   dy = 15.409761;
   dz = -20.570586;
   // Rotation: 
   thx = 35.999668;    phx = 31.714828;
   thy = 90.591055;    phy = 302.528411;
   thz = 125.993257;    phz = 32.957751;
   TGeoRotation *pMatrix195 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix194 = new TGeoCombiTrans("", dx,dy,dz,pMatrix195);
   // Combi transformation: 
   dx = -33.299144;
   dy = 9.551063;
   dz = -4.993527;
   // Rotation: 
   thx = 119.999107;    phx = 69.093455;
   thy = 148.580107;    phy = 268.170571;
   thz = 98.485441;    phz = 164.034606;
   TGeoRotation *pMatrix197 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix196 = new TGeoCombiTrans("", dx,dy,dz,pMatrix197);
   // Combi transformation: 
   dx = -33.299172;
   dy = 9.551064;
   dz = 4.993527;
   // Rotation: 
   thx = 120.000905;    phx = 249.096548;
   thy = 31.419823;    phy = 268.161202;
   thz = 81.520107;    phz = 164.034772;
   TGeoRotation *pMatrix199 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix198 = new TGeoCombiTrans("", dx,dy,dz,pMatrix199);
   // Combi transformation: 
   dx = -10.551193;
   dy = 5.749307;
   dz = 37.104944;
   // Rotation: 
   thx = 71.998677;    phx = 328.283198;
   thy = 89.046403;    phy = 58.593094;
   thz = 18.028302;    phz = 151.524588;
   TGeoRotation *pMatrix201 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix200 = new TGeoCombiTrans("", dx,dy,dz,pMatrix201);
   // Combi transformation: 
   dx = 10.551193;
   dy = 5.749306;
   dz = 37.104952;
   // Rotation: 
   thx = 107.998140;    phx = 31.717165;
   thy = 89.045012;    phy = 121.406874;
   thz = 18.025203;    phz = 28.470559;
   TGeoRotation *pMatrix203 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix202 = new TGeoCombiTrans("", dx,dy,dz,pMatrix203);
   // Combi transformation: 
   dx = 33.299152;
   dy = 9.551063;
   dz = 4.993527;
   // Rotation: 
   thx = 59.999963;    phx = 110.903021;
   thy = 31.419877;    phy = 271.832218;
   thz = 81.517367;    phz = 15.963348;
   TGeoRotation *pMatrix205 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix204 = new TGeoCombiTrans("", dx,dy,dz,pMatrix205);
   // Combi transformation: 
   dx = 33.299152;
   dy = 9.551063;
   dz = -4.993527;
   // Rotation: 
   thx = 59.999976;    phx = 290.903017;
   thy = 148.580113;    phy = 271.832067;
   thz = 98.482701;    phz = 15.963306;
   TGeoRotation *pMatrix207 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix206 = new TGeoCombiTrans("", dx,dy,dz,pMatrix207);
   // Combi transformation: 
   dx = 9.451314;
   dy = 5.188827;
   dz = -33.300222;
   // Rotation: 
   thx = 107.998388;    phx = 211.718141;
   thy = 90.958452;    phy = 121.406720;
   thz = 161.974353;    phz = 28.459821;
   TGeoRotation *pMatrix209 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix208 = new TGeoCombiTrans("", dx,dy,dz,pMatrix209);
   // Combi transformation: 
   dx = -9.451313;
   dy = 5.188826;
   dz = -33.300202;
   // Rotation: 
   thx = 72.001831;    phx = 148.282821;
   thy = 90.954999;    phy = 58.593116;
   thz = 161.974768;    phz = 151.529460;
   TGeoRotation *pMatrix211 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix210 = new TGeoCombiTrans("", dx,dy,dz,pMatrix211);
   // Combi transformation: 
   dx = -17.877846;
   dy = 0.000000;
   dz = -30.091570;
   // Rotation: 
   thx = 89.999908;    phx = 270.000031;
   thy = 59.289969;    phy = 179.999976;
   thz = 149.289969;    phz = 180.000186;
   TGeoRotation *pMatrix213 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix212 = new TGeoCombiTrans("", dx,dy,dz,pMatrix213);
   // Combi transformation: 
   dx = -17.877840;
   dy = -0.000008;
   dz = 30.091560;
   // Rotation: 
   thx = 90.002870;    phx = 89.999919;
   thy = 120.711996;    phy = 180.001624;
   thz = 30.711996;    phz = 179.995086;
   TGeoRotation *pMatrix215 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix214 = new TGeoCombiTrans("", dx,dy,dz,pMatrix215);
   // Combi transformation: 
   dx = 17.877840;
   dy = 0.000008;
   dz = -30.091560;
   // Rotation: 
   thx = 90.002869;    phx = 89.999863;
   thy = 59.288001;    phy = 0.001567;
   thz = 149.288000;    phz = 359.995033;
   TGeoRotation *pMatrix217 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix216 = new TGeoCombiTrans("", dx,dy,dz,pMatrix217);
   // Combi transformation: 
   dx = 17.877846;
   dy = 0.000000;
   dz = 30.091570;
   // Rotation: 
   thx = 89.999969;    phx = 270.000092;
   thy = 120.710027;    phy = 0.000074;
   thz = 30.710027;    phz = 0.000144;
   TGeoRotation *pMatrix219 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix218 = new TGeoCombiTrans("", dx,dy,dz,pMatrix219);
   // Combi transformation: 
   dx = 10.551193;
   dy = -5.749306;
   dz = 37.104952;
   // Rotation: 
   thx = 72.001844;    phx = 148.282837;
   thy = 89.045020;    phy = 238.593125;
   thz = 18.025218;    phz = 331.529413;
   TGeoRotation *pMatrix221 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix220 = new TGeoCombiTrans("", dx,dy,dz,pMatrix221);
   // Combi transformation: 
   dx = -10.551194;
   dy = -5.749306;
   dz = 37.104972;
   // Rotation: 
   thx = 107.998399;    phx = 211.718159;
   thy = 89.041550;    phy = 301.406739;
   thz = 18.025658;    phz = 208.459849;
   TGeoRotation *pMatrix223 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix222 = new TGeoCombiTrans("", dx,dy,dz,pMatrix223);
   // Combi transformation: 
   dx = -33.299152;
   dy = -9.551063;
   dz = 4.993527;
   // Rotation: 
   thx = 59.999982;    phx = 290.903023;
   thy = 31.419858;    phy = 91.832219;
   thz = 81.517369;    phz = 195.963355;
   TGeoRotation *pMatrix225 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix224 = new TGeoCombiTrans("", dx,dy,dz,pMatrix225);
   // Combi transformation: 
   dx = -33.299152;
   dy = -9.551063;
   dz = -4.993527;
   // Rotation: 
   thx = 59.999983;    phx = 110.903071;
   thy = 148.580121;    phy = 91.832122;
   thz = 98.482699;    phz = 195.963363;
   TGeoRotation *pMatrix227 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix226 = new TGeoCombiTrans("", dx,dy,dz,pMatrix227);
   // Combi transformation: 
   dx = -9.451313;
   dy = -5.188827;
   dz = -33.300202;
   // Rotation: 
   thx = 107.998169;    phx = 31.717166;
   thy = 90.955009;    phy = 301.406868;
   thz = 161.974767;    phz = 208.470493;
   TGeoRotation *pMatrix229 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix228 = new TGeoCombiTrans("", dx,dy,dz,pMatrix229);
   // Combi transformation: 
   dx = 9.451313;
   dy = -5.188827;
   dz = -33.300194;
   // Rotation: 
   thx = 71.998655;    phx = 328.283183;
   thy = 90.953645;    phy = 238.593096;
   thz = 161.971672;    phz = 331.524732;
   TGeoRotation *pMatrix231 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix230 = new TGeoCombiTrans("", dx,dy,dz,pMatrix231);
   // Combi transformation: 
   dx = 33.299172;
   dy = -9.551064;
   dz = -4.993527;
   // Rotation: 
   thx = 120.000921;    phx = 249.096562;
   thy = 148.580154;    phy = 88.161258;
   thz = 98.479915;    phz = 344.034802;
   TGeoRotation *pMatrix233 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix232 = new TGeoCombiTrans("", dx,dy,dz,pMatrix233);
   // Combi transformation: 
   dx = 33.299144;
   dy = -9.551063;
   dz = 4.993527;
   // Rotation: 
   thx = 119.999096;    phx = 69.093456;
   thy = 31.419881;    phy = 88.170570;
   thz = 81.514562;    phz = 344.034604;
   TGeoRotation *pMatrix235 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix234 = new TGeoCombiTrans("", dx,dy,dz,pMatrix235);
   // Combi transformation: 
   dx = -23.759283;
   dy = -15.409761;
   dz = 20.570586;
   // Rotation: 
   thx = 35.999690;    phx = 31.714863;
   thy = 89.408960;    phy = 122.528426;
   thz = 54.006721;    phz = 212.957755;
   TGeoRotation *pMatrix237 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix236 = new TGeoCombiTrans("", dx,dy,dz,pMatrix237);
   // Combi transformation: 
   dx = -30.091570;
   dy = -17.877846;
   dz = 0.000001;
   // Rotation: 
   thx = 179.999880;    phx = 339.168622;
   thy = 89.999906;    phy = 300.710017;
   thz = 90.000074;    phz = 210.710017;
   TGeoRotation *pMatrix239 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix238 = new TGeoCombiTrans("", dx,dy,dz,pMatrix239);
   // Combi transformation: 
   dx = -23.759295;
   dy = -15.409770;
   dz = -20.570598;
   // Rotation: 
   thx = 35.998959;    phx = 211.714637;
   thy = 90.593071;    phy = 122.531017;
   thz = 125.992504;    phz = 212.961809;
   TGeoRotation *pMatrix241 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix240 = new TGeoCombiTrans("", dx,dy,dz,pMatrix241);
   // Combi transformation: 
   dx = 23.759295;
   dy = -15.409770;
   dz = -20.570598;
   // Rotation: 
   thx = 144.001022;    phx = 148.285417;
   thy = 90.593125;    phy = 57.468964;
   thz = 125.992522;    phz = 327.038132;
   TGeoRotation *pMatrix243 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix242 = new TGeoCombiTrans("", dx,dy,dz,pMatrix243);
   // Combi transformation: 
   dx = 30.091560;
   dy = -17.877840;
   dz = -0.000008;
   // Rotation: 
   thx = 0.002835;    phx = 177.685309;
   thy = 90.001348;    phy = 239.288004;
   thz = 89.997506;    phz = 329.288004;
   TGeoRotation *pMatrix245 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix244 = new TGeoCombiTrans("", dx,dy,dz,pMatrix245);
   // Combi transformation: 
   dx = 23.759293;
   dy = -15.417118;
   dz = 20.564993;
   // Rotation: 
   thx = 143.997811;    phx = 328.284393;
   thy = 89.407923;    phy = 57.469478;
   thz = 54.004244;    phz = 327.039356;
   TGeoRotation *pMatrix247 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix246 = new TGeoCombiTrans("", dx,dy,dz,pMatrix247);
   // Combi transformation: 
   dx = 15.408711;
   dy = -20.568487;
   dz = 23.761733;
   // Rotation: 
   thx = 59.999282;    phx = 69.096525;
   thy = 57.473554;    phy = 180.701056;
   thz = 47.243711;    phz = 306.831920;
   TGeoRotation *pMatrix249 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix248 = new TGeoCombiTrans("", dx,dy,dz,pMatrix249);
   // Combi transformation: 
   dx = -15.408720;
   dy = -20.568498;
   dz = 23.761745;
   // Rotation: 
   thx = 120.000202;    phx = 290.902930;
   thy = 57.470916;    phy = 359.296565;
   thz = 47.245663;    phz = 233.164310;
   TGeoRotation *pMatrix251 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix250 = new TGeoCombiTrans("", dx,dy,dz,pMatrix251);
   // Combi transformation: 
   dx = -20.572702;
   dy = -23.757893;
   dz = 15.408718;
   // Rotation: 
   thx = 107.999978;    phx = 148.280010;
   thy = 147.464671;    phy = 268.899074;
   thz = 63.884622;    phz = 229.114643;
   TGeoRotation *pMatrix253 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix252 = new TGeoCombiTrans("", dx,dy,dz,pMatrix253);
   // Combi transformation: 
   dx = -20.572697;
   dy = -23.760693;
   dz = -15.404511;
   // Rotation: 
   thx = 107.998436;    phx = 328.282072;
   thy = 32.533182;    phy = 268.900833;
   thz = 116.114061;    phz = 229.118090;
   TGeoRotation *pMatrix255 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix254 = new TGeoCombiTrans("", dx,dy,dz,pMatrix255);
   // Combi transformation: 
   dx = -15.408718;
   dy = -20.568492;
   dz = -23.761743;
   // Rotation: 
   thx = 120.002416;    phx = 110.905701;
   thy = 122.528553;    phy = 359.297775;
   thz = 132.752897;    phz = 233.168485;
   TGeoRotation *pMatrix257 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix256 = new TGeoCombiTrans("", dx,dy,dz,pMatrix257);
   // Combi transformation: 
   dx = 15.408718;
   dy = -20.568492;
   dz = -23.761743;
   // Rotation: 
   thx = 59.997613;    phx = 249.094310;
   thy = 122.528569;    phy = 180.702224;
   thz = 132.752907;    phz = 306.831555;
   TGeoRotation *pMatrix259 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix258 = new TGeoCombiTrans("", dx,dy,dz,pMatrix259);
   // Combi transformation: 
   dx = 20.572697;
   dy = -23.760693;
   dz = -15.404511;
   // Rotation: 
   thx = 72.001528;    phx = 31.717934;
   thy = 32.533197;    phy = 271.099119;
   thz = 116.114052;    phz = 310.881932;
   TGeoRotation *pMatrix261 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix260 = new TGeoCombiTrans("", dx,dy,dz,pMatrix261);
   // Combi transformation: 
   dx = 20.572708;
   dy = -23.757896;
   dz = 15.408720;
   // Rotation: 
   thx = 72.001971;    phx = 211.717320;
   thy = 147.464135;    phy = 271.102879;
   thz = 63.882559;    phz = 310.882458;
   TGeoRotation *pMatrix263 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix262 = new TGeoCombiTrans("", dx,dy,dz,pMatrix263);
   // Combi transformation: 
   dx = 0.000007;
   dy = -30.106950;
   dz = 17.851240;
   // Rotation: 
   thx = 89.999802;    phx = 359.997182;
   thy = 30.711961;    phy = 89.997515;
   thz = 59.288039;    phz = 269.997064;
   TGeoRotation *pMatrix265 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix264 = new TGeoCombiTrans("", dx,dy,dz,pMatrix265);
   // Combi transformation: 
   dx = 0.000000;
   dy = -30.106960;
   dz = -17.851246;
   // Rotation: 
   thx = 89.999969;    phx = 179.999939;
   thy = 149.289983;    phy = 89.999991;
   thz = 120.710017;    phz = 269.999921;
   TGeoRotation *pMatrix267 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix266 = new TGeoCombiTrans("", dx,dy,dz,pMatrix267);
   // Combi transformation: 
   dx = 5.186027;
   dy = -33.299522;
   dz = 9.455164;
   // Rotation: 
   thx = 144.000159;    phx = 211.714607;
   thy = 121.401815;    phy = 358.877107;
   thz = 74.213880;    phz = 278.814916;
   TGeoRotation *pMatrix269 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix268 = new TGeoCombiTrans("", dx,dy,dz,pMatrix269);
   // Combi transformation: 
   dx = -5.186027;
   dy = -33.299494;
   dz = 9.455163;
   // Rotation: 
   thx = 36.000027;    phx = 148.279936;
   thy = 121.402050;    phy = 181.117225;
   thz = 74.213941;    phz = 261.179365;
   TGeoRotation *pMatrix271 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix270 = new TGeoCombiTrans("", dx,dy,dz,pMatrix271);
   // Combi transformation: 
   dx = -5.186027;
   dy = -33.299502;
   dz = -9.455163;
   // Rotation: 
   thx = 35.998890;    phx = 328.284975;
   thy = 58.598035;    phy = 181.118854;
   thz = 105.784138;    phz = 261.182313;
   TGeoRotation *pMatrix273 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix272 = new TGeoCombiTrans("", dx,dy,dz,pMatrix273);
   // Combi transformation: 
   dx = 5.186027;
   dy = -33.299502;
   dz = -9.455163;
   // Rotation: 
   thx = 144.001081;    phx = 31.714950;
   thy = 58.597988;    phy = 358.881139;
   thz = 105.784111;    phz = 278.817681;
   TGeoRotation *pMatrix275 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix274 = new TGeoCombiTrans("", dx,dy,dz,pMatrix275);
   // Combi transformation: 
   dx = 2.378000;
   dy = 34.999751;
   dz = -0.000003;
   // Rotation: 
   thx = 0.000076;    phx = 0.000000;
   thy = 90.000076;    phy = 356.107989;
   thz = 90.000005;    phz = 86.107989;
   TGeoRotation *pMatrix277 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix276 = new TGeoCombiTrans("", dx,dy,dz,pMatrix277);
   // Combi transformation: 
   dx = -12.004690;
   dy = 29.055614;
   dz = 15.567327;
   // Rotation: 
   thx = 108.000064;    phx = 31.720002;
   thy = 147.253161;    phy = 152.065125;
   thz = 63.642423;    phz = 112.455498;
   TGeoRotation *pMatrix279 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix278 = new TGeoCombiTrans("", dx,dy,dz,pMatrix279);
   // Combi transformation: 
   dx = 12.004689;
   dy = 29.055613;
   dz = 15.567326;
   // Rotation: 
   thx = 71.999975;    phx = 328.279972;
   thy = 147.253203;    phy = 27.934872;
   thz = 63.642442;    phz = 67.544446;
   TGeoRotation *pMatrix281 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix280 = new TGeoCombiTrans("", dx,dy,dz,pMatrix281);
   // Combi transformation: 
   dx = 12.004688;
   dy = 29.055613;
   dz = -15.567326;
   // Rotation: 
   thx = 72.000006;    phx = 148.279960;
   thy = 32.746759;    phy = 27.934874;
   thz = 116.357538;    phz = 67.544409;
   TGeoRotation *pMatrix283 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix282 = new TGeoCombiTrans("", dx,dy,dz,pMatrix283);
   // Combi transformation: 
   dx = -12.004689;
   dy = 29.055614;
   dz = -15.567326;
   // Rotation: 
   thx = 108.000020;    phx = 211.720014;
   thy = 32.746794;    phy = 152.065106;
   thz = 116.357559;    phz = 112.455543;
   TGeoRotation *pMatrix285 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix284 = new TGeoCombiTrans("", dx,dy,dz,pMatrix285);
   // Combi transformation: 
   dx = -27.580815;
   dy = 19.423452;
   dz = -9.627355;
   // Rotation: 
   thx = 35.999997;    phx = 211.719996;
   thy = 58.680510;    phy = 64.843480;
   thz = 105.925603;    phz = 144.844956;
   TGeoRotation *pMatrix287 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix286 = new TGeoCombiTrans("", dx,dy,dz,pMatrix287);
   // Combi transformation: 
   dx = -27.580814;
   dy = 19.423452;
   dz = 9.627355;
   // Rotation: 
   thx = 35.999994;    phx = 31.719979;
   thy = 121.319482;    phy = 64.843480;
   thz = 74.074390;    phz = 144.844954;
   TGeoRotation *pMatrix289 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix288 = new TGeoCombiTrans("", dx,dy,dz,pMatrix289);
   // Combi transformation: 
   dx = 27.580817;
   dy = 19.423452;
   dz = 9.627354;
   // Rotation: 
   thx = 143.999943;    phx = 328.280005;
   thy = 121.319523;    phy = 115.156443;
   thz = 74.074345;    phz = 35.155015;
   TGeoRotation *pMatrix291 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix290 = new TGeoCombiTrans("", dx,dy,dz,pMatrix291);
   // Combi transformation: 
   dx = 27.580818;
   dy = 19.423452;
   dz = -9.627354;
   // Rotation: 
   thx = 143.999933;    phx = 148.279978;
   thy = 58.680464;    phy = 115.156428;
   thz = 105.925651;    phz = 35.155003;
   TGeoRotation *pMatrix293 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix292 = new TGeoCombiTrans("", dx,dy,dz,pMatrix293);
   // Combi transformation: 
   dx = -19.423802;
   dy = 9.629103;
   dz = -27.579767;
   // Rotation: 
   thx = 119.999134;    phx = 290.906591;
   thy = 68.706365;    phy = 213.910512;
   thz = 141.833355;    phz = 153.638634;
   TGeoRotation *pMatrix295 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix294 = new TGeoCombiTrans("", dx,dy,dz,pMatrix295);
   // Combi transformation: 
   dx = -19.423802;
   dy = 9.629105;
   dz = 27.579765;
   // Rotation: 
   thx = 119.999139;    phx = 110.906501;
   thy = 111.293700;    phy = 213.910468;
   thz = 38.166694;    phz = 153.638641;
   TGeoRotation *pMatrix297 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix296 = new TGeoCombiTrans("", dx,dy,dz,pMatrix297);
   // Combi transformation: 
   dx = 19.423802;
   dy = 9.629105;
   dz = 27.579764;
   // Rotation: 
   thx = 60.000876;    phx = 249.093505;
   thy = 111.293709;    phy = 326.089540;
   thz = 38.166687;    phz = 26.361343;
   TGeoRotation *pMatrix299 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix298 = new TGeoCombiTrans("", dx,dy,dz,pMatrix299);
   // Combi transformation: 
   dx = 19.423802;
   dy = 9.629105;
   dz = -27.579764;
   // Rotation: 
   thx = 60.000878;    phx = 69.093503;
   thy = 68.706285;    phy = 326.089535;
   thz = 141.833311;    phz = 26.361330;
   TGeoRotation *pMatrix301 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix300 = new TGeoCombiTrans("", dx,dy,dz,pMatrix301);
   // Combi transformation: 
   dx = 34.999749;
   dy = 0.000003;
   dz = 2.378000;
   // Rotation: 
   thx = 90.000076;    phx = 90.000076;
   thy = 3.892042;    phy = 179.998959;
   thz = 86.107958;    phz = 0.000081;
   TGeoRotation *pMatrix303 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix302 = new TGeoCombiTrans("", dx,dy,dz,pMatrix303);
   // Combi transformation: 
   dx = -19.423802;
   dy = -9.628753;
   dz = 27.579768;
   // Rotation: 
   thx = 60.000874;    phx = 69.093409;
   thy = 111.293631;    phy = 146.089495;
   thz = 38.166635;    phz = 206.361368;
   TGeoRotation *pMatrix305 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix304 = new TGeoCombiTrans("", dx,dy,dz,pMatrix305);
   // Combi transformation: 
   dx = -19.423802;
   dy = -9.629105;
   dz = -27.579765;
   // Rotation: 
   thx = 60.000854;    phx = 249.093504;
   thy = 68.706300;    phy = 146.089532;
   thz = 141.833299;    phz = 206.361366;
   TGeoRotation *pMatrix307 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix306 = new TGeoCombiTrans("", dx,dy,dz,pMatrix307);
   // Combi transformation: 
   dx = 19.423802;
   dy = -9.629105;
   dz = -27.579764;
   // Rotation: 
   thx = 119.999129;    phx = 110.906504;
   thy = 68.706289;    phy = 33.910473;
   thz = 141.833307;    phz = 333.638666;
   TGeoRotation *pMatrix309 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix308 = new TGeoCombiTrans("", dx,dy,dz,pMatrix309);
   // Combi transformation: 
   dx = 19.423802;
   dy = -9.628755;
   dz = 27.579764;
   // Rotation: 
   thx = 119.999115;    phx = 290.906502;
   thy = 111.293714;    phy = 33.910466;
   thz = 38.166683;    phz = 333.638677;
   TGeoRotation *pMatrix311 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix310 = new TGeoCombiTrans("", dx,dy,dz,pMatrix311);
   // Combi transformation: 
   dx = -27.580814;
   dy = -19.423451;
   dz = 9.627354;
   // Rotation: 
   thx = 143.999987;    phx = 148.279981;
   thy = 121.319514;    phy = 295.156529;
   thz = 74.074409;    phz = 215.155055;
   TGeoRotation *pMatrix313 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix312 = new TGeoCombiTrans("", dx,dy,dz,pMatrix313);
   // Combi transformation: 
   dx = -27.580816;
   dy = -19.423452;
   dz = -9.627354;
   // Rotation: 
   thx = 143.999940;    phx = 328.280039;
   thy = 58.680476;    phy = 295.156470;
   thz = 105.925659;    phz = 215.155046;
   TGeoRotation *pMatrix315 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix314 = new TGeoCombiTrans("", dx,dy,dz,pMatrix315);
   // Combi transformation: 
   dx = 27.580817;
   dy = -19.423453;
   dz = -9.627355;
   // Rotation: 
   thx = 36.000039;    phx = 31.719964;
   thy = 58.680498;    phy = 244.843541;
   thz = 105.925658;    phz = 324.844975;
   TGeoRotation *pMatrix317 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix316 = new TGeoCombiTrans("", dx,dy,dz,pMatrix317);
   // Combi transformation: 
   dx = 27.580817;
   dy = -19.423453;
   dz = 9.627354;
   // Rotation: 
   thx = 36.000048;    phx = 211.720011;
   thy = 121.319517;    phy = 244.843564;
   thz = 74.074351;    phz = 324.844998;
   TGeoRotation *pMatrix319 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix318 = new TGeoCombiTrans("", dx,dy,dz,pMatrix319);
   // Combi transformation: 
   dx = 12.004691;
   dy = -29.055614;
   dz = 15.567327;
   // Rotation: 
   thx = 108.000065;    phx = 211.720015;
   thy = 147.253162;    phy = 332.065141;
   thz = 63.642425;    phz = 292.455511;
   TGeoRotation *pMatrix321 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix320 = new TGeoCombiTrans("", dx,dy,dz,pMatrix321);
   // Combi transformation: 
   dx = -12.004688;
   dy = -29.055614;
   dz = 15.567326;
   // Rotation: 
   thx = 72.000016;    phx = 148.279957;
   thy = 147.253226;    phy = 207.934910;
   thz = 63.642437;    phz = 247.544410;
   TGeoRotation *pMatrix323 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix322 = new TGeoCombiTrans("", dx,dy,dz,pMatrix323);
   // Combi transformation: 
   dx = -12.004690;
   dy = -29.055614;
   dz = -15.567327;
   // Rotation: 
   thx = 71.999938;    phx = 328.279996;
   thy = 32.746826;    phy = 207.934861;
   thz = 116.357565;    phz = 247.544494;
   TGeoRotation *pMatrix325 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix324 = new TGeoCombiTrans("", dx,dy,dz,pMatrix325);
   // Combi transformation: 
   dx = 12.004689;
   dy = -29.055614;
   dz = -15.567326;
   // Rotation: 
   thx = 108.000023;    phx = 31.720016;
   thy = 32.746791;    phy = 332.065118;
   thz = 116.357554;    phz = 292.455545;
   TGeoRotation *pMatrix327 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix326 = new TGeoCombiTrans("", dx,dy,dz,pMatrix327);
   // Combi transformation: 
   dx = -2.378000;
   dy = -34.999750;
   dz = 0.000000;
   // Rotation: 
   thx = 0.000003;    phx = 0.000000;
   thy = 89.999997;    phy = 176.107973;
   thz = 90.000000;    phz = 266.107973;
   TGeoRotation *pMatrix329 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix328 = new TGeoCombiTrans("", dx,dy,dz,pMatrix329);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix331 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix330 = new TGeoCombiTrans("", dx,dy,dz,pMatrix331);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 39.100000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix333 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix332 = new TGeoCombiTrans("", dx,dy,dz,pMatrix333);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = -39.100000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 270.000000;
   thz = 180.000000;    phz = 0.000000;
   TGeoRotation *pMatrix335 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix334 = new TGeoCombiTrans("", dx,dy,dz,pMatrix335);
   // Combi transformation: 
   dx = -39.100000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 90.000000;
   thy = 180.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 180.000000;
   TGeoRotation *pMatrix337 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix336 = new TGeoCombiTrans("", dx,dy,dz,pMatrix337);
/****************************************************************************/

   // Get the top Volume
    TGeoVolume *top =  gGeoManager->GetTopVolume();

   // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
   // Shape: CBSphereWorld type: TGeoSphere
   rmin   = 24.250000;
   rmax   = 70.000000;
   theta1 = 0.000000;
   theta2 = 180.000000;
   phi1   = 0.000000;
   phi2   = 360.000000;
   TGeoShape *pCBSphereWorld_2 = new TGeoSphere("CBSphereWorld",rmin,rmax,theta1, theta2,phi1,phi2);
   // Volume: CBLogWorld
   TGeoVolume*
   pCBLogWorld_82bb8e8 = new TGeoVolume("CBLogWorld",pCBSphereWorld_2, pMed1);
   pCBLogWorld_82bb8e8->SetVisLeaves(kTRUE);
   top->AddNode(pCBLogWorld_82bb8e8, 0, pMatrix10);

   // Shape: PGON type: TGeoPgon
   phi1    = 0.000000;
   dphi    = 360.000000;
   nedges = 5;
   nz      = 2;
   TGeoPgon *pgon = new TGeoPgon("PGON",phi1,dphi,nedges,nz);
      z     = -10.000000;
      rmin  = 0.000000;
      rmax  = 3.688000;
   pgon->DefineSection(0, z,rmin,rmax);
      z     = 10.000000;
      rmin  = 0.000000;
      rmax  = 6.638000;
   pgon->DefineSection(1, z,rmin,rmax);
   TGeoShape *pPGON_3 = pgon;
   // Volume: crystalLogNAJA
   TGeoVolume *
   pcrystalLogNAJA_82bbfd8 = new TGeoVolume("crystalLogNAJA",pPGON_3, pMed9);
   AddSensitiveVolume(pcrystalLogNAJA_82bbfd8);
   pcrystalLogNAJA_82bbfd8->SetVisLeaves(kTRUE);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJA_82bbfd8, 0, pMatrix12);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJA_82bbfd8, 1, pMatrix14);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJA_82bbfd8, 2, pMatrix16);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJA_82bbfd8, 3, pMatrix18);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJA_82bbfd8, 4, pMatrix20);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJA_82bbfd8, 5, pMatrix22);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJA_82bbfd8, 6, pMatrix24);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJA_82bbfd8, 7, pMatrix26);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJA_82bbfd8, 8, pMatrix28);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJA_82bbfd8, 9, pMatrix30);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJA_82bbfd8, 10, pMatrix32);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJA_82bbfd8, 11, pMatrix34);

   fNbOfSensitiveVol+=12;

   // Shape: TRB1 type: TGeoTrap
   dz     = 10.000000;
   theta  = 0.000000;
   phi    = 0.000000;
   h1     = 1.542000;
   bl1    = 4.848000;
   tl1    = 2.658000;
   alpha1 = 0.000000;
   h2     = 2.778000; 
   bl2    = 8.726000;
   tl2    = 4.780000;
   alpha2 = 0.000000;
   TGeoShape *pTRB1_4 = new TGeoTrap("TRB1", dz,theta,phi,h1,bl1,tl1,alpha1,h2,bl2,tl2,alpha2);
   // Shape: TRB2 type: TGeoTrap
   dz     = 10.000000;
   theta  = 0.000000;
   phi    = 0.000000;
   h1     = 1.863000;
   bl1    = 4.850000;
   tl1    = 2.162000;
   alpha1 = 0.000000;
   h2     = 3.353000;
   bl2    = 8.730000;
   tl2    = 3.892000;
   alpha2 = 0.000000;
   TGeoShape *pTRB2_5 = new TGeoTrap("TRB2", dz,theta,phi,h1,bl1,tl1,alpha1,h2,bl2,tl2,alpha2);
   // Combi transformation: TRB
   dx = 0.000000;
   dy = -4.748000;
   dz = -0.292480;
   // Rotation: 
   thx = 90.000000;    phx = 180.000000;
   thy = 97.775000;    phy = 270.000000;
   thz = 7.775000;    phz = 270.000000;
   TGeoRotation *pMatrix3 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix2 = new TGeoCombiTrans("TRB", dx,dy,dz,pMatrix3);
   pBoolNode = new TGeoUnion(pTRB1_4,pTRB2_5,0,pMatrix2);
   // Shape: TRB type: TGeoCompositeShape
   TGeoShape *pTRB_6 = new TGeoCompositeShape("TRB", pBoolNode);
   pTRB_6->SetTitle("TRB1+TRB2:TRB");
   // Volume: crystalLogNAJB
   TGeoVolume*
   pcrystalLogNAJB_82bc628 = new TGeoVolume("crystalLogNAJB",pTRB_6, pMed9);
   AddSensitiveVolume(pcrystalLogNAJA_82bbfd8);
   pcrystalLogNAJB_82bc628->SetVisLeaves(kTRUE);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 0, pMatrix36);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 1, pMatrix38);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 2, pMatrix40);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 3, pMatrix42);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 4, pMatrix44);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 5, pMatrix46);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 6, pMatrix48);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 7, pMatrix50);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 8, pMatrix52);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 9, pMatrix54);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 10, pMatrix56);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 11, pMatrix58);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 12, pMatrix60);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 13, pMatrix62);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 14, pMatrix64);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 15, pMatrix66);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 16, pMatrix68);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 17, pMatrix70);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 18, pMatrix72);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 19, pMatrix74);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 20, pMatrix76);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 21, pMatrix78);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 22, pMatrix80);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 23, pMatrix82);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 24, pMatrix84);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 25, pMatrix86);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 26, pMatrix88);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 27, pMatrix90);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 28, pMatrix92);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 29, pMatrix94);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 30, pMatrix96);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 31, pMatrix98);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 32, pMatrix100);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 33, pMatrix102);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 34, pMatrix104);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 35, pMatrix106);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 36, pMatrix108);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 37, pMatrix110);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 38, pMatrix112);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 39, pMatrix114);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 40, pMatrix116);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 41, pMatrix118);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 42, pMatrix120);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 43, pMatrix122);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 44, pMatrix124);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 45, pMatrix126);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 46, pMatrix128);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 47, pMatrix130);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 48, pMatrix132);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 49, pMatrix134);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 50, pMatrix136);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 51, pMatrix138);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 52, pMatrix140);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 53, pMatrix142);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 54, pMatrix144);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 55, pMatrix146);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 56, pMatrix148);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 57, pMatrix150);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 58, pMatrix152);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJB_82bc628, 59, pMatrix154);
   fNbOfSensitiveVol+=60;
   // Shape: TRC1 type: TGeoTrap
   dz     = 10.000000;
   theta  = 0.000000;
   phi    = 0.000000;
   h1     = 2.209000;
   bl1    = 3.614000;
   tl1    = 4.265000;
   alpha1 = 0.000000;
   h2     = 3.976000;
   bl2    = 6.505000;
   tl2    = 7.676000;
   alpha2 = 0.000000;
   TGeoShape *pTRC1_7 = new TGeoTrap("TRC1", dz,theta,phi,h1,bl1,tl1,alpha1,h2,bl2,tl2,alpha2);
   // Shape: TRC2 type: TGeoTrap
   dz     = 10.000000;
   theta  = 0.000000;
   phi    = 0.000000;
   h1     = 0.808000;
   bl1    = 4.302000;
   tl1    = 0.001000;
   alpha1 = 0.000000;
   h2     = 1.454000;
   bl2    = 7.743504;
   tl2    = 0.005324;
   alpha2 = 0.000000;
   TGeoShape *pTRC2_8 = new TGeoTrap("TRC2", dz,theta,phi,h1,bl1,tl1,alpha1,h2,bl2,tl2,alpha2);
   // Combi transformation: TRC_21
   dx = 0.000000;
   dy = 4.229809;
   dz = 0.040235;
   // Rotation: 
   thx = 90.000000;    phx = 360.000000;
   thy = 96.883000;    phy = 90.000000;
   thz = 6.883000;    phz = 90.000000;
   TGeoRotation *pMatrix5 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix4 = new TGeoCombiTrans("TRC_21", dx,dy,dz,pMatrix5);
   pBoolNode = new TGeoUnion(pTRC1_7,pTRC2_8,0,pMatrix4);
   // Shape: TRC_21 type: TGeoCompositeShape
   TGeoShape *pTRC_21_9 = new TGeoCompositeShape("TRC_21", pBoolNode);
   pTRC_21_9->SetTitle("TRC1+TRC2:TRC_21");
   // Shape: TRC3 type: TGeoTrap
   dz     = 10.000000;
   theta  = 0.000000;
   phi    = 0.000000;
   h1     = 1.046000;
   bl1    = 3.618000;
   tl1    = 0.001000;
   alpha1 = 0.000000;
   h2     = 1.883000;
   bl2    = 6.515776;
   tl2    = 0.003459;
   alpha2 = 0.000000;
   TGeoShape *pTRC3_10 = new TGeoTrap("TRC3", dz,theta,phi,h1,bl1,tl1,alpha1,h2,bl2,tl2,alpha2);
   // Combi transformation: TRC
   dx = 0.000000;
   dy = -4.557794;
   dz = -0.043355;
   // Rotation: 
   thx = 90.000000;    phx = 180.000000;
   thy = 97.428000;    phy = 270.000000;
   thz = 7.428000;    phz = 270.000000;
   TGeoRotation *pMatrix7 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix6 = new TGeoCombiTrans("TRC", dx,dy,dz,pMatrix7);
   pBoolNode = new TGeoUnion(pTRC_21_9,pTRC3_10,0,pMatrix6);
   // Shape: TRC type: TGeoCompositeShape
   TGeoShape *pTRC_11 = new TGeoCompositeShape("TRC", pBoolNode);
   pTRC_11->SetTitle("TRC_21+TRC3:TRC");
   // Volume: crystalLogNAJC
   TGeoVolume*
   pcrystalLogNAJC_82bcfa8 = new TGeoVolume("crystalLogNAJC",pTRC_11, pMed9);
   AddSensitiveVolume(pcrystalLogNAJC_82bcfa8);
   pcrystalLogNAJC_82bcfa8->SetVisLeaves(kTRUE);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 0, pMatrix156);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 1, pMatrix158);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 2, pMatrix160);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 3, pMatrix162);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 4, pMatrix164);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 5, pMatrix166);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 6, pMatrix168);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 7, pMatrix170);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 8, pMatrix172);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 9, pMatrix174);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 10, pMatrix176);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 11, pMatrix178);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 12, pMatrix180);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 13, pMatrix182);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 14, pMatrix184);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 15, pMatrix186);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 16, pMatrix188);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 17, pMatrix190);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 18, pMatrix192);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 19, pMatrix194);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 20, pMatrix196);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 21, pMatrix198);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 22, pMatrix200);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 23, pMatrix202);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 24, pMatrix204);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 25, pMatrix206);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 26, pMatrix208);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 27, pMatrix210);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 28, pMatrix212);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 29, pMatrix214);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 30, pMatrix216);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 31, pMatrix218);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 32, pMatrix220);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 33, pMatrix222);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 34, pMatrix224);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 35, pMatrix226);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 36, pMatrix228);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 37, pMatrix230);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 38, pMatrix232);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 39, pMatrix234);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 40, pMatrix236);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 41, pMatrix238);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 42, pMatrix240);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 43, pMatrix242);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 44, pMatrix244);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 45, pMatrix246);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 46, pMatrix248);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 47, pMatrix250);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 48, pMatrix252);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 49, pMatrix254);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 50, pMatrix256);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 51, pMatrix258);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 52, pMatrix260);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 53, pMatrix262);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 54, pMatrix264);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 55, pMatrix266);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 56, pMatrix268);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 57, pMatrix270);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 58, pMatrix272);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJC_82bcfa8, 59, pMatrix274);
   fNbOfSensitiveVol+=60;
   // Shape: TRAP1 type: TGeoTrap
   dz     = 10.000000;
   theta  = 0.000000;
   phi    = 0.000000;
   h1     = 1.702000;
   bl1    = 5.068000;
   tl1    = 2.162000;
   alpha1 = 0.000000;
   h2     = 3.064000;
   bl2    = 9.122000;
   tl2    = 3.890000;
   alpha2 = 0.000000;
   TGeoShape *pTRAP1_12 = new TGeoTrap("TRAP1", dz,theta,phi,h1,bl1,tl1,alpha1,h2,bl2,tl2,alpha2);
   // Shape: TRAP2 type: TGeoTrap
   dz     = 10.000000;
   theta  = 0.000000;
   phi    = 0.000000;
   h1     = 1.702000;
   bl1    = 5.068000;
   tl1    = 2.162000;
   alpha1 = 0.000000;
   h2     = 3.064000;
   bl2    = 9.122000;
   tl2    = 3.890000;
   alpha2 = 0.000000;
   TGeoShape *pTRAP2_13 = new TGeoTrap("TRAP2", dz,theta,phi,h1,bl1,tl1,alpha1,h2,bl2,tl2,alpha2);
   // Combi transformation: TRAP
   dx = 0.000000;
   dy = -4.745065;
   dz = -0.322321;
   // Rotation: 
   thx = 90.000000;    phx = 180.000000;
   thy = 97.772000;    phy = 270.000000;
   thz = 7.772000;    phz = 270.000000;
   TGeoRotation *pMatrix9 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans* pMatrix8 = new TGeoCombiTrans("TRAP", dx,dy,dz,pMatrix9);
   pBoolNode = new TGeoUnion(pTRAP1_12,pTRAP2_13,0,pMatrix8);
   // Shape: TRAP type: TGeoCompositeShape
   TGeoShape *pTRAP_14 = new TGeoCompositeShape("TRAP", pBoolNode);
   pTRAP_14->SetTitle("TRAP1+TRAP2:TRAP");
   // Volume: crystalLogNAJD
   TGeoVolume*
   pcrystalLogNAJD_82bd5a8 = new TGeoVolume("crystalLogNAJD",pTRAP_14, pMed9);
   AddSensitiveVolume(pcrystalLogNAJD_82bd5a8);
   pcrystalLogNAJD_82bd5a8->SetVisLeaves(kTRUE);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 0, pMatrix276);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 1, pMatrix278);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 2, pMatrix280);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 3, pMatrix282);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 4, pMatrix284);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 5, pMatrix286);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 6, pMatrix288);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 7, pMatrix290);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 8, pMatrix292);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 9, pMatrix294);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 10, pMatrix296);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 11, pMatrix298);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 12, pMatrix300);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 13, pMatrix302);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 14, pMatrix304);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 15, pMatrix306);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 16, pMatrix308);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 17, pMatrix310);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 18, pMatrix312);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 19, pMatrix314);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 20, pMatrix316);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 21, pMatrix318);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 22, pMatrix320);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 23, pMatrix322);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 24, pMatrix324);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 25, pMatrix326);
   pCBLogWorld_82bb8e8->AddNode(pcrystalLogNAJD_82bd5a8, 26, pMatrix328);
   fNbOfSensitiveVol+=27;
   // Shape: CBReactionChamber type: TGeoSphere
   rmin   = 24.250000;
   rmax   = 24.500000;
   theta1 = 6.423161;
   theta2 = 173.576839;
   phi1   = 0.000000;
   phi2   = 360.000000;
   TGeoShape *pCBReactionChamber_15 = new TGeoSphere("CBReactionChamber",rmin,rmax,theta1, theta2,phi1,phi2);
   // Volume: CBChamberLog
   TGeoVolume*
   pCBChamberLog_82bd740 = new TGeoVolume("CBChamberLog",pCBReactionChamber_15, pMed21);
   pCBChamberLog_82bd740->SetVisLeaves(kTRUE);
   pCBLogWorld_82bb8e8->AddNode(pCBChamberLog_82bd740, 0, pMatrix330);
   // Shape: CBConicalTube type: TGeoConeSeg
   dz    = 14.600000;
   rmin1 = 2.730000;
   rmax1 = 2.855000;
   rmin2 = 5.125000;
   rmax2 = 5.250000;
   phi1  = 0.000000;
   phi2  = 360.000000;
   TGeoShape *pCBConicalTube_16 = new TGeoConeSeg("CBConicalTube", dz,rmin1,rmax1,rmin2,rmax2,phi1,phi2);
   // Volume: CBConicalTubeLog
   TGeoVolume*
   pCBConicalTubeLog_82bd8d0 = new TGeoVolume("CBConicalTubeLog",pCBConicalTube_16, pMed21);
   pCBConicalTubeLog_82bd8d0->SetVisLeaves(kTRUE);
   pCBLogWorld_82bb8e8->AddNode(pCBConicalTubeLog_82bd8d0, 0, pMatrix332);
   pCBLogWorld_82bb8e8->AddNode(pCBConicalTubeLog_82bd8d0, 1, pMatrix334);
   pCBLogWorld_82bb8e8->AddNode(pCBConicalTubeLog_82bd8d0, 2, pMatrix336);

}


TGeoRotation* R3BCal::createMatrix( Double_t phi, Double_t theta, Double_t psi){

  // Rotation
  TGeoRotation * matrix = new TGeoRotation("");

  Double_t rm[9];

  Double_t degrad = TMath::Pi()/180.;

  // define trigonometry
  Double_t  sinPhi   = sin( degrad*phi )  ;
  Double_t  cosPhi   = TMath::Cos( degrad*phi )  ;
  Double_t  sinTheta = sin( degrad*theta );
  Double_t  cosTheta = TMath::Cos( degrad*theta );
  Double_t  sinPsi   = sin( degrad*psi )  ;
  Double_t  cosPsi   = TMath::Cos( degrad*psi )  ;


  // filling from Euler definition a la G4 !

  rm[0] =   cosPsi * cosPhi - cosTheta * sinPhi * sinPsi;
  rm[1] =   cosPsi * sinPhi + cosTheta * cosPhi * sinPsi;
  rm[2] =   sinPsi * sinTheta;

  rm[3] = - sinPsi * cosPhi - cosTheta * sinPhi * cosPsi;
  rm[4] = - sinPsi * sinPhi + cosTheta * cosPhi * cosPsi;
  rm[5] =   cosPsi * sinTheta;

  rm[6] =   sinTheta * sinPhi;
  rm[7] = - sinTheta * cosPhi;
  rm[8] =   cosTheta;


  matrix->SetMatrix( (const Double_t*) &rm[0] );

 return matrix;

}



ClassImp(R3BCal)
