
#include <iomanip>
#include <iostream>
#include "TGeoManager.h"
#include "TMath.h"


// Create Matrix Unity
TGeoRotation *fGlobalRot = new TGeoRotation();

// Create a null translation
TGeoTranslation *fGlobalTrans = new TGeoTranslation();
TGeoRotation *fRefRot = NULL;

TGeoManager* gGeoMan = NULL;
//TGeoVolume* gTop;


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
Bool_t fLabTrans = kFALSE;

TGeoCombiTrans* GetGlobalPosition(TGeoCombiTrans *fRef);

void ConstructGeometry1(TGeoMedium *lead);
void ConstructGeometry2(TGeoMedium *pMed38, TGeoMedium *pMed2);
void ConstructGeometry3(TGeoMedium *pMed38, TGeoMedium *pMed2);
void ConstructGeometry4(TGeoMedium *pMed2, TGeoMedium *pMed15,
                        TGeoMedium *pMed3, TGeoMedium *pMedAu);
void ConstructGeometry5(TGeoMedium *pMedCarbon);
void ConstructGeometry6(TGeoMedium *pMed16);
void ConstructGeometry7(TGeoMedium *pMed);



void create_target_geo(const char* geoTag = "LiH")
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
  TString geoFileName = geoPath + "/geometry/target_";
  geoFileName = geoFileName + geoTag + ".geo.root";
  // --------------------------------------------------------------------------



  // -----------------   Get and create the required media    -----------------
  FairGeoMedia* gGeoMedia = geoFace->getMedia();
  FairGeoBuilder* gGeoBuild = geoLoad->getGeoBuilder();

  FairGeoMedium* mLead = gGeoMedia->getMedium("lead");
  if ( ! mLead ) Fatal("Main", "FairMedium lead not found");
  gGeoBuild->createMedium(mLead);
  TGeoMedium* lead = gGeoMan->GetMedium("lead");
  if ( ! lead ) Fatal("Main", "Medium lead not found");

  FairGeoMedium* mCH2      = gGeoMedia->getMedium("CH2");
  if ( ! mCH2 ) Fatal("Main", "FairMedium CH2 not found");
  gGeoBuild->createMedium(mCH2);
  TGeoMedium* pMed38 = gGeoMan->GetMedium("CH2");
  if ( ! pMed38 ) Fatal("Main", "Medium CH2 not found");

  FairGeoMedium* mAir      = gGeoMedia->getMedium("Air");
  if ( ! mAir ) Fatal("Main", "FairMedium Air not found");
  gGeoBuild->createMedium(mAir);
  TGeoMedium* pMed2 = gGeoMan->GetMedium("Air");
  if ( ! pMed2 ) Fatal("Main", "Medium Air not found");
 
  FairGeoMedium* mMylar      = gGeoMedia->getMedium("mylar");
  if ( ! mMylar ) Fatal("Main", "FairMedium mylar not found");
  gGeoBuild->createMedium(mMylar);
  TGeoMedium* pMed15 = gGeoMan->GetMedium("mylar");
  if ( ! pMed15 ) Fatal("Main", "Medium mylar not found");

  FairGeoMedium* mH2      = gGeoMedia->getMedium("H2");
  if ( ! mH2 ) Fatal("Main", "FairMedium H2 not found");
  gGeoBuild->createMedium(mH2);
  TGeoMedium* pMed3 = gGeoMan->GetMedium("H2");
  if ( ! pMed3 ) Fatal("Main", "Medium H2 not found");

  FairGeoMedium* mAu      = gGeoMedia->getMedium("gold");
  if ( ! mAu ) Fatal("Main", "FairMedium gold not found");
  gGeoBuild->createMedium(mAu);
  TGeoMedium* pMedAu = gGeoMan->GetMedium("gold");
  if ( ! pMedAu ) Fatal("Main", "Medium gold not found");

  FairGeoMedium* mCarbon      = gGeoMedia->getMedium("carbon");
  if ( ! mCarbon ) Fatal("Main", "FairMedium carbon not found");
  gGeoBuild->createMedium(mCarbon);
  TGeoMedium* pMedCarbon = gGeoMan->GetMedium("carbon");
  if ( ! pMedCarbon ) Fatal("Main", "Medium carbon not found");

  FairGeoMedium* mVac      = gGeoMedia->getMedium("vacuum");
  if ( ! mVac ) Fatal("Main", "FairMedium vacuum not found");
  gGeoBuild->createMedium(mVac);
  TGeoMedium* pMed = gGeoMan->GetMedium("vacuum");
  if ( ! pMed ) Fatal("Main", "Medium vacuum not found");
  // --------------------------------------------------------------------------



  // --------------   Create geometry and top volume  -------------------------
  gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoMan->SetName("TARGETgeom");
  TGeoVolume* gTop = new TGeoVolumeAssembly("TOP");
  gGeoMan->SetTopVolume(gTop);
  // --------------------------------------------------------------------------



  if (0 == strncmp(geoTag, "LeadTarget", 10)) ConstructGeometry1(lead);  	//for s318
  //if (geoTag == "Para")       ConstructGeometry2(pMed2,pMed2); // equivalent to no target
  if (0 == strncmp(geoTag, "Para", 4))       ConstructGeometry2(pMed38,pMed2);
  if (0 == strncmp(geoTag, "Para45", 6))     ConstructGeometry3(pMed38,pMed2);
  if (0 == strncmp(geoTag, "LiH", 3))        ConstructGeometry4(pMed2, pMed15, pMed3, pMedAu);
  if (0 == strncmp(geoTag, "CTarget", 7))    ConstructGeometry5(pMedCarbon);	//for s318
  if (0 == strncmp(geoTag, "CH2Target", 9))  ConstructGeometry6(pMed38); 	//for s318
  if (0 == strncmp(geoTag, "ETTarget", 6))   ConstructGeometry7(pMed); 	//for s318, empty target



  // ---------------   Finish   -----------------------------------------------
  gGeoMan->CloseGeometry();
  gGeoMan->CheckOverlaps(0.001);
  gGeoMan->PrintOverlaps();
  gGeoMan->Test();

  TFile* geoFile = new TFile(geoFileName, "RECREATE");
  gTop->Write();
  geoFile->Close();
  // --------------------------------------------------------------------------
}





