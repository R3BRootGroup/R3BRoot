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

void create_mfi_geo(const char* geoTag)
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
  TString geoFileName = geoPath + "/geometry/mfi_";
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
  // --------------------------------------------------------------------------



  // --------------   Create geometry and top volume  -------------------------
  gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoMan->SetName("MFIgeom");
  TGeoVolume* top = new TGeoVolumeAssembly("TOP");
  gGeoMan->SetTopVolume(top);
  // --------------------------------------------------------------------------



  // out-of-file geometry definition
  Double_t dx,dy,dz;
  Double_t a;
  Double_t thx, phx, thy, phy, thz, phz;
  Double_t z, density, w;//, radl, absl;
  Int_t nel, numed;
  
  
  // TRANSFORMATION MATRICES
  //global position
  // Combi transformation:
  //from Ralf's tracker: without offsets
  //LABPOS(MFI1,-63.82 , 0.00 , 520.25)
  //ROTATE(MFI1,13.5);
  dx = -63.82; //s412 pschrock
  dy = 0.00;
  dz = 520.25;
  
  // Rotation:
  //pschrock: its easier to use 'Rotate' functions
  TGeoRotation *gRot = new TGeoRotation();

  TGeoCombiTrans*
  //    pMatrix2 = new TGeoCombiTrans("", dx,dy,dz,pMatrix3);
  pMatrix2 = new TGeoCombiTrans("", dx,dy,dz,gRot);
  
  //translation and rotation of active volume
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
  
  
  //create wrapping for mfi
  //use plastic for the moment (like in ggland)
  
  //front
  // Combi transformation:
  dx = 0.000000;
  dy = 0.000000;
  dz = -0.01300; //center of cladding
  // Rotation:
  thx = 90.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = 0.000000;    phz = 0.000000;
  TGeoRotation *pMatrix9 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix8 = new TGeoCombiTrans("", dx,dy,dz,pMatrix9);
  
  //back
  // Combi transformation:
  dx = 0.000000;
  dy = 0.000000;
  dz = 0.013000; //center of cladding
  // Rotation:
  thx = 90.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = 0.000000;    phz = 0.000000;
  TGeoRotation *pMatrix11 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix10 = new TGeoCombiTrans("", dx,dy,dz,pMatrix11);
  
  
  // World definition
  TGeoVolume* pWorld = gGeoManager->GetTopVolume();
  pWorld->SetVisLeaves(kTRUE);
  
  
  // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
  
  //box for mfi volumes
  // Shape: GFIBoxWorld type: TGeoBBox
  dx = 15.000;	//half width of box for mfi volumes
  dy = 10.00000;
  dz = 0.050000;
  TGeoShape *pMFIBoxWorld = new TGeoBBox("MFIBoxWorld", dx,dy,dz);
  // Volume: MFILogWorld
  TGeoVolume*
  pMFILogWorld = new TGeoVolume("MFILogWorld",pMFIBoxWorld, pMed1); //vacuum filled
  pMFILogWorld->SetVisLeaves(kTRUE);
  
  
  // Shape: FrontCladding type: TGeoBBox
  dx = 15.000000;
  dy = 10.000000;
  dz = 0.0015000; //half thickness of cladding
  TGeoShape *pFrontCladding = new TGeoBBox("FrontClading", dx,dy,dz);
  // Volume: logicUpFrame
  TGeoVolume* plogicFrontClading = new TGeoVolume("logicFrontCladding",pFrontCladding, pMed35);
  plogicFrontClading->SetVisLeaves(kTRUE);
  pMFILogWorld->AddNode(plogicFrontClading, 0, pMatrix8);
  
  
  // Shape: BackCladding type: TGeoBBox
  dx = 15.000000;
  dy = 10.000000;
  dz = 0.0015000; //half thickness of cladding
  TGeoShape *pBackCladding = new TGeoBBox("BackCladding", dx,dy,dz);
  // Volume: logicUpFrame
  TGeoVolume* plogicBackCladding = new TGeoVolume("logicBackCladding",pBackCladding, pMed35);
  plogicBackCladding->SetVisLeaves(kTRUE);
  pMFILogWorld->AddNode(plogicBackCladding, 0, pMatrix10);
  
  
  // Global positioning
  TGeoCombiTrans *pGlobal1 = GetGlobalPosition(pMatrix2); //position and rotation is in pMatrix2
  //    TGeoCombiTrans *pGlobal2 = GetGlobalPosition(pMatrix4); //from Gfi2
  
  pWorld->AddNode( pMFILogWorld, 0, pGlobal1 );
  
  
  //active volume of mfi
  // Shape: MFIBox type: TGeoBBox
  dx = 15.000000; //half width of active volume
  dy = 10.000000; //half width of active volume
  dz = 0.01250000; //250 um full thickness of fibers
  TGeoShape *pMFIBox = new TGeoBBox("MFIBox", dx,dy,dz);
  // Volume: MFILog
  TGeoVolume*
  pMFILog = new TGeoVolume("MFILog",pMFIBox, pMed35);
  pMFILog->SetVisLeaves(kTRUE);
  pMFILogWorld->AddNode(pMFILog, 0, pMatrix6); //put here the active volume of mfi, pMatrix6 has no more moving parameters
  
  
  // Add the sensitive part
//  AddSensitiveVolume(pMFILog);
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

