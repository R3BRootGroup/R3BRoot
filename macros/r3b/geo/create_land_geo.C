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
Double_t fZ = 1050.;
Bool_t fLocalTrans = kFALSE;
Bool_t fLabTrans = kFALSE;


TGeoCombiTrans* GetGlobalPosition(TGeoCombiTrans *fRef);


void create_land_geo(const char* geoTag)
{

  fGlobalTrans->SetTranslation(0.0,0.0,0.0);

  Double_t padle_h_dim1x = 100.0;
  Double_t padle_h_dim1y = 5.1;
  Double_t padle_h_dim1z = 5.1;
  Double_t padle_h_dim2x = 100.0;
  Double_t padle_h_dim2y = 5.0;
  Double_t padle_h_dim2z = 5.0;


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
  TString geoFileName = geoPath + "/geometry/land_";
  geoFileName = geoFileName + geoTag + ".geo.root";
  // --------------------------------------------------------------------------


  // -----------------   Get and create the required media    -----------------
  FairGeoMedia*   geoMedia = geoFace->getMedia();
  FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

  FairGeoMedium* mBC408      = geoMedia->getMedium("BC408");
  if ( ! mBC408 ) Fatal("Main", "FairMedium BC408 not found");
  geoBuild->createMedium(mBC408);
  TGeoMedium* BC408 = gGeoMan->GetMedium("BC408");
  if ( ! BC408 ) Fatal("Main", "Medium BC408 not found");

  FairGeoMedium* mIron      = geoMedia->getMedium("iron");
  if ( ! mIron ) Fatal("Main", "FairMedium iron not found");
  geoBuild->createMedium(mIron);
  TGeoMedium* Iron = gGeoMan->GetMedium("iron");
  if ( ! Iron ) Fatal("Main", "Medium iron not found");
  // --------------------------------------------------------------------------



  // --------------   Create geometry and top volume  -------------------------
  gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoMan->SetName("LANDgeom");
  TGeoVolume* top = new TGeoVolumeAssembly("TOP");
  gGeoMan->SetTopVolume(top);
  // --------------------------------------------------------------------------



  Double_t dx1, dx2, dy1, dy2, dz;
  Double_t tx,ty,tz;

  // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
  //------------------ Iron shell ------------------------------------------
  TGeoShape* padle_h_box1 = new TGeoBBox("padle_h_box1",
					 padle_h_dim1x,
					 padle_h_dim1y,
					 padle_h_dim1z);
  TGeoShape* padle_h_box2 = new TGeoBBox("padle_h_box2",
					 padle_h_dim2x,
					 padle_h_dim2y,
					 padle_h_dim2z);

  // Create a composite shape
  TGeoCompositeShape *sheetbox = new TGeoCompositeShape("diffbox",
							"padle_h_box1 - padle_h_box2");

  //------------------ Iron endpoints --------------------------------------
  TGeoVolume *bvol = new TGeoVolume("sheetbox",sheetbox,Iron);
  //end-cap list
  dx1 = 5.10;
  dx2 = 2.51;
  dy1 = 5.10;
  dy2 = 2.51;
  dz  = 5.00;
  TGeoVolume *trap = gGeoManager->MakeTrd2("atrap", BC408,
					   dx1, dx2, dy1, dy2, dz);
 
  //------------------ Iron sheets -----------------------------------------
  Double_t padle_h_dim3x = 100.0;
  Double_t padle_h_dim3y = 5.0;
  Double_t padle_h_dim3z = 0.125;
  TGeoVolume *padle_h_box3 = gGeoManager->MakeBox("padle_h_box3",Iron,
          padle_h_dim3x, padle_h_dim3y, padle_h_dim3z);

  Double_t padle_h_dim4x = 100.0;
  Double_t padle_h_dim4y = 5.0;
  Double_t padle_h_dim4z = 0.25;
  TGeoVolume *padle_h_box4 = gGeoManager->MakeBox("padle_h_box4",Iron,
          padle_h_dim4x, padle_h_dim4y, padle_h_dim4z);

  //------------------ BC408 sheets -----------------------------------------
  Double_t padle_h_dim5x = 100.0;
  Double_t padle_h_dim5y = 5.0;
  Double_t padle_h_dim5z = 0.25;
  TGeoVolume *padle_h_box5 = gGeoManager->MakeBox("padle_h_box5",
						  BC408,
						  padle_h_dim5x,
						  padle_h_dim5y,
						  padle_h_dim5z);



  // Make the elementary assembly of the whole structure
  //------------------ Paddle shell assembly --------------------------------
  TGeoVolume *aLand = new TGeoVolumeAssembly("ALAND");
  // Add Iron shell
  TGeoRotation *zeroRotation= new TGeoRotation();
  zeroRotation->RotateX(0.);
  zeroRotation->RotateY(0.);
  zeroRotation->RotateZ(0.);
  Double_t xx = 0.;
  Double_t yy = 0.;
  Double_t zz = 0.;
  aLand->AddNode(bvol,1,new TGeoCombiTrans(xx,yy,zz,zeroRotation));

  // Add Iron endpoints
  TGeoRotation *rot1 = new TGeoRotation();
  rot1->RotateX(0.);
  rot1->RotateY(90.);
  rot1->RotateZ(0.);
  xx = 105.;
  yy = 0.;
  zz = 0.;
  aLand->AddNode(trap,2,new TGeoCombiTrans(xx,yy,zz,rot1));

  TGeoRotation *rot2 = new TGeoRotation();
  rot2->RotateX(0.);
  rot2->RotateY(270.);
  rot2->RotateZ(0.);
  xx = -105.;
  yy = 0.;
  zz = 0.;
  aLand->AddNode(trap,3,new TGeoCombiTrans(xx,yy,zz,rot2));

  //------------------ Paddle assembly -------------------------------------
  // Iron end paddles
  xx = 0.;
  yy = 0.;
  zz = -4.875;
  aLand->AddNode(padle_h_box3,1,new TGeoCombiTrans(xx,yy,zz,zeroRotation));
  zz =  4.875;
  aLand->AddNode(padle_h_box3,2,new TGeoCombiTrans(xx,yy,zz,zeroRotation));

  // Iron padddles
  xx = 0.;
  yy = 0.;
  zz = -4.;
  for (int i = 1; i < 10; i++)
  {
    aLand->AddNode(padle_h_box4,i,new TGeoCombiTrans(xx,yy,zz,zeroRotation));
    zz+=1;
  }

  // BC408 paddles
  xx = 0.;
  yy = 0.;
  zz = -4.5;
  for (int i = 1; i < 11; i++)
  {
    aLand->AddNode(padle_h_box5,i,new TGeoCombiTrans(xx,yy,zz,zeroRotation));
    zz+=1;
  }

  //----------------------- Assembly ------------------------
  TGeoVolume *cell = new TGeoVolumeAssembly("CELL");

  TGeoRotation *rot4 = new TGeoRotation();
  rot4->RotateX(0.);
  rot4->RotateY(0.);
  rot4->RotateZ(90.);

  tx=0.;
  int nrPaddle=0, nrPlane=0;

  // Assembly includes spacing of 0.1cm on eather side of LAND paddles
  double spacing =0.2;

  // With LAND centerd around (0,0,0) the start of assembly is calculated with spacings
  double startZ=-(10.2 * 10 + 9 * spacing - 10.2)/2;
  double startY=-(10.2 * 20 + 19 * spacing - 10.2)/2;

  for (tz=startZ; nrPlane < 10; tz+=10.2+spacing)
  {
    nrPlane++;
    int j=0;
    for (ty=startY; j < 20; ty+=10.2+spacing)
    {
      j++;
      nrPaddle++;
      if (nrPlane % 2 == 1)
        cell->AddNode(aLand,nrPaddle,new TGeoCombiTrans(tx,ty,tz,zeroRotation));
      else
        cell->AddNode(aLand,nrPaddle,new TGeoCombiTrans(ty,tx,tz,rot4));
    }
  }

  // Add the whole assembly to the world
  tx = 0.0;
  ty = 0.0;
  tz = fZ; // cm
  TGeoCombiTrans *t0 = new TGeoCombiTrans(tx,ty,tz,zeroRotation);

  top->AddNode(cell,1, /*GetGlobalPosition(t0)*/t0 );



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