//void ConstructGeometry1(TGeoMedium *lead,  TGeoMedium *pMed2)
void ConstructGeometry1(TGeoMedium *lead)
{
  cout << endl;
  cout << "create_target_geo.C-I- R3BTarget:: ConstructGeometry() "<< endl;
  cout << "-I- R3BTarget Target type: Lead target (199mg/cm2) "<< endl;
  cout << endl;


  Double_t dx, dy, dz;
  Double_t thx, thy, thz;
  Double_t phx, phy, phz;
  // Combi transformation: 
  dx = 0.000000;
  dy = 0.000000;
  dz = 0.000000;       //-(0.15+half thickness)
  // Rotation: 
  thx = 90.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = 0.000000;     phz = 0.000000;
  TGeoRotation *pRot = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans* pMatrix =
    new TGeoCombiTrans("", dx,dy,dz,pRot);

  /*
  // Shape: TargetEnveloppe type: TGeoBBox
  // TargetEnveloppe Tube 1
  dx   = 1.6000;
  dy   = 1.6000;
  dz   = 0.009000;
  TGeoShape *pTargetEnveloppe = new TGeoBBox("TargetEnveloppe",dx,dy,dz);
  TGeoVolume *pTargetEnveloppe_log = new TGeoVolume("TargetEnveloppe",pTargetEnveloppe, pMed2);
  pTargetEnveloppe_log->SetVisLeaves(kTRUE);
  TGeoCombiTrans* pGlobal = GetGlobalPosition(pMatrix);
  gTop->AddNode(pTargetEnveloppe_log, 0, pGlobal);
  */

  // Shape: World type: TGeoBBox
  TGeoVolume* pWorld = gGeoManager->GetTopVolume();
  pWorld->SetVisLeaves(kTRUE);

  // Shape: LeadTarget type: TGeoBBox
  dx = 1.500000;	//s318
  dy = 1.500000;	//s318
  dz = 0.008766520;     //s318: 199mg/cm2 = 0.17533040mm
  TGeoShape *pLeadTarget = new TGeoBBox("LeadTarget", dx,dy,dz);
  TGeoVolume* pleadTarget_log
    = new TGeoVolume("leadTarget_log",pLeadTarget, lead);
  pleadTarget_log->SetVisLeaves(kTRUE);
    
  TGeoVolumeAssembly *keep = new TGeoVolumeAssembly("Target");
  keep->AddNode(pleadTarget_log, 0, pMatrix);

  //pTargetEnveloppe_log->AddNode(pleadTarget_log, 0, pGlobal);
  TGeoCombiTrans* pGlobal = GetGlobalPosition(pMatrix);
  pWorld->AddNode(keep, 0, pGlobal);
//  gTop->AddNode(keep, 0, pGlobal);
}





