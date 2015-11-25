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


void create_tra_geo(const char* geoTag)
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
  TString geoFileName = geoPath + "/geometry/tra_";
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
  
  
	/******************************************************/
	/*************      MontagePlatform  ************************/
	/*****************************************************/
  
	//Combi transformation:
	dx = 0.000000;
	dy = -14.090000;
	dz = 0.000000;
	// Rotation:
	thx = 90.000000;    phx = 0.000000;
	thy = 0.000000;    phy = 0.000000;
	thz = 90.000000;    phz = 270.000000;
	TGeoRotation *pMatrix15 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans* pMatrix14 = new TGeoCombiTrans("", dx,dy,dz,pMatrix15);
  
  
	rmin = 2.750000;
	rmax = 18.000000;
	dz   = 0.150000;
	phi1 = 0.000000;
	phi2 = 360.000000;
	TGeoShape *pMontagePlatform = new TGeoTubeSeg("MontagePlatform",rmin,rmax,dz,phi1,phi2);
	TGeoVolume*  pMontagePlatformLog = new TGeoVolume("MontagePlatformLog",pMontagePlatform, pMedCu);
  
	/*********************************************************************/
	/*************** MontageRing type *************************************/
	/********************************************************************/
	dx = 0.000000;
	dy = -17.000000;
	dz = 0.000000;
	// Rotation:
	thx = 90.000000;    phx = 0.000000;
	thy = 0.000000;    phy = 0.000000;
	thz = 90.000000;    phz = 270.000000;
	TGeoRotation *      pMatrix17 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*  pMatrix16 = new TGeoCombiTrans("", dx,dy,dz,pMatrix17);
  
	rmin = 12.000000;
	rmax = 15.000000;
	dz   = 0.250000;
	phi1 = 0.000000;
	phi2 = 360.000000;
	TGeoShape *  pMontageRing = new TGeoTubeSeg("MontageRing",rmin,rmax,dz,phi1,phi2);
	TGeoVolume*  pMontageRingLog = new TGeoVolume("MontageRingLog",pMontageRing, pMedAl);
  
	//	/*********************************************************************/
	//	/*************************** TargetWheel ******************************/
	//	/*********************************************************************/
	//	dx = -6.240000;
	//	dy = -3.900000;
	//	dz = 0.000000;
	//	// Rotation:
	//	thx = 90.000000;    phx = 0.000000;
	//	thy = 90.000000;    phy = 90.000000;
	//	thz = 0.000000;    phz = 0.000000;
	//	TGeoRotation *      pMatrix29 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	//	TGeoCombiTrans*  pMatrix28 = new TGeoCombiTrans("", dx,dy,dz,pMatrix29);
	//
	//
	//	rmin = 0.410000;
	//	//rmax = 5.300000;
	//	rmax = 6.300000;
	//	dz   = 0.100000;
	//	phi1 = 0.000000;
	//	phi2 = 360.000000;
	//	TGeoShape *   ptargetWheel = new TGeoTubeSeg("targetWheel",rmin,rmax,dz,phi1,phi2);
	//	TGeoVolume*  ptargetWheelLog = new TGeoVolume("targetWheelLog",ptargetWheel, pMedAl);
	//
	//	/*********** **********   TargetWheel2 ********************/
	//	dx = -6.240000;
	//	dy = -3.900000;
	//	dz = 0.000000;
	//	//Rotation:
	//	thx = 90.000000;    phx = 0.000000;
	//	thy = 90.000000;    phy = 90.000000;
	//	thz = 0.000000;    phz = 0.000000;
	//	TGeoRotation *     pMatrix31 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	//	TGeoCombiTrans* pMatrix30 = new TGeoCombiTrans("", dx,dy,dz,pMatrix31);
	//
	//
	//	//rmin = 9.500000;
	//	rmin = 8.500000;
	//	rmax = 10.000000;
	//	dz   = 0.100000;
	//	phi1 = 0.000000;
	//	phi2 = 360.000000;
	//	TGeoShape *   ptargetWheel2 = new TGeoTubeSeg("targetWheel2",rmin,rmax,dz,phi1,phi2);
	//	TGeoVolume*  ptargetWheel2Log = new TGeoVolume("targetWheel2Log",ptargetWheel2, pMedAl);
  
	//=================== Target Wheel Correct geometry =======================
  
	//The Air box for the target frame
	dx = 1.550000;       //s318
	dy = 1.550000;       //s318
	dz = 0.2;
	TGeoShape * TargetFrame = new TGeoBBox("pTargetFrame", dx,dy,dz);
	//Target wheel
	rmin = 0.410000;
	rmax = 10.00000;
	dz   = 0.100000;
	phi1 = 0.000000;
	phi2 = 360.000000;
	TGeoShape *   targetWheelShape = new TGeoTubeSeg("pTargetWheelShape",rmin,rmax,dz,phi1,phi2);
  
	// Create non-overlapping volumes
	TGeoCombiTrans * twZero = new TGeoCombiTrans("twZero", 0., 0., 0., zeroRot);
	twZero->RegisterYourself();
  
	//Transformations for the frame
	//TGeoRotation * zeroRot = new TGeoRotation; //zero rotation
	TGeoCombiTrans * tShift = new TGeoCombiTrans("tShift", 6.2400, 3.900, 0.0, zeroRot);
	tShift->RegisterYourself();
  
	TGeoCompositeShape * tWheel = new TGeoCompositeShape("TargWheel", "pTargetWheelShape - pTargetFrame:tShift");
	TGeoVolume * pTargetWheelLog = new TGeoVolume("TargetWheelLog", tWheel, pMedAl);
	//vAlCaseLog->SetVisLeaves(kTRUE);
  
	//Transformation
	dx = -6.240000;
	dy = -3.900000;
	//dz = 0.100000;
	dz = 0.000000;
  
	thx = 90.000000;    phx = 0.000000;
	thy = 90.000000;    phy = 90.000000;
	thz = 0.000000;    phz = 0.000000;
	TGeoRotation *      pMatrix29 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*  pMatrix28 = new TGeoCombiTrans("", dx,dy,dz,pMatrix29);
  
  
  
  
	/*********** **********   Target motor ********************/
	dx = -6.240000;
	dy = -3.900000;
	dz = -4.370000;
	//Rotation:
	thx = 90.000000;    phx = 0.000000;
	thy = 90.000000;    phy = 90.000000;
	thz = 0.000000;    phz = 0.000000;
	TGeoRotation *     pMatrix01 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans* pMatrix02 = new TGeoCombiTrans("", dx,dy,dz,pMatrix01);
  
	//rmin = 9.500000;
	rmin = 0.1500000;
	rmax = 2.100000;
	dz   = 2.7900000;
	phi1 = 0.000000;
	phi2 = 360.000000;
	TGeoShape *   ptargetMotor = new TGeoTubeSeg("targetMotor",rmin,rmax,dz,phi1,phi2);
	//TGeoVolume*  ptargetMotorLog = new TGeoVolume("targetMotorLog",ptargetMotor, pMedAl);
	TGeoVolume*  ptargetMotorLog = new TGeoVolume("targetMotorLog",ptargetMotor, pMedFe);
  
	/*********** **********   Motor holder ********************/
	dx = 3.000000;
	dy = 6.100000;
	dz = 0.500000;
	TGeoShape *   pMotorHolder1 = new TGeoBBox("MotorHolder1", dx,dy,dz);
	TGeoVolume*  pMotorHolderLog1 = new TGeoVolume("MotorHolderLog1",pMotorHolder1, pMedAl);
  
	dx = 3.480000;
	dy = 2.300000;
	dz = 0.500000;
	TGeoShape *   pMotorHolder2 = new TGeoBBox("MotorHolder2", dx,dy,dz);
	TGeoVolume*  pMotorHolderLog2 = new TGeoVolume("MotorHolderLog2",pMotorHolder2, pMedAl);
  
	dx = -13.64000;
	dy = -7.70000;
	dz = -1.05000;
	//Rotation:
	thx = 90.000000;    phx = 0.000000;
	thy = 90.000000;    phy = 90.00000;
	thz = 0.000000;    phz = 0.000000;
	TGeoRotation *     pMatrix03 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans* pMatrix04 = new TGeoCombiTrans("", dx,dy,dz,pMatrix03);
  
	dx = -7.14000;
	dy = -3.90000;
	dz = -1.05000;
  
	TGeoRotation *     pMatrix05 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans* pMatrix06 = new TGeoCombiTrans("", dx,dy,dz,pMatrix05);
  
	/*********************************************************************/
	/***********************  SSD electronics box   ***************************/
	/*********************************************************************/
  
	dx = 4.050000;
	dy = 3.465000;
	dz = 0.800000;
	TGeoShape *   pinnerElectronicsBox = new TGeoBBox("innerElectronicsBox", dx,dy,dz);
  
	dx = 4.070000;
	dy = 3.365000;
	dz = 0.700000;
  
	TGeoShape *   pinnerElectronicsVac = new TGeoBBox("innerElectronicsVac", dx,dy,dz);
  
	//===========  Create non-overlapping volumes =================
  
	//TGeoRotation * zeroRot = new TGeoRotation;
	//TGeoCombiTrans * tZero = new TGeoCombiTrans("tZero", 0., 0., 0., zeroRot);
	//tZero->RegisterYourself();
  
	TGeoCompositeShape * vAlCase = new TGeoCompositeShape("AlCasing", "innerElectronicsBox - innerElectronicsVac:tZero");
	TGeoVolume * vAlCaseLog = new TGeoVolume("AlCasingLog", vAlCase, pMedAl);
  
	//vAlCaseLog->SetVisLeaves(kTRUE);
  
	//==============================================================
  
	dx = 4.090000;
	dy = 3.362000;
	dz = 0.050000;
  
	TGeoShape *   pinnerElectronicsBoard = new TGeoBBox("innerElectronicsBoard", dx,dy,dz);
	TGeoVolume*  pinnerElectronicsBoardLog = new TGeoVolume("innerElectronicsBoardLog",pinnerElectronicsBoard, pMedSi);
  
	/**************  Transformations for the electronics *************************/
  
	dx = 0.000000;
	dy = -8.000000;
	dz = 10.820000;
	// Rotation:
	thx = 90.000000;    phx = 270.000000;
	thy = 90.000000;    phy = 0.000000;
	thz = 0.000000;    phz = 0.000000;
	TGeoRotation *       pMatrix19 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*   pMatrix18 = new TGeoCombiTrans("", dx,dy,dz,pMatrix19);
  
	//*****************************************
	dx = -2.100000;
	dy = -8.000000;
	dz = 4.470000;
	// Rotation:
	thx = 90.000000;    phx = 90.000000;
	thy = 0.000000;    phy = 0.000000;
	thz = 90.000000;    phz = 360.000000;
	TGeoRotation *      pMatrix21 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*  pMatrix20 = new TGeoCombiTrans("", dx,dy,dz,pMatrix21);
  
	//********************************************
	dx = 2.100000;
	dy = 8.000000;
	dz = 4.470000;
	// Rotation:
	thx = 90.000000;    phx = 90.000000;
	thy = 180.000000;    phy = 0.000000;
	thz = 90.000000;    phz = 180.000000;
	TGeoRotation *      pMatrix23 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*  pMatrix22 = new TGeoCombiTrans("", dx,dy,dz,pMatrix23);
  
	//********************************************
	dx = 8.000000;
	dy = -2.100000;
	dz = 4.470000;
	//Rotation:
	thx = 90.000000;    phx = 0.000000;
	thy = 180.000000;    phy = 0.000000;
	thz = 90.000000;    phz = 90.000000;
	TGeoRotation *      pMatrix25 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*  pMatrix24 = new TGeoCombiTrans("", dx,dy,dz,pMatrix25);
  
	//*******************************************
	dx = -8.000000;
	dy = 2.100000;
	dz = 4.470000;
	// Rotation:
	thx = 90.000000;    phx = 0.000000;
	thy = 180.000000;    phy = 0.000000;
	thz = 90.000000;    phz = 90.000000;
	TGeoRotation *       pMatrix27 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*   pMatrix26 = new TGeoCombiTrans("", dx,dy,dz,pMatrix27);
  
	//*********************************************
  
	dx = 0.000000;
	dy = 8.000000;
	dz = 13.470000;
	// Rotation:
	thx = 90.000000;    phx = 270.000000;
	thy = 90.000000;    phy = 0.000000;
	thz = 0.000000;    phz = 0.000000;
	TGeoRotation *       pMatrix33 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*   pMatrix32 = new TGeoCombiTrans("", dx,dy,dz,pMatrix33);
  
  
  
	//*********************************************************************/
	//**********   Copper holding structure for the box- SSDs (upstream part)********/
	/********************************************************************/
	Double_t Hdx = 0.500000; //X-thickness
	Double_t Hdy = 0.250000;//Y-thickness
  
	Double_t Hz = 1.20000;// Z-positioning
  
  
	dx = Hdx;
	dy = Hdy;
	dz = 8.400000;
	TGeoShape *   pHolderBox1 = new TGeoBBox("pHolderBox1", dx,dy,dz);
	TGeoVolume*  pHolderBoxLog1 = new TGeoVolume("pHolderBoxLog1",pHolderBox1, pMedCu);
  
	dx = -14.1400;
	dy = -5.40000;
	dz = Hz;
	// Rotation:
	thx = 0.000000;      phx = 0.000000;
	thy = 90.00000;      phy = 0.000000;
	thz = 90.000000;    phz = 90.000000;
  
	TGeoRotation *       pMatrixHold11 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*   pMatrixHold1 = new TGeoCombiTrans("", dx,dy,dz,pMatrixHold11);
  
	dx = -3.27000;
	dy = -5.40000;
	dz = Hz;
  
	TGeoRotation *       pMatrixHold21= new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*   pMatrixHold2 = new TGeoCombiTrans("", dx,dy,dz,pMatrixHold21);
  
	/**********************************************************/
	dx = Hdx;
	dy = Hdy;
	dz = 5.1500;
	TGeoShape *   pHolderBox3 = new TGeoBBox("pHolderBox3", dx,dy,dz);
	TGeoVolume*  pHolderBoxLog3 = new TGeoVolume("pHolderBoxLog3",pHolderBox3, pMedCu);
  
	dx = 12.1400;
	dy = -8.70;
	dz = Hz;
	// Rotation:
	thx = 0.000000;      phx = 0.000000;
	thy = 90.00000;      phy = 0.000000;
	thz = 90.000000;    phz = 90.000000;
  
	TGeoRotation *       pMatrixHold31 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*   pMatrixHold3 = new TGeoCombiTrans("", dx,dy,dz,pMatrixHold31);
  
	/**********************************************************/
	dx = Hdx;
	dy = Hdy;
	dz = 7.4500;
	TGeoShape *   pHolderBox4 = new TGeoBBox("pHolderBox4", dx,dy,dz);
	TGeoVolume*  pHolderBoxLog4 = new TGeoVolume("pHolderBoxLog4",pHolderBox4, pMedCu);
  
	dx = 3.27000;
	dy = 4.4500;
	dz = Hz;
	// Rotation:
	thx = 0.000000;      phx = 0.000000;
	thy = 90.00000;      phy = 0.000000;
	thz = 90.000000;    phz = 90.000000;
  
	TGeoRotation *       pMatrixHold41 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*   pMatrixHold4 = new TGeoCombiTrans("", dx,dy,dz,pMatrixHold41);
  
	/**********************************************************/
	dx = Hdx;
	dy = Hdy;
	dz = 8.700;
	TGeoShape *   pHolderBox5 = new TGeoBBox("pHolderBox5", dx,dy,dz);
	TGeoVolume*  pHolderBoxLog5 = new TGeoVolume("pHolderBoxLog5",pHolderBox5, pMedCu);
  
	dx = -5.7000;
	dy = 3.27000;
	dz = Hz;
	// Rotation:
	thx = 180.000000;  phx = 0.000000;
	thy = 90.00000;      phy = 90.000000;
	thz = 90.000000;    phz = 0.000000;
  
	TGeoRotation *       pMatrixHold51 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*   pMatrixHold5 = new TGeoCombiTrans("", dx,dy,dz,pMatrixHold51);
  
	/**********************************************************/
	dx = Hdx;
	dy = Hdy;
	dz = 7.7000;
	TGeoShape *   pHolderBox6 = new TGeoBBox("pHolderBox6", dx,dy,dz);
	TGeoVolume*  pHolderBoxLog6 = new TGeoVolume("pHolderBoxLog6",pHolderBox6, pMedCu);
  
	dx = 4.7000;
	dy = -3.27000;
	dz = Hz;
	// Rotation:
	thx = 180.000000;  phx = 0.000000;
	thy = 90.00000;      phy = 90.000000;
	thz = 90.000000;    phz = 0.000000;
  
	TGeoRotation *       pMatrixHold61 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*   pMatrixHold6 = new TGeoCombiTrans("", dx,dy,dz,pMatrixHold61);
  
  
	//*********************************************************************/
	//**********   Copper holding structure for the box- SSDs (downstream part)********/
	/********************************************************************/
  
	Hz = 7.80000;// Z-positioning
  
  
	dx = Hdx;
	dy = Hdy;
	dz = 8.400000;
	TGeoShape *   pHolderBox7 = new TGeoBBox("pHolderBox7", dx,dy,dz);
	TGeoVolume*  pHolderBoxLog7 = new TGeoVolume("pHolderBoxLog7",pHolderBox7, pMedCu);
  
	dx = -14.1400;
	dy = -5.40000;
	dz = Hz;
	// Rotation:
	thx = 0.000000;      phx = 0.000000;
	thy = 90.00000;      phy = 0.000000;
	thz = 90.000000;    phz = 90.000000;
  
	TGeoRotation *       pMatrixHold71 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*   pMatrixHold7 = new TGeoCombiTrans("", dx,dy,dz,pMatrixHold71);
  
	dx = -3.27000;
	dy = -5.40000;
	dz = Hz;
  
	TGeoRotation *       pMatrixHold81= new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*   pMatrixHold8 = new TGeoCombiTrans("", dx,dy,dz,pMatrixHold81);
  
	/**********************************************************/
	dx = Hdx;
	dy = Hdy;
	dz = 5.1500;
	TGeoShape *   pHolderBox9 = new TGeoBBox("pHolderBox9", dx,dy,dz);
	TGeoVolume*  pHolderBoxLog9 = new TGeoVolume("pHolderBoxLog9",pHolderBox9, pMedCu);
  
	dx = 12.1400;
	dy = -8.70;
	dz = Hz;
	// Rotation:
	thx = 0.000000;      phx = 0.000000;
	thy = 90.00000;      phy = 0.000000;
	thz = 90.000000;    phz = 90.000000;
  
	TGeoRotation *       pMatrixHold91 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*   pMatrixHold9 = new TGeoCombiTrans("", dx,dy,dz,pMatrixHold91);
  
	/**********************************************************/
	dx = Hdx;
	dy = Hdy;
	dz = 7.4500;
	TGeoShape *   pHolderBox10 = new TGeoBBox("pHolderBox10", dx,dy,dz);
	TGeoVolume*  pHolderBoxLog10 = new TGeoVolume("pHolderBoxLog10",pHolderBox10, pMedCu);
  
	dx = 3.27000;
	dy = 4.4500;
	dz = Hz;
	// Rotation:
	thx = 0.000000;      phx = 0.000000;
	thy = 90.00000;      phy = 0.000000;
	thz = 90.000000;    phz = 90.000000;
  
	TGeoRotation *       pMatrixHold101 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*   pMatrixHold10 = new TGeoCombiTrans("", dx,dy,dz,pMatrixHold101);
  
	/**********************************************************/
	dx = Hdx;
	dy = Hdy;
	dz = 8.700;
	TGeoShape *   pHolderBox11 = new TGeoBBox("pHolderBox11", dx,dy,dz);
	TGeoVolume*  pHolderBoxLog11 = new TGeoVolume("pHolderBoxLog11",pHolderBox11, pMedCu);
  
	dx = -5.7000;
	dy = 3.27000;
	dz = Hz;
	// Rotation:
	thx = 180.000000;  phx = 0.000000;
	thy = 90.00000;      phy = 90.000000;
	thz = 90.000000;    phz = 0.000000;
  
	TGeoRotation *       pMatrixHold11_0 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*   pMatrixHold11_1 = new TGeoCombiTrans("", dx,dy,dz,pMatrixHold11_0);
  
	/**********************************************************/
	dx = Hdx;
	dy = Hdy;
	dz = 7.7000;
	TGeoShape *   pHolderBox12 = new TGeoBBox("pHolderBox12", dx,dy,dz);
	TGeoVolume*  pHolderBoxLog12 = new TGeoVolume("pHolderBoxLog12",pHolderBox12, pMedCu);
  
	dx = 4.7000;
	dy = -3.27000;
	dz = Hz;
	// Rotation:
	thx = 180.000000;  phx = 0.000000;
	thy = 90.00000;      phy = 90.000000;
	thz = 90.000000;    phz = 0.000000;
  
	TGeoRotation *       pMatrixHold121 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*   pMatrixHold12 = new TGeoCombiTrans("", dx,dy,dz,pMatrixHold121);
  
	//*********************************************************************/
	//**************   Copper holding structure for the in-beam SSDs ***************/
	/********************************************************************/
  
	//Long Holder
	dx = Hdx;
	dy = Hdy;
	dz = 12.50000;
	TGeoShape *   pHolderSSD1 = new TGeoBBox("pHolderSSD1", dx,dy,dz);
	TGeoVolume*  pHolderSSDLog1 = new TGeoVolume("pHolderSSDLog1",pHolderSSD1, pMedCu);
  
	//Short holder
	dz = 5.10000;
	TGeoShape *   pHolderSSD2 = new TGeoBBox("pHolderSSD2", dx,dy,dz);
	TGeoVolume*  pHolderSSDLog2 = new TGeoVolume("pHolderSSDLog2",pHolderSSD2, pMedCu);
  
	//POsitions of the long one
	dx = 3.0000;
	dy = -1.400;
	dz = 12.400;
	// Rotation:
	thx = 90.000000;    phx = 0.000000;
	thy = 0.00000;      phy = 0.000000;
	thz = 90.000000;    phz = 270.000000;
  
	TGeoRotation *       pMatrixSSD11 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*   pMatrixSSD1 = new TGeoCombiTrans("", dx,dy,dz,pMatrixSSD11);
  
	dx = -3.0000;
	dy = -1.4000;
	dz = 12.4000;
	// Rotation:
	thx = 90.000000;    phx = 0.000000;
	thy = 0.00000;      phy = 0.000000;
	thz = 90.000000;    phz = 270.000000;
  
	TGeoRotation *       pMatrixSSD21 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*   pMatrixSSD2 = new TGeoCombiTrans("", dx,dy,dz,pMatrixSSD21);
  
	//Positions of the short one
  
	dx = 2.8000;
	dy = -8.8000;
	dz = 11.8900;
  
	TGeoRotation *       pMatrixSSD31 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*   pMatrixSSD3 = new TGeoCombiTrans("", dx,dy,dz,pMatrixSSD31);
  
	dx = -2.8000;
	dy = -8.8000;
	dz = 11.8900;
  
	TGeoRotation *       pMatrixSSD41 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*   pMatrixSSD4 = new TGeoCombiTrans("", dx,dy,dz,pMatrixSSD41);
	//**************************************************************//
	//*********************   Si Sensors ******************************//
	//***************************************************************/
  
	// Si Shape & volume: TraBox type: TGeoBBox
	dx = 3.600000;
	dy = 2.070000;
	dz = 0.015000;
	// Volume: TraLog
	TGeoVolume *TraLog = gGeoManager->MakeBox("TraLog",pMedSi,dx,dy,dz);
  
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
  
  
  
	/************ Medium inside reaction chamber, around tracker ****************/
  TGeoVolumeAssembly* reactionChamber = new TGeoVolumeAssembly("TRAChmb");
  
	TGeoShape *aTraMedium_ReactionChamber = new TGeoSphere("aTraMedium_ReactionChamber", 0., 24.250000);	//inner reaction chamber volume
	TGeoVolume *aTraMedium_ReactionChamberLog = new TGeoVolume("aTraMedium_ReactionChamberLog", aTraMedium_ReactionChamber, pMedFill);	//pMedVac or pMedAir
	
	//three conical in out and bottom beam tubes. need additional short cylinders in order to match chamber with cones.
	dz = 14.600000;
	rmin1 = 0.0;
	rmax1 = 2.730000;
	rmin2 = 0.0;
	rmax2 = 5.125000;
	phi1 = 0.000000;
	phi2 = 360.000000;
	TGeoShape *aTraMedium_ConicalTube = new TGeoConeSeg("aTraMedium_ConicalTube", dz,rmin1,rmax1,rmin2,rmax2,phi1,phi2);
	TGeoVolume* aTraMedium_ConicalTubeLog = new TGeoVolume("aTraMedium_ConicalTubeLog",aTraMedium_ConicalTube, pMedFill);
	// Combi transformation:
	dx = 0.000000;
	dy = 0.000000;
	dz = 40.8500000;
	// Rotation:
	TGeoCombiTrans* pCombTrans1 = new TGeoCombiTrans("", dx,dy,dz, zeroRot);
	// Combi transformation:
	dx = 0.000000;
	dy = 0.000000;
	dz = -40.8500000;
	// Rotation:
	TGeoRotation *pRot1 = new TGeoRotation("", 0, 180, 0);
	TGeoCombiTrans* pCombTrans2 = new TGeoCombiTrans("", dx,dy,dz, pRot1);
	// Combi transformation:
	dx = 0.000000;
	dy = -40.8500000;
	dz = 0.000000;
	// Rotation:
	TGeoRotation *pRot2 = new TGeoRotation("", 0, 90, 0);
	TGeoCombiTrans* pCombTrans3 = new TGeoCombiTrans("", dx,dy,dz, pRot2);
	aTraMedium_ConicalTubeLog->SetVisLeaves(kTRUE);
	aTraMedium_ConicalTubeLog->SetLineColor(33);
