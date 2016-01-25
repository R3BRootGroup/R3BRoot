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
Bool_t fLabTrans = kFALSE;

TGeoCombiTrans* GetGlobalPosition(TGeoCombiTrans *fRef);

// --------------------------------------------------------------------------
// Configurable geometry for the CALIFA Demonstrator.
// Use this macro to create root files with the CALIFA Demonstrator geometry
// for different configurations and positions/angles of the petals.
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// Select the position and type of petal in each posible position
// of the Demonstrator stand filling the vector petals.
// There are six elements, one per sector. Possible values are 0 (no petal),
// 1 (single petal) or 2 (double petal).
// Example: Int_t* petals={0,1,0,2,1,0} represents two single petals in
// sectors 1 and 4 and a double petal in sector 3 (sectors start at zero).
// --------------------------------------------------------------------------
Int_t petals[6]={2,0,1,1,1,0};
// --------------------------------------------------------------------------
// The Demonstrator stand permits a flexible positioning of the petals.
// A rotation and translation PER PETAL can be defined. This transformation
// is defined with respecto to their respective stand "fixing plates".
// Displacements are quoted in cm, being Z
// Please, consult CALIFA experts (hector.alvarez@usc.es) regarding this 
// transformation. 
// --------------------------------------------------------------------------
//NOMINAL POSITION
TGeoCombiTrans matDemo_1(0., 0., 0., new TGeoRotation("rotDemo_1", 0., 0., 0.));
TGeoCombiTrans matDemo_2(0., 0., 0., new TGeoRotation("rotDemo_2", 0., 0., 0.));
TGeoCombiTrans matDemo_3(0., 0., 0., new TGeoRotation("rotDemo_3", 0., 0., 0.));
TGeoCombiTrans matDemo_4(0., 0., 0., new TGeoRotation("rotDemo_4", 0., 0., 0.));
TGeoCombiTrans matDemo_5(0., 0., 0., new TGeoRotation("rotDemo_5", 0., 0., 0.));
TGeoCombiTrans matDemo_6(0., 0., 0., new TGeoRotation("rotDemo_6", 0., 0., 0.));
/*
// EXAMPLE: S438B 
Int_t petals[6]={0,1,0,0,1,0};
TGeoCombiTrans matDemo_1(0., 1.92093, -1.75703, new TGeoRotation("rotDemo_1", 0., 20.3304, 0.));
TGeoCombiTrans matDemo_2(0., 1.92093, -1.75703, new TGeoRotation("rotDemo_2", 0., 20.3304, 0.));
TGeoCombiTrans matDemo_3(0., 1.92093, -1.75703, new TGeoRotation("rotDemo_3", 0., 20.3304, 0.));
TGeoCombiTrans matDemo_4(0., 1.92093, -1.75703, new TGeoRotation("rotDemo_4", 0., 20.3304, 0.));
TGeoCombiTrans matDemo_5(0., 1.92093, -1.75703, new TGeoRotation("rotDemo_5", 0., 20.3304, 0.));
TGeoCombiTrans matDemo_6(0., 1.92093, -1.75703, new TGeoRotation("rotDemo_6", 0., 20.3304, 0.));
*/
void create_califa_geo(const char* geoTag)
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
  TString geoFileName = geoPath + "/geometry/califa_";
  geoFileName = geoFileName + geoTag + ".geo.root";
  // --------------------------------------------------------------------------
  
  
  
  // -----------------   Get and create the required media    -----------------
  FairGeoMedia*   geoMedia = geoFace->getMedia();
  FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();
  
  FairGeoMedium* mAir      = geoMedia->getMedium("Air");
  if ( ! mAir ) Fatal("Main", "FairMedium Air not found");
  geoBuild->createMedium(mAir);
  TGeoMedium* pAirMedium = gGeoMan->GetMedium("Air");
  if ( ! pAirMedium ) Fatal("Main", "Medium Air not found");
  
  FairGeoMedium* mCsI      = geoMedia->getMedium("CsI");
  if ( ! mCsI ) Fatal("Main", "FairMedium CsI not found");
  geoBuild->createMedium(mCsI);
  TGeoMedium* pCsIMedium = gGeoMan->GetMedium("CsI");
  if ( ! pCsIMedium ) Fatal("Main", "Medium CsI not found");
  
  FairGeoMedium* mCar      = geoMedia->getMedium("CarbonFibre");
  if ( ! mCar ) Fatal("Main", "FairMedium CarbonFibre not found");
  geoBuild->createMedium(mCar);
  TGeoMedium* pCarbonFibreMedium = gGeoMan->GetMedium("CarbonFibre");
  if ( ! pCarbonFibreMedium ) Fatal("Main", "Medium CarbonFibre not found");
  
  FairGeoMedium* mLaCl      = geoMedia->getMedium("LaCl");
  if ( ! mLaCl ) Fatal("Main", "FairMedium LaCl not found");
  geoBuild->createMedium(mLaCl);
  TGeoMedium* pLaClMedium = gGeoMan->GetMedium("LaCl");
  if ( ! pLaClMedium ) Fatal("Main", "Medium LaCl not found");
  
  FairGeoMedium* mLaBr      = geoMedia->getMedium("LaBr");
  if ( ! mLaBr ) Fatal("Main", "FairMedium LaBr not found");
  geoBuild->createMedium(mLaBr);
  TGeoMedium* pLaBrMedium = gGeoMan->GetMedium("LaBr");
  if ( ! pLaBrMedium ) Fatal("Main", "Medium LaBr not found");
  
  FairGeoMedium* mTfl      = geoMedia->getMedium("Tefflon");
  if ( ! mTfl ) Fatal("Main", "FairMedium Tefflon not found");
  geoBuild->createMedium(mTfl);
  TGeoMedium* pWrappingMedium = gGeoMan->GetMedium("Tefflon");
  if ( ! pWrappingMedium ) Fatal("Main", "Medium Tefflon not found");
  
  FairGeoMedium* mAl      = geoMedia->getMedium("aluminium");
  if ( ! mAl ) Fatal("Main", "FairMedium aluminium not found");
  geoBuild->createMedium(mAl);
  TGeoMedium* pAlHousing = gGeoMan->GetMedium("aluminium");
  if ( ! pAlHousing ) Fatal("Main", "Medium aluminium not found");
  // --------------------------------------------------------------------------
  
  
  
  // --------------   Create geometry and top volume  -------------------------
  gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoMan->SetName("CALIFAgeom");
  TGeoVolume* top = new TGeoVolumeAssembly("TOP");
  gGeoMan->SetTopVolume(top);
  // --------------------------------------------------------------------------
  
  
  // Defintion of the Mother Volume
  Double_t length = 200.;
  TGeoShape *pCBWorldOut = new TGeoBBox("Califa_boxOut",
					length/2.0,
					length/2.0,
					length/2.0);
  
  
  TGeoShape *pCBWorldIn1 = new TGeoTube("Califa_Centerpart1",  // hole to accommodate the tracker
					0.,  // Rmin
					26.4, // Rmax
					100./2.0);  // half length shortened by 5cm
  
  TGeoCombiTrans *t_part1 = new TGeoCombiTrans("t_part1",0.,0.,-50,fRefRot);
  t_part1->RegisterYourself();
  
  TGeoShape *pCBCone = new TGeoCone("Califa_Cone",20.,0.,26.4,0.,3.2);
  TGeoCombiTrans *t_cone = new TGeoCombiTrans("t_cone",0.,0.,20,fRefRot);
  t_cone->RegisterYourself();
  
  TGeoShape *pCBWorldIn2 = new TGeoTube("Califa_Centerpart2",  // hole to accommodate the pipe through the end-cap
					0.,  // Rmin
					3.2,  // Rmax
					60./2.0);  // half length
  TGeoCombiTrans *t_part2 = new TGeoCombiTrans("t_part2",0.,0.,70,fRefRot);
  t_part2->RegisterYourself();
  
  TGeoCompositeShape *pCBWorld = new TGeoCompositeShape("Califa_box", " Califa_boxOut - (Califa_Centerpart1:t_part1 + Califa_Centerpart2:t_part2 + Califa_Cone:t_cone)");
  
  TGeoVolume* pWorld = new TGeoVolume("CalifaWorld", pCBWorld, pAirMedium);
  
  TGeoCombiTrans *t0 = new TGeoCombiTrans();
  TGeoCombiTrans *pGlobalc = GetGlobalPosition(t0);
  
  // add the sphere as Mother Volume
  top->AddNode(pWorld, 0, pGlobalc);
  
  
  //Color of endcap in visualization
  Color_t col_alveolus = kBlack+2;
  Color_t col_wrapping = kGreen+2;
  Color_t col_crystal = kGreen+2;
  Color_t col_crystal_left, col_crystal_right;
  
  
  //Color of barrel in visualization
  Color_t col_barrel_wrapping = kYellow;
  Color_t col_barrel_alveolus = kRed;
  Color_t col_barrel_crystal = kGreen;
  
  
  /*** Some definitions ***/
  TGeoRotation *rotUni = new TGeoRotation();          //unitary rotation
  
  Double_t* vertices_Alv=new Double_t[16];
  Double_t* vertices_inner_Alv=new Double_t[16];
  
  // --------------------------------------------------------------------------
  // Petals are made of alveoli Alveolus_9 to Alveolus_16, corresponding to
  // types (or rings) 8 to 1 respectively in the CALIFA BARREL TDR.
  // Here alveoli number increases as the polar angle decreases, opposite to
  // the TDR convention (the reason comes from the CAD definition of the alveoli).
  // --------------------------------------------------------------------------
  
  //ALVEOLI 9 (TYPE OR RING 8 IN TDR)
  vertices_Alv[0] = 0; vertices_Alv[1] = -0;
  vertices_inner_Alv[0] = 0.000999999999999997;	 vertices_inner_Alv[1] = -0.000999999999999997;
  vertices_Alv[2] = 5.98; vertices_Alv[3] = -0;
  vertices_inner_Alv[2] = 5.979;	 vertices_inner_Alv[3] = -0.000999999999999997;
  vertices_Alv[4] = 5.942019095; vertices_Alv[5] = -3.099767322;
  vertices_inner_Alv[4] = 5.941019095;	 vertices_inner_Alv[5] = -3.098767322;
  vertices_Alv[6] = 0.037980905; vertices_Alv[7] = -3.099767322;
  vertices_inner_Alv[6] = 0.038980905;	 vertices_inner_Alv[7] = -3.098767322;
  vertices_Alv[8] = -1.965623394; vertices_Alv[9] = 0.853362278;
  vertices_inner_Alv[8] = -1.964623394;	 vertices_inner_Alv[9] = 0.852362278;
  vertices_Alv[10] = 7.945623394; vertices_Alv[11] = 0.853362278;
  vertices_inner_Alv[10] = 7.944623394;	 vertices_inner_Alv[11] = 0.852362278;
  vertices_Alv[12] = 7.884567063; vertices_Alv[13] = -4.001009044;
  vertices_inner_Alv[12] = 7.883567063;	 vertices_inner_Alv[13] = -4.000009044;
  vertices_Alv[14] = -1.904567063; vertices_Alv[15] = -4.001009044;
  vertices_inner_Alv[14] = -1.903567063;	 vertices_inner_Alv[15] = -4.000009044;
  TGeoVolume *Alveolus_9=gGeoManager->MakeArb8("Alveolus_9", pCarbonFibreMedium,10, vertices_Alv);
  Alveolus_9->SetLineColor(kBlue);
  
  TGeoVolume *AlveolusInner_9=gGeoManager->MakeArb8("AlveolusInner_9", pAirMedium,9.97, vertices_inner_Alv);
  AlveolusInner_9->SetLineColor(kYellow);
  
  //ALVEOLI 10 (TYPE OR RING 7 IN TDR)
  vertices_Alv[0] = 0; vertices_Alv[1] = -0;
  vertices_inner_Alv[0] = 0.000999999999999997;	 vertices_inner_Alv[1] = -0.000999999999999997;
  vertices_Alv[2] = 6.033; vertices_Alv[3] = -0;
  vertices_inner_Alv[2] = 6.032;	 vertices_inner_Alv[3] = -0.000999999999999997;
  vertices_Alv[4] = 5.968673526; vertices_Alv[5] = -3.099332526;
  vertices_inner_Alv[4] = 5.967673526;	 vertices_inner_Alv[5] = -3.098332526;
  vertices_Alv[6] = 0.064326474; vertices_Alv[7] = -3.099332526;
  vertices_inner_Alv[6] = 0.065326474;	 vertices_inner_Alv[7] = -3.098332526;
  vertices_Alv[8] = -1.942983149; vertices_Alv[9] = 0.837117542;
  vertices_inner_Alv[8] = -1.941983149;	 vertices_inner_Alv[9] = 0.836117542;
  vertices_Alv[10] = 7.975983149; vertices_Alv[11] = 0.837117542;
  vertices_inner_Alv[10] = 7.974983149;	 vertices_inner_Alv[11] = 0.836117542;
  vertices_Alv[12] = 7.87426158; vertices_Alv[13] = -4.016322814;
  vertices_inner_Alv[12] = 7.87326158;	 vertices_inner_Alv[13] = -4.015322814;
  vertices_Alv[14] = -1.84126158; vertices_Alv[15] = -4.016322814;
  vertices_inner_Alv[14] = -1.84026158;	 vertices_inner_Alv[15] = -4.015322814;
  TGeoVolume *Alveolus_10=gGeoManager->MakeArb8("Alveolus_10", pCarbonFibreMedium,10, vertices_Alv);
  Alveolus_10->SetLineColor(kBlue);
  
  TGeoVolume *AlveolusInner_10=gGeoManager->MakeArb8("AlveolusInner_10", pAirMedium,9.97, vertices_inner_Alv);
  AlveolusInner_10->SetLineColor(kYellow);
  
  //ALVEOLI 11 (TYPE OR RING 6 IN TDR)
  vertices_Alv[0] = 0; vertices_Alv[1] = -0;
  vertices_inner_Alv[0] = 0.000999999999999997;	 vertices_inner_Alv[1] = -0.000999999999999997;
  vertices_Alv[2] = 6.084; vertices_Alv[3] = -0;
  vertices_inner_Alv[2] = 6.083;	 vertices_inner_Alv[3] = -0.000999999999999997;
  vertices_Alv[4] = 5.994197921; vertices_Alv[5] = -3.098699015;
  vertices_inner_Alv[4] = 5.993197921;	 vertices_inner_Alv[5] = -3.097699015;
  vertices_Alv[6] = 0.089802079; vertices_Alv[7] = -3.098699015;
  vertices_inner_Alv[6] = 0.090802079;	 vertices_inner_Alv[7] = -3.097699015;
  vertices_Alv[8] = -1.905548737; vertices_Alv[9] = 0.822216398;
  vertices_inner_Alv[8] = -1.904548737;	 vertices_inner_Alv[9] = 0.821216398;
  vertices_Alv[10] = 7.989548737; vertices_Alv[11] = 0.822216398;
  vertices_inner_Alv[10] = 7.988548737;	 vertices_inner_Alv[11] = 0.821216398;
  vertices_Alv[12] = 7.848027155; vertices_Alv[13] = -4.029860647;
  vertices_inner_Alv[12] = 7.847027155;	 vertices_inner_Alv[13] = -4.028860647;
  vertices_Alv[14] = -1.764027155; vertices_Alv[15] = -4.029860647;
  vertices_inner_Alv[14] = -1.763027155;	 vertices_inner_Alv[15] = -4.028860647;
  TGeoVolume *Alveolus_11=gGeoManager->MakeArb8("Alveolus_11", pCarbonFibreMedium,10, vertices_Alv);
  Alveolus_11->SetLineColor(kBlue);
  
  TGeoVolume *AlveolusInner_11=gGeoManager->MakeArb8("AlveolusInner_11", pAirMedium,9.97, vertices_inner_Alv);
  AlveolusInner_11->SetLineColor(kYellow);
  
  //ALVEOLI 12 (TYPE OR RING 5 IN TDR)
  vertices_Alv[0] = 0; vertices_Alv[1] = -0;
  vertices_inner_Alv[0] = 0.000999999999999997;	 vertices_inner_Alv[1] = -0.000999999999999997;
  vertices_Alv[2] = 6.135; vertices_Alv[3] = -0;
  vertices_inner_Alv[2] = 6.134;	 vertices_inner_Alv[3] = -0.000999999999999997;
  vertices_Alv[4] = 6.019512114; vertices_Alv[5] = -3.097848051;
  vertices_inner_Alv[4] = 6.018512114;	 vertices_inner_Alv[5] = -3.096848051;
  vertices_Alv[6] = 0.115487886; vertices_Alv[7] = -3.097848051;
  vertices_inner_Alv[6] = 0.116487886;	 vertices_inner_Alv[7] = -3.096848051;
  vertices_Alv[8] = -1.853335196; vertices_Alv[9] = 0.808351323;
  vertices_inner_Alv[8] = -1.852335196;	 vertices_inner_Alv[9] = 0.807351323;
  vertices_Alv[10] = 7.988335196; vertices_Alv[11] = 0.808351323;
  vertices_inner_Alv[10] = 7.987335196;	 vertices_inner_Alv[11] = 0.807351323;
  vertices_Alv[12] = 7.807156588; vertices_Alv[13] = -4.041933801;
  vertices_inner_Alv[12] = 7.806156588;	 vertices_inner_Alv[13] = -4.040933801;
  vertices_Alv[14] = -1.672156588; vertices_Alv[15] = -4.041933801;
  vertices_inner_Alv[14] = -1.671156588;	 vertices_inner_Alv[15] = -4.040933801;
  TGeoVolume *Alveolus_12=gGeoManager->MakeArb8("Alveolus_12", pCarbonFibreMedium,10, vertices_Alv);
  Alveolus_12->SetLineColor(kBlue);
  
  TGeoVolume *AlveolusInner_12=gGeoManager->MakeArb8("AlveolusInner_12", pAirMedium,9.97, vertices_inner_Alv);
  AlveolusInner_12->SetLineColor(kYellow);
  
  //ALVEOLI 13 (TYPE OR RING 4 IN TDR)
  vertices_Alv[0] = 0; vertices_Alv[1] = -0;
  vertices_inner_Alv[0] = 0.000999999999999997;	 vertices_inner_Alv[1] = -0.000999999999999997;
  vertices_Alv[2] = 6.182; vertices_Alv[3] = -0;
  vertices_inner_Alv[2] = 6.181;	 vertices_inner_Alv[3] = -0.000999999999999997;
  vertices_Alv[4] = 6.042725988; vertices_Alv[5] = -3.096869831;
  vertices_inner_Alv[4] = 6.041725988;	 vertices_inner_Alv[5] = -3.095869831;
  vertices_Alv[6] = 0.139274012; vertices_Alv[7] = -3.096869831;
  vertices_inner_Alv[6] = 0.140274012;	 vertices_inner_Alv[7] = -3.095869831;
  vertices_Alv[8] = -1.787073659; vertices_Alv[9] = 0.797070006;
  vertices_inner_Alv[8] = -1.786073659;	 vertices_inner_Alv[9] = 0.796070006;
  vertices_Alv[10] = 7.969073659; vertices_Alv[11] = 0.797070006;
  vertices_inner_Alv[10] = 7.968073659;	 vertices_inner_Alv[11] = 0.796070006;
  vertices_Alv[12] = 7.751185; vertices_Alv[13] = -4.05114328;
  vertices_inner_Alv[12] = 7.750185;	 vertices_inner_Alv[13] = -4.05014328;
  vertices_Alv[14] = -1.569185; vertices_Alv[15] = -4.05114328;
  vertices_inner_Alv[14] = -1.568185;	 vertices_inner_Alv[15] = -4.05014328;
  TGeoVolume *Alveolus_13=gGeoManager->MakeArb8("Alveolus_13", pCarbonFibreMedium,10, vertices_Alv);
  Alveolus_13->SetLineColor(kBlue);
  
  TGeoVolume *AlveolusInner_13=gGeoManager->MakeArb8("AlveolusInner_13", pAirMedium,9.97, vertices_inner_Alv);
  AlveolusInner_13->SetLineColor(kYellow);
  
  //ALVEOLI 14 (TYPE OR RING 3 IN TDR)
  vertices_Alv[0] = 0; vertices_Alv[1] = -0;
  vertices_inner_Alv[0] = 0.000999999999999997;	 vertices_inner_Alv[1] = -0.000999999999999997;
  vertices_Alv[2] = 6.229; vertices_Alv[3] = -0;
  vertices_inner_Alv[2] = 6.228;	 vertices_inner_Alv[3] = -0.000999999999999997;
  vertices_Alv[4] = 6.067298849; vertices_Alv[5] = -3.095779827;
  vertices_inner_Alv[4] = 6.066298849;	 vertices_inner_Alv[5] = -3.094779827;
  vertices_Alv[6] = 0.161701151; vertices_Alv[7] = -3.095779827;
  vertices_inner_Alv[6] = 0.162701151;	 vertices_inner_Alv[7] = -3.094779827;
  vertices_Alv[8] = -2.0485665132; vertices_Alv[9] = 0.9459129216;
  vertices_inner_Alv[8] = -2.0475665132;	 vertices_inner_Alv[9] = 0.9449129216;
  vertices_Alv[10] = 8.2775665132; vertices_Alv[11] = 0.9459129216;
  vertices_inner_Alv[10] = 8.2765665132;	 vertices_inner_Alv[11] = 0.9449129216;
  vertices_Alv[12] = 8.005589567; vertices_Alv[13] = -4.2499352286;
  vertices_inner_Alv[12] = 8.004589567;	 vertices_inner_Alv[13] = -4.2489352286;
  vertices_Alv[14] = -1.776589567; vertices_Alv[15] = -4.2499352286;
  vertices_inner_Alv[14] = -1.775589567;	 vertices_inner_Alv[15] = -4.2489352286;
  TGeoVolume *Alveolus_14=gGeoManager->MakeArb8("Alveolus_14", pCarbonFibreMedium,12, vertices_Alv);
  Alveolus_14->SetLineColor(kBlue);
  
  TGeoVolume *AlveolusInner_14=gGeoManager->MakeArb8("AlveolusInner_14", pAirMedium,11.97, vertices_inner_Alv);
  AlveolusInner_14->SetLineColor(kYellow);
  
  //ALVEOLI 15 (TYPE OR RING 2 IN TDR)
  vertices_Alv[0] = 0; vertices_Alv[1] = -0;
  vertices_inner_Alv[0] = 0.000999999999999997;	 vertices_inner_Alv[1] = -0.000999999999999997;
  vertices_Alv[2] = 6.271; vertices_Alv[3] = -0;
  vertices_inner_Alv[2] = 6.27;	 vertices_inner_Alv[3] = -0.000999999999999997;
  vertices_Alv[4] = 6.087150242; vertices_Alv[5] = -3.094543467;
  vertices_inner_Alv[4] = 6.086150242;	 vertices_inner_Alv[5] = -3.093543467;
  vertices_Alv[6] = 0.183849758; vertices_Alv[7] = -3.094543467;
  vertices_inner_Alv[6] = 0.184849758;	 vertices_inner_Alv[7] = -3.093543467;
  vertices_Alv[8] = -1.9387963908; vertices_Alv[9] = 0.9377394288;
  vertices_inner_Alv[8] = -1.9377963908;	 vertices_inner_Alv[9] = 0.9367394288;
  vertices_Alv[10] = 8.2097963908; vertices_Alv[11] = 0.9377394288;
  vertices_inner_Alv[10] = 8.2087963908;	 vertices_inner_Alv[11] = 0.9367394288;
  vertices_Alv[12] = 7.900154432; vertices_Alv[13] = -4.2551770842;
  vertices_inner_Alv[12] = 7.899154432;	 vertices_inner_Alv[13] = -4.2541770842;
  vertices_Alv[14] = -1.629154432; vertices_Alv[15] = -4.2551770842;
  vertices_inner_Alv[14] = -1.628154432;	 vertices_inner_Alv[15] = -4.2541770842;
  TGeoVolume *Alveolus_15=gGeoManager->MakeArb8("Alveolus_15", pCarbonFibreMedium,12, vertices_Alv);
  Alveolus_15->SetLineColor(kBlue);
  
  TGeoVolume *AlveolusInner_15=gGeoManager->MakeArb8("AlveolusInner_15", pAirMedium,11.97, vertices_inner_Alv);
  AlveolusInner_15->SetLineColor(kYellow);
  
  //ALVEOLI 16 (TYPE OR RING 1 IN TDR)
  vertices_Alv[0] = 0; vertices_Alv[1] = -0;
  vertices_inner_Alv[0] = 0.000999999999999997;	 vertices_inner_Alv[1] = -0.000999999999999997;
  vertices_Alv[2] = 6.313; vertices_Alv[3] = -0;
  vertices_inner_Alv[2] = 6.312;	 vertices_inner_Alv[3] = -0.000999999999999997;
  vertices_Alv[4] = 6.108630646; vertices_Alv[5] = -3.093256079;
  vertices_inner_Alv[4] = 6.107630646;	 vertices_inner_Alv[5] = -3.092256079;
  vertices_Alv[6] = 0.204369354; vertices_Alv[7] = -3.093256079;
  vertices_inner_Alv[6] = 0.205369354;	 vertices_inner_Alv[7] = -3.092256079;
  vertices_Alv[8] = -1.8122563056; vertices_Alv[9] = 0.933190458;
  vertices_inner_Alv[8] = -1.8112563056;	 vertices_inner_Alv[9] = 0.932190458;
  vertices_Alv[10] = 8.1252563056; vertices_Alv[11] = 0.933190458;
  vertices_inner_Alv[10] = 8.1242563056;	 vertices_inner_Alv[11] = 0.932190458;
  vertices_Alv[12] = 7.7825239588; vertices_Alv[13] = -4.2567746258;
  vertices_inner_Alv[12] = 7.7815239588;	 vertices_inner_Alv[13] = -4.2557746258;
  vertices_Alv[14] = -1.4695239588; vertices_Alv[15] = -4.2567746258;
  vertices_inner_Alv[14] = -1.4685239588;	 vertices_inner_Alv[15] = -4.2557746258;
  TGeoVolume *Alveolus_16=gGeoManager->MakeArb8("Alveolus_16", pCarbonFibreMedium,12, vertices_Alv);
  Alveolus_16->SetLineColor(kBlue);
  
  TGeoVolume *AlveolusInner_16=gGeoManager->MakeArb8("AlveolusInner_16", pAirMedium,11.97, vertices_inner_Alv);
  AlveolusInner_16->SetLineColor(kYellow);
  
  // --------------------------------------------------------------------------
  // Crystals 4_1 to 4_4 belong to Alveolus_9 and Alveolus_10.
  // Crystals 5_1 to 5_4 belong to Alveolus_11, Alveolus_12 and Alveolus_13.
  // Crystals 6_1 to 6_4 belong to Alveolus_14, Alveolus_15 and Alveolus_16.
  // --------------------------------------------------------------------------

  //Crystals with and without wrapping
  Double_t* vertices=new Double_t[16]; 
  Double_t* vertices2=new Double_t[16]; 

  //Crystals 4_1
  vertices[0] = 2.92525; vertices[1] = -0; 				 vertices2[0] = 2.95525; vertices2[1] = -0.03;
  vertices[2] = 5.8505; vertices[3] = -0; 				 vertices2[2] = 5.8205; vertices2[3] = -0.03;
  vertices[4] = 5.8505; vertices[5] = -1.535; 				 vertices2[4] = 5.8205; vertices2[5] = -1.505;
  vertices[6] = 2.92525; vertices[7] = -1.535; 				 vertices2[6] = 2.95525; vertices2[7] = -1.505;
  vertices[8] = 2.92525; vertices[9] = 0.71190775305; 				 vertices2[8] = 2.95525; vertices2[9] = 0.68190775305;
  vertices[10] = 7.48315401295; vertices[11] = 0.71190775305; 				 vertices2[10] = 7.45315401295; vertices2[11] = 0.68190775305;
  vertices[12] = 7.48315401295; vertices[13] = -1.535; 				 vertices2[12] = 7.45315401295; vertices2[13] = -1.505;
  vertices[14] = 2.92525; vertices[15] = -1.535; 				 vertices2[14] = 2.95525; vertices2[15] = -1.505;
  TGeoVolume *CrystalWithWrapping_4_1 = gGeoManager->MakeArb8("CrystalWithWrapping_4_1", pWrappingMedium,8.5, vertices);
  CrystalWithWrapping_4_1->SetLineColor(kGreen);
  Double_t originOffset_4_1[3]; memset(originOffset_4_1, 0, 3*sizeof(Double_t));
  for (int tmpi = 0; tmpi < 8; tmpi++) { originOffset_4_1[0] += vertices2[2*tmpi]/8.0; originOffset_4_1[1] += vertices2[2*tmpi+1]/8.0;}
  for(int tmpi = 0; tmpi < 8; tmpi++) { vertices2[2*tmpi] -= originOffset_4_1[0]; vertices2[2*tmpi+1] -= originOffset_4_1[1]; }
  TGeoVolume *Crystal_4_1 = gGeoManager->MakeArb8("Crystal_4_1", pCsIMedium,8.47, vertices2);
  Crystal_4_1->SetLineColor(kRed);
  
  //Crystals 4_2
  vertices[0] = 2.92525; vertices[1] = -1.535; 				 vertices2[0] = 2.95525; vertices2[1] = -1.565;
  vertices[2] = 5.8505; vertices[3] = -1.535; 				 vertices2[2] = 5.8205; vertices2[3] = -1.565;
  vertices[4] = 5.8505; vertices[5] = -3.07; 				 vertices2[4] = 5.8205; vertices2[5] = -3.04;
  vertices[6] = 2.92525; vertices[7] = -3.07; 				 vertices2[6] = 2.95525; vertices2[7] = -3.04;
  vertices[8] = 2.92525; vertices[9] = -1.535; 				 vertices2[8] = 2.95525; vertices2[9] = -1.565;
  vertices[10] = 7.48315401295; vertices[11] = -1.535; 				 vertices2[10] = 7.45315401295; vertices2[11] = -1.565;
  vertices[12] = 7.48315401295; vertices[13] = -3.78190775305; 				 vertices2[12] = 7.45315401295; vertices2[13] = -3.75190775305;
  vertices[14] = 2.92525; vertices[15] = -3.78190775305; 				 vertices2[14] = 2.95525; vertices2[15] = -3.75190775305;
  TGeoVolume *CrystalWithWrapping_4_2 = gGeoManager->MakeArb8("CrystalWithWrapping_4_2", pWrappingMedium,8.5, vertices);
  CrystalWithWrapping_4_2->SetLineColor(kGreen);
  Double_t originOffset_4_2[3]; memset(originOffset_4_2, 0, 3*sizeof(Double_t));
  for (int tmpi = 0; tmpi < 8; tmpi++) { originOffset_4_2[0] += vertices2[2*tmpi]/8.0; originOffset_4_2[1] += vertices2[2*tmpi+1]/8.0;}
  for(int tmpi = 0; tmpi < 8; tmpi++) { vertices2[2*tmpi] -= originOffset_4_2[0]; vertices2[2*tmpi+1] -= originOffset_4_2[1]; }
  TGeoVolume *Crystal_4_2 = gGeoManager->MakeArb8("Crystal_4_2", pCsIMedium,8.47, vertices2);
  Crystal_4_2->SetLineColor(kRed);

  //Crystals 4_3
  vertices[0] = 0; vertices[1] = -0; 				 vertices2[0] = 0.03; vertices2[1] = -0.03;
  vertices[2] = 2.92525; vertices[3] = -0; 				 vertices2[2] = 2.89525; vertices2[3] = -0.03;
  vertices[4] = 2.92525; vertices[5] = -1.535; 				 vertices2[4] = 2.89525; vertices2[5] = -1.505;
  vertices[6] = 0; vertices[7] = -1.535; 				 vertices2[6] = 0.03; vertices2[7] = -1.505;
  vertices[8] = -1.63265401295; vertices[9] = 0.71190775305; 				 vertices2[8] = -1.60265401295; vertices2[9] = 0.68190775305;
  vertices[10] = 2.92525; vertices[11] = 0.71190775305; 				 vertices2[10] = 2.89525; vertices2[11] = 0.68190775305;
  vertices[12] = 2.92525; vertices[13] = -1.535; 				 vertices2[12] = 2.89525; vertices2[13] = -1.505;
  vertices[14] = -1.63265401295; vertices[15] = -1.535; 				 vertices2[14] = -1.60265401295; vertices2[15] = -1.505;
  TGeoVolume *CrystalWithWrapping_4_3 = gGeoManager->MakeArb8("CrystalWithWrapping_4_3", pWrappingMedium,8.5, vertices);
  CrystalWithWrapping_4_3->SetLineColor(kGreen);
  Double_t originOffset_4_3[3]; memset(originOffset_4_3, 0, 3*sizeof(Double_t));
  for (int tmpi = 0; tmpi < 8; tmpi++) { originOffset_4_3[0] += vertices2[2*tmpi]/8.0; originOffset_4_3[1] += vertices2[2*tmpi+1]/8.0;}
  for(int tmpi = 0; tmpi < 8; tmpi++) { vertices2[2*tmpi] -= originOffset_4_3[0]; vertices2[2*tmpi+1] -= originOffset_4_3[1]; }
  TGeoVolume *Crystal_4_3 = gGeoManager->MakeArb8("Crystal_4_3", pCsIMedium,8.47, vertices2);
  Crystal_4_3->SetLineColor(kRed);

  //Crystals 4_4
  vertices[0] = 0; vertices[1] = -1.535; 				 vertices2[0] = 0.03; vertices2[1] = -1.565;
  vertices[2] = 2.92525; vertices[3] = -1.535; 				 vertices2[2] = 2.89525; vertices2[3] = -1.565;
  vertices[4] = 2.92525; vertices[5] = -3.07; 				 vertices2[4] = 2.89525; vertices2[5] = -3.04;
  vertices[6] = 0; vertices[7] = -3.07; 				 vertices2[6] = 0.03; vertices2[7] = -3.04;
  vertices[8] = -1.63265401295; vertices[9] = -1.535; 				 vertices2[8] = -1.60265401295; vertices2[9] = -1.565;
  vertices[10] = 2.92525; vertices[11] = -1.535; 				 vertices2[10] = 2.89525; vertices2[11] = -1.565;
  vertices[12] = 2.92525; vertices[13] = -3.78190775305; 				 vertices2[12] = 2.89525; vertices2[13] = -3.75190775305;
  vertices[14] = -1.63265401295; vertices[15] = -3.78190775305; 				 vertices2[14] = -1.60265401295; vertices2[15] = -3.75190775305;
  TGeoVolume *CrystalWithWrapping_4_4 = gGeoManager->MakeArb8("CrystalWithWrapping_4_4", pWrappingMedium,8.5, vertices);
  CrystalWithWrapping_4_4->SetLineColor(kGreen);
  Double_t originOffset_4_4[3]; memset(originOffset_4_4, 0, 3*sizeof(Double_t));
  for (int tmpi = 0; tmpi < 8; tmpi++) { originOffset_4_4[0] += vertices2[2*tmpi]/8.0; originOffset_4_4[1] += vertices2[2*tmpi+1]/8.0;}
  for(int tmpi = 0; tmpi < 8; tmpi++) { vertices2[2*tmpi] -= originOffset_4_4[0]; vertices2[2*tmpi+1] -= originOffset_4_4[1]; }
  TGeoVolume *Crystal_4_4 = gGeoManager->MakeArb8("Crystal_4_4", pCsIMedium,8.47, vertices2);
  Crystal_4_4->SetLineColor(kRed);
  
  //Crystals 5_1
  vertices[0] = 2.9385; vertices[1] = -0; 				 vertices2[0] = 2.9685; vertices2[1] = -0.03;
  vertices[2] = 5.877; vertices[3] = -0; 				 vertices2[2] = 5.847; vertices2[3] = -0.03;
  vertices[4] = 5.877; vertices[5] = -1.534; 				 vertices2[4] = 5.847; vertices2[5] = -1.504;
  vertices[6] = 2.9385; vertices[7] = -1.534; 				 vertices2[6] = 2.9685; vertices2[7] = -1.504;
  vertices[8] = 2.9385; vertices[9] = 0.7161341373; 				 vertices2[8] = 2.9685; vertices2[9] = 0.6861341373;
  vertices[10] = 7.4207317284; vertices[11] = 0.7161341373; 				 vertices2[10] = 7.3907317284; vertices2[11] = 0.6861341373;
  vertices[12] = 7.4207317284; vertices[13] = -1.534; 				 vertices2[12] = 7.3907317284; vertices2[13] = -1.504;
  vertices[14] = 2.9385; vertices[15] = -1.534; 				 vertices2[14] = 2.9685; vertices2[15] = -1.504;
  TGeoVolume *CrystalWithWrapping_5_1 = gGeoManager->MakeArb8("CrystalWithWrapping_5_1", pWrappingMedium,9, vertices);
  CrystalWithWrapping_5_1->SetLineColor(kGreen);
  Double_t originOffset_5_1[3]; memset(originOffset_5_1, 0, 3*sizeof(Double_t));
  for (int tmpi = 0; tmpi < 8; tmpi++) { originOffset_5_1[0] += vertices2[2*tmpi]/8.0; originOffset_5_1[1] += vertices2[2*tmpi+1]/8.0;}
  for(int tmpi = 0; tmpi < 8; tmpi++) { vertices2[2*tmpi] -= originOffset_5_1[0]; vertices2[2*tmpi+1] -= originOffset_5_1[1]; }
  TGeoVolume *Crystal_5_1 = gGeoManager->MakeArb8("Crystal_5_1", pCsIMedium,8.97, vertices2);
  Crystal_5_1->SetLineColor(kRed);
  
  //Crystals 5_2
  vertices[0] = 2.9385; vertices[1] = -1.534; 				 vertices2[0] = 2.9685; vertices2[1] = -1.564;
  vertices[2] = 5.877; vertices[3] = -1.534; 				 vertices2[2] = 5.847; vertices2[3] = -1.564;
  vertices[4] = 5.877; vertices[5] = -3.068; 				 vertices2[4] = 5.847; vertices2[5] = -3.038;
  vertices[6] = 2.9385; vertices[7] = -3.068; 				 vertices2[6] = 2.9685; vertices2[7] = -3.038;
  vertices[8] = 2.9385; vertices[9] = -1.534; 				 vertices2[8] = 2.9685; vertices2[9] = -1.564;
  vertices[10] = 7.4207317284; vertices[11] = -1.534; 				 vertices2[10] = 7.3907317284; vertices2[11] = -1.564;
  vertices[12] = 7.4207317284; vertices[13] = -3.7841341373; 				 vertices2[12] = 7.3907317284; vertices2[13] = -3.7541341373;
  vertices[14] = 2.9385; vertices[15] = -3.7841341373; 				 vertices2[14] = 2.9685; vertices2[15] = -3.7541341373;
  TGeoVolume *CrystalWithWrapping_5_2 = gGeoManager->MakeArb8("CrystalWithWrapping_5_2", pWrappingMedium,9, vertices);
  CrystalWithWrapping_5_2->SetLineColor(kGreen);
  Double_t originOffset_5_2[3]; memset(originOffset_5_2, 0, 3*sizeof(Double_t));
  for (int tmpi = 0; tmpi < 8; tmpi++) { originOffset_5_2[0] += vertices2[2*tmpi]/8.0; originOffset_5_2[1] += vertices2[2*tmpi+1]/8.0;}
  for(int tmpi = 0; tmpi < 8; tmpi++) { vertices2[2*tmpi] -= originOffset_5_2[0]; vertices2[2*tmpi+1] -= originOffset_5_2[1]; }
  TGeoVolume *Crystal_5_2 = gGeoManager->MakeArb8("Crystal_5_2", pCsIMedium,8.97, vertices2);
  Crystal_5_2->SetLineColor(kRed);
  
  //Crystals 5_3
  vertices[0] = 0; vertices[1] = -0; 				 vertices2[0] = 0.03; vertices2[1] = -0.03;
  vertices[2] = 2.9385; vertices[3] = -0; 				 vertices2[2] = 2.9085; vertices2[3] = -0.03;
  vertices[4] = 2.9385; vertices[5] = -1.534; 				 vertices2[4] = 2.9085; vertices2[5] = -1.504;
  vertices[6] = 0; vertices[7] = -1.534; 				 vertices2[6] = 0.03; vertices2[7] = -1.504;
  vertices[8] = -1.5437317284; vertices[9] = 0.7161341373; 				 vertices2[8] = -1.5137317284; vertices2[9] = 0.6861341373;
  vertices[10] = 2.9385; vertices[11] = 0.7161341373; 				 vertices2[10] = 2.9085; vertices2[11] = 0.6861341373;
  vertices[12] = 2.9385; vertices[13] = -1.534; 				 vertices2[12] = 2.9085; vertices2[13] = -1.504;
  vertices[14] = -1.5437317284; vertices[15] = -1.534; 				 vertices2[14] = -1.5137317284; vertices2[15] = -1.504;
  TGeoVolume *CrystalWithWrapping_5_3 = gGeoManager->MakeArb8("CrystalWithWrapping_5_3", pWrappingMedium,9, vertices);
  CrystalWithWrapping_5_3->SetLineColor(kGreen);
  Double_t originOffset_5_3[3]; memset(originOffset_5_3, 0, 3*sizeof(Double_t));
  for (int tmpi = 0; tmpi < 8; tmpi++) { originOffset_5_3[0] += vertices2[2*tmpi]/8.0; originOffset_5_3[1] += vertices2[2*tmpi+1]/8.0;}
  for(int tmpi = 0; tmpi < 8; tmpi++) { vertices2[2*tmpi] -= originOffset_5_3[0]; vertices2[2*tmpi+1] -= originOffset_5_3[1]; }
  TGeoVolume *Crystal_5_3 = gGeoManager->MakeArb8("Crystal_5_3", pCsIMedium,8.97, vertices2);
  Crystal_5_3->SetLineColor(kRed);
  
  //Crystals 5_4
  vertices[0] = 0; vertices[1] = -1.534; 				 vertices2[0] = 0.03; vertices2[1] = -1.564;
  vertices[2] = 2.9385; vertices[3] = -1.534; 				 vertices2[2] = 2.9085; vertices2[3] = -1.564;
  vertices[4] = 2.9385; vertices[5] = -3.068; 				 vertices2[4] = 2.9085; vertices2[5] = -3.038;
  vertices[6] = 0; vertices[7] = -3.068; 				 vertices2[6] = 0.03; vertices2[7] = -3.038;
  vertices[8] = -1.5437317284; vertices[9] = -1.534; 				 vertices2[8] = -1.5137317284; vertices2[9] = -1.564;
  vertices[10] = 2.9385; vertices[11] = -1.534; 				 vertices2[10] = 2.9085; vertices2[11] = -1.564;
  vertices[12] = 2.9385; vertices[13] = -3.7841341373; 				 vertices2[12] = 2.9085; vertices2[13] = -3.7541341373;
  vertices[14] = -1.5437317284; vertices[15] = -3.7841341373; 				 vertices2[14] = -1.5137317284; vertices2[15] = -3.7541341373;
  TGeoVolume *CrystalWithWrapping_5_4 = gGeoManager->MakeArb8("CrystalWithWrapping_5_4", pWrappingMedium,9, vertices);
  CrystalWithWrapping_5_4->SetLineColor(kGreen);
  Double_t originOffset_5_4[3]; memset(originOffset_5_4, 0, 3*sizeof(Double_t));
  for (int tmpi = 0; tmpi < 8; tmpi++) { originOffset_5_4[0] += vertices2[2*tmpi]/8.0; originOffset_5_4[1] += vertices2[2*tmpi+1]/8.0;}
  for(int tmpi = 0; tmpi < 8; tmpi++) { vertices2[2*tmpi] -= originOffset_5_4[0]; vertices2[2*tmpi+1] -= originOffset_5_4[1]; }
  TGeoVolume *Crystal_5_4 = gGeoManager->MakeArb8("Crystal_5_4", pCsIMedium,8.97, vertices2);
  Crystal_5_4->SetLineColor(kRed);
  
  //Crystals 6_1
  vertices[0] = 2.93625; vertices[1] = -0; 				 vertices2[0] = 2.96625; vertices2[1] = -0.03;
  vertices[2] = 5.8725; vertices[3] = -0; 				 vertices2[2] = 5.8425; vertices2[3] = -0.03;
  vertices[4] = 5.8725; vertices[5] = -1.532; 				 vertices2[4] = 5.8425; vertices2[5] = -1.502;
  vertices[6] = 2.93625; vertices[7] = -1.532; 				 vertices2[6] = 2.96625; vertices2[7] = -1.502;
  vertices[8] = 2.93625; vertices[9] = 0.8472423509; 				 vertices2[8] = 2.96625; vertices2[9] = 0.8172423509;
  vertices[10] = 7.4197531404; vertices[11] = 0.8472423509; 				 vertices2[10] = 7.3897531404; vertices2[11] = 0.8172423509;
  vertices[12] = 7.4197531404; vertices[13] = -1.532; 				 vertices2[12] = 7.3897531404; vertices2[13] = -1.502;
  vertices[14] = 2.93625; vertices[15] = -1.532; 				 vertices2[14] = 2.96625; vertices2[15] = -1.502;
  TGeoVolume *CrystalWithWrapping_6_1 = gGeoManager->MakeArb8("CrystalWithWrapping_6_1", pWrappingMedium,11, vertices);
  CrystalWithWrapping_6_1->SetLineColor(kGreen);
  Double_t originOffset_6_1[3]; memset(originOffset_6_1, 0, 3*sizeof(Double_t));
  for (int tmpi = 0; tmpi < 8; tmpi++) { originOffset_6_1[0] += vertices2[2*tmpi]/8.0; originOffset_6_1[1] += vertices2[2*tmpi+1]/8.0;}
  for(int tmpi = 0; tmpi < 8; tmpi++) { vertices2[2*tmpi] -= originOffset_6_1[0]; vertices2[2*tmpi+1] -= originOffset_6_1[1]; }
  TGeoVolume *Crystal_6_1 = gGeoManager->MakeArb8("Crystal_6_1", pCsIMedium,10.97, vertices2);
  Crystal_6_1->SetLineColor(kRed);
  
  //Crystals 6_2
  vertices[0] = 2.93625; vertices[1] = -1.532; 				 vertices2[0] = 2.96625; vertices2[1] = -1.562;
  vertices[2] = 5.8725; vertices[3] = -1.532; 				 vertices2[2] = 5.8425; vertices2[3] = -1.562;
  vertices[4] = 5.8725; vertices[5] = -3.064; 				 vertices2[4] = 5.8425; vertices2[5] = -3.034;
  vertices[6] = 2.93625; vertices[7] = -3.064; 				 vertices2[6] = 2.96625; vertices2[7] = -3.034;
  vertices[8] = 2.93625; vertices[9] = -1.532; 				 vertices2[8] = 2.96625; vertices2[9] = -1.562;
  vertices[10] = 7.4197531404; vertices[11] = -1.532; 				 vertices2[10] = 7.3897531404; vertices2[11] = -1.562;
  vertices[12] = 7.4197531404; vertices[13] = -3.9112423509; 				 vertices2[12] = 7.3897531404; vertices2[13] = -3.8812423509;
  vertices[14] = 2.93625; vertices[15] = -3.9112423509; 				 vertices2[14] = 2.96625; vertices2[15] = -3.8812423509;
  TGeoVolume *CrystalWithWrapping_6_2 = gGeoManager->MakeArb8("CrystalWithWrapping_6_2", pWrappingMedium,11, vertices);
  CrystalWithWrapping_6_2->SetLineColor(kGreen);
  Double_t originOffset_6_2[3]; memset(originOffset_6_2, 0, 3*sizeof(Double_t));
  for (int tmpi = 0; tmpi < 8; tmpi++) { originOffset_6_2[0] += vertices2[2*tmpi]/8.0; originOffset_6_2[1] += vertices2[2*tmpi+1]/8.0;}
  for(int tmpi = 0; tmpi < 8; tmpi++) { vertices2[2*tmpi] -= originOffset_6_2[0]; vertices2[2*tmpi+1] -= originOffset_6_2[1]; }
  TGeoVolume *Crystal_6_2 = gGeoManager->MakeArb8("Crystal_6_2", pCsIMedium,10.97, vertices2);
  Crystal_6_2->SetLineColor(kRed);
  
  //Crystals 6_3
  vertices[0] = 0; vertices[1] = -1.532; 				 vertices2[0] = 0.03; vertices2[1] = -1.562;
  vertices[2] = 2.93625; vertices[3] = -1.532; 				 vertices2[2] = 2.90625; vertices2[3] = -1.562;
  vertices[4] = 2.93625; vertices[5] = -3.064; 				 vertices2[4] = 2.90625; vertices2[5] = -3.034;
  vertices[6] = 0; vertices[7] = -3.064; 				 vertices2[6] = 0.03; vertices2[7] = -3.034;
  vertices[8] = -1.5472531404; vertices[9] = -1.532; 				 vertices2[8] = -1.5172531404; vertices2[9] = -1.562;
  vertices[10] = 2.93625; vertices[11] = -1.532; 				 vertices2[10] = 2.90625; vertices2[11] = -1.562;
  vertices[12] = 2.93625; vertices[13] = -3.9112423509; 				 vertices2[12] = 2.90625; vertices2[13] = -3.8812423509;
  vertices[14] = -1.5472531404; vertices[15] = -3.9112423509; 				 vertices2[14] = -1.5172531404; vertices2[15] = -3.8812423509;
  TGeoVolume *CrystalWithWrapping_6_3 = gGeoManager->MakeArb8("CrystalWithWrapping_6_3", pWrappingMedium,11, vertices);
  CrystalWithWrapping_6_3->SetLineColor(kGreen);
  Double_t originOffset_6_3[3]; memset(originOffset_6_3, 0, 3*sizeof(Double_t));
  for (int tmpi = 0; tmpi < 8; tmpi++) { originOffset_6_3[0] += vertices2[2*tmpi]/8.0; originOffset_6_3[1] += vertices2[2*tmpi+1]/8.0;}
  for(int tmpi = 0; tmpi < 8; tmpi++) { vertices2[2*tmpi] -= originOffset_6_3[0]; vertices2[2*tmpi+1] -= originOffset_6_3[1]; }
  TGeoVolume *Crystal_6_3 = gGeoManager->MakeArb8("Crystal_6_3", pCsIMedium,10.97, vertices2);
  Crystal_6_3->SetLineColor(kRed);
  
  //Crystals 6_4
  vertices[0] = 0; vertices[1] = -0; 				 vertices2[0] = 0.03; vertices2[1] = -0.03;
  vertices[2] = 2.93625; vertices[3] = -0; 				 vertices2[2] = 2.90625; vertices2[3] = -0.03;
  vertices[4] = 2.93625; vertices[5] = -1.532; 				 vertices2[4] = 2.90625; vertices2[5] = -1.502;
  vertices[6] = 0; vertices[7] = -1.532; 				 vertices2[6] = 0.03; vertices2[7] = -1.502;
  vertices[8] = -1.5472531404; vertices[9] = 0.8472423509; 				 vertices2[8] = -1.5172531404; vertices2[9] = 0.8172423509;
  vertices[10] = 2.93625; vertices[11] = 0.8472423509; 				 vertices2[10] = 2.90625; vertices2[11] = 0.8172423509;
  vertices[12] = 2.93625; vertices[13] = -1.532; 				 vertices2[12] = 2.90625; vertices2[13] = -1.502;
  vertices[14] = -1.5472531404; vertices[15] = -1.532; 				 vertices2[14] = -1.5172531404; vertices2[15] = -1.502;
  TGeoVolume *CrystalWithWrapping_6_4 = gGeoManager->MakeArb8("CrystalWithWrapping_6_4", pWrappingMedium,11, vertices);
  CrystalWithWrapping_6_4->SetLineColor(kGreen);
  Double_t originOffset_6_4[3]; memset(originOffset_6_4, 0, 3*sizeof(Double_t));
  for (int tmpi = 0; tmpi < 8; tmpi++) { originOffset_6_4[0] += vertices2[2*tmpi]/8.0; originOffset_6_4[1] += vertices2[2*tmpi+1]/8.0;}
  for(int tmpi = 0; tmpi < 8; tmpi++) { vertices2[2*tmpi] -= originOffset_6_4[0]; vertices2[2*tmpi+1] -= originOffset_6_4[1]; }
  TGeoVolume *Crystal_6_4 = gGeoManager->MakeArb8("Crystal_6_4", pCsIMedium,10.97, vertices2);
  Crystal_6_4->SetLineColor(kRed);
  
  //Some common geometrical operations
  TGeoRotation *rotUni = new TGeoRotation();          //unitary rotation
  //TGeoTranslation* noTrans=new TGeoTranslation("noTrans",0.,0.,0.);
  //TGeoRotation *rotSymmetric = new TGeoRotation(); //Symmetric crystal
  //rotSymmetric->RotateZ(180);
  //The right transformation describing the difference in the coordinate system in CAD and simulation
  TGeoRotation *rotDef = new TGeoRotation();
  rotDef->RotateX(-90);
  TGeoTranslation* shiftDef=new TGeoTranslation("shiftDef",0.,10.,0.);
  TGeoCombiTrans* transDef = new TGeoCombiTrans(*shiftDef,*rotDef);
 
  //Adding (AddNode) the Crystal_ volumes in the CrystalWithWrapping_ volumes
  CrystalWithWrapping_4_1->AddNode(Crystal_4_1,1,new TGeoCombiTrans(originOffset_4_1[0], originOffset_4_1[1],0.0,rotUni));
  CrystalWithWrapping_4_2->AddNode(Crystal_4_2,1,new TGeoCombiTrans(originOffset_4_2[0], originOffset_4_2[1],0.0,rotUni));
  CrystalWithWrapping_4_3->AddNode(Crystal_4_3,1,new TGeoCombiTrans(originOffset_4_3[0], originOffset_4_3[1],0.0,rotUni));
  CrystalWithWrapping_4_4->AddNode(Crystal_4_4,1,new TGeoCombiTrans(originOffset_4_4[0], originOffset_4_4[1],0.0,rotUni));
  CrystalWithWrapping_5_1->AddNode(Crystal_5_1,1,new TGeoCombiTrans(originOffset_5_1[0], originOffset_5_1[1],0.0,rotUni));
  CrystalWithWrapping_5_2->AddNode(Crystal_5_2,1,new TGeoCombiTrans(originOffset_5_2[0], originOffset_5_2[1],0.0,rotUni));
  CrystalWithWrapping_5_3->AddNode(Crystal_5_3,1,new TGeoCombiTrans(originOffset_5_3[0], originOffset_5_3[1],0.0,rotUni));
  CrystalWithWrapping_5_4->AddNode(Crystal_5_4,1,new TGeoCombiTrans(originOffset_5_4[0], originOffset_5_4[1],0.0,rotUni));
  CrystalWithWrapping_6_1->AddNode(Crystal_6_1,1,new TGeoCombiTrans(originOffset_6_1[0], originOffset_6_1[1],0.0,rotUni));
  CrystalWithWrapping_6_2->AddNode(Crystal_6_2,1,new TGeoCombiTrans(originOffset_6_2[0], originOffset_6_2[1],0.0,rotUni));
  CrystalWithWrapping_6_3->AddNode(Crystal_6_3,1,new TGeoCombiTrans(originOffset_6_3[0], originOffset_6_3[1],0.0,rotUni));
  CrystalWithWrapping_6_4->AddNode(Crystal_6_4,1,new TGeoCombiTrans(originOffset_6_4[0], originOffset_6_4[1],0.0,rotUni));

  //Adding (AddNode) the CrystalWithWrapping_ volumes inside each AlveolusInner_
  AlveolusInner_9->AddNode(CrystalWithWrapping_4_1,0,new TGeoCombiTrans(0.0647499999999996,-0.014883661,-1.47,rotUni));
  AlveolusInner_9->AddNode(CrystalWithWrapping_4_2,1,new TGeoCombiTrans(0.0647499999999996,-0.014883661,-1.47,rotUni));
  AlveolusInner_9->AddNode(CrystalWithWrapping_4_3,2,new TGeoCombiTrans(0.0647499999999996,-0.014883661,-1.47,rotUni));
  AlveolusInner_9->AddNode(CrystalWithWrapping_4_4,3,new TGeoCombiTrans(0.0647499999999996,-0.014883661,-1.47,rotUni));
  Alveolus_9->AddNode(AlveolusInner_9,1,new TGeoCombiTrans(0,0,0,rotUni));
  AlveolusInner_10->AddNode(CrystalWithWrapping_4_1,0,new TGeoCombiTrans(0.0912499999999996,-0.0146662630000001,-1.47,rotUni));
  AlveolusInner_10->AddNode(CrystalWithWrapping_4_2,1,new TGeoCombiTrans(0.0912499999999996,-0.0146662630000001,-1.47,rotUni));
  AlveolusInner_10->AddNode(CrystalWithWrapping_4_3,2,new TGeoCombiTrans(0.0912499999999996,-0.0146662630000001,-1.47,rotUni));
  AlveolusInner_10->AddNode(CrystalWithWrapping_4_4,3,new TGeoCombiTrans(0.0912499999999996,-0.0146662630000001,-1.47,rotUni));
  Alveolus_10->AddNode(AlveolusInner_10,1,new TGeoCombiTrans(0,0,0,rotUni));
  AlveolusInner_11->AddNode(CrystalWithWrapping_5_1,0,new TGeoCombiTrans(0.1035,-0.0153495075000001,-0.97,rotUni));
  AlveolusInner_11->AddNode(CrystalWithWrapping_5_2,1,new TGeoCombiTrans(0.1035,-0.0153495075000001,-0.97,rotUni));
  AlveolusInner_11->AddNode(CrystalWithWrapping_5_3,2,new TGeoCombiTrans(0.1035,-0.0153495075000001,-0.97,rotUni));
  AlveolusInner_11->AddNode(CrystalWithWrapping_5_4,3,new TGeoCombiTrans(0.1035,-0.0153495075000001,-0.97,rotUni));
  Alveolus_11->AddNode(AlveolusInner_11,1,new TGeoCombiTrans(0,0,0,rotUni));
  AlveolusInner_12->AddNode(CrystalWithWrapping_5_1,0,new TGeoCombiTrans(0.129,-0.0149240255,-0.97,rotUni));
  AlveolusInner_12->AddNode(CrystalWithWrapping_5_2,1,new TGeoCombiTrans(0.129,-0.0149240255,-0.97,rotUni));
  AlveolusInner_12->AddNode(CrystalWithWrapping_5_3,2,new TGeoCombiTrans(0.129,-0.0149240255,-0.97,rotUni));
  AlveolusInner_12->AddNode(CrystalWithWrapping_5_4,3,new TGeoCombiTrans(0.129,-0.0149240255,-0.97,rotUni));
  Alveolus_12->AddNode(AlveolusInner_12,1,new TGeoCombiTrans(0,0,0,rotUni));
  AlveolusInner_13->AddNode(CrystalWithWrapping_5_1,0,new TGeoCombiTrans(0.1525,-0.0144349155000001,-0.97,rotUni));
  AlveolusInner_13->AddNode(CrystalWithWrapping_5_2,1,new TGeoCombiTrans(0.1525,-0.0144349155000001,-0.97,rotUni));
  AlveolusInner_13->AddNode(CrystalWithWrapping_5_3,2,new TGeoCombiTrans(0.1525,-0.0144349155000001,-0.97,rotUni));
  AlveolusInner_13->AddNode(CrystalWithWrapping_5_4,3,new TGeoCombiTrans(0.1525,-0.0144349155000001,-0.97,rotUni));
  Alveolus_13->AddNode(AlveolusInner_13,1,new TGeoCombiTrans(0,0,0,rotUni));
  AlveolusInner_14->AddNode(CrystalWithWrapping_6_1,0,new TGeoCombiTrans(0.17825,-0.0158899135000001,-0.97,rotUni));
  AlveolusInner_14->AddNode(CrystalWithWrapping_6_2,1,new TGeoCombiTrans(0.17825,-0.0158899135000001,-0.97,rotUni));
  AlveolusInner_14->AddNode(CrystalWithWrapping_6_3,2,new TGeoCombiTrans(0.17825,-0.0158899135000001,-0.97,rotUni));
  AlveolusInner_14->AddNode(CrystalWithWrapping_6_4,3,new TGeoCombiTrans(0.17825,-0.0158899135000001,-0.97,rotUni));
  Alveolus_14->AddNode(AlveolusInner_14,1,new TGeoCombiTrans(0,0,0,rotUni));
  AlveolusInner_15->AddNode(CrystalWithWrapping_6_1,0,new TGeoCombiTrans(0.19925,-0.0152717335000001,-0.97,rotUni));
  AlveolusInner_15->AddNode(CrystalWithWrapping_6_2,1,new TGeoCombiTrans(0.19925,-0.0152717335000001,-0.97,rotUni));
  AlveolusInner_15->AddNode(CrystalWithWrapping_6_3,2,new TGeoCombiTrans(0.19925,-0.0152717335000001,-0.97,rotUni));
  AlveolusInner_15->AddNode(CrystalWithWrapping_6_4,3,new TGeoCombiTrans(0.19925,-0.0152717335000001,-0.97,rotUni));
  Alveolus_15->AddNode(AlveolusInner_15,1,new TGeoCombiTrans(0,0,0,rotUni));
  AlveolusInner_16->AddNode(CrystalWithWrapping_6_1,0,new TGeoCombiTrans(0.22025,-0.0146280395,-0.97,rotUni));
  AlveolusInner_16->AddNode(CrystalWithWrapping_6_2,1,new TGeoCombiTrans(0.22025,-0.0146280395,-0.97,rotUni));
  AlveolusInner_16->AddNode(CrystalWithWrapping_6_3,2,new TGeoCombiTrans(0.22025,-0.0146280395,-0.97,rotUni));
  AlveolusInner_16->AddNode(CrystalWithWrapping_6_4,3,new TGeoCombiTrans(0.22025,-0.0146280395,-0.97,rotUni));
  Alveolus_16->AddNode(AlveolusInner_16,1,new TGeoCombiTrans(0,0,0,rotUni));

  //Positioning in space of alveoli
  Double_t phiEuler=0; Double_t thetaEuler=0; Double_t psiEuler=0;
  Double_t rotAngle=0;
  phiEuler = 180;
  thetaEuler = 180.*TMath::ATan2(-0.1275567287250409800,0.9918312764563167500)/TMath::Pi();
  psiEuler = -180;

  TGeoRotation *rotAlv_9 = new TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler);
  TGeoCombiTrans *transAlvBase = new TGeoCombiTrans();
  TGeoRotation *rotPlace_9 = new TGeoRotation();

  if(petals[0]>0){
    //First sector
    //single petal or first half of double petal in first sector
    TGeoCombiTrans* transAlv_9_0 = new TGeoCombiTrans(-2.98999999999993,30.3953961794098,4.44438599416416,rotAlv_9);
    *transAlvBase =  (*transAlv_9_0) * (*transDef) ;
    rotPlace_9->SetAngles(0., 0., 0.);
    *transAlv_9_0 =  (*rotPlace_9) * matDemo_1 * (*transAlvBase);
    pWorld->AddNode(Alveolus_9,0,transAlv_9_0);
    TGeoCombiTrans* transAlv_9_1 = new TGeoCombiTrans();
    rotAlv_9->SetAngles(-11.25, 0., 0.);
    *transAlv_9_1 = (*rotPlace_9) * matDemo_1 * (*rotAlv_9) * (*transAlvBase);
    pWorld->AddNode(Alveolus_9,1,transAlv_9_1);
    if(petals[0]>1){
      //second half of double petal in first sector
      TGeoCombiTrans* transAlv_9_2 = new TGeoCombiTrans();
      rotAlv_9->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_9_2 = (*rotPlace_9) * matDemo_1 * (*rotAlv_9) * (*transAlvBase);
      pWorld->AddNode(Alveolus_9,2,transAlv_9_2); 
      TGeoCombiTrans* transAlv_9_3 = new TGeoCombiTrans();
      rotAlv_9->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_9_3 = (*rotPlace_9) * matDemo_1 * (*rotAlv_9) * (*transAlvBase);
      pWorld->AddNode(Alveolus_9,3,transAlv_9_3); 
    }
  }
  
  if(petals[1]>0){
    //Second sector
    //single petal or first half of double petal in second sector	 
    *rotAlv_9 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_9_4 = new TGeoCombiTrans(-2.98999999999993,30.3953961794098,4.44438599416416,rotAlv_9);    
    rotPlace_9->SetAngles(-60., 0., 0.);
    *transAlvBase = (*transAlv_9_4) * (*transDef) ;
    *transAlv_9_4 =  (*rotPlace_9) * matDemo_2 * (*transAlvBase);
    pWorld->AddNode(Alveolus_9,4,transAlv_9_4);
    TGeoCombiTrans* transAlv_9_5 = new TGeoCombiTrans();
    rotAlv_9->SetAngles(-11.25, 0., 0.);
    *transAlv_9_5 = (*rotPlace_9) * matDemo_2 * (*rotAlv_9) * (*transAlvBase);
    pWorld->AddNode(Alveolus_9,5,transAlv_9_5);
    if(petals[1]>1){
      //second half of double petal in second sector
      TGeoCombiTrans* transAlv_9_6 = new TGeoCombiTrans();
      rotAlv_9->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_9_6 = (*rotPlace_9) * matDemo_2 * (*rotAlv_9) * (*transAlvBase);
      pWorld->AddNode(Alveolus_9,6,transAlv_9_6); 
      
      TGeoCombiTrans* transAlv_9_7 = new TGeoCombiTrans();
      rotAlv_9->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_9_7 = (*rotPlace_9) * matDemo_2 * (*rotAlv_9) * (*transAlvBase);
      pWorld->AddNode(Alveolus_9,7,transAlv_9_7); 
    }
  }

  if(petals[2]>0){
    //Third sector
    //single petal or first half of double petal in third sector
    *rotAlv_9 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler);
    TGeoCombiTrans* transAlv_9_8 = new TGeoCombiTrans(-2.98999999999993,30.3953961794098,4.44438599416416,rotAlv_9);
    *transAlvBase =  (*transAlv_9_8) * (*transDef) ;
    rotPlace_9->SetAngles(-120., 0., 0.);
    *transAlv_9_8 =  (*rotPlace_9) * matDemo_3 * (*transAlvBase);
    pWorld->AddNode(Alveolus_9,8,transAlv_9_8);
    
    TGeoCombiTrans* transAlv_9_9 = new TGeoCombiTrans();
    rotAlv_9->SetAngles(-11.25, 0., 0.);
    *transAlv_9_9 = (*rotPlace_9) * matDemo_3 * (*rotAlv_9) * (*transAlvBase);
    pWorld->AddNode(Alveolus_9,9,transAlv_9_9);
    if(petals[2]>1){
      //second half of double petal in third sector
      TGeoCombiTrans* transAlv_9_10 = new TGeoCombiTrans();
      rotAlv_9->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_9_10 = (*rotPlace_9) * matDemo_3 * (*rotAlv_9) * (*transAlvBase);
      pWorld->AddNode(Alveolus_9,10,transAlv_9_10); 
      
      TGeoCombiTrans* transAlv_9_11 = new TGeoCombiTrans();
      rotAlv_9->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_9_11 = (*rotPlace_9) * matDemo_3 * (*rotAlv_9) * (*transAlvBase);
      pWorld->AddNode(Alveolus_9,11,transAlv_9_11); 
    }
  }

  if(petals[3]>0){
    //Fourth sector
    //single petal or first half of double petal in fourth sector
    *rotAlv_9 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_9_12 = new TGeoCombiTrans(-2.98999999999993,30.3953961794098,4.44438599416416,rotAlv_9);
    *transAlvBase =  (*transAlv_9_12) * (*transDef) ;
    rotPlace_9->SetAngles(-180., 0., 0.);
    *transAlv_9_12 =  (*rotPlace_9) * matDemo_4 * (*transAlvBase);
    pWorld->AddNode(Alveolus_9,12,transAlv_9_12);
    
    TGeoCombiTrans* transAlv_9_13 = new TGeoCombiTrans();
    rotAlv_9->SetAngles(-11.25, 0., 0.);
    *transAlv_9_13 = (*rotPlace_9) * matDemo_4 * (*rotAlv_9) * (*transAlvBase);
    pWorld->AddNode(Alveolus_9,13,transAlv_9_13);
    if(petals[3]>1){
      //second half of double petal in fourth sector
      TGeoCombiTrans* transAlv_9_14 = new TGeoCombiTrans();
      rotAlv_9->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_9_14 = (*rotPlace_9) * matDemo_4 * (*rotAlv_9) * (*transAlvBase);
      pWorld->AddNode(Alveolus_9,14,transAlv_9_14); 
      
      TGeoCombiTrans* transAlv_9_15 = new TGeoCombiTrans();
      rotAlv_9->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_9_15 = (*rotPlace_9) * matDemo_4 * (*rotAlv_9) * (*transAlvBase);
      pWorld->AddNode(Alveolus_9,15,transAlv_9_15); 
    }
  }
  
  if(petals[4]>0){
    //Fifth sector
    //single petal or first half of double petal in fifth sector
    *rotAlv_9 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_9_16 = new TGeoCombiTrans(-2.98999999999993,30.3953961794098,4.44438599416416,rotAlv_9);
    *transAlvBase =  (*transAlv_9_16) * (*transDef) ;
    rotPlace_9->SetAngles(-240., 0., 0.);
    *transAlv_9_16 =  (*rotPlace_9) * matDemo_5 * (*transAlvBase);
    pWorld->AddNode(Alveolus_9,16,transAlv_9_16);
    
    TGeoCombiTrans* transAlv_9_17 = new TGeoCombiTrans();
    rotAlv_9->SetAngles(-11.25, 0., 0.);
    *transAlv_9_17 = (*rotPlace_9) * matDemo_5 * (*rotAlv_9) * (*transAlvBase);
    pWorld->AddNode(Alveolus_9,17,transAlv_9_17);
    if(petals[4]>1){
      //second half of double petal in fifth sector
      TGeoCombiTrans* transAlv_9_18 = new TGeoCombiTrans();
      rotAlv_9->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_9_18 = (*rotPlace_9) * matDemo_5 * (*rotAlv_9) * (*transAlvBase);
      pWorld->AddNode(Alveolus_9,18,transAlv_9_18); 
      
      TGeoCombiTrans* transAlv_9_19 = new TGeoCombiTrans();
      rotAlv_9->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_9_19 = (*rotPlace_9) * matDemo_5 * (*rotAlv_9) * (*transAlvBase);
      pWorld->AddNode(Alveolus_9,19,transAlv_9_19); 
    }
  }
  
  if(petals[5]>0){
    //Fifth sector
    //single petal or first half of double petal in fifth sector
    *rotAlv_9 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_9_20 = new TGeoCombiTrans(-2.98999999999993,30.3953961794098,4.44438599416416,rotAlv_9);
    *transAlvBase =  (*transAlv_9_20) * (*transDef) ;
    rotPlace_9->SetAngles(-300., 0., 0.);
    *transAlv_9_20 =  (*rotPlace_9) * matDemo_6 * (*transAlvBase);
    pWorld->AddNode(Alveolus_9,20,transAlv_9_20);
    
    TGeoCombiTrans* transAlv_9_21 = new TGeoCombiTrans();
    rotAlv_9->SetAngles(-11.25, 0., 0.);
    *transAlv_9_21 = (*rotPlace_9) * matDemo_6 * (*rotAlv_9) * (*transAlvBase);
    pWorld->AddNode(Alveolus_9,21,transAlv_9_21);
    if(petals[5]>1){
      //second half of double petal in fifth sector
      TGeoCombiTrans* transAlv_9_22 = new TGeoCombiTrans();
      rotAlv_9->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_9_22 = (*rotPlace_9) * matDemo_6 * (*rotAlv_9) * (*transAlvBase);
      pWorld->AddNode(Alveolus_9,22,transAlv_9_22); 
      
      TGeoCombiTrans* transAlv_9_23 = new TGeoCombiTrans();
      rotAlv_9->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_9_23 = (*rotPlace_9) * matDemo_6 * (*rotAlv_9) * (*transAlvBase);
      pWorld->AddNode(Alveolus_9,23,transAlv_9_23); 
    }
  }
  
  phiEuler = 180;
  thetaEuler = 180.*TMath::ATan2(-0.2131210329617397300,0.9770258058563863200)/TMath::Pi();
  psiEuler = -180;
  
  TGeoRotation *rotAlv_10 = new TGeoRotation("rotAlv10",phiEuler,thetaEuler,psiEuler);
  TGeoRotation *rotPlace_10 = new TGeoRotation();

  if(petals[0]>0){
    //First sector
    //single petal or first half of double petal in first sector
    TGeoCombiTrans* transAlv_10_0 = new TGeoCombiTrans(-3.01649999999993,30.6605329493663,7.63929090825073,rotAlv_10);
    *transAlvBase =  (*transAlv_10_0) * (*transDef) ;
    rotPlace_10->SetAngles(0., 0., 0.);
    *transAlv_10_0 =  (*rotPlace_10) * matDemo_1 * (*transAlvBase);
    pWorld->AddNode(Alveolus_10,0,transAlv_10_0);
    TGeoCombiTrans* transAlv_10_1 = new TGeoCombiTrans();
    rotAlv_10->SetAngles(-11.25, 0., 0.);
    *transAlv_10_1 = (*rotPlace_10) * matDemo_1 * (*rotAlv_10) * (*transAlvBase);
    pWorld->AddNode(Alveolus_10,1,transAlv_10_1);
    if(petals[0]>1){
      //second half of double petal in first sector
      TGeoCombiTrans* transAlv_10_2 = new TGeoCombiTrans();
      rotAlv_10->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_10_2 = (*rotPlace_10) * matDemo_1 * (*rotAlv_10) * (*transAlvBase);
      pWorld->AddNode(Alveolus_10,2,transAlv_10_2); 
      TGeoCombiTrans* transAlv_10_3 = new TGeoCombiTrans();
      rotAlv_10->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_10_3 = (*rotPlace_10) * matDemo_1 * (*rotAlv_10) * (*transAlvBase);
      pWorld->AddNode(Alveolus_10,3,transAlv_10_3); 
    }
  }

  if(petals[1]>0){
    //Second sector
    //single petal or first half of double petal in second sector
    *rotAlv_10 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_10_4 = new TGeoCombiTrans(-3.01649999999993,30.6605329493663,7.63929090825073,rotAlv_10);
    *transAlvBase =  (*transAlv_10_4) * (*transDef) ;
    rotPlace_10->SetAngles(-60., 0., 0.);
    *transAlv_10_4 =  (*rotPlace_10) * matDemo_2 * (*transAlvBase);
    pWorld->AddNode(Alveolus_10,4,transAlv_10_4);
    TGeoCombiTrans* transAlv_10_5 = new TGeoCombiTrans();
    rotAlv_10->SetAngles(-11.25, 0., 0.);
    *transAlv_10_5 = (*rotPlace_10) * matDemo_2 * (*rotAlv_10) * (*transAlvBase);
    pWorld->AddNode(Alveolus_10,5,transAlv_10_5);
    if(petals[1]>1){
      //second half of double petal in first sector
      TGeoCombiTrans* transAlv_10_6 = new TGeoCombiTrans();
      rotAlv_10->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_10_6 = (*rotPlace_10) * matDemo_2 * (*rotAlv_10) * (*transAlvBase);
      pWorld->AddNode(Alveolus_10,6,transAlv_10_6); 
      TGeoCombiTrans* transAlv_10_7 = new TGeoCombiTrans();
      rotAlv_10->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_10_7 = (*rotPlace_10) * matDemo_2 * (*rotAlv_10) * (*transAlvBase);
      pWorld->AddNode(Alveolus_10,7,transAlv_10_7); 
    }
  }


  if(petals[2]>0){
    //Third sector
    //single petal or first half of double petal in third sector
    *rotAlv_10 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_10_8 = new TGeoCombiTrans(-3.01649999999993,30.6605329493663,7.63929090825073,rotAlv_10);
    *transAlvBase =  (*transAlv_10_8) * (*transDef) ;
    rotPlace_10->SetAngles(-120., 0., 0.);
    *transAlv_10_8 =  (*rotPlace_10) * matDemo_3 * (*transAlvBase);
    pWorld->AddNode(Alveolus_10,8,transAlv_10_8);
    
    TGeoCombiTrans* transAlv_10_9 = new TGeoCombiTrans();
    rotAlv_10->SetAngles(-11.25, 0., 0.);
    *transAlv_10_9 = (*rotPlace_10) * matDemo_3 * (*rotAlv_10) * (*transAlvBase);
    pWorld->AddNode(Alveolus_10,9,transAlv_10_9);
    if(petals[2]>1){
      //second half of double petal in third sector
      TGeoCombiTrans* transAlv_10_10 = new TGeoCombiTrans();
      rotAlv_10->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_10_10 = (*rotPlace_10) * matDemo_3 * (*rotAlv_10) * (*transAlvBase);
      pWorld->AddNode(Alveolus_10,10,transAlv_10_10); 
      
      TGeoCombiTrans* transAlv_10_11 = new TGeoCombiTrans();
      rotAlv_10->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_10_11 = (*rotPlace_10) * matDemo_3 * (*rotAlv_10) * (*transAlvBase);
      pWorld->AddNode(Alveolus_10,11,transAlv_10_11); 
    }
  }

  if(petals[3]>0){
    //Fourth sector
    //single petal or first half of double petal in fourth sector
    *rotAlv_10 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_10_12 = new TGeoCombiTrans(-3.01649999999993,30.6605329493663,7.63929090825073,rotAlv_10);
    *transAlvBase =  (*transAlv_10_12) * (*transDef) ;
    rotPlace_10->SetAngles(-180., 0., 0.);
    *transAlv_10_12 =  (*rotPlace_10) * matDemo_4 * (*transAlvBase);
    pWorld->AddNode(Alveolus_10,12,transAlv_10_12);
    
    TGeoCombiTrans* transAlv_10_13 = new TGeoCombiTrans();
    rotAlv_10->SetAngles(-11.25, 0., 0.);
    *transAlv_10_13 = (*rotPlace_10) * matDemo_4 * (*rotAlv_10) * (*transAlvBase);
    pWorld->AddNode(Alveolus_10,13,transAlv_10_13);
    if(petals[3]>1){
      //second half of double petal in fourth sector
      TGeoCombiTrans* transAlv_10_14 = new TGeoCombiTrans();
      rotAlv_10->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_10_14 = (*rotPlace_10) * matDemo_4 * (*rotAlv_10) * (*transAlvBase);
      pWorld->AddNode(Alveolus_10,14,transAlv_10_14); 
      
      TGeoCombiTrans* transAlv_10_15 = new TGeoCombiTrans();
      rotAlv_10->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_10_15 = (*rotPlace_10) * matDemo_4 * (*rotAlv_10) * (*transAlvBase);
      pWorld->AddNode(Alveolus_10,15,transAlv_10_15); 
    }
  }
  
  if(petals[4]>0){
    //Fifth sector
    //single petal or first half of double petal in fifth sector
    *rotAlv_10 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_10_16 = new TGeoCombiTrans(-3.01649999999993,30.6605329493663,7.63929090825073,rotAlv_10);
    *transAlvBase =  (*transAlv_10_16) * (*transDef) ;
    rotPlace_10->SetAngles(-240., 0., 0.);
    *transAlv_10_16 =  (*rotPlace_10) * matDemo_5 * (*transAlvBase);
    pWorld->AddNode(Alveolus_10,16,transAlv_10_16);
    
    TGeoCombiTrans* transAlv_10_17 = new TGeoCombiTrans();
    rotAlv_10->SetAngles(-11.25, 0., 0.);
    *transAlv_10_17 = (*rotPlace_10) * matDemo_5 * (*rotAlv_10) * (*transAlvBase);
    pWorld->AddNode(Alveolus_10,17,transAlv_10_17);
    if(petals[4]>1){
      //second half of double petal in fifth sector
      TGeoCombiTrans* transAlv_10_18 = new TGeoCombiTrans();
      rotAlv_10->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_10_18 = (*rotPlace_10) * matDemo_5 * (*rotAlv_10) * (*transAlvBase);
      pWorld->AddNode(Alveolus_10,18,transAlv_10_18); 
      
      TGeoCombiTrans* transAlv_10_19 = new TGeoCombiTrans();
      rotAlv_10->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_10_19 = (*rotPlace_10) * matDemo_5 * (*rotAlv_10) * (*transAlvBase);
      pWorld->AddNode(Alveolus_10,19,transAlv_10_19); 
    }
  }
  
  if(petals[5]>0){
    //Fifth sector
    //single petal or first half of double petal in fifth sector
    *rotAlv_10 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_10_20 = new TGeoCombiTrans(-3.01649999999993,30.6605329493663,7.63929090825073,rotAlv_10);
    *transAlvBase =  (*transAlv_10_20) * (*transDef) ;
    rotPlace_10->SetAngles(-300., 0., 0.);
    *transAlv_10_20 =  (*rotPlace_10) * matDemo_6 * (*transAlvBase);
    pWorld->AddNode(Alveolus_10,20,transAlv_10_20);
    
    TGeoCombiTrans* transAlv_10_21 = new TGeoCombiTrans();
    rotAlv_10->SetAngles(-11.25, 0., 0.);
    *transAlv_10_21 = (*rotPlace_10) * matDemo_6 * (*rotAlv_10) * (*transAlvBase);
    pWorld->AddNode(Alveolus_10,21,transAlv_10_21);
    if(petals[5]>1){
      //second half of double petal in fifth sector
      TGeoCombiTrans* transAlv_10_22 = new TGeoCombiTrans();
      rotAlv_10->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_10_22 = (*rotPlace_10) * matDemo_6 * (*rotAlv_10) * (*transAlvBase);
      pWorld->AddNode(Alveolus_10,22,transAlv_10_22); 
      
      TGeoCombiTrans* transAlv_10_23 = new TGeoCombiTrans();
      rotAlv_10->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_10_23 = (*rotPlace_10) * matDemo_6 * (*rotAlv_10) * (*transAlvBase);
      pWorld->AddNode(Alveolus_10,23,transAlv_10_23); 
    }
  }


  phiEuler = 180;
  thetaEuler = 180.*TMath::ATan2(-0.2971184379757390000,0.9548406326790127600)/TMath::Pi();
  psiEuler = -180;
  
  TGeoRotation *rotAlv_11 = new TGeoRotation("rotAlv11",phiEuler,thetaEuler,psiEuler);
  TGeoRotation *rotPlace_11 = new TGeoRotation();

  if(petals[0]>0){
    //First sector
    //single petal or first half of double petal in first sector
    TGeoCombiTrans* transAlv_11_0 = new TGeoCombiTrans(-3.04199999999993,30.9206806111464,10.9180918399028,rotAlv_11);
    *transAlvBase =  (*transAlv_11_0) * (*transDef) ;
    rotPlace_11->SetAngles(0., 0., 0.);
    *transAlv_11_0 =  (*rotPlace_11) * matDemo_1 * (*transAlvBase);
    pWorld->AddNode(Alveolus_11,0,transAlv_11_0);
    TGeoCombiTrans* transAlv_11_1 = new TGeoCombiTrans();
    rotAlv_11->SetAngles(-11.25, 0., 0.);
    *transAlv_11_1 = (*rotPlace_11) * matDemo_1 * (*rotAlv_11) * (*transAlvBase);
    pWorld->AddNode(Alveolus_11,1,transAlv_11_1);
    if(petals[0]>1){
      //second half of double petal in first sector
      TGeoCombiTrans* transAlv_11_2 = new TGeoCombiTrans();
      rotAlv_11->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_11_2 = (*rotPlace_11) * matDemo_1 * (*rotAlv_11) * (*transAlvBase);
      pWorld->AddNode(Alveolus_11,2,transAlv_11_2); 
      TGeoCombiTrans* transAlv_11_3 = new TGeoCombiTrans();
      rotAlv_11->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_11_3 = (*rotPlace_11) * matDemo_1 * (*rotAlv_11) * (*transAlvBase);
      pWorld->AddNode(Alveolus_11,3,transAlv_11_3); 
    }
  }

  if(petals[1]>0){
    //Second sector
    //single petal or first half of double petal in second sector
    *rotAlv_11 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_11_4 = new TGeoCombiTrans(-3.04199999999993,30.9206806111464,10.9180918399028,rotAlv_11);
    *transAlvBase =  (*transAlv_11_4) * (*transDef) ;
    rotPlace_11->SetAngles(-60., 0., 0.);
    *transAlv_11_4 =  (*rotPlace_11) * matDemo_2 * (*transAlvBase);
    pWorld->AddNode(Alveolus_11,4,transAlv_11_4);
    TGeoCombiTrans* transAlv_11_5 = new TGeoCombiTrans();
    rotAlv_11->SetAngles(-11.25, 0., 0.);
    *transAlv_11_5 = (*rotPlace_11) * matDemo_2 * (*rotAlv_11) * (*transAlvBase);
    pWorld->AddNode(Alveolus_11,5,transAlv_11_5);
    if(petals[1]>1){
      //second half of double petal in first sector
      TGeoCombiTrans* transAlv_11_6 = new TGeoCombiTrans();
      rotAlv_11->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_11_6 = (*rotPlace_11) * matDemo_2 * (*rotAlv_11) * (*transAlvBase);
      pWorld->AddNode(Alveolus_11,6,transAlv_11_6); 
      TGeoCombiTrans* transAlv_11_7 = new TGeoCombiTrans();
      rotAlv_11->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_11_7 = (*rotPlace_11) * matDemo_2 * (*rotAlv_11) * (*transAlvBase);
      pWorld->AddNode(Alveolus_11,7,transAlv_11_7); 
    }
  }


  if(petals[2]>0){
    //Third sector
    //single petal or first half of double petal in third sector
    *rotAlv_11 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_11_8 = new TGeoCombiTrans(-3.04199999999993,30.9206806111464,10.9180918399028,rotAlv_11);
    *transAlvBase =  (*transAlv_11_8) * (*transDef) ;
    rotPlace_11->SetAngles(-120., 0., 0.);
    *transAlv_11_8 =  (*rotPlace_11) * matDemo_3 * (*transAlvBase);
    pWorld->AddNode(Alveolus_11,8,transAlv_11_8);
    
    TGeoCombiTrans* transAlv_11_9 = new TGeoCombiTrans();
    rotAlv_11->SetAngles(-11.25, 0., 0.);
    *transAlv_11_9 = (*rotPlace_11) * matDemo_3 * (*rotAlv_11) * (*transAlvBase);
    pWorld->AddNode(Alveolus_11,9,transAlv_11_9);
    if(petals[2]>1){
      //second half of double petal in third sector
      TGeoCombiTrans* transAlv_11_10 = new TGeoCombiTrans();
      rotAlv_11->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_11_10 = (*rotPlace_11) * matDemo_3 * (*rotAlv_11) * (*transAlvBase);
      pWorld->AddNode(Alveolus_11,10,transAlv_11_10); 
      
      TGeoCombiTrans* transAlv_11_11 = new TGeoCombiTrans();
      rotAlv_11->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_11_11 = (*rotPlace_11) * matDemo_3 * (*rotAlv_11) * (*transAlvBase);
      pWorld->AddNode(Alveolus_11,11,transAlv_11_11); 
    }
  }

  if(petals[3]>0){
    //Fourth sector
    //single petal or first half of double petal in fourth sector    
    *rotAlv_11 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_11_12 = new TGeoCombiTrans(-3.04199999999993,30.9206806111464,10.9180918399028,rotAlv_11);
    *transAlvBase =  (*transAlv_11_12) * (*transDef) ;
    rotPlace_11->SetAngles(-180., 0., 0.);
    *transAlv_11_12 =  (*rotPlace_11) * matDemo_4 * (*transAlvBase);
    pWorld->AddNode(Alveolus_11,12,transAlv_11_12);
    
    TGeoCombiTrans* transAlv_11_13 = new TGeoCombiTrans();
    rotAlv_11->SetAngles(-11.25, 0., 0.);
    *transAlv_11_13 = (*rotPlace_11) * matDemo_4 * (*rotAlv_11) * (*transAlvBase);
    pWorld->AddNode(Alveolus_11,13,transAlv_11_13);
    if(petals[3]>1){
      //second half of double petal in fourth sector
      TGeoCombiTrans* transAlv_11_14 = new TGeoCombiTrans();
      rotAlv_11->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_11_14 = (*rotPlace_11) * matDemo_4 * (*rotAlv_11) * (*transAlvBase);
      pWorld->AddNode(Alveolus_11,14,transAlv_11_14); 
      
      TGeoCombiTrans* transAlv_11_15 = new TGeoCombiTrans();
      rotAlv_11->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_11_15 = (*rotPlace_11) * matDemo_4 * (*rotAlv_11) * (*transAlvBase);
      pWorld->AddNode(Alveolus_11,15,transAlv_11_15); 
    }
  }
  
  if(petals[4]>0){
    //Fifth sector
    //single petal or first half of double petal in fifth sector
    *rotAlv_11 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_11_16 = new TGeoCombiTrans(-3.04199999999993,30.9206806111464,10.9180918399028,rotAlv_11);
    *transAlvBase =  (*transAlv_11_16) * (*transDef) ;
    rotPlace_11->SetAngles(-240., 0., 0.);
    *transAlv_11_16 =  (*rotPlace_11) * matDemo_5 * (*transAlvBase);
    pWorld->AddNode(Alveolus_11,16,transAlv_11_16);
    
    TGeoCombiTrans* transAlv_11_17 = new TGeoCombiTrans();
    rotAlv_11->SetAngles(-11.25, 0., 0.);
    *transAlv_11_17 = (*rotPlace_11) * matDemo_5 * (*rotAlv_11) * (*transAlvBase);
    pWorld->AddNode(Alveolus_11,17,transAlv_11_17);
    if(petals[4]>1){
      //second half of double petal in fifth sector
      TGeoCombiTrans* transAlv_11_18 = new TGeoCombiTrans();
      rotAlv_11->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_11_18 = (*rotPlace_11) * matDemo_5 * (*rotAlv_11) * (*transAlvBase);
      pWorld->AddNode(Alveolus_11,18,transAlv_11_18); 
      
      TGeoCombiTrans* transAlv_11_19 = new TGeoCombiTrans();
      rotAlv_11->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_11_19 = (*rotPlace_11) * matDemo_5 * (*rotAlv_11) * (*transAlvBase);
      pWorld->AddNode(Alveolus_11,19,transAlv_11_19); 
    }
  }
  
  if(petals[5]>0){
    //Fifth sector
    //single petal or first half of double petal in fifth sector
    *rotAlv_11 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_11_20 = new TGeoCombiTrans(-3.04199999999993,30.9206806111464,10.9180918399028,rotAlv_11);
    *transAlvBase =  (*transAlv_11_20) * (*transDef) ;
    rotPlace_11->SetAngles(-300., 0., 0.);
    *transAlv_11_20 =  (*rotPlace_11) * matDemo_6 * (*transAlvBase);
    pWorld->AddNode(Alveolus_11,20,transAlv_11_20);
    
    TGeoCombiTrans* transAlv_11_21 = new TGeoCombiTrans();
    rotAlv_11->SetAngles(-11.25, 0., 0.);
    *transAlv_11_21 = (*rotPlace_11) * matDemo_6 * (*rotAlv_11) * (*transAlvBase);
    pWorld->AddNode(Alveolus_11,21,transAlv_11_21);
    if(petals[5]>1){
      //second half of double petal in fifth sector
      TGeoCombiTrans* transAlv_11_22 = new TGeoCombiTrans();
      rotAlv_11->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_11_22 = (*rotPlace_11) * matDemo_6 * (*rotAlv_11) * (*transAlvBase);
      pWorld->AddNode(Alveolus_11,22,transAlv_11_22); 
      
      TGeoCombiTrans* transAlv_11_23 = new TGeoCombiTrans();
      rotAlv_11->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_11_23 = (*rotPlace_11) * matDemo_6 * (*rotAlv_11) * (*transAlvBase);
      pWorld->AddNode(Alveolus_11,23,transAlv_11_23); 
    }
  }
  
  phiEuler = 180;
  thetaEuler = 180.*TMath::ATan2(-0.3788871076768108300,0.9254428991765514300)/TMath::Pi();
  psiEuler = -180;
  
  TGeoRotation *rotAlv_12 = new TGeoRotation("rotAlv12",phiEuler,thetaEuler,psiEuler);
  TGeoRotation *rotPlace_12 = new TGeoRotation();

  if(petals[0]>0){
    //First sector
    //single petal or first half of double petal in first sector
    TGeoCombiTrans* transAlv_12_0 = new TGeoCombiTrans(-3.06749999999993,31.1737346881315,14.3082258908461,rotAlv_12);
    *transAlvBase =  (*transAlv_12_0) * (*transDef) ;
    rotPlace_12->SetAngles(0., 0., 0.);
    *transAlv_12_0 =  (*rotPlace_12) * matDemo_1 * (*transAlvBase);
    pWorld->AddNode(Alveolus_12,0,transAlv_12_0);
    TGeoCombiTrans* transAlv_12_1 = new TGeoCombiTrans();
    rotAlv_12->SetAngles(-11.25, 0., 0.);
    *transAlv_12_1 = (*rotPlace_12) * matDemo_1 * (*rotAlv_12) * (*transAlvBase);
    pWorld->AddNode(Alveolus_12,1,transAlv_12_1);
    if(petals[0]>1){
      //second half of double petal in first sector
      TGeoCombiTrans* transAlv_12_2 = new TGeoCombiTrans();
      rotAlv_12->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_12_2 = (*rotPlace_12) * matDemo_1 * (*rotAlv_12) * (*transAlvBase);
      pWorld->AddNode(Alveolus_12,2,transAlv_12_2); 
      TGeoCombiTrans* transAlv_12_3 = new TGeoCombiTrans();
      rotAlv_12->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_12_3 = (*rotPlace_12) * matDemo_1 * (*rotAlv_12) * (*transAlvBase);
      pWorld->AddNode(Alveolus_12,3,transAlv_12_3); 
    }
  }

  if(petals[1]>0){
    //Second sector
    //single petal or first half of double petal in second sector
    *rotAlv_12 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_12_4 = new TGeoCombiTrans(-3.06749999999993,31.1737346881315,14.3082258908461,rotAlv_12);
    *transAlvBase =  (*transAlv_12_4) * (*transDef) ;
    rotPlace_12->SetAngles(-60., 0., 0.);
    *transAlv_12_4 =  (*rotPlace_12) * matDemo_2 * (*transAlvBase);
    pWorld->AddNode(Alveolus_12,4,transAlv_12_4);
    TGeoCombiTrans* transAlv_12_5 = new TGeoCombiTrans();
    rotAlv_12->SetAngles(-11.25, 0., 0.);
    *transAlv_12_5 = (*rotPlace_12) * matDemo_2 * (*rotAlv_12) * (*transAlvBase);
    pWorld->AddNode(Alveolus_12,5,transAlv_12_5);
    if(petals[1]>1){
      //second half of double petal in first sector
      TGeoCombiTrans* transAlv_12_6 = new TGeoCombiTrans();
      rotAlv_12->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_12_6 = (*rotPlace_12) * matDemo_2 * (*rotAlv_12) * (*transAlvBase);
      pWorld->AddNode(Alveolus_12,6,transAlv_12_6); 
      TGeoCombiTrans* transAlv_12_7 = new TGeoCombiTrans();
      rotAlv_12->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_12_7 = (*rotPlace_12) * matDemo_2 * (*rotAlv_12) * (*transAlvBase);
      pWorld->AddNode(Alveolus_12,7,transAlv_12_7); 
    }
  }


  if(petals[2]>0){
    //Third sector
    //single petal or first half of double petal in third sector
    *rotAlv_12 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_12_8 = new TGeoCombiTrans(-3.06749999999993,31.1737346881315,14.3082258908461,rotAlv_12);
    *transAlvBase =  (*transAlv_12_8) * (*transDef) ;
    rotPlace_12->SetAngles(-120., 0., 0.);
    *transAlv_12_8 =  (*rotPlace_12) * matDemo_3 * (*transAlvBase);
    pWorld->AddNode(Alveolus_12,8,transAlv_12_8);
    
    TGeoCombiTrans* transAlv_12_9 = new TGeoCombiTrans();
    rotAlv_12->SetAngles(-11.25, 0., 0.);
    *transAlv_12_9 = (*rotPlace_12) * matDemo_3 * (*rotAlv_12) * (*transAlvBase);
    pWorld->AddNode(Alveolus_12,9,transAlv_12_9);
    if(petals[2]>1){
      //second half of double petal in third sector
      TGeoCombiTrans* transAlv_12_10 = new TGeoCombiTrans();
      rotAlv_12->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_12_10 = (*rotPlace_12) * matDemo_3 * (*rotAlv_12) * (*transAlvBase);
      pWorld->AddNode(Alveolus_12,10,transAlv_12_10); 
      
      TGeoCombiTrans* transAlv_12_11 = new TGeoCombiTrans();
      rotAlv_12->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_12_11 = (*rotPlace_12) * matDemo_3 * (*rotAlv_12) * (*transAlvBase);
      pWorld->AddNode(Alveolus_12,11,transAlv_12_11); 
    }
  }

  if(petals[3]>0){
    //Fourth sector
    //single petal or first half of double petal in fourth sector
    *rotAlv_12 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_12_12 = new TGeoCombiTrans(-3.06749999999993,31.1737346881315,14.3082258908461,rotAlv_12);
    *transAlvBase =  (*transAlv_12_12) * (*transDef) ;
    rotPlace_12->SetAngles(-180., 0., 0.);
    *transAlv_12_12 =  (*rotPlace_12) * matDemo_4 * (*transAlvBase);
    pWorld->AddNode(Alveolus_12,12,transAlv_12_12);
    
    TGeoCombiTrans* transAlv_12_13 = new TGeoCombiTrans();
    rotAlv_12->SetAngles(-11.25, 0., 0.);
    *transAlv_12_13 = (*rotPlace_12) * matDemo_4 * (*rotAlv_12) * (*transAlvBase);
    pWorld->AddNode(Alveolus_12,13,transAlv_12_13);
    if(petals[3]>1){
      //second half of double petal in fourth sector
      TGeoCombiTrans* transAlv_12_14 = new TGeoCombiTrans();
      rotAlv_12->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_12_14 = (*rotPlace_12) * matDemo_4 * (*rotAlv_12) * (*transAlvBase);
      pWorld->AddNode(Alveolus_12,14,transAlv_12_14); 
      
      TGeoCombiTrans* transAlv_12_15 = new TGeoCombiTrans();
      rotAlv_12->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_12_15 = (*rotPlace_12) * matDemo_4 * (*rotAlv_12) * (*transAlvBase);
      pWorld->AddNode(Alveolus_12,15,transAlv_12_15); 
    }
  }
  
  if(petals[4]>0){
    //Fifth sector
    //single petal or first half of double petal in fifth sector
    *rotAlv_12 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_12_16 = new TGeoCombiTrans(-3.06749999999993,31.1737346881315,14.3082258908461,rotAlv_12);
    *transAlvBase =  (*transAlv_12_16) * (*transDef) ;
    rotPlace_12->SetAngles(-240., 0., 0.);
    *transAlv_12_16 =  (*rotPlace_12) * matDemo_5 * (*transAlvBase);
    pWorld->AddNode(Alveolus_12,16,transAlv_12_16);
    
    TGeoCombiTrans* transAlv_12_17 = new TGeoCombiTrans();
    rotAlv_12->SetAngles(-11.25, 0., 0.);
    *transAlv_12_17 = (*rotPlace_12) * matDemo_5 * (*rotAlv_12) * (*transAlvBase);
    pWorld->AddNode(Alveolus_12,17,transAlv_12_17);
    if(petals[4]>1){
      //second half of double petal in fifth sector
      TGeoCombiTrans* transAlv_12_18 = new TGeoCombiTrans();
      rotAlv_12->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_12_18 = (*rotPlace_12) * matDemo_5 * (*rotAlv_12) * (*transAlvBase);
      pWorld->AddNode(Alveolus_12,18,transAlv_12_18); 
      
      TGeoCombiTrans* transAlv_12_19 = new TGeoCombiTrans();
      rotAlv_12->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_12_19 = (*rotPlace_12) * matDemo_5 * (*rotAlv_12) * (*transAlvBase);
      pWorld->AddNode(Alveolus_12,19,transAlv_12_19); 
    }
  }
  
  if(petals[5]>0){
    //Fifth sector
    //single petal or first half of double petal in fifth sector
    *rotAlv_12 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_12_20 = new TGeoCombiTrans(-3.06749999999993,31.1737346881315,14.3082258908461,rotAlv_12);
    *transAlvBase =  (*transAlv_12_20) * (*transDef) ;
    rotPlace_12->SetAngles(-300., 0., 0.);
    *transAlv_12_20 =  (*rotPlace_12) * matDemo_6 * (*transAlvBase);
    pWorld->AddNode(Alveolus_12,20,transAlv_12_20);
    
    TGeoCombiTrans* transAlv_12_21 = new TGeoCombiTrans();
    rotAlv_12->SetAngles(-11.25, 0., 0.);
    *transAlv_12_21 = (*rotPlace_12) * matDemo_6 * (*rotAlv_12) * (*transAlvBase);
    pWorld->AddNode(Alveolus_12,21,transAlv_12_21);
    if(petals[5]>1){
      //second half of double petal in fifth sector
      TGeoCombiTrans* transAlv_12_22 = new TGeoCombiTrans();
      rotAlv_12->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_12_22 = (*rotPlace_12) * matDemo_6 * (*rotAlv_12) * (*transAlvBase);
      pWorld->AddNode(Alveolus_12,22,transAlv_12_22); 
      
      TGeoCombiTrans* transAlv_12_23 = new TGeoCombiTrans();
      rotAlv_12->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_12_23 = (*rotPlace_12) * matDemo_6 * (*rotAlv_12) * (*transAlvBase);
      pWorld->AddNode(Alveolus_12,23,transAlv_12_23); 
    }
  }

  phiEuler = 180;
  thetaEuler = 180.*TMath::ATan2(-0.4578676224535794800,0.8890203823921623000)/TMath::Pi();
  psiEuler = -180;

  TGeoRotation *rotAlv_13 = new TGeoRotation("rotAlv13",phiEuler,thetaEuler,psiEuler);
  TGeoRotation *rotPlace_13 = new TGeoRotation();

  if(petals[0]>0){
    //First sector
    //single petal or first half of double petal in first sector
    TGeoCombiTrans* transAlv_13_0 = new TGeoCombiTrans(-3.09099999999993,31.417956426523,17.8408437262792,rotAlv_13);
    *transAlvBase =  (*transAlv_13_0) * (*transDef) ;
    rotPlace_13->SetAngles(0., 0., 0.);
    *transAlv_13_0 =  (*rotPlace_13) * matDemo_1 * (*transAlvBase);
    pWorld->AddNode(Alveolus_13,0,transAlv_13_0);
    TGeoCombiTrans* transAlv_13_1 = new TGeoCombiTrans();
    rotAlv_13->SetAngles(-11.25, 0., 0.);
    *transAlv_13_1 = (*rotPlace_13) * matDemo_1 * (*rotAlv_13) * (*transAlvBase);
    pWorld->AddNode(Alveolus_13,1,transAlv_13_1);
    if(petals[0]>1){
      //second half of double petal in first sector
      TGeoCombiTrans* transAlv_13_2 = new TGeoCombiTrans();
      rotAlv_13->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_13_2 = (*rotPlace_13) * matDemo_1 * (*rotAlv_13) * (*transAlvBase);
      pWorld->AddNode(Alveolus_13,2,transAlv_13_2); 
      TGeoCombiTrans* transAlv_13_3 = new TGeoCombiTrans();
      rotAlv_13->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_13_3 = (*rotPlace_13) * matDemo_1 * (*rotAlv_13) * (*transAlvBase);
      pWorld->AddNode(Alveolus_13,3,transAlv_13_3); 
    }
  }

  if(petals[1]>0){
    //Second sector
    //single petal or first half of double petal in second sector
    *rotAlv_13 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_13_4 = new TGeoCombiTrans(-3.09099999999993,31.417956426523,17.8408437262792,rotAlv_13);
    *transAlvBase =  (*transAlv_13_4) * (*transDef) ;
    rotPlace_13->SetAngles(-60., 0., 0.);
    *transAlv_13_4 =  (*rotPlace_13) * matDemo_2 * (*transAlvBase);
    pWorld->AddNode(Alveolus_13,4,transAlv_13_4);
    TGeoCombiTrans* transAlv_13_5 = new TGeoCombiTrans();
    rotAlv_13->SetAngles(-11.25, 0., 0.);
    *transAlv_13_5 = (*rotPlace_13) * matDemo_2 * (*rotAlv_13) * (*transAlvBase);
    pWorld->AddNode(Alveolus_13,5,transAlv_13_5);
    if(petals[1]>1){
      //second half of double petal in first sector
      TGeoCombiTrans* transAlv_13_6 = new TGeoCombiTrans();
      rotAlv_13->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_13_6 = (*rotPlace_13) * matDemo_2 * (*rotAlv_13) * (*transAlvBase);
      pWorld->AddNode(Alveolus_13,6,transAlv_13_6); 
      TGeoCombiTrans* transAlv_13_7 = new TGeoCombiTrans();
      rotAlv_13->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_13_7 = (*rotPlace_13) * matDemo_2 * (*rotAlv_13) * (*transAlvBase);
      pWorld->AddNode(Alveolus_13,7,transAlv_13_7); 
    }
  }


  if(petals[2]>0){
    //Third sector
    //single petal or first half of double petal in third sector
    *rotAlv_13 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_13_8 = new TGeoCombiTrans(-3.09099999999993,31.417956426523,17.8408437262792,rotAlv_13);
    *transAlvBase =  (*transAlv_13_8) * (*transDef) ;
    rotPlace_13->SetAngles(-120., 0., 0.);
    *transAlv_13_8 =  (*rotPlace_13) * matDemo_3 * (*transAlvBase);
    pWorld->AddNode(Alveolus_13,8,transAlv_13_8);
    
    TGeoCombiTrans* transAlv_13_9 = new TGeoCombiTrans();
    rotAlv_13->SetAngles(-11.25, 0., 0.);
    *transAlv_13_9 = (*rotPlace_13) * matDemo_3 * (*rotAlv_13) * (*transAlvBase);
    pWorld->AddNode(Alveolus_13,9,transAlv_13_9);
    if(petals[2]>1){
      //second half of double petal in third sector
      TGeoCombiTrans* transAlv_13_10 = new TGeoCombiTrans();
      rotAlv_13->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_13_10 = (*rotPlace_13) * matDemo_3 * (*rotAlv_13) * (*transAlvBase);
      pWorld->AddNode(Alveolus_13,10,transAlv_13_10); 
      
      TGeoCombiTrans* transAlv_13_11 = new TGeoCombiTrans();
      rotAlv_13->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_13_11 = (*rotPlace_13) * matDemo_3 * (*rotAlv_13) * (*transAlvBase);
      pWorld->AddNode(Alveolus_13,11,transAlv_13_11); 
    }
  }

  if(petals[3]>0){
    //Fourth sector
    //single petal or first half of double petal in fourth sector
    *rotAlv_13 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_13_12 = new TGeoCombiTrans(-3.09099999999993,31.417956426523,17.8408437262792,rotAlv_13);
    *transAlvBase =  (*transAlv_13_12) * (*transDef) ;
    rotPlace_13->SetAngles(-180., 0., 0.);
    *transAlv_13_12 =  (*rotPlace_13) * matDemo_4 * (*transAlvBase);
    pWorld->AddNode(Alveolus_13,12,transAlv_13_12);
    
    TGeoCombiTrans* transAlv_13_13 = new TGeoCombiTrans();
    rotAlv_13->SetAngles(-11.25, 0., 0.);
    *transAlv_13_13 = (*rotPlace_13) * matDemo_4 * (*rotAlv_13) * (*transAlvBase);
    pWorld->AddNode(Alveolus_13,13,transAlv_13_13);
    if(petals[3]>1){
      //second half of double petal in fourth sector
      TGeoCombiTrans* transAlv_13_14 = new TGeoCombiTrans();
      rotAlv_13->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_13_14 = (*rotPlace_13) * matDemo_4 * (*rotAlv_13) * (*transAlvBase);
      pWorld->AddNode(Alveolus_13,14,transAlv_13_14); 
      
      TGeoCombiTrans* transAlv_13_15 = new TGeoCombiTrans();
      rotAlv_13->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_13_15 = (*rotPlace_13) * matDemo_4 * (*rotAlv_13) * (*transAlvBase);
      pWorld->AddNode(Alveolus_13,15,transAlv_13_15); 
    }
  }
  
  if(petals[4]>0){
    //Fifth sector
    //single petal or first half of double petal in fifth sector
    *rotAlv_13 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_13_16 = new TGeoCombiTrans(-3.09099999999993,31.417956426523,17.8408437262792,rotAlv_13);
    *transAlvBase =  (*transAlv_13_16) * (*transDef) ;
    rotPlace_13->SetAngles(-240., 0., 0.);
    *transAlv_13_16 =  (*rotPlace_13) * matDemo_5 * (*transAlvBase);
    pWorld->AddNode(Alveolus_13,16,transAlv_13_16);
    
    TGeoCombiTrans* transAlv_13_17 = new TGeoCombiTrans();
    rotAlv_13->SetAngles(-11.25, 0., 0.);
    *transAlv_13_17 = (*rotPlace_13) * matDemo_5 * (*rotAlv_13) * (*transAlvBase);
    pWorld->AddNode(Alveolus_13,17,transAlv_13_17);
    if(petals[4]>1){
      //second half of double petal in fifth sector
      TGeoCombiTrans* transAlv_13_18 = new TGeoCombiTrans();
      rotAlv_13->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_13_18 = (*rotPlace_13) * matDemo_5 * (*rotAlv_13) * (*transAlvBase);
      pWorld->AddNode(Alveolus_13,18,transAlv_13_18); 
      
      TGeoCombiTrans* transAlv_13_19 = new TGeoCombiTrans();
      rotAlv_13->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_13_19 = (*rotPlace_13) * matDemo_5 * (*rotAlv_13) * (*transAlvBase);
      pWorld->AddNode(Alveolus_13,19,transAlv_13_19); 
    }
  }
  
  if(petals[5]>0){
    //Fifth sector
    //single petal or first half of double petal in fifth sector
    *rotAlv_13 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_13_20 = new TGeoCombiTrans(-3.09099999999993,31.417956426523,17.8408437262792,rotAlv_13);
    *transAlvBase =  (*transAlv_13_20) * (*transDef) ;
    rotPlace_13->SetAngles(-300., 0., 0.);
    *transAlv_13_20 =  (*rotPlace_13) * matDemo_6 * (*transAlvBase);
    pWorld->AddNode(Alveolus_13,20,transAlv_13_20);
    
    TGeoCombiTrans* transAlv_13_21 = new TGeoCombiTrans();
    rotAlv_13->SetAngles(-11.25, 0., 0.);
    *transAlv_13_21 = (*rotPlace_13) * matDemo_6 * (*rotAlv_13) * (*transAlvBase);
    pWorld->AddNode(Alveolus_13,21,transAlv_13_21);
    if(petals[5]>1){
      //second half of double petal in fifth sector
      TGeoCombiTrans* transAlv_13_22 = new TGeoCombiTrans();
      rotAlv_13->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_13_22 = (*rotPlace_13) * matDemo_6 * (*rotAlv_13) * (*transAlvBase);
      pWorld->AddNode(Alveolus_13,22,transAlv_13_22); 
      
      TGeoCombiTrans* transAlv_13_23 = new TGeoCombiTrans();
      rotAlv_13->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_13_23 = (*rotPlace_13) * matDemo_6 * (*rotAlv_13) * (*transAlvBase);
      pWorld->AddNode(Alveolus_13,23,transAlv_13_23); 
    }
  }

  phiEuler = 180;
  thetaEuler = 180.*TMath::ATan2(-0.5334425026911959700,0.8458363295121305000)/TMath::Pi();
  psiEuler = -180;

  TGeoRotation *rotAlv_14 = new TGeoRotation("rotAlv14",phiEuler,thetaEuler,psiEuler);
  TGeoRotation *rotPlace_14 = new TGeoRotation();

  if(petals[0]>0){
    //First sector
    //single petal or first half of double petal in first sector
    TGeoCombiTrans* transAlv_14_0 = new TGeoCombiTrans(-3.11449999999993,33.343093197755,22.6194116972268,rotAlv_14);
    *transAlvBase =  (*transAlv_14_0) * (*transDef) ;
    rotPlace_14->SetAngles(0., 0., 0.);
    *transAlv_14_0 =  (*rotPlace_14) * matDemo_1 * (*transAlvBase);
    pWorld->AddNode(Alveolus_14,0,transAlv_14_0);
    TGeoCombiTrans* transAlv_14_1 = new TGeoCombiTrans();
    rotAlv_14->SetAngles(-11.25, 0., 0.);
    *transAlv_14_1 = (*rotPlace_14) * matDemo_1 * (*rotAlv_14) * (*transAlvBase);
    pWorld->AddNode(Alveolus_14,1,transAlv_14_1);
    if(petals[0]>1){
      //second half of double petal in first sector
      TGeoCombiTrans* transAlv_14_2 = new TGeoCombiTrans();
      rotAlv_14->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_14_2 = (*rotPlace_14) * matDemo_1 * (*rotAlv_14) * (*transAlvBase);
      pWorld->AddNode(Alveolus_14,2,transAlv_14_2); 
      TGeoCombiTrans* transAlv_14_3 = new TGeoCombiTrans();
      rotAlv_14->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_14_3 = (*rotPlace_14) * matDemo_1 * (*rotAlv_14) * (*transAlvBase);
      pWorld->AddNode(Alveolus_14,3,transAlv_14_3); 
    }
  }

  if(petals[1]>0){
    //Second sector
    //single petal or first half of double petal in second sector
    *rotAlv_14 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_14_4 = new TGeoCombiTrans(-3.11449999999993,33.343093197755,22.6194116972268,rotAlv_14);
    *transAlvBase =  (*transAlv_14_4) * (*transDef) ;
    rotPlace_14->SetAngles(-60., 0., 0.);
    *transAlv_14_4 =  (*rotPlace_14) * matDemo_2 * (*transAlvBase);
    pWorld->AddNode(Alveolus_14,4,transAlv_14_4);
    TGeoCombiTrans* transAlv_14_5 = new TGeoCombiTrans();
    rotAlv_14->SetAngles(-11.25, 0., 0.);
    *transAlv_14_5 = (*rotPlace_14) * matDemo_2 * (*rotAlv_14) * (*transAlvBase);
    pWorld->AddNode(Alveolus_14,5,transAlv_14_5);
    if(petals[1]>1){
      //second half of double petal in first sector
      TGeoCombiTrans* transAlv_14_6 = new TGeoCombiTrans();
      rotAlv_14->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_14_6 = (*rotPlace_14) * matDemo_2 * (*rotAlv_14) * (*transAlvBase);
      pWorld->AddNode(Alveolus_14,6,transAlv_14_6); 
      TGeoCombiTrans* transAlv_14_7 = new TGeoCombiTrans();
      rotAlv_14->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_14_7 = (*rotPlace_14) * matDemo_2 * (*rotAlv_14) * (*transAlvBase);
      pWorld->AddNode(Alveolus_14,7,transAlv_14_7); 
    }
  }


  if(petals[2]>0){
    //Third sector
    //single petal or first half of double petal in third sector
    *rotAlv_14 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_14_8 = new TGeoCombiTrans(-3.11449999999993,33.343093197755,22.6194116972268,rotAlv_14);
    *transAlvBase =  (*transAlv_14_8) * (*transDef) ;
    rotPlace_14->SetAngles(-120., 0., 0.);
    *transAlv_14_8 =  (*rotPlace_14) * matDemo_3 * (*transAlvBase);
    pWorld->AddNode(Alveolus_14,8,transAlv_14_8);
    
    TGeoCombiTrans* transAlv_14_9 = new TGeoCombiTrans();
    rotAlv_14->SetAngles(-11.25, 0., 0.);
    *transAlv_14_9 = (*rotPlace_14) * matDemo_3 * (*rotAlv_14) * (*transAlvBase);
    pWorld->AddNode(Alveolus_14,9,transAlv_14_9);
    if(petals[2]>1){
      //second half of double petal in third sector
      TGeoCombiTrans* transAlv_14_10 = new TGeoCombiTrans();
      rotAlv_14->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_14_10 = (*rotPlace_14) * matDemo_3 * (*rotAlv_14) * (*transAlvBase);
      pWorld->AddNode(Alveolus_14,10,transAlv_14_10); 
      
      TGeoCombiTrans* transAlv_14_11 = new TGeoCombiTrans();
      rotAlv_14->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_14_11 = (*rotPlace_14) * matDemo_3 * (*rotAlv_14) * (*transAlvBase);
      pWorld->AddNode(Alveolus_14,11,transAlv_14_11); 
    }
  }

  if(petals[3]>0){
    //Fourth sector
    //single petal or first half of double petal in fourth sector
    *rotAlv_14 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_14_12 = new TGeoCombiTrans(-3.11449999999993,33.343093197755,22.6194116972268,rotAlv_14);
    *transAlvBase =  (*transAlv_14_12) * (*transDef) ;
    rotPlace_14->SetAngles(-180., 0., 0.);
    *transAlv_14_12 =  (*rotPlace_14) * matDemo_4 * (*transAlvBase);
    pWorld->AddNode(Alveolus_14,12,transAlv_14_12);
    
    TGeoCombiTrans* transAlv_14_13 = new TGeoCombiTrans();
    rotAlv_14->SetAngles(-11.25, 0., 0.);
    *transAlv_14_13 = (*rotPlace_14) * matDemo_4 * (*rotAlv_14) * (*transAlvBase);
    pWorld->AddNode(Alveolus_14,13,transAlv_14_13);
    if(petals[3]>1){
      //second half of double petal in fourth sector
      TGeoCombiTrans* transAlv_14_14 = new TGeoCombiTrans();
      rotAlv_14->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_14_14 = (*rotPlace_14) * matDemo_4 * (*rotAlv_14) * (*transAlvBase);
      pWorld->AddNode(Alveolus_14,14,transAlv_14_14); 
      
      TGeoCombiTrans* transAlv_14_15 = new TGeoCombiTrans();
      rotAlv_14->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_14_15 = (*rotPlace_14) * matDemo_4 * (*rotAlv_14) * (*transAlvBase);
      pWorld->AddNode(Alveolus_14,15,transAlv_14_15); 
    }
  }
  
  if(petals[4]>0){
    //Fifth sector
    //single petal or first half of double petal in fifth sector
    *rotAlv_14 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_14_16 = new TGeoCombiTrans(-3.11449999999993,33.343093197755,22.6194116972268,rotAlv_14);
    *transAlvBase =  (*transAlv_14_16) * (*transDef) ;
    rotPlace_14->SetAngles(-240., 0., 0.);
    *transAlv_14_16 =  (*rotPlace_14) * matDemo_5 * (*transAlvBase);
    pWorld->AddNode(Alveolus_14,16,transAlv_14_16);
    
    TGeoCombiTrans* transAlv_14_17 = new TGeoCombiTrans();
    rotAlv_14->SetAngles(-11.25, 0., 0.);
    *transAlv_14_17 = (*rotPlace_14) * matDemo_5 * (*rotAlv_14) * (*transAlvBase);
    pWorld->AddNode(Alveolus_14,17,transAlv_14_17);
    if(petals[4]>1){
      //second half of double petal in fifth sector
      TGeoCombiTrans* transAlv_14_18 = new TGeoCombiTrans();
      rotAlv_14->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_14_18 = (*rotPlace_14) * matDemo_5 * (*rotAlv_14) * (*transAlvBase);
      pWorld->AddNode(Alveolus_14,18,transAlv_14_18); 
      
      TGeoCombiTrans* transAlv_14_19 = new TGeoCombiTrans();
      rotAlv_14->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_14_19 = (*rotPlace_14) * matDemo_5 * (*rotAlv_14) * (*transAlvBase);
      pWorld->AddNode(Alveolus_14,19,transAlv_14_19); 
    }
  }
  
  if(petals[5]>0){
    //Fifth sector
    //single petal or first half of double petal in fifth sector
    *rotAlv_14 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_14_20 = new TGeoCombiTrans(-3.11449999999993,33.343093197755,22.6194116972268,rotAlv_14);
    *transAlvBase =  (*transAlv_14_20) * (*transDef) ;
    rotPlace_14->SetAngles(-300., 0., 0.);
    *transAlv_14_20 =  (*rotPlace_14) * matDemo_6 * (*transAlvBase);
    pWorld->AddNode(Alveolus_14,20,transAlv_14_20);
    
    TGeoCombiTrans* transAlv_14_21 = new TGeoCombiTrans();
    rotAlv_14->SetAngles(-11.25, 0., 0.);
    *transAlv_14_21 = (*rotPlace_14) * matDemo_6 * (*rotAlv_14) * (*transAlvBase);
    pWorld->AddNode(Alveolus_14,21,transAlv_14_21);
    if(petals[5]>1){
      //second half of double petal in fifth sector
      TGeoCombiTrans* transAlv_14_22 = new TGeoCombiTrans();
      rotAlv_14->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_14_22 = (*rotPlace_14) * matDemo_6 * (*rotAlv_14) * (*transAlvBase);
      pWorld->AddNode(Alveolus_14,22,transAlv_14_22); 
      
      TGeoCombiTrans* transAlv_14_23 = new TGeoCombiTrans();
      rotAlv_14->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_14_23 = (*rotPlace_14) * matDemo_6 * (*rotAlv_14) * (*transAlvBase);
      pWorld->AddNode(Alveolus_14,23,transAlv_14_23); 
    }
  }

  phiEuler = 180;
  thetaEuler = 180.*TMath::ATan2(-0.6050038521676790500,0.7962225435531633500)/TMath::Pi();
  psiEuler = -180;

  TGeoRotation *rotAlv_15 = new TGeoRotation("rotAlv15",phiEuler,thetaEuler,psiEuler);
  TGeoRotation *rotPlace_15 = new TGeoRotation();

  if(petals[0]>0){
    //First sector
    //single petal or first half of double petal in first sector
    TGeoCombiTrans* transAlv_15_0 = new TGeoCombiTrans(-3.13549999999993,33.4646558050777,26.6976862352101,rotAlv_15);
    *transAlvBase =  (*transAlv_15_0) * (*transDef) ;
    rotPlace_15->SetAngles(0., 0., 0.);
    *transAlv_15_0 =  (*rotPlace_15) * matDemo_1 * (*transAlvBase);
    pWorld->AddNode(Alveolus_15,0,transAlv_15_0);
    TGeoCombiTrans* transAlv_15_1 = new TGeoCombiTrans();
    rotAlv_15->SetAngles(-11.25, 0., 0.);
    *transAlv_15_1 = (*rotPlace_15) * matDemo_1 * (*rotAlv_15) * (*transAlvBase);
    pWorld->AddNode(Alveolus_15,1,transAlv_15_1);
    if(petals[0]>1){
      //second half of double petal in first sector
      TGeoCombiTrans* transAlv_15_2 = new TGeoCombiTrans();
      rotAlv_15->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_15_2 = (*rotPlace_15) * matDemo_1 * (*rotAlv_15) * (*transAlvBase);
      pWorld->AddNode(Alveolus_15,2,transAlv_15_2); 
      TGeoCombiTrans* transAlv_15_3 = new TGeoCombiTrans();
      rotAlv_15->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_15_3 = (*rotPlace_15) * matDemo_1 * (*rotAlv_15) * (*transAlvBase);
      pWorld->AddNode(Alveolus_15,3,transAlv_15_3); 
    }
  }

  if(petals[1]>0){
    //Second sector
    //single petal or first half of double petal in second sector
    *rotAlv_15 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_15_4 = new TGeoCombiTrans(-3.13549999999993,33.4646558050777,26.6976862352101,rotAlv_15);
    *transAlvBase =  (*transAlv_15_4) * (*transDef) ;
    rotPlace_15->SetAngles(-60., 0., 0.);
    *transAlv_15_4 =  (*rotPlace_15) * matDemo_2 * (*transAlvBase);
    pWorld->AddNode(Alveolus_15,4,transAlv_15_4);
    TGeoCombiTrans* transAlv_15_5 = new TGeoCombiTrans();
    rotAlv_15->SetAngles(-11.25, 0., 0.);
    *transAlv_15_5 = (*rotPlace_15) * matDemo_2 * (*rotAlv_15) * (*transAlvBase);
    pWorld->AddNode(Alveolus_15,5,transAlv_15_5);
    if(petals[1]>1){
      //second half of double petal in first sector
      TGeoCombiTrans* transAlv_15_6 = new TGeoCombiTrans();
      rotAlv_15->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_15_6 = (*rotPlace_15) * matDemo_2 * (*rotAlv_15) * (*transAlvBase);
      pWorld->AddNode(Alveolus_15,6,transAlv_15_6); 
      TGeoCombiTrans* transAlv_15_7 = new TGeoCombiTrans();
      rotAlv_15->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_15_7 = (*rotPlace_15) * matDemo_2 * (*rotAlv_15) * (*transAlvBase);
      pWorld->AddNode(Alveolus_15,7,transAlv_15_7); 
    }
  }


  if(petals[2]>0){
    //Third sector
    //single petal or first half of double petal in third sector
    *rotAlv_15 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_15_8 = new TGeoCombiTrans(-3.13549999999993,33.4646558050777,26.6976862352101,rotAlv_15);
    *transAlvBase =  (*transAlv_15_8) * (*transDef) ;
    rotPlace_15->SetAngles(-120., 0., 0.);
    *transAlv_15_8 =  (*rotPlace_15) * matDemo_3 * (*transAlvBase);
    pWorld->AddNode(Alveolus_15,8,transAlv_15_8);
    
    TGeoCombiTrans* transAlv_15_9 = new TGeoCombiTrans();
    rotAlv_15->SetAngles(-11.25, 0., 0.);
    *transAlv_15_9 = (*rotPlace_15) * matDemo_3 * (*rotAlv_15) * (*transAlvBase);
    pWorld->AddNode(Alveolus_15,9,transAlv_15_9);
    if(petals[2]>1){
      //second half of double petal in third sector
      TGeoCombiTrans* transAlv_15_10 = new TGeoCombiTrans();
      rotAlv_15->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_15_10 = (*rotPlace_15) * matDemo_3 * (*rotAlv_15) * (*transAlvBase);
      pWorld->AddNode(Alveolus_15,10,transAlv_15_10); 
      
      TGeoCombiTrans* transAlv_15_11 = new TGeoCombiTrans();
      rotAlv_15->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_15_11 = (*rotPlace_15) * matDemo_3 * (*rotAlv_15) * (*transAlvBase);
      pWorld->AddNode(Alveolus_15,11,transAlv_15_11); 
    }
  }

  if(petals[3]>0){
    //Fourth sector
    //single petal or first half of double petal in fourth sector
    *rotAlv_15 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_15_12 = new TGeoCombiTrans(-3.13549999999993,33.4646558050777,26.6976862352101,rotAlv_15);
    *transAlvBase =  (*transAlv_15_12) * (*transDef) ;
    rotPlace_15->SetAngles(-180., 0., 0.);
    *transAlv_15_12 =  (*rotPlace_15) * matDemo_4 * (*transAlvBase);
    pWorld->AddNode(Alveolus_15,12,transAlv_15_12);
    
    TGeoCombiTrans* transAlv_15_13 = new TGeoCombiTrans();
    rotAlv_15->SetAngles(-11.25, 0., 0.);
    *transAlv_15_13 = (*rotPlace_15) * matDemo_4 * (*rotAlv_15) * (*transAlvBase);
    pWorld->AddNode(Alveolus_15,13,transAlv_15_13);
    if(petals[3]>1){
      //second half of double petal in fourth sector
      TGeoCombiTrans* transAlv_15_14 = new TGeoCombiTrans();
      rotAlv_15->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_15_14 = (*rotPlace_15) * matDemo_4 * (*rotAlv_15) * (*transAlvBase);
      pWorld->AddNode(Alveolus_15,14,transAlv_15_14); 
      
      TGeoCombiTrans* transAlv_15_15 = new TGeoCombiTrans();
      rotAlv_15->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_15_15 = (*rotPlace_15) * matDemo_4 * (*rotAlv_15) * (*transAlvBase);
      pWorld->AddNode(Alveolus_15,15,transAlv_15_15); 
    }
  }
  
  if(petals[4]>0){
    //Fifth sector
    //single petal or first half of double petal in fifth sector
    *rotAlv_15 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_15_16 = new TGeoCombiTrans(-3.13549999999993,33.4646558050777,26.6976862352101,rotAlv_15);
    *transAlvBase =  (*transAlv_15_16) * (*transDef) ;
    rotPlace_15->SetAngles(-240., 0., 0.);
    *transAlv_15_16 =  (*rotPlace_15) * matDemo_5 * (*transAlvBase);
    pWorld->AddNode(Alveolus_15,16,transAlv_15_16);
    
    TGeoCombiTrans* transAlv_15_17 = new TGeoCombiTrans();
    rotAlv_15->SetAngles(-11.25, 0., 0.);
    *transAlv_15_17 = (*rotPlace_15) * matDemo_5 * (*rotAlv_15) * (*transAlvBase);
    pWorld->AddNode(Alveolus_15,17,transAlv_15_17);
    if(petals[4]>1){
      //second half of double petal in fifth sector
      TGeoCombiTrans* transAlv_15_18 = new TGeoCombiTrans();
      rotAlv_15->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_15_18 = (*rotPlace_15) * matDemo_5 * (*rotAlv_15) * (*transAlvBase);
      pWorld->AddNode(Alveolus_15,18,transAlv_15_18); 
      
      TGeoCombiTrans* transAlv_15_19 = new TGeoCombiTrans();
      rotAlv_15->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_15_19 = (*rotPlace_15) * matDemo_5 * (*rotAlv_15) * (*transAlvBase);
      pWorld->AddNode(Alveolus_15,19,transAlv_15_19); 
    }
  }
  
  if(petals[5]>0){
    //Fifth sector
    //single petal or first half of double petal in fifth sector
    *rotAlv_15 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_15_20 = new TGeoCombiTrans(-3.13549999999993,33.4646558050777,26.6976862352101,rotAlv_15);
    *transAlvBase =  (*transAlv_15_20) * (*transDef) ;
    rotPlace_15->SetAngles(-300., 0., 0.);
    *transAlv_15_20 =  (*rotPlace_15) * matDemo_6 * (*transAlvBase);
    pWorld->AddNode(Alveolus_15,20,transAlv_15_20);
    
    TGeoCombiTrans* transAlv_15_21 = new TGeoCombiTrans();
    rotAlv_15->SetAngles(-11.25, 0., 0.);
    *transAlv_15_21 = (*rotPlace_15) * matDemo_6 * (*rotAlv_15) * (*transAlvBase);
    pWorld->AddNode(Alveolus_15,21,transAlv_15_21);
    if(petals[5]>1){
      //second half of double petal in fifth sector
      TGeoCombiTrans* transAlv_15_22 = new TGeoCombiTrans();
      rotAlv_15->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_15_22 = (*rotPlace_15) * matDemo_6 * (*rotAlv_15) * (*transAlvBase);
      pWorld->AddNode(Alveolus_15,22,transAlv_15_22); 
      
      TGeoCombiTrans* transAlv_15_23 = new TGeoCombiTrans();
      rotAlv_15->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_15_23 = (*rotPlace_15) * matDemo_6 * (*rotAlv_15) * (*transAlvBase);
      pWorld->AddNode(Alveolus_15,23,transAlv_15_23); 
    }
  }

  phiEuler = 180;
  thetaEuler = 180.*TMath::ATan2(-0.6720370049050432600,0.7405175649761855900)/TMath::Pi();
  psiEuler = -180;
  
  TGeoRotation *rotAlv_16 = new TGeoRotation("rotAlv16",phiEuler,thetaEuler,psiEuler);
  TGeoRotation *rotPlace_16 = new TGeoRotation();

  if(petals[0]>0){
    //First sector
    //single petal or first half of double petal in first sector
    TGeoCombiTrans* transAlv_16_0 = new TGeoCombiTrans(-3.15649999999993,33.559817680364,31.0463924828861,rotAlv_16);
    *transAlvBase =  (*transAlv_16_0) * (*transDef) ;
    rotPlace_16->SetAngles(0., 0., 0.);
    *transAlv_16_0 =  (*rotPlace_16) * matDemo_1 * (*transAlvBase);
    pWorld->AddNode(Alveolus_16,0,transAlv_16_0);
    TGeoCombiTrans* transAlv_16_1 = new TGeoCombiTrans();
    rotAlv_16->SetAngles(-11.25, 0., 0.);
    *transAlv_16_1 = (*rotPlace_16) * matDemo_1 * (*rotAlv_16) * (*transAlvBase);
    pWorld->AddNode(Alveolus_16,1,transAlv_16_1);
    if(petals[0]>1){
      //second half of double petal in first sector
      TGeoCombiTrans* transAlv_16_2 = new TGeoCombiTrans();
      rotAlv_16->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_16_2 = (*rotPlace_16) * matDemo_1 * (*rotAlv_16) * (*transAlvBase);
      pWorld->AddNode(Alveolus_16,2,transAlv_16_2); 
      TGeoCombiTrans* transAlv_16_3 = new TGeoCombiTrans();
      rotAlv_16->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_16_3 = (*rotPlace_16) * matDemo_1 * (*rotAlv_16) * (*transAlvBase);
      pWorld->AddNode(Alveolus_16,3,transAlv_16_3); 
    }
  }

  if(petals[1]>0){
    //Second sector
    //single petal or first half of double petal in second sector
    *rotAlv_16 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_16_4 = new TGeoCombiTrans(-3.15649999999993,33.559817680364,31.0463924828861,rotAlv_16);
    *transAlvBase =  (*transAlv_16_4) * (*transDef) ;
    rotPlace_16->SetAngles(-60., 0., 0.);
    *transAlv_16_4 =  (*rotPlace_16) * matDemo_2 * (*transAlvBase);
    pWorld->AddNode(Alveolus_16,4,transAlv_16_4);
    TGeoCombiTrans* transAlv_16_5 = new TGeoCombiTrans();
    rotAlv_16->SetAngles(-11.25, 0., 0.);
    *transAlv_16_5 = (*rotPlace_16) * matDemo_2 * (*rotAlv_16) * (*transAlvBase);
    pWorld->AddNode(Alveolus_16,5,transAlv_16_5);
    if(petals[1]>1){
      //second half of double petal in first sector
      TGeoCombiTrans* transAlv_16_6 = new TGeoCombiTrans();
      rotAlv_16->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_16_6 = (*rotPlace_16) * matDemo_2 * (*rotAlv_16) * (*transAlvBase);
      pWorld->AddNode(Alveolus_16,6,transAlv_16_6); 
      TGeoCombiTrans* transAlv_16_7 = new TGeoCombiTrans();
      rotAlv_16->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_16_7 = (*rotPlace_16) * matDemo_2 * (*rotAlv_16) * (*transAlvBase);
      pWorld->AddNode(Alveolus_16,7,transAlv_16_7); 
    }
  }


  if(petals[2]>0){
    //Third sector
    //single petal or first half of double petal in third sector
    *rotAlv_16 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_16_8 = new TGeoCombiTrans(-3.15649999999993,33.559817680364,31.0463924828861,rotAlv_16);
    *transAlvBase =  (*transAlv_16_8) * (*transDef) ;
    rotPlace_16->SetAngles(-120., 0., 0.);
    *transAlv_16_8 =  (*rotPlace_16) * matDemo_3 * (*transAlvBase);
    pWorld->AddNode(Alveolus_16,8,transAlv_16_8);
    
    TGeoCombiTrans* transAlv_16_9 = new TGeoCombiTrans();
    rotAlv_16->SetAngles(-11.25, 0., 0.);
    *transAlv_16_9 = (*rotPlace_16) * matDemo_3 * (*rotAlv_16) * (*transAlvBase);
    pWorld->AddNode(Alveolus_16,9,transAlv_16_9);
    if(petals[2]>1){
      //second half of double petal in third sector
      TGeoCombiTrans* transAlv_16_10 = new TGeoCombiTrans();
      rotAlv_16->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_16_10 = (*rotPlace_16) * matDemo_3 * (*rotAlv_16) * (*transAlvBase);
      pWorld->AddNode(Alveolus_16,10,transAlv_16_10); 
      
      TGeoCombiTrans* transAlv_16_11 = new TGeoCombiTrans();
      rotAlv_16->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_16_11 = (*rotPlace_16) * matDemo_3 * (*rotAlv_16) * (*transAlvBase);
      pWorld->AddNode(Alveolus_16,11,transAlv_16_11); 
    }
  }

  if(petals[3]>0){
    //Fourth sector
    //single petal or first half of double petal in fourth sector
    *rotAlv_16 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_16_12 = new TGeoCombiTrans(-3.15649999999993,33.559817680364,31.0463924828861,rotAlv_16);
    *transAlvBase =  (*transAlv_16_12) * (*transDef) ;
    rotPlace_16->SetAngles(-180., 0., 0.);
    *transAlv_16_12 =  (*rotPlace_16) * matDemo_4 * (*transAlvBase);
    pWorld->AddNode(Alveolus_16,12,transAlv_16_12);
    
    TGeoCombiTrans* transAlv_16_13 = new TGeoCombiTrans();
    rotAlv_16->SetAngles(-11.25, 0., 0.);
    *transAlv_16_13 = (*rotPlace_16) * matDemo_4 * (*rotAlv_16) * (*transAlvBase);
    pWorld->AddNode(Alveolus_16,13,transAlv_16_13);
    if(petals[3]>1){
      //second half of double petal in fourth sector
      TGeoCombiTrans* transAlv_16_14 = new TGeoCombiTrans();
      rotAlv_16->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_16_14 = (*rotPlace_16) * matDemo_4 * (*rotAlv_16) * (*transAlvBase);
      pWorld->AddNode(Alveolus_16,14,transAlv_16_14); 
      
      TGeoCombiTrans* transAlv_16_15 = new TGeoCombiTrans();
      rotAlv_16->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_16_15 = (*rotPlace_16) * matDemo_4 * (*rotAlv_16) * (*transAlvBase);
      pWorld->AddNode(Alveolus_16,15,transAlv_16_15); 
    }
  }
  
  if(petals[4]>0){
    //Fifth sector
    //single petal or first half of double petal in fifth sector
    *rotAlv_16 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_16_16 = new TGeoCombiTrans(-3.15649999999993,33.559817680364,31.0463924828861,rotAlv_16);
    *transAlvBase =  (*transAlv_16_16) * (*transDef) ;
    rotPlace_16->SetAngles(-240., 0., 0.);
    *transAlv_16_16 =  (*rotPlace_16) * matDemo_5 * (*transAlvBase);
    pWorld->AddNode(Alveolus_16,16,transAlv_16_16);
    
    TGeoCombiTrans* transAlv_16_17 = new TGeoCombiTrans();
    rotAlv_16->SetAngles(-11.25, 0., 0.);
    *transAlv_16_17 = (*rotPlace_16) * matDemo_5 * (*rotAlv_16) * (*transAlvBase);
    pWorld->AddNode(Alveolus_16,17,transAlv_16_17);
    if(petals[4]>1){
      //second half of double petal in fifth sector
      TGeoCombiTrans* transAlv_16_18 = new TGeoCombiTrans();
      rotAlv_16->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_16_18 = (*rotPlace_16) * matDemo_5 * (*rotAlv_16) * (*transAlvBase);
      pWorld->AddNode(Alveolus_16,18,transAlv_16_18); 
      
      TGeoCombiTrans* transAlv_16_19 = new TGeoCombiTrans();
      rotAlv_16->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_16_19 = (*rotPlace_16) * matDemo_5 * (*rotAlv_16) * (*transAlvBase);
      pWorld->AddNode(Alveolus_16,19,transAlv_16_19); 
    }
  }
  
  if(petals[5]>0){
    //Fifth sector
    //single petal or first half of double petal in fifth sector
    *rotAlv_16 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
    TGeoCombiTrans* transAlv_16_20 = new TGeoCombiTrans(-3.15649999999993,33.559817680364,31.0463924828861,rotAlv_16);
    *transAlvBase =  (*transAlv_16_20) * (*transDef) ;
    rotPlace_16->SetAngles(-300., 0., 0.);
    *transAlv_16_20 =  (*rotPlace_16) * matDemo_6 * (*transAlvBase);
    pWorld->AddNode(Alveolus_16,20,transAlv_16_20);
    
    TGeoCombiTrans* transAlv_16_21 = new TGeoCombiTrans();
    rotAlv_16->SetAngles(-11.25, 0., 0.);
    *transAlv_16_21 = (*rotPlace_16) * matDemo_6 * (*rotAlv_16) * (*transAlvBase);
    pWorld->AddNode(Alveolus_16,21,transAlv_16_21);
    if(petals[5]>1){
      //second half of double petal in fifth sector
      TGeoCombiTrans* transAlv_16_22 = new TGeoCombiTrans();
      rotAlv_16->SetAngles(-11.25 * 2, 0., 0.);
      *transAlv_16_22 = (*rotPlace_16) * matDemo_6 * (*rotAlv_16) * (*transAlvBase);
      pWorld->AddNode(Alveolus_16,22,transAlv_16_22); 
      
      TGeoCombiTrans* transAlv_16_23 = new TGeoCombiTrans();
      rotAlv_16->SetAngles(-11.25 * 3, 0., 0.);
      *transAlv_16_23 = (*rotPlace_16) * matDemo_6 * (*rotAlv_16) * (*transAlvBase);
      pWorld->AddNode(Alveolus_16,23,transAlv_16_23); 
    }
  }

  /*
  phiEuler = 180;
  thetaEuler = 180.*TMath::ATan2(-0.6720370049050432600,0.7405175649761855900)/TMath::Pi();
  psiEuler = -180;
  
  TGeoRotation *rotAlv_16 = new TGeoRotation("rotAlv16",phiEuler,thetaEuler,psiEuler);
  TGeoRotation *rotPlace_16 = new TGeoRotation();
  *rotAlv_16 = TGeoRotation("rotAlv16",phiEuler,thetaEuler,psiEuler);
  TGeoCombiTrans* transAlv_16_0 = new TGeoCombiTrans(-3.15649999999993,33.559817680364,31.0463924828861,rotAlv_16);
  *transAlvBase =  (*transAlv_16_0) * (*transDef) ;
  rotPlace_16->SetAngles(-120, 0., 0.);
  *transAlv_16_0 =  (*rotPlace_16) * matDemo * (*transAlvBase);
  rotAlv_16->SetAngles(-11.25, 0., 0.);
  pWorld->AddNode(Alveolus_16,0,transAlv_16_0);
  
  TGeoCombiTrans* transAlv_16_1 = new TGeoCombiTrans();
  *transAlv_16_1 = (*rotPlace_16) * matDemo * (*rotAlv_16) * (*transAlvBase);
  pWorld->AddNode(Alveolus_16,1,transAlv_16_1);
  
  *rotAlv_16 = TGeoRotation("rotAlv16",phiEuler,thetaEuler,psiEuler);
  TGeoCombiTrans* transAlv_16_2 = new TGeoCombiTrans(-3.15649999999993,33.559817680364,31.0463924828861,rotAlv_16);
  *transAlvBase =  (*transAlv_16_2) * (*transDef) ;
  rotPlace_16->SetAngles(-300, 0., 0.);
  *transAlv_16_2 =  (*rotPlace_16) * matDemo * (*transAlvBase);
  rotAlv_16->SetAngles(-11.25, 0., 0.);
  pWorld->AddNode(Alveolus_16,2,transAlv_16_2);
  
  TGeoCombiTrans* transAlv_16_3 = new TGeoCombiTrans();
  *transAlv_16_3 = (*rotPlace_16) * matDemo * (*rotAlv_16) * (*transAlvBase);
  pWorld->AddNode(Alveolus_16,3,transAlv_16_3);
  */

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