//void ConstructGeometry2(TGeoMedium *pMed38)
void ConstructGeometry2(TGeoMedium *pMed38, TGeoMedium *pMed2)
{
  cout << endl;
  cout << "-I- R3BTarget:: ConstructGeometry() "<< endl;
  cout << "-I- R3BTarget Target type: Parafine @ 0 deg  "<< endl;
  cout << endl;

  
  Double_t dx,dy,dz;
  Double_t thx, phx, thy, phy, thz, phz;
  Double_t  phi1, phi2;


  // Combi transformation: 
  dx = 0.000000;
  dy = 0.000000;
  dz = 0.000000;
  // Rotation: 
  thx = 90.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = 0.000000;     phz = 0.000000;
  TGeoRotation *pMatrix3 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
  TGeoCombiTrans *pMatrix2 = new TGeoCombiTrans("", dx, dy, dz, pMatrix3);

  // Shape: World type: TGeoBBox
  TGeoVolume* pWorld = gGeoManager->GetTopVolume();
  pWorld->SetVisLeaves(kTRUE);
  
  // Shape: TargetEnveloppe type: TGeoTubeSeg
  // TargetEnveloppe Tube 1
  Double_t rmin = 0.000000;
  Double_t rmax = 1.005000;  
  dz   = 0.006000;
  TGeoShape *pTargetEnveloppe = new TGeoTube("TargetEnveloppe",rmin,rmax,dz);
  TGeoVolume *pTargetEnveloppe_log = new TGeoVolume("TargetEnveloppe",pTargetEnveloppe, pMed2);
  pTargetEnveloppe_log->SetVisLeaves(kTRUE);
  TGeoCombiTrans* pGlobal = GetGlobalPosition(pMatrix2);
//  gTop->AddNode(pTargetEnveloppe_log, 0, pGlobal);
 
  pWorld->AddNode(pTargetEnveloppe_log, 0, pGlobal);
 

  rmin = 0.000000;
  rmax = 1.000000;
  dz   = 0.005500;
  phi1 = 0.000000;
  phi2 = 360.000000;
  TGeoShape *pParafin0deg = new TGeoTubeSeg("Parafin0deg",
					    rmin, rmax, dz,
					    phi1, phi2);
  TGeoVolume* pParafin0deg_log =
    new TGeoVolume("Parafin0deg", pParafin0deg, pMed38);
  pParafin0deg_log->SetVisLeaves(kTRUE);

  
  //TGeoCombiTrans* pGlobal = GetGlobalPosition(pMatrix2);
  //gTop->AddNode(pParafin0deg_log, 0, pGlobal);
  pTargetEnveloppe_log->AddNode(pParafin0deg_log, 0, pGlobal);
}





