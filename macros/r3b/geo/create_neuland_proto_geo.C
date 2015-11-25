#include <iomanip>
#include <iostream>
#include "TGeoManager.h"
#include "TMath.h"

// Create Matrix Unity
TGeoRotation *fGlobalRot = new TGeoRotation();

// Create a null translation
TGeoTranslation *fGlobalTrans = new TGeoTranslation();
TGeoRotation *fRefRot = NULL;

TGeoManager*  gGeoMan = NULL;

Double_t fThetaX = 0.;
Double_t fThetaY = 0.;
Double_t fThetaZ = 0.;
Double_t fPhi   = 0.;
Double_t fTheta = 0.;
Double_t fPsi   = 0.;
Double_t fX = 0.;
Double_t fY = 0.;
Double_t fZ = 1177.5;
Bool_t fLocalTrans = kFALSE;
Bool_t fLabTrans = kTRUE;


TGeoCombiTrans* GetGlobalPosition(TGeoCombiTrans *fRef);


void create_neuland_geo(const char* geoTag)
{

  fGlobalTrans->SetTranslation(0.0,0.0,0.0);

  Double_t neuLAND_paddle_dimx   = 125.;   // half of the length [cm]
  Double_t neuLAND_paddle_dimy   = 2.4;    // half of the width [cm]
  Double_t neuLAND_paddle_dimz   = 2.4;    // half of the depth [cm]
  Double_t neuLAND_depth_dim     = 37.5;   // half detector depth [cm]
  Double_t neuLAND_gap_dim       = 0.03;   // total detector depth [cm]
  Double_t neuLAND_wrapping1_dim = 0.02;   // thickness of wrapping material [cm]
  Double_t neuLAND_wrapping2_dim = 0.05;   // thickness of wrapping material [cm]
  

  // -------   Load media from media file   -----------------------------------
  FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  TString geoPath = gSystem->Getenv("VMCWORKDIR");
  TString medFile = geoPath + "/geometry/media_r3b.geo";
  geoFace->setMediaFile(medFile);
  geoFace->readMedia();
  gGeoMan = gGeoManager;
  // --------------------------------------------------------------------------


  // -------   Geometry file name (output)   ----------------------------------
  TString geoFileName = geoPath + "/geometry/neuland_proto_";
  geoFileName = geoFileName + geoTag + ".geo.root";
  // --------------------------------------------------------------------------


  // -----------------   Get and create the required media    -----------------
  FairGeoMedia*   geoMedia = geoFace->getMedia();
  FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

  FairGeoMedium* mBC408      = geoMedia->getMedium("BC408");
  if ( ! mBC408 ) Fatal("Main", "FairMedium BC408 not found");
  geoBuild->createMedium(mBC408);
  TGeoMedium* pMed37 = gGeoMan->GetMedium("BC408");
  if ( ! pMed37 ) Fatal("Main", "Medium BC408 not found");

  FairGeoMedium* mCH2      = geoMedia->getMedium("CH2");
  if ( ! mCH2 ) Fatal("Main", "FairMedium CH2 not found");
  geoBuild->createMedium(mCH2);
  TGeoMedium* pMed38 = gGeoMan->GetMedium("CH2");
  if ( ! pMed38 ) Fatal("Main", "Medium CH2 not found");

  FairGeoMedium* mAl      = geoMedia->getMedium("aluminium");
  if ( ! mAl ) Fatal("Main", "FairMedium aluminium not found");
  geoBuild->createMedium(mAl);
  TGeoMedium* pMedAl = gGeoMan->GetMedium("aluminium");
  if ( ! pMedAl ) Fatal("Main", "Medium aluminium not found");
  // --------------------------------------------------------------------------



  // --------------   Create geometry and top volume  -------------------------
  gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoMan->SetName("NEULANDProtogeom");
  TGeoVolume* top = new TGeoVolumeAssembly("TOP");
  gGeoMan->SetTopVolume(top);
  // --------------------------------------------------------------------------



  Double_t tx,ty,tz;
  //------------------ BC408 paddles -----------------------------------------
  TGeoVolume *padle_h_box5 = gGeoManager->MakeBox("padle_h_box5", pMed37,
						  neuLAND_paddle_dimx,
						  neuLAND_paddle_dimy,
						  neuLAND_paddle_dimz);

  //------------------ wrapping Alu------------------------------------------
  TGeoShape* padle_h_box1 = new TGeoBBox("padle_h_box1",
					 neuLAND_paddle_dimx, 
					 neuLAND_paddle_dimy + neuLAND_wrapping1_dim, 
					 neuLAND_paddle_dimz + neuLAND_wrapping1_dim);
  TGeoShape* padle_h_box2 = new TGeoBBox("padle_h_box2",
					 neuLAND_paddle_dimx, 
					 neuLAND_paddle_dimy, 
					 neuLAND_paddle_dimz);
  
  // Create a composite shape
  TGeoCompositeShape *wrapping1 = new TGeoCompositeShape("diffbox", "padle_h_box1 - padle_h_box2");
  TGeoVolume *bvol1 = new TGeoVolume("wrapping1", wrapping1, pMedAl);

  //------------------ wrapping Tape------------------------------------------
  TGeoShape* padle_h_box3 = new TGeoBBox("padle_h_box3",
					 neuLAND_paddle_dimx,
					 neuLAND_paddle_dimy + neuLAND_wrapping1_dim + neuLAND_wrapping2_dim, 
					 neuLAND_paddle_dimz + neuLAND_wrapping1_dim + neuLAND_wrapping2_dim);
  TGeoShape* padle_h_box4 = new TGeoBBox("padle_h_box4",
					 neuLAND_paddle_dimx, 
					 neuLAND_paddle_dimy + neuLAND_wrapping1_dim, 
					 neuLAND_paddle_dimz + neuLAND_wrapping1_dim);

  // Create a composite shape
  TGeoCompositeShape *wrapping2 = new TGeoCompositeShape("diffbox", "padle_h_box3 - padle_h_box4");
  TGeoVolume *bvol2 = new TGeoVolume("wrapping2", wrapping2, pMed38);



  // Make the elementary assembly of the whole structure
  TGeoVolume *aLand = new TGeoVolumeAssembly("ALAND");

//   Double_t total_dimx = neuLAND_paddle_dimx;
  Double_t total_dimx = 25.;
  Double_t total_dimy = neuLAND_paddle_dimy +
    neuLAND_wrapping1_dim + 
    neuLAND_wrapping2_dim + 
    neuLAND_gap_dim;
  Double_t total_dimz = neuLAND_paddle_dimz +
    neuLAND_wrapping1_dim +
    neuLAND_wrapping2_dim +
    neuLAND_gap_dim;
  
  //paddles
  TGeoRotation *zeroRotation = new TGeoRotation();
  zeroRotation->RotateX(0.);
  zeroRotation->RotateY(0.);
  zeroRotation->RotateZ(0.);

  TGeoRotation *rot1 = new TGeoRotation();
  rot1->RotateX(0.);
  rot1->RotateY(0.);
  rot1->RotateZ(90.);

  Double_t xx = 0.;
  Double_t yy = 0.;
  Double_t zz = 0.;
  
  aLand->AddNode(padle_h_box5, 1, new TGeoCombiTrans(xx, yy, zz, zeroRotation));
  aLand->AddNode(bvol1, 1, new TGeoCombiTrans(xx, yy, zz, zeroRotation));
  aLand->AddNode(bvol2, 1, new TGeoCombiTrans(xx, yy, zz, zeroRotation));

  TGeoVolume *cell = new TGeoVolumeAssembly("CELL");

  Int_t nindex = 0, i = 0;
  tx = 0.;
  tz = -neuLAND_depth_dim + total_dimz;
  for(tz = -neuLAND_depth_dim + total_dimz; tz < neuLAND_depth_dim; tz += total_dimz*2) {
    i += 1;
    for (ty = -total_dimx + total_dimy; ty < total_dimx; ty += total_dimy*2) {
      nindex++;
      if(1 == nindex || 10 == nindex || 141 == nindex || 150 == nindex) {
	continue;
      } else {
        cell->AddNode(aLand, nindex, new TGeoCombiTrans(ty, tx, tz, rot1));
      }
    }
  }

  tx = 0.0;
  ty = 0.0;
  tz = fZ;
  TGeoCombiTrans *t0 = new TGeoCombiTrans(tx, ty, tz, zeroRotation);

  top->AddNode(cell, 1, /*GetGlobalPosition(t0)*/t0);



  // ---------------   Finish   -----------------------------------------------
  gGeoMan->CloseGeometry();
  gGeoMan->CheckOverlaps(0.001);
  gGeoMan->PrintOverlaps();
  gGeoMan->Test();

  TFile* geoFile = new TFile(geoFileName, "RECREATE");
  top->Write();
  geoFile->Close();
  // --------------------------------------------------------------------------
}