//	aTraMedium_ReactionChamberLog->AddNode(aTraMedium_ConicalTubeLog, 0, pCombTrans1);
//	aTraMedium_ReactionChamberLog->AddNode(aTraMedium_ConicalTubeLog, 1, pCombTrans2);
//	aTraMedium_ReactionChamberLog->AddNode(aTraMedium_ConicalTubeLog, 2, pCombTrans3);
  reactionChamber->AddNode(aTraMedium_ReactionChamberLog, 0, tZero);
	reactionChamber->AddNode(aTraMedium_ConicalTubeLog, 0, pCombTrans1);
	reactionChamber->AddNode(aTraMedium_ConicalTubeLog, 1, pCombTrans2);
	reactionChamber->AddNode(aTraMedium_ConicalTubeLog, 2, pCombTrans3);
	
	//need in and out bridging filling cylindrical volumes; also: bottom hole volume
	//placement
	dx = 0.000000;
	dy = 0.000000;
	dz = 25.2500000;	//cm, at reaction chamber edge
	TGeoCombiTrans* pCombTrans4 = new TGeoCombiTrans("", dx,dy,dz, zeroRot);
	TGeoCombiTrans* pCombTrans5 = new TGeoCombiTrans("", dx,dy,-dz, zeroRot);
	dx = 0.000000;
	dy = -25.2500000;
	dz = 0.000000;
	TGeoCombiTrans* pCombTrans6 = new TGeoCombiTrans("", dx,dy,dz, pRot2);
	//shape
	rmin = 0.0;
	rmax = 2.730000;
	dz = 1.000000;	//2cm thickness (length)
	phi1 = 0.0;
	phi2 = 360.0;
	TGeoShape *aTraMedium_BridgeChamberTube = new TGeoTubeSeg("aTraMedium_BridgeChamberTube",rmin,rmax,dz,phi1,phi2);
	TGeoVolume*  aTraMedium_BridgeChamberTubeLog = new TGeoVolume("aTraMedium_BridgeChamberTubeLog",aTraMedium_BridgeChamberTube, pMedFill);
	aTraMedium_BridgeChamberTubeLog->SetVisLeaves(kTRUE);
	aTraMedium_BridgeChamberTubeLog->SetLineColor(33);