//void ConstructGeometry3(TGeoMedium *pMed38)
void ConstructGeometry3(TGeoMedium *pMed38, TGeoMedium *pMed2)
{
  cout << endl;
  cout << "-I- R3BTarget:: ConstructGeometry() "<< endl;
  cout << "-I- R3BTarget Target type; Parafine @ 45 deg "<< endl;
  cout << endl;


  Double_t dx,dy,dz;
  Double_t theta, phi;
  Double_t thx, phx, thy, phy, thz, phz;
  Double_t alpha;


  // Combi transformation: 
  dx = 0.000000;
  dy = 0.000000;
  dz = 0.000000;
  // Rotation: 
  thx = 135.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = 45.000000;    phz = 0.000000;
  TGeoRotation *pMatrix3 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans *pMatrix2 = new TGeoCombiTrans("", dx,dy,dz,pMatrix3);
  
  // Shape: World type: TGeoBBox
  TGeoVolume* pWorld = gGeoManager->GetTopVolume();
  pWorld->SetVisLeaves(kTRUE);

  // Shape: TargetEnveloppe type: TGeoTubeSeg
  // TargetEnveloppe Tube 1
  dx    = 1.500000;
  dy    = 1.100000;
  dz    = 0.00600;
  alpha = 0.000000;
  theta = 45.000000;
  phi   = -180.000000;
   TGeoShape *pTargetEnveloppe = new TGeoPara("TargetEnveloppe",dx,dy,dz,alpha,theta,phi);
  TGeoVolume *pTargetEnveloppe_log = new TGeoVolume("TargetEnveloppe",pTargetEnveloppe, pMed2);
  pTargetEnveloppe_log->SetVisLeaves(kTRUE);
  TGeoCombiTrans* pGlobal = GetGlobalPosition(pMatrix2);
  //  gTop->AddNode(pTargetEnveloppe_log, 0, pGlobal);
  pWorld->AddNode(pTargetEnveloppe_log, 0, pGlobal);


  // Shape: Para45deg type: TGeoPara
  dx    = 1.450000;
  dy    = 1.000000;
  dz    = 0.005500;
  alpha = 0.000000;
  theta = 45.000000;
  phi   = -180.000000;
  TGeoShape *pPara45deg = new TGeoPara("Para45deg",dx,dy,dz,alpha,theta,phi);
  TGeoVolume *pPara45deg_log = new TGeoVolume("Para45deg",pPara45deg, pMed38);
  pPara45deg_log->SetVisLeaves(kTRUE);

  
  //TGeoCombiTrans* pGlobal = GetGlobalPosition(pMatrix2);
  //gTop->AddNode(pPara45deg_log, 0, pGlobal);
  pTargetEnveloppe_log->AddNode(pPara45deg_log, 0);
}





