#include <iomanip>
#include <iostream>
#include "TGeoManager.h"
#include "TMath.h"




// Create Matrix Unity
TGeoRotation *fGlobalRot = new TGeoRotation();

// Create a null translation
TGeoTranslation *fGlobalTrans = new TGeoTranslation();
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
Bool_t fLabTrans = kFALSE;




void create_aladin_geo(const char* geoTag)
{
  fGlobalTrans->SetTranslation(0.0,0.0,0.0);

  Double_t Aladin_width = 156.0;
  Double_t Aladin_length = 176.0;
  Double_t Aladin_gap = 52.;
  Double_t Aladin_angle = -7.0;
  Double_t DistanceToTarget = 350.0;
  Double_t Yoke_thickness = 52.;
  Double_t Correction = -95.0;
  Double_t DistanceFromtargetToAladinCenter = DistanceToTarget + Correction;
  TGeoRotation *rot_aladin = new TGeoRotation("Aladinrot");
  rot_aladin->RotateY(Aladin_angle);
  TGeoRotation *rot_mirror = new TGeoRotation("mirror");
  rot_mirror->RotateZ(180.0);
  rot_mirror->RotateY(Aladin_angle);
  
  
  
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
  TString geoFileName = geoPath + "/geometry/aladin_";
  geoFileName = geoFileName + geoTag + ".geo.root";
  // --------------------------------------------------------------------------
  
  
  
  // -----------------   Get and create the required media    -----------------
  FairGeoMedia*   geoMedia = geoFace->getMedia();
  FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();
  
  FairGeoMedium* mFe      = geoMedia->getMedium("iron");
  if ( ! mFe ) Fatal("Main", "FairMedium iron not found");
  geoBuild->createMedium(mFe);
  TGeoMedium* pMedFe = gGeoMan->GetMedium("iron");
  if ( ! pMedFe ) Fatal("Main", "Medium iron not found");
  
  FairGeoMedium* mHe      = geoMedia->getMedium("helium");
  if ( ! mHe ) Fatal("Main", "FairMedium helium not found");
  geoBuild->createMedium(mHe);
  TGeoMedium* pMed4 = gGeoMan->GetMedium("helium");
  if ( ! pMed4 ) Fatal("Main", "Medium helium not found");
  
  FairGeoMedium* mAl      = geoMedia->getMedium("aluminium");
  if ( ! mAl ) Fatal("Main", "FairMedium aluminium not found");
  geoBuild->createMedium(mAl);
  TGeoMedium* pMed21 = gGeoMan->GetMedium("aluminium");
  if ( ! pMed21 ) Fatal("Main", "Medium aluminium not found");
  
  FairGeoMedium* mAir      = geoMedia->getMedium("Air");
  if ( ! mAir ) Fatal("Main", "FairMedium Air not found");
  geoBuild->createMedium(mAir);
  TGeoMedium* pMed2 = gGeoMan->GetMedium("Air");
  if ( ! pMed2 ) Fatal("Main", "Medium Air not found");
  
  FairGeoMedium* mSteel      = geoMedia->getMedium("Steel");
  if ( ! mSteel ) Fatal("Main", "FairMedium Steel not found");
  geoBuild->createMedium(mSteel);
  TGeoMedium* pMedSteel = gGeoMan->GetMedium("Steel");
  if ( ! pMedSteel ) Fatal("Main", "Medium Steel not found");
  
  FairGeoMedium* mMylar      = geoMedia->getMedium("mylar");
  if ( ! mMylar ) Fatal("Main", "FairMedium mylar not found");
  geoBuild->createMedium(mMylar);
  TGeoMedium* pMedMylar = gGeoMan->GetMedium("mylar");
  if ( ! pMedMylar ) Fatal("Main", "Medium mylar not found");
  // --------------------------------------------------------------------------
  
  
  
  // --------------   Create geometry and top volume  -------------------------
  gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoMan->SetName("ALADINgeom");
  TGeoVolume* top = new TGeoVolumeAssembly("TOP");
  gGeoMan->SetTopVolume(top);
  // --------------------------------------------------------------------------
  
  
  
  TGeoVolume *cell = new TGeoVolumeAssembly("ALADINCELL");
  
  
  Double_t degrad = TMath::Pi()/180.;
  Double_t dx,dy,dz;
  Double_t pDx1,pDx2,pDx3,pDx4,pDy1,pDy2,pDz,pTheta,pPhi,pAlp1,pAlp2;
  Double_t fDx1, fDx2, fDy1, fDy2, fDz;
  Double_t thx, phx, thy, phy, thz, phz;
  
  
  // Yoke UP
  TGeoShape *solidFeYoke_up = new TGeoBBox("FeYoke_up",
                                           Aladin_width/2.0,
                                           Yoke_thickness/2.0,
                                           Aladin_length/2.0);
  
  TGeoTranslation *tr1 = new TGeoTranslation(0.0,
                                             Aladin_gap/2.0 + Yoke_thickness/2.0,
                                             DistanceFromtargetToAladinCenter
                                             );
  
  TGeoCombiTrans *t1
  = new TGeoCombiTrans(*tr1,*rot_aladin);
  
  TGeoVolume* pVolFeYoke_up = new TGeoVolume("FeYokeVolup",solidFeYoke_up, pMedFe);
  pVolFeYoke_up->SetVisLeaves(kTRUE);
  
  // Yoke Down
  TGeoShape *solidFeYoke_down = new TGeoBBox("FeYoke_down",
                                             Aladin_width/2.0,
                                             Yoke_thickness/2.0,
                                             Aladin_length/2.0);
  
  TGeoTranslation *tr2 = new TGeoTranslation(0.0,
                                             -1.0*(Aladin_gap/2.0+ Yoke_thickness/2.0),
                                             DistanceFromtargetToAladinCenter
                                             );
  
  TGeoCombiTrans *t2
  = new TGeoCombiTrans(*tr2,*rot_aladin);
  
  TGeoVolume* pVolFeYoke_down = new TGeoVolume("FeYokeVoldown",solidFeYoke_down, pMedFe);
  pVolFeYoke_down->SetVisLeaves(kTRUE);
  
  // Body
  //  --- Left side
  Double_t x0,y0,z0;
  Double_t xrot,yrot,zrot;
  //
  x0 = Aladin_width/2.0 + Yoke_thickness/2.0;
  y0 = 0.0;
  z0 = 0.0;
  zrot = z0*TMath::Cos(Aladin_angle*degrad) - x0*TMath::Sin(Aladin_angle*degrad) +
  DistanceFromtargetToAladinCenter;
  xrot = -1.0*z0*TMath::Sin(Aladin_angle*degrad) + x0*TMath::Cos(Aladin_angle*degrad);
  yrot = y0;
  
  // Yoke Left sided
  TGeoShape *solidFeYoke_left = new TGeoBBox("FeYoke_left",
                                             Yoke_thickness/2.0,
                                             Aladin_gap/2.0+Yoke_thickness,
                                             Aladin_length/2.0);
  
  TGeoTranslation *tr3 = new TGeoTranslation(xrot,
                                             yrot,
                                             zrot
                                             );
  
  TGeoCombiTrans *t3
  = new TGeoCombiTrans(*tr3,*rot_aladin);
  
  TGeoVolume* pVolFeYoke_left = new TGeoVolume("FeYokeVolleft",solidFeYoke_left, pMedFe);
  pVolFeYoke_left->SetVisLeaves(kTRUE);
  
  //  --- Right side
  x0 = -1.0 *(Aladin_width/2.0 + Yoke_thickness/2.0) ;
  y0 = 0.0;
  z0 = 0.0;
  zrot = z0*cos(Aladin_angle*degrad) - x0*sin(Aladin_angle*degrad) +
  DistanceFromtargetToAladinCenter;
  xrot = -1.0*z0*sin(Aladin_angle*degrad) + x0*cos(Aladin_angle*degrad);
  yrot = y0;
  
  // Yoke Down
  TGeoShape *solidFeYoke_right = new TGeoBBox("FeYoke_right",
                                              Yoke_thickness/2.0,
                                              Aladin_gap/2.0+Yoke_thickness,
                                              Aladin_length/2.0);
  
  TGeoTranslation *tr4 = new TGeoTranslation(xrot,
                                             yrot,
                                             zrot
                                             );
  
  TGeoCombiTrans *t4
  = new TGeoCombiTrans(*tr4,*rot_aladin);
  
  TGeoVolume* pVolFeYoke_right = new TGeoVolume("FeYokeVolright",solidFeYoke_right, pMedFe);
  pVolFeYoke_right->SetVisLeaves(kTRUE);
  
  
  
  cell->AddNode(pVolFeYoke_up,   1, t1);
  cell->AddNode(pVolFeYoke_down, 2, t2);
  cell->AddNode(pVolFeYoke_left, 3, t3);
  cell->AddNode(pVolFeYoke_right,4, t4);
  
  
  
  //-----------------------------------------------------------
  
  TGeoCombiTrans *pMatrix0 = new TGeoCombiTrans("");
  
  // Aladin chamber part1
  // Combi transformation:
  dx = 6.000000;
  dy = 0.000000;
  dz = 206.0;
  TGeoRotation *gRot1 = new TGeoRotation();
  gRot1->RotateX(0.);
  gRot1->RotateY(-7.000000);
  gRot1->RotateZ(0.);
  TGeoCombiTrans *pMatrix58 = new TGeoCombiTrans("", dx,dy,dz,gRot1);
  
  // Aladin chamber part2
  // Combi transformation:
  dx = -5.000000;
  dy = 0.000000;
  dz = 295.3;
  TGeoRotation *gRot2 = new TGeoRotation();
  gRot2->RotateX(0.);
  gRot2->RotateY(-7.000000);
  gRot2->RotateZ(0.);
  TGeoCombiTrans *pMatrix59 = new TGeoCombiTrans("", dx,dy,dz,gRot2);
  
  // Aladin chamber part3
  // Combi transformation:
  dx = -11.000000;
  dy = 0.000000;
  dz = 344.0;
  TGeoRotation *gRot3 = new TGeoRotation();
  gRot3->RotateX(0.);
  gRot3->RotateY(-7.000000);
  gRot3->RotateZ(0.);
  TGeoCombiTrans *pMatrix60 = new TGeoCombiTrans("", dx,dy,dz,gRot3);
  
  
  
  // Helium-Aladin chamber part1
  // Combi transformation:
  dx = 6.000000;
  dy = 0.000000;
  dz = 206.0;
  TGeoRotation *gRot4 = new TGeoRotation();
  gRot4->RotateX(0.);
  gRot4->RotateY(-7.000000);
  gRot4->RotateZ(0.);
  TGeoCombiTrans *pMatrix61 = new TGeoCombiTrans("", dx,dy,dz,gRot4);
  
  // Helium-Aladin chamber part2
  // Combi transformation:
  dx = -5.000000;
  dy = 0.000000;
  dz = 295.3;
  TGeoRotation *gRot5 = new TGeoRotation();
  gRot5->RotateX(0.);
  gRot5->RotateY(-7.000000);
  gRot5->RotateZ(0.);
  TGeoCombiTrans *pMatrix62 = new TGeoCombiTrans("", dx,dy,dz,gRot5);
  
  // Helium-Aladin chamber part3
  // Combi transformation:
  dx = -11.000000;
  dy = 0.000000;
  dz = 344.0;
  TGeoRotation *gRot6 = new TGeoRotation();
  gRot6->RotateX(0.);
  gRot6->RotateY(-7.000000);
  gRot6->RotateZ(0.);
  TGeoCombiTrans *pMatrix63 = new TGeoCombiTrans("", dx,dy,dz,gRot6);
  
  
  
  // incoming window - Steel
  // Combi transformation:
  dx = 16.400000;
  dy = 0.000000;
  dz = 117.5;
  TGeoRotation *gRot7 = new TGeoRotation();
  gRot7->RotateX(0.);
  gRot7->RotateY(-0.500000);
  gRot7->RotateZ(0.);
  TGeoCombiTrans *pMatrix64 = new TGeoCombiTrans("", dx,dy,dz,gRot7);
  
  // outgoing window - Mylar
  // Combi transformation:
  dx = -16.000000;
  dy = 0.000000;
  dz = 375.0;
  TGeoRotation *gRot8 = new TGeoRotation();
  gRot8->RotateX(0.);
  gRot8->RotateY(-7.000000);
  gRot8->RotateZ(0.);
  TGeoCombiTrans *pMatrix65 = new TGeoCombiTrans("", dx,dy,dz,gRot8);
  
  
  
  // incoming helium chamber befor Aladin
  // Combi transformation:
  dx = 15.500000;
  dy = 0.000000;
  dz = 126.1;
  TGeoRotation *gRot9 = new TGeoRotation();
  gRot9->RotateX(0.);
  gRot9->RotateY(-93.700000);
  gRot9->RotateZ(0.);
  TGeoCombiTrans *pMatrix66 = new TGeoCombiTrans("", dx,dy,dz,gRot9);
  
  
  
  // Aladin chamber part1
  Double_t xy[8][2];
  xy[0][0] = 60.90;	xy[0][1] = -21.5;
  xy[1][0] = 60.90;	xy[1][1] = 21.5;
  xy[2][0] = -60.90;	xy[2][1] = 21.5;
  xy[3][0] = -60.90;	xy[3][1] = -21.5;
  xy[4][0] = 67.1;	xy[4][1] = -22.3;
  xy[5][0] = 67.1;	xy[5][1] = 22.3;
  xy[6][0] = -67.1;	xy[6][1] = 22.3;
  xy[7][0] = -67.1;	xy[7][1] = -22.3;
  TGeoShape *pAladinChamber1 = new TGeoArb8("AladinChamber1", 72.0, &xy[0][0]);
  TGeoVolume* pAladinChamberLog1 = new TGeoVolume("AladinChamberLog1",pAladinChamber1, pMed21);
  //   pAWorld->AddNode(pAladinChamberLog,1,pMatrix58);
  
  // Aladin chamber part2
  //   Double_t xy[8][2];
  xy[0][0] = 67.1;	xy[0][1] = -22.3;
  xy[1][0] = 67.1;	xy[1][1] = 22.3;
  xy[2][0] = -67.1;	xy[2][1] = 22.3;
  xy[3][0] = -67.1;	xy[3][1] = -22.3;
  xy[4][0] = 69.1;	xy[4][1] = -23.4;
  xy[5][0] = 69.1;	xy[5][1] = 23.4;
  xy[6][0] = -69.1;	xy[6][1] = 23.4;
  xy[7][0] = -69.1;	xy[7][1] = -23.4;
  TGeoShape *pAladinChamber2 = new TGeoArb8("AladinChamber2", 17.8, &xy[0][0]);
  TGeoVolume* pAladinChamberLog2 = new TGeoVolume("AladinChamberLog2",pAladinChamber2, pMed21);
  
  // Aladin chamber part3
  //   Double_t xy[8][2];
  xy[0][0] = 69.1;	xy[0][1] = -23.4;
  xy[1][0] = 69.1;	xy[1][1] = 23.4;
  xy[2][0] = -69.1;	xy[2][1] = 23.4;
  xy[3][0] = -69.1;	xy[3][1] = -23.4;
  xy[4][0] = 79.05;	xy[4][1] = -28.5;
  xy[5][0] = 79.05;	xy[5][1] = 28.5;
  xy[6][0] = -79.05;	xy[6][1] = 28.5;
  xy[7][0] = -79.05;	xy[7][1] = -28.5;
  TGeoShape *pAladinChamber3 = new TGeoArb8("AladinChamber3", 30.95, &xy[0][0]);
  TGeoVolume* pAladinChamberLog3 = new TGeoVolume("AladinChamberLog3",pAladinChamber3, pMed21);
  
  
  
  // Helium-Aladin chamber part1
  Double_t ab[8][2];
  ab[0][0] = 60.40;	ab[0][1] = -21;
  ab[1][0] = 60.40;	ab[1][1] = 21;
  ab[2][0] = -60.40;	ab[2][1] = 21;
  ab[3][0] = -60.40;	ab[3][1] = -21;
  ab[4][0] = 66.6;	ab[4][1] = -21.8;
  ab[5][0] = 66.6;	ab[5][1] = 21.8;
  ab[6][0] = -66.6;	ab[6][1] = 21.8;
  ab[7][0] = -66.6;	ab[7][1] = -21.8;
  TGeoShape *pHeliumAladinChamber1 = new TGeoArb8("HeliumAladinChamber1", 72.0, &ab[0][0]);
  TGeoVolume* pHeliumAladinChamberLog1 = new TGeoVolume("HeliumAladinChamberLog1",pHeliumAladinChamber1, pMed4);
  
  // Helium-Aladin chamber part2
  ab[0][0] = 66.6;	ab[0][1] = -21.8;
  ab[1][0] = 66.6;	ab[1][1] = 21.8;
  ab[2][0] = -66.6;	ab[2][1] = 21.8;
  ab[3][0] = -66.6;	ab[3][1] = -21.8;
  ab[4][0] = 68.6;	ab[4][1] = -22.9;
  ab[5][0] = 68.6;	ab[5][1] = 22.9;
  ab[6][0] = -68.6;	ab[6][1] = 22.9;
  ab[7][0] = -68.6;	ab[7][1] = -22.9;
  TGeoShape *pHeliumAladinChamber2 = new TGeoArb8("HeliumAladinChamber2", 17.8, &ab[0][0]);
  TGeoVolume* pHeliumAladinChamberLog2 = new TGeoVolume("HeliumAladinChamberLog2",pHeliumAladinChamber2, pMed4);
  
  // Helium-Aladin chamber part3
  ab[0][0] = 68.6;	ab[0][1] = -22.9;
  ab[1][0] = 68.6;	ab[1][1] = 22.9;
  ab[2][0] = -68.6;	ab[2][1] = 22.9;
  ab[3][0] = -68.6;	ab[3][1] = -22.9;
  ab[4][0] = 78.55;	ab[4][1] = -28.0;
  ab[5][0] = 78.55;	ab[5][1] = 28.0;
  ab[6][0] = -78.55;	ab[6][1] = 28.0;
  ab[7][0] = -78.55;	ab[7][1] = -28.0;
  TGeoShape *pHeliumAladinChamber3 = new TGeoArb8("HeliumAladinChamber3", 30.95, &ab[0][0]);
  TGeoVolume* pHeliumAladinChamberLog3 = new TGeoVolume("HeliumAladinChamberLog3",pHeliumAladinChamber3, pMed4);
  
  
  // incoming window shape - Steel
  dx = 61.000000;
  dy = 26.000000;
  dz = 0.006500;
  TGeoShape *pinWINBox = new TGeoBBox("inWINBox", dx,dy,dz);
  TGeoVolume *pinWINLog = new TGeoVolume("inWINLog",pinWINBox, pMedSteel);
  
  // outgoing window shape - Mylar
  dx = 84.000000;
  dy = 34.000000;
  dz = 0.004950;
  TGeoShape *poutWINBox = new TGeoBBox("outWINBox", dx,dy,dz);
  TGeoVolume *poutWINLog = new TGeoVolume("outWINLog",poutWINBox, pMedMylar);
  
  
  // incoming helium chamber befor Aladin
  pDx1 = 11.685500;  //down
  pDx2 = 11.685500;  //down
  pDx3 = 4.863500;  //up
  pDx4 = 4.863500;  //up
  pDy1 = 19.900000;  //down
  pDy2 = 19.900000;  //up
  pDz  = 60.400000;
  pTheta = 0.000000;
  pPhi = 0.000000;
  pAlp1 = 0.00000;
  pAlp2 = 0.00000;
  TGeoShape *pinHELBox = new TGeoTrap("inHELBox", pDz,pTheta,pPhi,pDy1,pDx1,pDx2,pAlp1,pDy2,pDx3,pDx4,pAlp2);
  TGeoVolume *pinHELLog = new TGeoVolume("inHELLog",pinHELBox, pMed4);
  
  //
  
  /******************************************************/
  /*************      Aladin Front Flange  ************************/
  /*****************************************************/
  
  //Material steel, thickness 4cm, inner diameter: 20cm, outer diameter: 133.4 cm. Distance from xb centre: 112.87 cm
  //Combi transformation:
  dx = 0.000000;
  dy = 0.000000;
  dz = 114.870000;	//cm
  // Rotation:
  //thx = 0.000000;    phx = 0.000000;
  //thy = 0.000000;    phy = 0.000000;
  //thz = 0.000000;    phz = 0.000000;
  TGeoRotation *pMatrix15 = new TGeoRotation("ro0");
  TGeoCombiTrans* pMatrix14 = new TGeoCombiTrans("", dx,dy,dz,pMatrix15);
  
  double rmin = 10.000000;//radius, not diameter
  double rmax = 66.700000;
  dz   = 2.000000;//half thickness
  double phi1 = 0.000000;
  double phi2 = 360.000000;
  TGeoShape *pAladinFrontFlange = new TGeoTubeSeg("AladinFrontFlange",rmin,rmax,dz,phi1,phi2);
  TGeoVolume*  pAladinFrontFlangeLog = new TGeoVolume("AladinFrontFlangeLog",pAladinFrontFlange, pMedSteel);
  
  cell->AddNode(pAladinFrontFlangeLog,1,pMatrix14);
  
  /******************************************************/
  /*************      Aladin shape corrections  ************************/
  /*****************************************************/
  
  // Combi transformation "up":
  dx = 0.000000;
  dy = 25.900000;
  dz = 255.0;
  TGeoRotation *gRot11 = new TGeoRotation();
  gRot11->RotateX(180.);
  gRot11->RotateY(-7.000000);
  gRot11->RotateZ(0.);
  TGeoCombiTrans *pMatrix67 = new TGeoCombiTrans("", dx,dy,dz,gRot11);
  
  // shape "up":
  Double_t cd[8][2];
  cd[0][0] = 78.0;     cd[0][1] = 0.0;
  cd[1][0] = 78.0;     cd[1][1] = 0.001;
  cd[2][0] = -78.0;    cd[2][1] = 0.001;
  cd[3][0] = -78.0;    cd[3][1] = 0.0;
  cd[4][0] = 78.0;     cd[4][1] = 0.0;
  cd[5][0] = 78.0;     cd[5][1] = 1.0;
  cd[6][0] = -78.0;    cd[6][1] = 1.0;
  cd[7][0] = -78.0;    cd[7][1] = 0.0;
  TGeoShape *pAupBox = new TGeoArb8("AupBox", 88.0, &cd[0][0]);
  TGeoVolume* pAupLog = new TGeoVolume("AupLog",pAupBox, pMedFe);
  
  
  // Combi transformation "down":
  dx = 0.000000;
  dy = -25.900000;
  dz = 255.0;
  TGeoRotation *gRot12 = new TGeoRotation();
  gRot12->RotateX(0.);
  gRot12->RotateY(-7.000000);
  gRot12->RotateZ(0.);
  TGeoCombiTrans *pMatrix68 = new TGeoCombiTrans("", dx,dy,dz,gRot12);
  
  // shape "down":
  cd[0][0] = 78.0;     cd[0][1] = 0.0;
  cd[1][0] = 78.0;     cd[1][1] = 1.0;
  cd[2][0] = -78.0;    cd[2][1] = 1.0;
  cd[3][0] = -78.0;    cd[3][1] = 0.0;
  cd[4][0] = 78.0;     cd[4][1] = 0.0;
  cd[5][0] = 78.0;     cd[5][1] = 0.001;
  cd[6][0] = -78.0;    cd[6][1] = 0.001;
  cd[7][0] = -78.0;    cd[7][1] = 0.0;
  TGeoShape *pAdownBox = new TGeoArb8("AdownBox", 88.0, &cd[0][0]);
  TGeoVolume* pAdownLog = new TGeoVolume("AdownLog",pAdownBox, pMedFe);
  
  
  
  //   pAWorld->AddNode(pHeliumAladinChamberLog1,1,pMatrix61);
  //   pAWorld->AddNode(pHeliumAladinChamberLog2,1,pMatrix62);
  //   pAWorld->AddNode(pHeliumAladinChamberLog3,1,pMatrix63);
  
  pAladinChamberLog1->AddNode(pHeliumAladinChamberLog1,1,pMatrix0);
  pAladinChamberLog2->AddNode(pHeliumAladinChamberLog2,1,pMatrix0);
  pAladinChamberLog3->AddNode(pHeliumAladinChamberLog3,1,pMatrix0);
  
  cell->AddNode(pAladinChamberLog1,1,pMatrix58);
  cell->AddNode(pAladinChamberLog2,1,pMatrix59);
  cell->AddNode(pAladinChamberLog3,1,pMatrix60);
  
  cell->AddNode(pinWINLog,1,pMatrix64);
  cell->AddNode(poutWINLog,1,pMatrix65);
  
  cell->AddNode(pinHELLog,1,pMatrix66);
  
  cell->AddNode(pAupLog,1,pMatrix67);
  cell->AddNode(pAdownLog,1,pMatrix68);


    TGeoVolumeAssembly* top_cell = new TGeoVolumeAssembly("ALADIN");

    TGeoCombiTrans* inverse_matr = new TGeoCombiTrans("");
    inverse_matr->SetTranslation(0., 0., -1. * DistanceFromtargetToAladinCenter);
    inverse_matr->RotateY(-1. * Aladin_angle);
    top_cell->AddNode(cell, 1, inverse_matr);

    TGeoCombiTrans* aladin_matr = new TGeoCombiTrans(0., 0., DistanceFromtargetToAladinCenter, rot_aladin);

  
  top->AddNode(top_cell, 1, aladin_matr);
  
  
  
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