//	aTraMedium_ReactionChamberLog->AddNode(aTraMedium_BridgeChamberTubeLog, 0, pCombTrans4);
//	aTraMedium_ReactionChamberLog->AddNode(aTraMedium_BridgeChamberTubeLog, 1, pCombTrans5);
//	aTraMedium_ReactionChamberLog->AddNode(aTraMedium_BridgeChamberTubeLog, 2, pCombTrans6);
	reactionChamber->AddNode(aTraMedium_BridgeChamberTubeLog, 0, pCombTrans4);
	reactionChamber->AddNode(aTraMedium_BridgeChamberTubeLog, 1, pCombTrans5);
	reactionChamber->AddNode(aTraMedium_BridgeChamberTubeLog, 2, pCombTrans6);
  
	//linear beam tube going downstream until the Aladin entrance window
	//placement
	dx = 0.000000;
	dy = 0.000000;
	dz = 87.3500000;	//cm
	TGeoCombiTrans* pCombTrans7 = new TGeoCombiTrans("", dx,dy,dz, zeroRot);
	//shape (approximation as a cone segment, matching radii at XB_out and Aladin_in)
	rmin1 = 0.0;
	rmax1 = 5.125000;
	rmin2 = 0.0;
	rmax2 = 10.000000;
	dz = 31.900000;	//2cm thickness (length)
	phi1 = 0.0;
	phi2 = 360.0;
	TGeoShape *aTraMedium_BeamLineDownstream = new TGeoConeSeg("aTraMedium_BeamLineDownstream", dz,rmin1,rmax1,rmin2,rmax2,phi1,phi2);
	TGeoVolume* aTraMedium_BeamLineDownstreamLog = new TGeoVolume("aTraMedium_BeamLineDownstreamLog",aTraMedium_BeamLineDownstream, pMedFill);
	aTraMedium_BeamLineDownstreamLog->SetVisLeaves(kTRUE);
	aTraMedium_BeamLineDownstreamLog->SetLineColor(33);