void ConstructGeometry4(TGeoMedium *pMed2, TGeoMedium *pMed15,
			TGeoMedium *pMed3, TGeoMedium *pMedAu)
{
  cout << endl;
  cout << "-I- R3BTarget:: ConstructGeometry() "<< endl;
  cout << "-I- R3BTarget Target type: LiH "<< endl;
  cout << endl;
  
  
  Double_t dx,dy,dz;
  Double_t thx, phx, thy, phy, thz, phz;
  Double_t phi1, phi2;


  // Combi transformation: 
  dx = 0.000000;
  dy = 0.000000;
  //dz = 3.025000;  // tanslation along z
  dz=3.0; // this means the origine (0,0,0) starts at the edge of the Mylar entrance foil and the H2 liq.
  // Rotation: 
  thx = 90.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = 0.000000;    phz = 0.000000;
  TGeoRotation *pMatrix3 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans *pMatrix2 = new TGeoCombiTrans("", dx,dy,dz,pMatrix3);
  dz=dz*2.+ 5.5;
  TGeoCombiTrans *pMatrix2b = new TGeoCombiTrans("", dx,dy,dz,pMatrix3);
  dz=dz + 5.5 + 1.02500;
  TGeoCombiTrans *pMatrix2c = new TGeoCombiTrans("", dx,dy,dz,pMatrix3);

  // Combi transformation: 
  dx = 0.000000;
  dy = 0.000000;
  dz = 0.000000; //1.765000;
  // Rotation: 
  thx = 90.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = 0.000000;    phz = 0.000000;
  TGeoRotation *pMatrix5 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans *pMatrix4 = new TGeoCombiTrans("", dx,dy,dz,pMatrix5);

  // Combi transformation: 
  dx = 0.000000;
  dy = 0.000000;
  dz = 0.000000; //0.007500;
  // Rotation: 
  thx = 90.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = 0.000000;    phz = 0.000000;
  TGeoRotation *pMatrix7 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans *pMatrix6 = new TGeoCombiTrans("", dx,dy,dz,pMatrix7);

  // Combi transformation: 
  dx = 0.000000;
  dy = 0.000000;
  //dz = 3.012500;  // 0.007500;
  dz = 3.00500;  // 0.007500;
  // Rotation: 
  thx = 90.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = 0.000000;    phz = 0.000000;
  TGeoRotation *pMatrix9 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans *pMatrix8 = new TGeoCombiTrans("", dx,dy,dz,pMatrix9);

  // Combi transformation: 
  dx = 0.000000;
  dy = 0.000000;
  //dz = -3.012500;  // 3.522500;
  dz = -3.00500;  // 3.522500;
  // Rotation: 
  thx = 90.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = 0.000000;    phz = 0.000000;
  TGeoRotation *pMatrix11 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans *pMatrix10 = new TGeoCombiTrans("", dx,dy,dz,pMatrix11);

  // Combi transformation: 
  dx = 0.000000;
  dy = 0.000000;
  dz = 8.4500;// cone  for final specs 
  // Rotation: 
  thx = 90.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = 0.000000;    phz = 0.000000;
  TGeoRotation *pMatrix13 = new TGeoRotation("", thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans *pMatrix12 = new TGeoCombiTrans("", dx,dy,dz,pMatrix13);

  // Shape: World type: TGeoBBox
  TGeoVolume* pWorld = gGeoManager->GetTopVolume();
  pWorld->SetVisLeaves(kTRUE);

  // Shape: TargetEnveloppe type: TGeoTubeSeg
  // TargetEnveloppe Tube 1
  Double_t rmin = 0.000000;
  //Double_t rmax = 3.525000;  
  //dz   = 3.0250;
  Double_t rmax = 3.51000;  
  dz   = 3.010;
  TGeoShape *pTargetEnveloppe = new TGeoTube("TargetEnveloppe",rmin,rmax,dz);
  TGeoVolume *pTargetEnveloppe_log = new TGeoVolume("TargetEnveloppe",pTargetEnveloppe, pMed2);
  pTargetEnveloppe_log->SetVisLeaves(kTRUE);
  TGeoCombiTrans* pGlobal = GetGlobalPosition(pMatrix2);
  //  gTop->AddNode(pTargetEnveloppe_log, 0, pGlobal);
  pWorld->AddNode(pTargetEnveloppe_log, 0, pGlobal);
  
  
  // Shape: Target1 type: TGeoTubeSeg
  // Mylar Tube
  rmin = 3.5;          //1.000000;
  //rmax = 3.5250;     //for 250 Mylar
  rmax = 3.510;        //for 100 Mylar
  dz   = 3.;           //1.750000;
  TGeoShape *pTarget1 = new TGeoTube("Target1",rmin,rmax,dz);
  TGeoVolume *pTarget1_log = new TGeoVolume("Target1",pTarget1, pMed15);   // Mylar
  pTarget1_log->SetVisLeaves(kTRUE);
  pTargetEnveloppe_log->AddNode(pTarget1_log, 0, pMatrix4);
  
  
  // Shape: Target2 type: TGeoTubeSeg
  // H2 Tube 1
  rmin = 0.000000;
  rmax = 3.5;           //1.000000;
  dz   = 3.;            //1.750000;
  TGeoShape *pTarget2 = new TGeoTube("Target2",rmin,rmax,dz);
  TGeoVolume *pTarget2_log = new TGeoVolume("Target2",pTarget2, pMed3);   // H2
  pTarget2_log->SetVisLeaves(kTRUE);
  pTargetEnveloppe_log->AddNode(pTarget2_log, 0, pMatrix6);
  
  
  // Shape: Target3 type: TGeoTubeSeg
  rmin = 0.000000;
  //rmax = 3.5250;    //for 250 mylar;
  //dz   = 0.012500;  // =half thickness for Mylar
  rmax = 3.510;     //for 100 mylar;
  dz   = 0.00500; // =half thickness for Mylar
  TGeoShape *pTarget3 = new TGeoTube("Target3",rmin,rmax,dz);
  TGeoVolume *pTarget3_log = new TGeoVolume("Target3",pTarget3, pMed15);    // Mylar
  pTarget3_log->SetVisLeaves(kTRUE);
  pTargetEnveloppe_log->AddNode(pTarget3_log, 0, pMatrix8);
  pTargetEnveloppe_log->AddNode(pTarget3_log, 1, pMatrix10);
  
  
  // Shape: Target4 type: TGeoTubeSeg
  rmin = 6.637;   // Cone
  rmax = 6.647;   // Cone   0.1mm
  Double_t rmin2 = 1.6;     // Cone
  Double_t rmax2 = 1.61;    // Cone  0.1mm
  dz   = 10.60000;   // Cone =half length for final specs
  phi1 = 0.000000;
  phi2 = 360.000000;
  TGeoShape *pTarget4 = new TGeoCone("Target4",dz,rmin,rmax,rmin2,rmax2);
  TGeoVolume *pTarget4_log = new TGeoVolume("Target4",pTarget4, pMedAu);    // Gold
  pTarget4_log->SetVisLeaves(kTRUE);
//gTop->AddNode(pTarget4_log, 0, pMatrix12);
//pWorld->AddNode(pTarget4_log, 0, pMatrix12);
}





void ConstructGeometry5(TGeoMedium *pMedCarbon)
{
  cout << endl;
  cout << "-I- R3BTarget:: ConstructGeometry() "<< endl;
  cout << "-I- R3BTarget Target type: Carbon target (2.01 mm) "<< endl;
  cout << endl;
  

  Double_t dx, dy, dz;
  Double_t thx, thy, thz;
  Double_t phx, phy, phz;


  // Combi transformation: 
  dx = 0.000000;
  dy = 0.000000;
  dz = 0.000000;	//-(0.15cm+half thickness)
  // Rotation: 
  thx = 90.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = 0.000000;     phz = 0.000000;
  TGeoRotation *pRot = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans *pMatrix = new TGeoCombiTrans("", dx,dy,dz,pRot);

  // Shape: World type: TGeoBBox
  TGeoVolume* pWorld = gGeoManager->GetTopVolume();
  pWorld->SetVisLeaves(kTRUE);

  dx = 1.500000;	//s318
  dy = 1.500000;	//s318
  dz = 0.1005;		//s318 half length?!?
  TGeoShape *pCarbonTarget = new TGeoBBox("CarbonTarget", dx,dy,dz);
  TGeoVolume *pCarbonTarget_log = new TGeoVolume("CarbonTarget_log",pCarbonTarget, pMedCarbon);
  pCarbonTarget_log->SetVisLeaves(kTRUE);

  TGeoVolumeAssembly *keep = new TGeoVolumeAssembly("Target");
  keep->AddNode(pCarbonTarget_log, 0, pMatrix);

  TGeoCombiTrans* pGlobal = GetGlobalPosition(pMatrix);
  // gTop->AddNode(keep, 0, pGlobal);
  pWorld->AddNode(keep, 0, pGlobal);
}





void ConstructGeometry6(TGeoMedium *pMed16)
{
  cout << endl;
  cout << "-I- R3BTarget::ConstructGeometry() "<< endl;
  cout << "-I- R3BTarget Target type: CH2 target (2.31 mm) "<< endl;
  cout << endl;


  Double_t dx, dy, dz;
  Double_t thx, thy, thz;
  Double_t phx, phy, phz;


  // Combi transformation: 
  dx = 0.000000;
  dy = 0.000000;
  dz = 0.000000;       //-(0.15cm+half thickness)
  // Rotation: 
  thx = 90.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = 0.000000;     phz = 0.000000;
  TGeoRotation *pRot = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans *pMatrix = new TGeoCombiTrans("", dx,dy,dz,pRot);

  // Shape: World type: TGeoBBox
  TGeoVolume* pWorld = gGeoManager->GetTopVolume();
  pWorld->SetVisLeaves(kTRUE);


  dx = 1.500000;	//s318
  dy = 1.500000;	//s318
  dz = 0.1155;		//s318
  TGeoShape *pCH2Target = new TGeoBBox("CH2Target", dx,dy,dz);
  TGeoVolume *pCH2Target_log = new TGeoVolume("CH2Target_log",pCH2Target, pMed16);
  pCH2Target_log->SetVisLeaves(kTRUE);
  
  TGeoVolumeAssembly *keep = new TGeoVolumeAssembly("Target");
  keep->AddNode(pCH2Target_log, 0, pMatrix);

  TGeoCombiTrans* pGlobal = GetGlobalPosition(pMatrix);
  // gTop->AddNode(keep, 0, pGlobal);
  pWorld->AddNode(keep, 0, pGlobal);
}





void ConstructGeometry7(TGeoMedium *pMed)
{
  cout << endl;
  cout << "-I- R3BTarget:: ConstructGeometry() "<< endl;
  cout << "-I- R3BTarget Target type: Empty target (vacuum) "<< endl;
  cout << endl;
  

  Double_t dx, dy, dz;
  Double_t thx, thy, thz;
  Double_t phx, phy, phz;


  // Combi transformation: 
  dx = 0.000000;
  dy = 0.000000;
  dz = 0.000000;	//-(0.15cm+half thickness)
  // Rotation: 
  thx = 90.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = 0.000000;     phz = 0.000000;
  TGeoRotation *pRot = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans *pMatrix = new TGeoCombiTrans("", dx,dy,dz,pRot);

  // Shape: World type: TGeoBBox
  TGeoVolume* pWorld = gGeoManager->GetTopVolume();
  pWorld->SetVisLeaves(kTRUE);


  dx = 1.500000;	//s318
  dy = 1.500000;	//s318
  dz = 0.1000;		//arbitrary thickness, 2mm for convenience
  TGeoShape *pVacuumTarget = new TGeoBBox("VacuumTarget", dx,dy,dz);
  TGeoVolume *pVacuumTarget_log = new TGeoVolume("VacuumTarget_log",pVacuumTarget, pMed);
  pVacuumTarget_log->SetVisLeaves(kTRUE);

  TGeoVolumeAssembly *keep = new TGeoVolumeAssembly("Target");
  keep->AddNode(pVacuumTarget_log, 0, pMatrix);

  TGeoCombiTrans* pGlobal = GetGlobalPosition(pMatrix);
  //  gTop->AddNode(keep, 0, pGlobal);
  pWorld->AddNode(keep, 0, pGlobal);
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