TGeoCombiTrans* GetGlobalPosition(TGeoCombiTrans *fRef)
{
  if (fLocalTrans == kTRUE ) {
    
    if ( ( fThetaX == 0 )  && ( fThetaY==0 )  && ( fThetaZ == 0 )
	 &&
	 ( fX == 0 ) && ( fY == 0 ) && ( fZ == 0 )
	 )  return fRef;
    
    
    // X axis
    Double_t xAxis[3] = { 1. , 0. , 0. };
    Double_t yAxis[3] = { 0. , 1. , 0. };
    Double_t zAxis[3] = { 0. , 0. , 1. };
    // Reference Rotation
    fRefRot = fRef->GetRotation();
    
    if (fRefRot) {
      Double_t mX[3] = {0.,0.,0.};
      Double_t mY[3] = {0.,0.,0.};
      Double_t mZ[3] = {0.,0.,0.};
      
      fRefRot->LocalToMasterVect(xAxis,mX);
      fRefRot->LocalToMasterVect(yAxis,mY);
      fRefRot->LocalToMasterVect(zAxis,mZ);
      
      Double_t a[4]={ mX[0],mX[1],mX[2], fThetaX };
      Double_t b[4]={ mY[0],mY[1],mY[2], fThetaY };
      Double_t c[4]={ mZ[0],mZ[1],mZ[2], fThetaZ };
      
      ROOT::Math::AxisAngle aX(a,a+4);
      ROOT::Math::AxisAngle aY(b,b+4);
      ROOT::Math::AxisAngle aZ(c,c+4);
      
      ROOT::Math::Rotation3D fMatX( aX );
      ROOT::Math::Rotation3D fMatY( aY );
      ROOT::Math::Rotation3D fMatZ( aZ );
      
      ROOT::Math::Rotation3D  fRotXYZ = (fMatZ * (fMatY * fMatX));
      
      //cout << fRotXYZ << endl;
      
      Double_t fRotable[9]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
      fRotXYZ.GetComponents(
			    fRotable[0],fRotable[3],fRotable[6],
			    fRotable[1],fRotable[4],fRotable[7],
			    fRotable[2],fRotable[5],fRotable[8]
			    );
      TGeoRotation *pRot = new TGeoRotation();
      pRot->SetMatrix(fRotable);
      TGeoCombiTrans *pTmp = new TGeoCombiTrans(*fGlobalTrans,*pRot);
      
      // ne peut pas etre applique ici
      // il faut differencier trans et rot dans la multi.
      TGeoRotation rot_id;
      rot_id.SetAngles(0.0,0.0,0.0);
      
      TGeoCombiTrans c1;
      c1.SetRotation(rot_id);
      const Double_t *t = pTmp->GetTranslation();
      c1.SetTranslation(t[0],t[1],t[2]);
      
      TGeoCombiTrans c2;
      c2.SetRotation(rot_id);
      const Double_t *tt = fRefRot->GetTranslation();
      c2.SetTranslation(tt[0],tt[1],tt[2]);
      
      TGeoCombiTrans cc = c1 * c2 ;
      
      TGeoCombiTrans c3;
      c3.SetRotation(pTmp->GetRotation());
      TGeoCombiTrans c4;
      c4.SetRotation(fRefRot);
      
      TGeoCombiTrans ccc = c3 * c4;
      
      TGeoCombiTrans pGlobal;
      pGlobal.SetRotation(ccc.GetRotation());
      const Double_t *allt = cc.GetTranslation();
      pGlobal.SetTranslation(allt[0],allt[1],allt[2]);
      
      return  ( new TGeoCombiTrans( pGlobal ) );
      
    }else{
      
      cout << "-E- R3BDetector::GetGlobalPosition() \
	      No. Ref. Transformation defined ! " << endl;
      cout << "-E- R3BDetector::GetGlobalPosition() \
	      cannot create Local Transformation " << endl;
      return NULL;
    } //! fRefRot
    
  } else {
    // Lab Transf.
    if ( ( fPhi == 0 )  && ( fTheta==0 )  && ( fPsi == 0 )
	 &&
	 ( fX == 0 ) && ( fY == 0 ) && ( fZ == 0 )
	 )  return fRef;
    
    
    return ( new TGeoCombiTrans(*fGlobalTrans,*fGlobalRot) );
    
  }
}