//	aTraMedium_ReactionChamberLog->AddNode(aTraMedium_BeamLineDownstreamLog, 0, pCombTrans7);
	reactionChamber->AddNode(aTraMedium_BeamLineDownstreamLog, 0, pCombTrans7);
  
  
	/************ Assembling everything together ****************/
	TGeoVolume *aTra = new TGeoVolumeAssembly("ATRA");
  
	aTra->AddNode(pMontagePlatformLog,1,pMatrix14);
	aTra->AddNode(pMontageRingLog,1,pMatrix16);
  
	//aTra->AddNode(ptargetWheelLog,1,pMatrix28);
	//aTra->AddNode(ptargetWheel2Log,1,pMatrix30);
  
	aTra->AddNode(pTargetWheelLog,1,pMatrix28);
  
	aTra->AddNode(ptargetMotorLog,1,pMatrix02);
	aTra->AddNode(pMotorHolderLog1,1,pMatrix04);
	aTra->AddNode(pMotorHolderLog2,1,pMatrix06);
  
	//Adding  front end electronics
  
	aTra->AddNode(vAlCaseLog,1,pMatrix18);
	aTra->AddNode(vAlCaseLog,2,pMatrix20);
	aTra->AddNode(vAlCaseLog,3,pMatrix22);
	aTra->AddNode(vAlCaseLog,4,pMatrix24);
	aTra->AddNode(vAlCaseLog,5,pMatrix26);
	aTra->AddNode(vAlCaseLog,6,pMatrix32);
  
	aTra->AddNode(pinnerElectronicsBoardLog,1,pMatrix18);
	aTra->AddNode(pinnerElectronicsBoardLog,2,pMatrix20);
	aTra->AddNode(pinnerElectronicsBoardLog,3,pMatrix22);
	aTra->AddNode(pinnerElectronicsBoardLog,4,pMatrix24);
	aTra->AddNode(pinnerElectronicsBoardLog,5,pMatrix26);
	aTra->AddNode(pinnerElectronicsBoardLog,6,pMatrix32);
  
	//Copper holders for the SSDs
  
	aTra->AddNode(pHolderBoxLog1,1,pMatrixHold1);
	aTra->AddNode(pHolderBoxLog1,2,pMatrixHold2);
	aTra->AddNode(pHolderBoxLog3,3,pMatrixHold3);
	aTra->AddNode(pHolderBoxLog4,4,pMatrixHold4);
	aTra->AddNode(pHolderBoxLog5,5,pMatrixHold5);
	aTra->AddNode(pHolderBoxLog6,6,pMatrixHold6);
	aTra->AddNode(pHolderBoxLog7,7,pMatrixHold7);
	aTra->AddNode(pHolderBoxLog7,8,pMatrixHold8);
	aTra->AddNode(pHolderBoxLog9,9,pMatrixHold9);
	aTra->AddNode(pHolderBoxLog10,10,pMatrixHold10);
	aTra->AddNode(pHolderBoxLog11,11,pMatrixHold11_1);
	aTra->AddNode(pHolderBoxLog12,12,pMatrixHold12);
	aTra->AddNode(pHolderSSDLog1,1,pMatrixSSD1);
	aTra->AddNode(pHolderSSDLog1,2,pMatrixSSD2);
	aTra->AddNode(pHolderSSDLog2,1,pMatrixSSD3);
	aTra->AddNode(pHolderSSDLog2,2,pMatrixSSD4);
  
