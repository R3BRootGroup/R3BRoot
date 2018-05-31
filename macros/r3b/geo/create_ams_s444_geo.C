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

TGeoCombiTrans* GetGlobalPosition(TGeoCombiTrans *fRef);

void create_ams_geo(const char* geoTag="s444")
{
// --------------------------------------------------------------------------
// Configurable geometry for the AMS Detectors.
// Use this macro to create root files with the different configurations 
// and positions/angles of the silicon detectors.
//
// Execute macro:  root -l
//                 .L create_ams_s444_geo.C
//                 create_ams_geo()
// --------------------------------------------------------------------------

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
  TString geoFileName = geoPath + "/geometry/ams_";
  geoFileName = geoFileName + geoTag + ".geo.root";
  // --------------------------------------------------------------------------

  // -----------------   Get and create the required media    -----------------
  FairGeoMedia*   geoMedia = geoFace->getMedia();
  FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

  FairGeoMedium* mAir      = geoMedia->getMedium("Air");
  if ( ! mAir ) Fatal("Main", "FairMedium Air not found");
  geoBuild->createMedium(mAir);
  TGeoMedium* pMedAir = gGeoMan->GetMedium("Air");
  if ( ! pMedAir ) Fatal("Main", "Medium Air not found");

  FairGeoMedium* mVac      = geoMedia->getMedium("vacuum");
  if ( ! mVac ) Fatal("Main", "FairMedium vacuum not found");
  geoBuild->createMedium(mVac);
  TGeoMedium* pMedVac = gGeoMan->GetMedium("vacuum");
  if ( ! pMedVac ) Fatal("Main", "Medium vacuum not found");
  
  FairGeoMedium* mSi      = geoMedia->getMedium("silicon");
  if ( ! mSi ) Fatal("Main", "FairMedium silicon not found");
  geoBuild->createMedium(mSi);
  TGeoMedium* pMedSi = gGeoMan->GetMedium("silicon");
  if ( ! pMedSi ) Fatal("Main", "Medium silicon not found");

  FairGeoMedium* mCopper      = geoMedia->getMedium("copper");
  if ( ! mCopper ) Fatal("Main", "FairMedium copper not found");
  geoBuild->createMedium(mCopper);
  TGeoMedium* pMedCu = gGeoMan->GetMedium("copper");
  if ( ! pMedCu ) Fatal("Main", "Medium copper not found");
  
  FairGeoMedium* mAl      = geoMedia->getMedium("aluminium");
  if ( ! mAl ) Fatal("Main", "FairMedium aluminium not found");
  geoBuild->createMedium(mAl);
  TGeoMedium* pMedAl = gGeoMan->GetMedium("aluminium");
  if ( ! pMedAl ) Fatal("Main", "Medium aluminium not found");

  FairGeoMedium* mFe      = geoMedia->getMedium("iron");
  if ( ! mFe ) Fatal("Main", "FairMedium iron not found");
  geoBuild->createMedium(mFe);
  TGeoMedium* pMedFe = gGeoMan->GetMedium("iron");
  if ( ! pMedFe ) Fatal("Main", "Medium iron not found");
  // --------------------------------------------------------------------------



  // --------------   Create geometry and top volume  -------------------------
  gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoMan->SetName("TRAgeom");
  TGeoVolume* top = new TGeoVolumeAssembly("TOP");
  gGeoMan->SetTopVolume(top);
  // --------------------------------------------------------------------------



	// out-of-file geometry definition
	Double_t dx,dy,dz;
	Double_t rmin, rmax, rmin1, rmax1, rmin2, rmax2;
	Double_t thx, phx, thy, phy, thz, phz;
	Double_t  phi1, phi2;
	Double_t tx,ty,tz;
  
	TGeoRotation * zeroRot = new TGeoRotation; //zero rotation
	TGeoCombiTrans * tZero = new TGeoCombiTrans("tZero", 0., 0., 0., zeroRot);
	tZero->RegisterYourself();
  
  
	//-------------------------------------------------------------------
  
	// Fill Chamber: Vacuum or Air. Needed still: an external call interface for choosing which.
	TGeoMedium * pMedFill=pMedVac;
  //pMedFill = new TGeoMedium("Fill_Air", numed,pMat2, par);
  //pMedFill = (TGeoMedium*) pMedAir->Clone();
  //pMedFill->SetName("Fill_Air");
//  pMedFill = (TGeoMedium*) pMedVac->Clone();
//  pMedFill->SetName("Fill_Vacuum");
  
	//-------------------------------------------------------------------
  
  
	// Shape: World type: TGeoBBox
	TGeoVolume* pWorld = gGeoManager->GetTopVolume();
	pWorld->SetVisLeaves(kTRUE);
  
	//**************************************************************//
	//*********************   Si Sensors ******************************//
	//***************************************************************/
  
	// Si Shape & volume: TraBox type: TGeoBBox
	dx = 3.600000;
	dy = 2.070000;
	dz = 0.015000;
	// Volume: TraLog
	TGeoVolume *TraLog = gGeoManager->MakeBox("TraLog",pMedSi,dx,dy,dz);
	TraLog->SetVisLeaves(kTRUE);
	TraLog->SetLineColor(33);

  
	//TRANSFORMATION MATRICES
	dx = 0.000000;
	dy = 0.000000;
	dz = 13.50000;
	// Rotation:
	thx = 90.000000;    phx = 0.000000;
	thy = 90.000000;    phy = 90.000000;
	thz = 0.000000;    phz = 0.000000;
	TGeoRotation *      pMatrix3 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*  pMatrix2 = new TGeoCombiTrans("", dx,dy,dz,pMatrix3);
  
	//Combi transformation:
	dx = 0.000000;
	dy = 0.000000;
	dz = 10.850000;
	// Rotation:
	thx = 90.000000;    phx = 0.000000;
	thy = 90.000000;    phy = 90.000000;
	thz = 0.000000;    phz = 0.000000;
	TGeoRotation *       pMatrix5 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*   pMatrix4 = new TGeoCombiTrans("", dx,dy,dz,pMatrix5);
  
	//Combi transformation:
	dx = 0.000000;
	dy = -2.100000;
	dz = 4.470000;
	// Rotation:
	thx = 0.000000;    phx = 0.000000;
	thy = 90.000000;    phy = 0.000000;
	thz = 90.000000;    phz = 90.000000;
	TGeoRotation *       pMatrix7 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*   pMatrix6 = new TGeoCombiTrans("", dx,dy,dz,pMatrix7);
  
	//Combi transformation:
	dx = 0.000000;
	dy = 2.100000;
	dz = 4.470000;
	// Rotation:
	thx = 180.000000;    phx = 0.000000;
	thy = 90.000000;    phy = 0.000000;
	thz = 90.000000;    phz = 270.000000;
	TGeoRotation *	    pMatrix9 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*   pMatrix8 = new TGeoCombiTrans("", dx,dy,dz,pMatrix9);
  
	//Combi transformation:
	dx = 2.100000;
	dy = 0.000000;
	dz = 4.470000;
	// Rotation:
	thx = 180.000000;    phx = 0.000000;
	thy = 90.000000;    phy = 90.000000;
	thz = 90.000000;    phz = 0.000000;
	TGeoRotation *        pMatrix11 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*   pMatrix10 = new TGeoCombiTrans("", dx,dy,dz,pMatrix11);
  
	//Combi transformation:
	dx = -2.100000;
	dy = 0.000000;
	dz = 4.470000;
	// Rotation:
	thx = 0.000000;    phx = 0.000000;
	thy = 90.000000;    phy = 90.000000;
	thz = 90.000000;    phz = 180.000000;
	TGeoRotation *	    pMatrix13 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*   pMatrix12 = new TGeoCombiTrans("", dx,dy,dz,pMatrix13);
 

	/************ Assembling everything together ****************/
	TGeoVolume *aTra = new TGeoVolumeAssembly("ATRA");
    
	//Si sensors
	//aTra->AddNode(TraLog,1, pMatrix4);
	aTra->AddNode(TraLog,1, pMatrix6);//down
	aTra->AddNode(TraLog,2, pMatrix8);//up
	aTra->AddNode(TraLog,3, pMatrix10);//left
	aTra->AddNode(TraLog,4, pMatrix12);//right
	//aTra->AddNode(TraLog,6, pMatrix2);
  
  
	TGeoRotation *rotg = new TGeoRotation();
	rotg->RotateX(0.);
	rotg->RotateY(0.);
	rotg->RotateZ(0.);
	dx=tx=0.0;
	dy=ty=0.0;
	dz=tz=0.0;
  
  TGeoCombiTrans *t_zero = new TGeoCombiTrans("t_zero");

  TGeoCombiTrans *t0 = new TGeoCombiTrans(tx,ty,tz,rotg);
  pWorld->AddNode(aTra, 1, GetGlobalPosition(t0));
   
  // ---------------   Finish   -----------------------------------------------
  gGeoMan->CloseGeometry();
  gGeoMan->CheckOverlaps(0.001);
  gGeoMan->PrintOverlaps();
  gGeoMan->Test();

  TFile* geoFile = new TFile(geoFileName, "RECREATE");
  top->Write();
  geoFile->Close();

  std::cout << "Creating geometry: "<<geoFileName<< std::endl;

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

