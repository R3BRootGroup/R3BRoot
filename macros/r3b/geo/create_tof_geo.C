#include <iomanip>
#include <iostream>
#include "TGeoManager.h"
#include "TMath.h"




// Create Matrix Unity
TGeoRotation *fGlobalRot = new TGeoRotation();

// Create a null translation
TGeoTranslation *fGlobalTrans = new TGeoTranslation();
fGlobalTrans->SetTranslation(0.0,0.0,0.0);
TGeoRotation *fRefRot = NULL;

TGeoManager*   gGeoMan           = NULL;




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




void create_tof_geo(const char* geoTag)
{
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
  TString geoFileName = geoPath + "/geometry/tof_";
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
  
  FairGeoMedium* mTof      = geoMedia->getMedium("plasticForTOF");
  if ( ! mTof ) Fatal("Main", "FairMedium plasticForTOF not found");
  geoBuild->createMedium(mTof);
  TGeoMedium* pMed34 = gGeoMan->GetMedium("plasticForTOF");
  if ( ! pMed34 ) Fatal("Main", "Medium plasticForTOF not found");
  // --------------------------------------------------------------------------



  // --------------   Create geometry and top volume  -------------------------
  gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoMan->SetName("TOFgeom");
  TGeoVolume* top = new TGeoVolumeAssembly("TOP");
  gGeoMan->SetTopVolume(top);
  // --------------------------------------------------------------------------



  // out-of-file geometry definition
  Double_t dx,dy,dz;
  Double_t a;
  //   Double_t thx, phx, thy, phy, thz, phz;
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
  if (gGeoManager->GetMedium("plasticForTOF") ){
    pMed34=gGeoManager->GetMedium("plasticForTOF");
  }else{
    nel     = 2;
    density = 1.032000;
    TGeoMixture*
    pMat34 = new TGeoMixture("plasticForTOF", nel,density);
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
    pMed34 = new TGeoMedium("plasticForTOF", numed,pMat34,par);
  }
  
  
  // TRANSFORMATION MATRICES
  // Combi transformation:
  dx = -417.359574; //Justyna
  dy = 2.400000;    //Justyna
  dz = 960.777114;  //Justyna
  
  //    dx = -421.33683; //Christoph
  //    dy = 2.12;    //Christoph
  //    dz = 958.387337;  //Christoph
  
  /*    dx = -171.1;  //position directrly (15cm) after DCH2
   dy = 2.400000;
   dz = 548.95;*/
  // dz = 0.;
  /*   // Rotation:
   thx = -121.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = -31.000000;    phz = 0.000000;*/  //this
  
  //   TGeoRotation *pMatrix3 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);  //this
  TGeoRotation *gRot = new TGeoRotation();
  gRot->RotateX(0.);
  gRot->RotateY(-31.000000);
  gRot->RotateZ(0.);
  
  TGeoCombiTrans*
  //   pMatrix2 = new TGeoCombiTrans("", dx,dy,dz,pMatrix3);  //this
  pMatrix2 = new TGeoCombiTrans("", dx,dy,dz,gRot);  //this
  
  
  
  
  
  
  
  /* PREVIOUS!!!
   // TRANSFORMATION MATRICES
   // Combi transformation:
   dx = 419.700000;
   dy = 0.000000;
   dz = 952.400000;
   // dz = 0.;
   // Rotation:
   thx = 121.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 31.000000;    phz = 0.000000;
   TGeoRotation *pMatrix3 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix2 = new TGeoCombiTrans("", dx,dy,dz,pMatrix3);
   */
  
  //Top Volume
  TGeoVolume* pWorld = gGeoManager->GetTopVolume();
  pWorld->SetVisLeaves(kTRUE);
  
  
  TGeoVolumeAssembly *ptof = new TGeoVolumeAssembly("TOF");
  
  
  // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
  // Shape: TOFBox type: TGeoBBox
  dx = 94.450000;
  dy = 73.450000;
  dz = 0.500000;
  TGeoShape *pTOFBox = new TGeoBBox("TOFBox", dx,dy,dz);
  // Volume: TOFLog
  TGeoVolume*
  pTOFLog = new TGeoVolume("TOFLog",pTOFBox, pMed34);
  pTOFLog->SetVisLeaves(kTRUE);
  
  TGeoCombiTrans *t0 = new TGeoCombiTrans("t0");
  ptof->AddNode(pTOFLog, 0, t0);
  
  TGeoCombiTrans *pGlobal = GetGlobalPosition(pMatrix2);
  if (pGlobal){
    pWorld->AddNode(ptof, 0, pGlobal);
  }else{
    pWorld->AddNode(ptof, 0, pMatrix2);
  }
  
//  AddSensitiveVolume(pTOFLog);
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
    fRefRot = fRef;
    
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
      c4.SetRotation(fRefRot->GetRotation());
      
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