//	AddSensitiveVolume(TraLog);
//	fNbOfSensitiveVol+=1;
  
	//Si sensors
	aTra->AddNode(TraLog,1, pMatrix4);
	aTra->AddNode(TraLog,2, pMatrix6);
	aTra->AddNode(TraLog,3, pMatrix8);
	aTra->AddNode(TraLog,4, pMatrix10);
	aTra->AddNode(TraLog,5, pMatrix12);
	aTra->AddNode(TraLog,6, pMatrix2);
  
  
	TGeoRotation *rotg = new TGeoRotation();
	rotg->RotateX(0.);
	rotg->RotateY(0.);
	rotg->RotateZ(0.);
	dx=tx=0.0;
	dy=ty=0.0;
	dz=tz=0.0;
  
	TGeoCombiTrans *t_zero = new TGeoCombiTrans("t_zero");
  aTraMedium_ReactionChamberLog->AddNode(aTra,1, t_zero);
	
	TGeoCombiTrans *t0 = new TGeoCombiTrans(tx,ty,tz,rotg);
	//pWorld->AddNode(aTra,1, GetGlobalPosition(t0));
//	pWorld->AddNode(aTraMedium_ReactionChamberLog,1, GetGlobalPosition(t0));
	pWorld->AddNode(reactionChamber, 1, GetGlobalPosition(t0));
  
  
   
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

