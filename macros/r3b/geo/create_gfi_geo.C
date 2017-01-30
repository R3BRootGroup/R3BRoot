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
Double_t fZ = 0.;
Bool_t fLocalTrans = kFALSE;
Bool_t fLabTrans = kTRUE;

TGeoCombiTrans* GetGlobalPosition(TGeoCombiTrans *fRef);

void create_gfi_geo(const char* geoTag)
{

  fGlobalTrans->SetTranslation(0.0,0.0,0.0);

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
  TString geoFileName = geoPath + "/geometry/gfi_";
  geoFileName = geoFileName + geoTag + ".geo.root";
  // --------------------------------------------------------------------------



  // -----------------   Get and create the required media    -----------------
  FairGeoMedia*   geoMedia = geoFace->getMedia();
  FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

  FairGeoMedium* mAir      = geoMedia->getMedium("Air");
  if ( ! mAir ) Fatal("Main", "FairMedium Air not found");
  geoBuild->createMedium(mAir);
  TGeoMedium* pMed2 = gGeoMan->GetMedium("Air");
  if ( ! pMed2 ) Fatal("Main", "Medium Air not found");

  FairGeoMedium* mVac      = geoMedia->getMedium("vacuum");
  if ( ! mVac ) Fatal("Main", "FairMedium vacuum not found");
  geoBuild->createMedium(mVac);
  TGeoMedium* pMed1 = gGeoMan->GetMedium("vacuum");
  if ( ! pMed1 ) Fatal("Main", "Medium vacuum not found");
  
  FairGeoMedium* mGfi      = geoMedia->getMedium("plasticForGFI");
  if ( ! mGfi ) Fatal("Main", "FairMedium plasticForGFI not found");
  geoBuild->createMedium(mGfi);
  TGeoMedium* pMed35 = gGeoMan->GetMedium("plasticForGFI");
  if ( ! pMed35 ) Fatal("Main", "Medium plasticForGFI not found");

  FairGeoMedium* mAl      = geoMedia->getMedium("aluminium");
  if ( ! mAl ) Fatal("Main", "FairMedium aluminium not found");
  geoBuild->createMedium(mAl);
  TGeoMedium* pMed21 = gGeoMan->GetMedium("aluminium");
  if ( ! pMed21 ) Fatal("Main", "Medium aluminium not found");
  // --------------------------------------------------------------------------



  // --------------   Create geometry and top volume  -------------------------
  gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoMan->SetName("GFIgeom");
  TGeoVolume* top = new TGeoVolumeAssembly("TOP");
  gGeoMan->SetTopVolume(top);
  // --------------------------------------------------------------------------



  // out-of-file geometry definition
  Double_t dx,dy,dz;
  Double_t a;
  Double_t thx, phx, thy, phy, thz, phz;
  Double_t z, density, radl, absl, w;
  Int_t nel, numed;
  
  
  
  // TRANSFORMATION MATRICES
  // Combi transformation:
  //GFI1 position
  //dx = -71.973310; //Justyna
  //dy = 0.000000;   //Justyna
  //dz = 513.967775; //Justyna
  //dx = -72.164874; //Justyna new
  //dy = -0.010000;   //Justyna new
  //dz = 513.910302; //Justyna new
  //dx = -73.381; //dE tracker
  //dy = 0.070;
  //dz = 513.421;
  
  //LABPOS(GFI1,-73.274339,0.069976,513.649524)
  dx = -73.274339; //dE tracker, correction due to wrong angle
  dy = 0.069976;
  dz = 513.649524;
  
  /*
   dx = 73.700000;
   dy = 0.000000;
   dz = 525.400000;
   */
  // Rotation:
  thx = -106.700000;    phx = 0.000000;
  //   thx = 106.700000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = -16.700000;    phz = 0.000000;
  TGeoRotation *pMatrix3 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix2 = new TGeoCombiTrans("", dx,dy,dz,pMatrix3);
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
  // Combi transformation:
  dx = 0.000000;
  dy = 27.000000;
  dz = 0.000000;
  // Rotation:
  thx = 90.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = 0.000000;    phz = 0.000000;
  TGeoRotation *pMatrix9 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix8 = new TGeoCombiTrans("", dx,dy,dz,pMatrix9);
  // Combi transformation:
  dx = 0.000000;
  dy = -27.000000;
  dz = 0.000000;
  // Rotation:
  thx = 90.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = 0.000000;    phz = 0.000000;
  TGeoRotation *pMatrix11 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix10 = new TGeoCombiTrans("", dx,dy,dz,pMatrix11);
  // Combi transformation:
  dx = 27.000000;
  dy = 0.000000;
  dz = 0.000000;
  // Rotation:
  thx = 90.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = 0.000000;    phz = 0.000000;
  TGeoRotation *pMatrix13 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix12 = new TGeoCombiTrans("", dx,dy,dz,pMatrix13);
  // Combi transformation:
  dx = -27.000000;
  dy = 0.000000;
  dz = 0.000000;
  // Rotation:
  thx = 90.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = 0.000000;    phz = 0.000000;
  TGeoRotation *pMatrix15 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix14 = new TGeoCombiTrans("", dx,dy,dz,pMatrix15);
  // Combi transformation:
  //GFI2 position
  //dx = -145.270039; //Justyna
  //dy = 0.000000;    //Justyna
  //dz = 730.318956;  //Justyna
  //dx = -145.212570; //Justyna new
  //dy = -0.010000;    //Justyna new
  //dz = 730.336197;  //Justyna new
  //dx = -147.486; //dE tracker
  //dy = 0.070;
  //dz = 729.798;
  
  //LABPOS(GFI2,-147.135037,0.069976,729.680342)
  dx = -147.135037; //dE tracker, correction due to wrong angle
  dy = 0.069976;
  dz = 729.680342;
  
  /*
   dx = 141.800000;
   dy = 0.000000;
   dz = 727.300000;
   */
  // Rotation:
  thx = -106.700000;    phx = 0.000000;
  //   thx = 106.700000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = -16.700000;    phz = 0.000000;
  TGeoRotation *pMatrix5 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix4 = new TGeoCombiTrans("", dx,dy,dz,pMatrix5);
  
  
  // World definition
  TGeoVolume* pWorld = gGeoManager->GetTopVolume();
  pWorld->SetVisLeaves(kTRUE);
  
  
  
  
  // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
  // Shape: GFIBoxWorld type: TGeoBBox
  dx = 29.00000;
  dy = 29.00000;
  dz = 0.050000;
  TGeoShape *pGFIBoxWorld = new TGeoBBox("GFIBoxWorld", dx,dy,dz);
  // Volume: GFILogWorld
  TGeoVolume*
  pGFILogWorld = new TGeoVolume("GFILogWorld",pGFIBoxWorld, pMed2);
  pGFILogWorld->SetVisLeaves(kTRUE);
  
  // Global positioning
  
  TGeoCombiTrans *pGlobal1 = GetGlobalPosition(pMatrix2);
  TGeoCombiTrans *pGlobal2 = GetGlobalPosition(pMatrix4);
    
  TGeoVolumeAssembly *pw1 = new TGeoVolumeAssembly("GFI");
  pWorld->AddNode(pw1, 0, GetGlobalPosition(new TGeoCombiTrans()));
    
  pw1->AddNode( pGFILogWorld, 0, pGlobal1 );
  pw1->AddNode( pGFILogWorld, 1, pGlobal2 );
  
  
  
  // Shape: GFIBox type: TGeoBBox
  dx = 25.000000;
  dy = 25.000000;
  dz = 0.050000;
  TGeoShape *pGFIBox = new TGeoBBox("GFIBox", dx,dy,dz);
  // Volume: GFILog
  TGeoVolume*
  pGFILog = new TGeoVolume("GFILog",pGFIBox, pMed35);
  pGFILog->SetVisLeaves(kTRUE);
  pGFILogWorld->AddNode(pGFILog, 0, pMatrix6);
  
  // Shape: UpFrame type: TGeoBBox
  dx = 29.000000;
  dy = 2.000000;
  dz = 0.050000;
  TGeoShape *pUpFrame = new TGeoBBox("UpFrame", dx,dy,dz);
  // Volume: logicUpFrame
  TGeoVolume*
  plogicUpFrame = new TGeoVolume("logicUpFrame",pUpFrame, pMed21);
  plogicUpFrame->SetVisLeaves(kTRUE);
  pGFILogWorld->AddNode(plogicUpFrame, 0, pMatrix8);
  // Shape: DownFrame type: TGeoBBox
  dx = 29.000000;
  dy = 2.000000;
  dz = 0.050000;
  TGeoShape *pDownFrame = new TGeoBBox("DownFrame", dx,dy,dz);
  // Volume: logicDownFrame
  TGeoVolume*
  plogicDownFrame = new TGeoVolume("logicDownFrame",pDownFrame, pMed21);
  plogicDownFrame->SetVisLeaves(kTRUE);
  pGFILogWorld->AddNode(plogicDownFrame, 0, pMatrix10);
  // Shape: RightFrame type: TGeoBBox
  dx = 2.000000;
  dy = 25.000000;
  dz = 0.050000;
  TGeoShape *pRightFrame = new TGeoBBox("RightFrame", dx,dy,dz);
  // Volume: logicRightFrame
  TGeoVolume*
  plogicRightFrame = new TGeoVolume("logicRightFrame",pRightFrame, pMed21);
  plogicRightFrame->SetVisLeaves(kTRUE);
  pGFILogWorld->AddNode(plogicRightFrame, 0, pMatrix12);
  // Shape: LeftFrame type: TGeoBBox
  dx = 2.000000;
  dy = 25.000000;
  dz = 0.050000;
  TGeoShape *pLeftFrame = new TGeoBBox("LeftFrame", dx,dy,dz);
  // Volume: logicLeftFrame
  TGeoVolume*
  plogicLeftFrame = new TGeoVolume("logicLeftFrame",pLeftFrame, pMed21);
  plogicLeftFrame->SetVisLeaves(kTRUE);
  pGFILogWorld->AddNode(plogicLeftFrame, 0, pMatrix14);
  
  // Add the sensitive part
//  AddSensitiveVolume(pGFILog);
//  fNbOfSensitiveVol+=1;
  
  
  
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

