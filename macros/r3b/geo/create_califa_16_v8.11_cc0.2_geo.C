#include <iomanip>
#include <iostream>
#include "TGeoManager.h"
#include "TMath.h"

#define Barrel 1

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


  //WORLD
  //TGeoVolume *pAWorld  =  top;
  
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
  

  // --------------   Create iPhos endcap -------------------------------------

  /*** settings ***/

  //Color of endcap in visualization
  Color_t col_alveolus = kBlack+2;
  Color_t col_wrapping = kGreen+2;
  Color_t col_crystal = kGreen+2;
  Color_t col_crystal_left, col_crystal_right;


  //Color of barrel in visualization
  Color_t col_barrel_wrapping = kRed;
  Color_t col_barrel_alveolus = kRed;
  Color_t col_barrel_crystal = kRed;

 
  //Inner radius of endcap
  //Double_t inner_radius = 58.7;
  Double_t inner_radius = 41.7;

  //Full length of endcap crystals (in cm)
  Double_t l=0.;
  
  //number of endcap rings in polar angle
  const Int_t num_theta = 14;

  //maximum number of endcap crystals in azimuthal angle
  //for inner rings
  const Int_t max_num_phi = 32;

  //maximum number of endcap crystals in azimuthal angle
  //for inner rings
  const Int_t draw_phi_blocks = 16;


  //theta angles for endcap rings (from outside)
  //Double_t theta_crystals[num_theta] = {2.4,2.4,2.3,2.3,2.3,2.4,2.4,2.5,2.6,2.7,2.5,2.6,2.9,3.5};
  Double_t theta_crystals[num_theta] = {2.5,2.5,2.4,2.4,2.4,2.5,2.5,2.6,2.7,2.8,2.6,2.7,3.1,3.8};

  Double_t theta[num_theta+1] = {0.};
  Double_t start = 45.5;
  Double_t sum = 0.;
  // Calculate theta angles from start point and crystal angles
  // HERE: i counts from high to small angles
  for (unsigned int i = 0; i < num_theta+1; i++) {
    theta[num_theta - i] = start - sum;
    if (i < num_theta)
      sum += theta_crystals[i];
  }

  //Thickness of alveoli
  Double_t thick_alv = 0.025;       //cm

  //Thickness of wrapping;
  Double_t thick_wrap_iPhos = 0.0065;     //cm
  Double_t thick_wrap_phoswich = 0.0130;     //cm

  //Thickness of Al housing for phoswich
  Double_t thick_al_housing = 0.04; //cm

  //how many phoswich rings?
  unsigned int num_phoswich = 4;

  //left and right crystals after ring (counted from inside start @ 1)
  unsigned int cut = 8;

  //Endcap shift in beam (z) direction
  Double_t shift_z = 2.6;            //cm

  /*** END of settings ***/

  /*** Some definitions ***/
  TGeoRotation *rotUni = new TGeoRotation();          //unitary rotation 
 
  TGeoVolume *Alveolus_EC[num_theta];
  TGeoVolume *CrystalWithWrapping[2*num_theta];
  TGeoVolume *Crystal[2*num_theta];
  TGeoVolume *Crystal2[2*num_theta];

  //HERE: i counts from small to high angles
  for(unsigned int i = 0; i < num_theta; i++) {

    //Define num_phi for first two rings 16
    //middle part 32
    Int_t num_phi = max_num_phi;
    if(i<2)
      num_phi = 16;

    Double_t dphi = 360. / (Double_t)num_phi;
    Double_t phi = dphi / 2 + 5.625;

    //Define crystal length dependent on ring
    if(i < 4)
      l = 15.;
    else
      l = 22;

    //Define different color for phoswich and iPhos
    if(i < 4) {
      //phoswich
      col_alveolus = kBlue+2;
      col_wrapping = kBlue+2;
      col_crystal = kGreen + 3 - i; //kBlue+2;
      col_crystal_left = kGreen + 3 - i; //kGreen+2;
      col_crystal_right = kSpring - 1 - i;
    } else {
      //iPhos
      col_alveolus = kBlack+2;
      col_wrapping = kBlack;
      col_crystal = kAzure + i - 3;
      col_crystal_left = kAzure + i - 3; //kGreen+2;
      col_crystal_right = kAzure + i - 14;
    }

    //Define thick_alv dependent on ring
    //(between ring i and i - 1)
    Double_t thick_alv_theta = 0.; 
    if(i == 4 || i == 8 || i == 10 || i == 12) {
      thick_alv_theta = thick_alv;
    } else {
      thick_alv_theta = 0.;
    }
    Double_t thick_wrap = thick_wrap_iPhos;
    Double_t thick_alv_phi = thick_alv;
    //Aluminum housing 0.1cm
    if(i < 4) {
      thick_alv_phi = thick_al_housing;
      thick_wrap = thick_wrap_phoswich;
    }

    //Define vectors from origin to edges of the alveoli
    TVector3 A, B, C, D, A2, B2, C2, D2;

    //Gap between different num_phi
    Double_t gap = 0.;
    if(i == 1)
      gap = 0.1;

    A.SetMagThetaPhi(inner_radius - thick_alv / 4. - thick_wrap / 2., (theta[i])*TMath::Pi()/180., 0);
    B.SetMagThetaPhi(inner_radius - thick_alv / 4. - thick_wrap / 2., (theta[i])*TMath::Pi()/180., dphi*TMath::Pi()/180.);
    C.SetMagThetaPhi(inner_radius - thick_alv / 4. - thick_wrap / 2., (theta[i+1] - gap)*TMath::Pi()/180., 0);
    D.SetMagThetaPhi(inner_radius - thick_alv / 4. - thick_wrap / 2., (theta[i+1] - gap)*TMath::Pi()/180., dphi*TMath::Pi()/180.);

    A2.SetMagThetaPhi(inner_radius + l + thick_alv / 4. + thick_wrap / 2., (theta[i])*TMath::Pi()/180., 0);
    B2.SetMagThetaPhi(inner_radius + l + thick_alv / 4. + thick_wrap / 2., (theta[i])*TMath::Pi()/180., dphi*TMath::Pi()/180.);
    C2.SetMagThetaPhi(inner_radius + l + thick_alv / 4. + thick_wrap / 2., (theta[i+1] - gap)*TMath::Pi()/180., 0);
    D2.SetMagThetaPhi(inner_radius + l + thick_alv / 4. + thick_wrap / 2., (theta[i+1] - gap)*TMath::Pi()/180., dphi*TMath::Pi()/180.);

    //Calculate theta to place the crystal via its center
    TVector3 place_theta = (A + B + C + D) * 0.25;

    //Define vectors for TGeoArb8 definition
    TVector3 p0[2], p1[2], p2[2], p3[2], p4[2], p5[2], p6[2], p7[2];
    p0[0] = (A-B) * 0.5;
    p0[1] = (C+D-A-B) * 0.25;
    p1[0] = (A-B) * 0.5;
    p1[1] = (C+D-A-B) * 0.25;
    p2[0] = (C-D) * 0.5;
    p2[1] = (C+D-A-B) * 0.25;
    p3[0] = (C-D) * 0.5;
    p3[1] = (C+D-A-B) * 0.25;
    p4[0] = (A2-B2) * 0.5;
    p4[1] = (C2+D2-A2-B2) * 0.25;
    p5[0] = (A2-B2) * 0.5;
    p5[1] = (C2+D2-A2-B2) * 0.25;
    p6[0] = (C2-D2) * 0.5;
    p6[1] = (C2+D2-A2-B2) * 0.25;
    p7[0] = (C2-D2) * 0.5;
    p7[1] = (C2+D2-A2-B2) * 0.25;


    //move origin in crystal center
    Double_t x_center = (p0[0].Mag() + p2[0].Mag())  / 4.;
    Double_t x_center2 = (p4[0].Mag()+p6[0].Mag())  / 4.;
    Double_t x_place = (x_center + x_center2) / 2.;

    if( i < cut) {
      //Vertices for Alveolus
      Double_t vertices_alv[16] = {-p0[0].Mag(), p0[1].Mag(), p1[0].Mag(), p1[1].Mag(), p2[0].Mag(), -p2[1].Mag(),
				   -p3[0].Mag(), -p3[1].Mag(), -p4[0].Mag(), p4[1].Mag(), p5[0].Mag(), p5[1].Mag(),
				   p6[0].Mag(), -p6[1].Mag(), -p7[0].Mag(), -p7[1].Mag()};
      //Vertices for CrystalWithWrapping
      Double_t vertices_wrap[16];
      if( i < 4 && i > 1) {
	for(unsigned int i_wrap = 0; i_wrap < 16; i_wrap++) {
	  if(vertices_alv[i_wrap] > 0.)
	    if(i_wrap % 2 == 0)
	      vertices_wrap[i_wrap] = vertices_alv[i_wrap] - thick_alv_phi;
	    else
	      vertices_wrap[i_wrap] = vertices_alv[i_wrap] - thick_alv_theta;
	  else
	    if(i_wrap % 2 == 0)
	      vertices_wrap[i_wrap] = vertices_alv[i_wrap] + thick_alv_phi;
	    else
	      vertices_wrap[i_wrap] = vertices_alv[i_wrap];
	}

      }
      for(unsigned int i_wrap = 0; i_wrap < 16; i_wrap++) {
	if(vertices_alv[i_wrap] > 0.)
	  if(i_wrap % 2 == 0)
	    vertices_wrap[i_wrap] = vertices_alv[i_wrap] - thick_alv_phi;
	  else
	    vertices_wrap[i_wrap] = vertices_alv[i_wrap] - thick_alv_theta;
	else
	  if(i_wrap % 2 == 0)
	    vertices_wrap[i_wrap] = vertices_alv[i_wrap] + thick_alv_phi;
	  else
	    vertices_wrap[i_wrap] = vertices_alv[i_wrap];
      }
      //Vertices for Crystal
      if(i < num_phoswich) {
	//Phoswich
	Double_t vertices_crys_first[16];
	Double_t vertices_crys_second[16];
	Double_t r = 7./l;   //first divided by length
	for(unsigned int i_crys = 0; i_crys < 8; i_crys++) {
	  if(vertices_wrap[i_crys] > 0.)
	    vertices_crys_first[i_crys] = vertices_wrap[i_crys] - thick_wrap;
	  else
	    vertices_crys_first[i_crys] = vertices_wrap[i_crys] + thick_wrap;
	}
	for(unsigned int i_crys = 8; i_crys < 16; i_crys++) {
	  if(vertices_wrap[i_crys] > 0.)
	    vertices_crys_first[i_crys] = (vertices_wrap[i_crys] - thick_wrap) * r + vertices_crys_first[i_crys-8] * (1-r);
	  else
	    vertices_crys_first[i_crys] = (vertices_wrap[i_crys] + thick_wrap) * r + vertices_crys_first[i_crys-8] * (1-r);
	}
	for(unsigned int i_crys = 0; i_crys < 8; i_crys++) {
	    vertices_crys_second[i_crys] = vertices_crys_first[i_crys+8];
	}
	for(unsigned int i_crys = 8; i_crys < 16; i_crys++) {
	  if(vertices_wrap[i_crys] > 0.)
	    vertices_crys_second[i_crys] = vertices_wrap[i_crys] - thick_wrap;
	  else
	    vertices_crys_second[i_crys] = vertices_wrap[i_crys] + thick_wrap;
	}


	//Define geometries
	Alveolus_EC[i] = gGeoManager->MakeArb8(Form("Alveolus_EC_%u", i + 1), pAlHousing, l / 2. + thick_wrap + thick_alv / 2., vertices_alv);
	Alveolus_EC[i]->SetLineColor(col_alveolus);
	CrystalWithWrapping[i] = gGeoManager->MakeArb8(Form("CrystalWithWrapping_%u", i + 1), pCarbonFibreMedium, l / 2. + thick_wrap, vertices_wrap);
	CrystalWithWrapping[i]->SetLineColor(col_wrapping);
	Crystal[2*i] = gGeoManager->MakeArb8(Form("Crystal_%u", 2*i + 1), pLaBrMedium, r * l / 2., vertices_crys_first);
	Crystal[2*i]->SetLineColor(col_crystal_left);
	Crystal[2*i+1] = gGeoManager->MakeArb8(Form("Crystal_%u", 2*i + 2), pLaClMedium, (1 - r) * l / 2., vertices_crys_second);
	Crystal[2*i+1]->SetLineColor(col_crystal_right);
	

	//***Place geometries***//

	//CrystalWithWrapping is moved to the back of the alveolus, so a layer of thickness "thick_alv"
	//is in front of the crystal
	Alveolus_EC[i]->AddNode(CrystalWithWrapping[i],1,new TGeoCombiTrans(0,0,thick_alv / 2., rotUni));
	CrystalWithWrapping[i]->AddNode(Crystal[2*i],1,new TGeoCombiTrans(0,0,(r-1) * l/2., rotUni));
	CrystalWithWrapping[i]->AddNode(Crystal[2*i+1],1,new TGeoCombiTrans(0,0,r * l/2., rotUni));


      } else {
	//iPhos with 32 crystals in a ring
	Double_t vertices_crys[16];
	for(unsigned int i_crys = 0; i_crys < 16; i_crys++) {
	  if(vertices_wrap[i_crys] > 0.)
	    vertices_crys[i_crys] = vertices_wrap[i_crys] - thick_wrap;
	  else
	    vertices_crys[i_crys] = vertices_wrap[i_crys] + thick_wrap;
	}

	//Define geometries
	Alveolus_EC[i] = gGeoManager->MakeArb8(Form("Alveolus_EC_%u", i + 1), pCarbonFibreMedium, l / 2. + thick_wrap + thick_alv / 2., vertices_alv);
	Alveolus_EC[i]->SetLineColor(col_alveolus);
	CrystalWithWrapping[i] = gGeoManager->MakeArb8(Form("CrystalWithWrapping_%u", i + 1), pCarbonFibreMedium, l / 2. + thick_wrap, vertices_wrap);
	CrystalWithWrapping[i]->SetLineColor(col_wrapping);
	Crystal[num_phoswich + i] = gGeoManager->MakeArb8(Form("Crystal_%u", num_phoswich + i + 1), pCsIMedium, l / 2., vertices_crys);
	Crystal[num_phoswich + i]->SetLineColor(col_crystal); 

	//***Place geometries***//

	//CrystalWithWrapping is moved to the back of the alveolus, so a layer of thickness "thick_alv"
	//is in front of the crystal
	Alveolus_EC[i]->AddNode(CrystalWithWrapping[i],1,new TGeoCombiTrans(0,0,thick_alv / 2., rotUni));
	CrystalWithWrapping[i]->AddNode(Crystal[num_phoswich + i],1,new TGeoCombiTrans(0,0,0, rotUni));
      }
	TGeoTranslation trans[num_theta][max_num_phi];
	TGeoTranslation ref;
	TGeoRotation rot[num_theta][max_num_phi];
	TGeoHMatrix ta[num_theta][max_num_phi];
	TGeoHMatrix *t[num_theta][max_num_phi];

	//Place alveolus
	for(unsigned int j = 0; j < ((int)((double)num_phi / 16. * (double)draw_phi_blocks)); j++) {
	  trans[i][j] = TGeoTranslation(Form("trans_%u_%u", i, j), 0, 0,inner_radius + l / 2);
	  rot[i][j]   = TGeoRotation(Form("rot_%u_%u", i, j),phi + j * dphi,place_theta.Theta() * 180. / TMath::Pi(),0);
	  ta[i][j]    = rot[i][j]*trans[i][j];
      
	  //Shift focus of endcap in z-direction
	  Double_t *vec;
	  vec = ta[i][j].GetTranslation();
	  vec[2]+=shift_z;
      
	  ta[i][j].SetTranslation(vec);
	  t[i][j]     = new TGeoHMatrix(ta[i][j]);
	  pWorld->AddNode(Alveolus_EC[i], j, t[i][j]);
	
      }
      
    } else {
      //outer rings with doubled crystal number in phi ( = 64)

      //Vertices for Alveolus
      Double_t vertices_alv[16] = {-p0[0].Mag(), p0[1].Mag(), p1[0].Mag(), p1[1].Mag(), p2[0].Mag(), -p2[1].Mag(),
				   -p3[0].Mag(), -p3[1].Mag(), -p4[0].Mag(), p4[1].Mag(), p5[0].Mag(), p5[1].Mag(),
				   p6[0].Mag(), -p6[1].Mag(), -p7[0].Mag(), -p7[1].Mag()};

      //Vertices for CrystalWithWrapping
      Double_t vertices_wrap_left[16] = {-p0[0].Mag() + thick_alv_phi + x_place, p0[1].Mag(), x_place, p1[1].Mag(), x_place, -p2[1].Mag(),
					 -p3[0].Mag() + thick_alv_phi + x_place, -p3[1].Mag(), -p4[0].Mag() + thick_alv_phi + x_place,
					 p4[1].Mag(), x_place, p5[1].Mag(), x_place, -p6[1].Mag(), -p7[0].Mag() + thick_alv_phi + x_place,
					 -p7[1].Mag()};
      Double_t vertices_wrap_right[16] = {-x_place, p0[1].Mag(), p1[0].Mag() - thick_alv_phi - x_place, p1[1].Mag(), 
					  p2[0].Mag() - thick_alv_phi - x_place, -p2[1].Mag(), -x_place, -p3[1].Mag(), -x_place, p4[1].Mag(),
					  p5[0].Mag() - thick_alv_phi - x_place, p5[1].Mag(), p6[0].Mag() - thick_alv_phi - x_place,
					  -p6[1].Mag(), -x_place, -p7[1].Mag()};

      // //Vertices for CrystalWithWrapping
      // Double_t vertices_wrap_left[16] = {-p0[0].Mag() + thick_alv_phi, p0[1].Mag(), 0, p1[1].Mag(), 0, -p2[1].Mag(),
      // 					 -p3[0].Mag() + thick_alv_phi, -p3[1].Mag(), -p4[0].Mag() + thick_alv_phi,
      // 					 p4[1].Mag(), 0, p5[1].Mag(), 0, -p6[1].Mag(), -p7[0].Mag() + thick_alv_phi,
      // 					 -p7[1].Mag()};
      // Double_t vertices_wrap_right[16] = {0, p0[1].Mag(), p1[0].Mag() - thick_alv_phi, p1[1].Mag(), 
      // 					  p2[0].Mag() - thick_alv_phi, -p2[1].Mag(), 0, -p3[1].Mag(), 0, p4[1].Mag(),
      // 					  p5[0].Mag() - thick_alv_phi, p5[1].Mag(), p6[0].Mag() - thick_alv_phi,
      // 					  -p6[1].Mag(), 0, -p7[1].Mag()};

      for(unsigned int i_wrap = 0; i_wrap < 16; i_wrap++) {
	  if(vertices_wrap_left[i_wrap] > 0.)
	    if(i_wrap % 2 != 0)
	      vertices_wrap_left[i_wrap] -= thick_alv_theta;
      }
      for(unsigned int i_wrap = 0; i_wrap < 16; i_wrap++) {
	  if(vertices_wrap_right[i_wrap] > 0.)
	    if(i_wrap % 2 != 0)
	      vertices_wrap_right[i_wrap] -= thick_alv_theta;
      }
	//Vertices for Crystal
	Double_t vertices_crys_left[16];
	for(unsigned int i_crys = 0; i_crys < 16; i_crys++) {
	  if(vertices_wrap_left[i_crys] >= 0.)
	    vertices_crys_left[i_crys] = vertices_wrap_left[i_crys] - thick_wrap;
	  else
	    vertices_crys_left[i_crys] = vertices_wrap_left[i_crys] + thick_wrap;
	}
	Double_t vertices_crys_right[16];
	for(unsigned int i_crys = 0; i_crys < 16; i_crys++) {
	  if(vertices_wrap_right[i_crys] > 0.)
	    vertices_crys_right[i_crys] = vertices_wrap_right[i_crys] - thick_wrap;
	  else
	    vertices_crys_right[i_crys] = vertices_wrap_right[i_crys] + thick_wrap;
	}

	//Define geometries
	Alveolus_EC[i] = gGeoManager->MakeArb8(Form("Alveolus_EC_%u", i + 1), pCarbonFibreMedium, l / 2. + thick_wrap + thick_alv / 2., vertices_alv);
	Alveolus_EC[i]->SetLineColor(col_alveolus);
	
	CrystalWithWrapping[2*i-cut] = gGeoManager->MakeArb8(Form("CrystalWithWrapping_%u", 2*i-cut+1), pCarbonFibreMedium, l / 2. + thick_wrap, vertices_wrap_left);
	CrystalWithWrapping[2*i-cut]->SetLineColor(col_wrapping);
	CrystalWithWrapping[2*i-cut+1] = gGeoManager->MakeArb8(Form("CrystalWithWrapping_%u", 2*i-cut+2), pCarbonFibreMedium, l / 2. + thick_wrap, vertices_wrap_right);
	CrystalWithWrapping[2*i-cut+1]->SetLineColor(col_wrapping);
	  
	Crystal[num_phoswich + 2*i-cut] = gGeoManager->MakeArb8(Form("Crystal_%u", num_phoswich + 2*i-cut + 1), pCsIMedium, l / 2., vertices_crys_left);
	Crystal[num_phoswich + 2*i-cut]->SetLineColor(col_crystal_left); 
	Crystal[num_phoswich + 2*i-cut+1] = gGeoManager->MakeArb8(Form("Crystal_%u", num_phoswich + 2*i-cut + 2), pCsIMedium, l / 2., vertices_crys_right);
	Crystal[num_phoswich + 2*i-cut+1]->SetLineColor(col_crystal_right);

      //***Place geometries***//

      //CrystalWithWrapping is moved to the back of the alveolus, so a layer of thickness "thick_alv"
      //is in front of the crystal
      Alveolus_EC[i]->AddNode(CrystalWithWrapping[2*i-cut],1,new TGeoCombiTrans(-x_place,0,thick_alv / 2., rotUni));
      Alveolus_EC[i]->AddNode(CrystalWithWrapping[2*i-cut+1],1,new TGeoCombiTrans(x_place,0,thick_alv / 2., rotUni));
      CrystalWithWrapping[2*i-cut]->AddNode(Crystal[num_phoswich + 2*i-cut],1,new TGeoCombiTrans(0,0,0, rotUni));
      CrystalWithWrapping[2*i-cut+1]->AddNode(Crystal[num_phoswich + 2*i-cut+1],1,new TGeoCombiTrans(0,0,0, rotUni));
    
      TGeoTranslation trans[num_theta][max_num_phi];
      TGeoTranslation ref;
      TGeoRotation rot[num_theta][max_num_phi];
      TGeoHMatrix ta[num_theta][max_num_phi];
      TGeoHMatrix *t[num_theta][max_num_phi];

      //Place alveolus
      for(unsigned int j = 0; j < ((int)((double)num_phi / 16. * (double)draw_phi_blocks)); j++) {
	trans[i][j] = TGeoTranslation(Form("trans_%u_%u", i, j), 0, 0,inner_radius + l / 2);
	rot[i][j]   = TGeoRotation(Form("rot_%u_%u", i, j),phi + j * dphi,place_theta.Theta() * 180. / TMath::Pi(),0);
	ta[i][j]    = rot[i][j]*trans[i][j];
      
	//Shift focus of endcap in z-direction
	Double_t *vec;
	vec = ta[i][j].GetTranslation();
	vec[2]+=shift_z;
      
	ta[i][j].SetTranslation(vec);
	t[i][j]     = new TGeoHMatrix(ta[i][j]);
	pWorld->AddNode(Alveolus_EC[i], j, t[i][j]);
      }

    }
  }

#if Barrel

  // --------------   Create CALIFA Barrel 8.11 -------------------------------------

  //Generated 1/9/2014 17:57:29 from macro createGeometryCLF811.pl by Hector Alvarez Pol (hector.alvarez.es) 


 //Alveolus
	 Double_t* vertices_Alv=new Double_t[16]; 
	 Double_t* vertices_inner_Alv=new Double_t[16]; 
	 vertices_Alv[0] = 0; vertices_Alv[1] = -0; 
	 vertices_inner_Alv[0] = 0.000999999999999997;	 vertices_inner_Alv[1] = -0.000999999999999997; 
	 vertices_Alv[2] = 6.807; vertices_Alv[3] = -0; 
	 vertices_inner_Alv[2] = 6.806;	 vertices_inner_Alv[3] = -0.000999999999999997; 
	 vertices_Alv[4] = 6.355273842; vertices_Alv[5] = -6.985409328; 
	 vertices_inner_Alv[4] = 6.354273842;	 vertices_inner_Alv[5] = -6.984409328; 
	 vertices_Alv[6] = 0.451726158; vertices_Alv[7] = -6.985409328; 
	 vertices_inner_Alv[6] = 0.452726158;	 vertices_inner_Alv[7] = -6.984409328; 
	 vertices_Alv[8] = -1.593513291; vertices_Alv[9] = 1.655196415; 
	 vertices_inner_Alv[8] = -1.592513291;	 vertices_inner_Alv[9] = 1.654196415; 
	 vertices_Alv[10] = 8.400513291; vertices_Alv[11] = 1.655196415; 
	 vertices_inner_Alv[10] = 8.399513291;	 vertices_inner_Alv[11] = 1.654196415; 
	 vertices_Alv[12] = 7.723128311; vertices_Alv[13] = -8.832113977; 
	 vertices_inner_Alv[12] = 7.722128311;	 vertices_inner_Alv[13] = -8.831113977; 
	 vertices_Alv[14] = -0.916128311; vertices_Alv[15] = -8.832113977; 
	 vertices_inner_Alv[14] = -0.915128311;	 vertices_inner_Alv[15] = -8.831113977; 
	 TGeoVolume *Alveolus_1=gGeoManager->MakeArb8("Alveolus_1", pCarbonFibreMedium,10, vertices_Alv); 
	 Alveolus_1->SetLineColor(col_barrel_alveolus); 

	 TGeoVolume *AlveolusInner_1=gGeoManager->MakeArb8("AlveolusInner_1", pAirMedium,9.97, vertices_inner_Alv); 
	 AlveolusInner_1->SetLineColor(col_barrel_wrapping); 

	 vertices_Alv[0] = 0; vertices_Alv[1] = -0; 
	 vertices_inner_Alv[0] = 0.000999999999999997;	 vertices_inner_Alv[1] = -0.000999999999999997; 
	 vertices_Alv[2] = 6.441; vertices_Alv[3] = -0; 
	 vertices_inner_Alv[2] = 6.44;	 vertices_inner_Alv[3] = -0.000999999999999997; 
	 vertices_Alv[4] = 6.17278443; vertices_Alv[5] = -4.992800858; 
	 vertices_inner_Alv[4] = 6.17178443;	 vertices_inner_Alv[5] = -4.991800858; 
	 vertices_Alv[6] = 0.26821557; vertices_Alv[7] = -4.992800858; 
	 vertices_inner_Alv[6] = 0.26921557;	 vertices_inner_Alv[7] = -4.991800858; 
	 vertices_Alv[8] = -1.702505526; vertices_Alv[9] = 0.961766999; 
	 vertices_inner_Alv[8] = -1.701505526;	 vertices_inner_Alv[9] = 0.960766999; 
	 vertices_Alv[10] = 8.143505526; vertices_Alv[11] = 0.961766999; 
	 vertices_inner_Alv[10] = 8.142505526;	 vertices_inner_Alv[11] = 0.960766999; 
	 vertices_Alv[12] = 7.763440254; vertices_Alv[13] = -6.131482259; 
	 vertices_inner_Alv[12] = 7.762440254;	 vertices_inner_Alv[13] = -6.130482259; 
	 vertices_Alv[14] = -1.322440254; vertices_Alv[15] = -6.131482259; 
	 vertices_inner_Alv[14] = -1.321440254;	 vertices_inner_Alv[15] = -6.130482259; 
	 TGeoVolume *Alveolus_2=gGeoManager->MakeArb8("Alveolus_2", pCarbonFibreMedium,10, vertices_Alv); 
	 Alveolus_2->SetLineColor(col_barrel_alveolus); 

	 TGeoVolume *AlveolusInner_2=gGeoManager->MakeArb8("AlveolusInner_2", pAirMedium,9.97, vertices_inner_Alv); 
	 AlveolusInner_2->SetLineColor(col_barrel_wrapping); 

	 vertices_Alv[0] = 0; vertices_Alv[1] = -0; 
	 vertices_inner_Alv[0] = 0.000999999999999997;	 vertices_inner_Alv[1] = -0.000999999999999997; 
	 vertices_Alv[2] = 6.351; vertices_Alv[3] = -0; 
	 vertices_inner_Alv[2] = 6.35;	 vertices_inner_Alv[3] = -0.000999999999999997; 
	 vertices_Alv[4] = 6.127236283; vertices_Alv[5] = -4.99499047; 
	 vertices_inner_Alv[4] = 6.126236283;	 vertices_inner_Alv[5] = -4.99399047; 
	 vertices_Alv[6] = 0.223763717; vertices_Alv[7] = -4.99499047; 
	 vertices_inner_Alv[6] = 0.224763717;	 vertices_inner_Alv[7] = -4.99399047; 
	 vertices_Alv[8] = -1.798335042; vertices_Alv[9] = 0.972668234; 
	 vertices_inner_Alv[8] = -1.797335042;	 vertices_inner_Alv[9] = 0.971668234; 
	 vertices_Alv[10] = 8.149335042; vertices_Alv[11] = 0.972668234; 
	 vertices_inner_Alv[10] = 8.148335042;	 vertices_inner_Alv[11] = 0.971668234; 
	 vertices_Alv[12] = 7.831895488; vertices_Alv[13] = -6.124586823; 
	 vertices_inner_Alv[12] = 7.830895488;	 vertices_inner_Alv[13] = -6.123586823; 
	 vertices_Alv[14] = -1.480895488; vertices_Alv[15] = -6.124586823; 
	 vertices_inner_Alv[14] = -1.479895488;	 vertices_inner_Alv[15] = -6.123586823; 
	 TGeoVolume *Alveolus_3=gGeoManager->MakeArb8("Alveolus_3", pCarbonFibreMedium,10, vertices_Alv); 
	 Alveolus_3->SetLineColor(col_barrel_alveolus); 

	 TGeoVolume *AlveolusInner_3=gGeoManager->MakeArb8("AlveolusInner_3", pAirMedium,9.97, vertices_inner_Alv); 
	 AlveolusInner_3->SetLineColor(col_barrel_wrapping); 

	 vertices_Alv[0] = 0; vertices_Alv[1] = -0; 
	 vertices_inner_Alv[0] = 0.000999999999999997;	 vertices_inner_Alv[1] = -0.000999999999999997; 
	 vertices_Alv[2] = 6.2575; vertices_Alv[3] = -0; 
	 vertices_inner_Alv[2] = 6.2565;	 vertices_inner_Alv[3] = -0.000999999999999997; 
	 vertices_Alv[4] = 6.080822201; vertices_Alv[5] = -4.996877521; 
	 vertices_inner_Alv[4] = 6.079822201;	 vertices_inner_Alv[5] = -4.995877521; 
	 vertices_Alv[6] = 0.176677799; vertices_Alv[7] = -4.996877521; 
	 vertices_inner_Alv[6] = 0.177677799;	 vertices_inner_Alv[7] = -4.995877521; 
	 vertices_Alv[8] = -1.873465321; vertices_Alv[9] = 0.986986076; 
	 vertices_inner_Alv[8] = -1.872465321;	 vertices_inner_Alv[9] = 0.985986076; 
	 vertices_Alv[10] = 8.13096532; vertices_Alv[11] = 0.986986076; 
	 vertices_inner_Alv[10] = 8.12996532;	 vertices_inner_Alv[11] = 0.985986076; 
	 vertices_Alv[12] = 7.879832678; vertices_Alv[13] = -6.113713347; 
	 vertices_inner_Alv[12] = 7.878832678;	 vertices_inner_Alv[13] = -6.112713347; 
	 vertices_Alv[14] = -1.622332678; vertices_Alv[15] = -6.113713347; 
	 vertices_inner_Alv[14] = -1.621332678;	 vertices_inner_Alv[15] = -6.112713347; 
	 TGeoVolume *Alveolus_4=gGeoManager->MakeArb8("Alveolus_4", pCarbonFibreMedium,10, vertices_Alv); 
	 Alveolus_4->SetLineColor(col_barrel_alveolus); 

	 TGeoVolume *AlveolusInner_4=gGeoManager->MakeArb8("AlveolusInner_4", pAirMedium,9.97, vertices_inner_Alv); 
	 AlveolusInner_4->SetLineColor(col_barrel_wrapping); 

	 vertices_Alv[0] = 0; vertices_Alv[1] = -0; 
	 vertices_inner_Alv[0] = 0.000999999999999997;	 vertices_inner_Alv[1] = -0.000999999999999997; 
	 vertices_Alv[2] = 6.16; vertices_Alv[3] = -0; 
	 vertices_inner_Alv[2] = 6.159;	 vertices_inner_Alv[3] = -0.000999999999999997; 
	 vertices_Alv[4] = 6.032604752; vertices_Alv[5] = -4.998376782; 
	 vertices_inner_Alv[4] = 6.031604752;	 vertices_inner_Alv[5] = -4.997376782; 
	 vertices_Alv[6] = 0.127395248; vertices_Alv[7] = -4.998376782; 
	 vertices_inner_Alv[6] = 0.128395248;	 vertices_inner_Alv[7] = -4.997376782; 
	 vertices_Alv[8] = -1.928206354; vertices_Alv[9] = 1.004081772; 
	 vertices_inner_Alv[8] = -1.927206354;	 vertices_inner_Alv[9] = 1.003081772; 
	 vertices_Alv[10] = 8.088206354; vertices_Alv[11] = 1.004081772; 
	 vertices_inner_Alv[10] = 8.087206354;	 vertices_inner_Alv[11] = 1.003081772; 
	 vertices_Alv[12] = 7.90627336; vertices_Alv[13] = -6.09935378; 
	 vertices_inner_Alv[12] = 7.90527336;	 vertices_inner_Alv[13] = -6.09835378; 
	 vertices_Alv[14] = -1.746273359; vertices_Alv[15] = -6.09935378; 
	 vertices_inner_Alv[14] = -1.745273359;	 vertices_inner_Alv[15] = -6.09835378; 
	 TGeoVolume *Alveolus_5=gGeoManager->MakeArb8("Alveolus_5", pCarbonFibreMedium,10, vertices_Alv); 
	 Alveolus_5->SetLineColor(col_barrel_alveolus); 

	 TGeoVolume *AlveolusInner_5=gGeoManager->MakeArb8("AlveolusInner_5", pAirMedium,9.97, vertices_inner_Alv); 
	 AlveolusInner_5->SetLineColor(col_barrel_wrapping); 

	 vertices_Alv[0] = 0; vertices_Alv[1] = -0; 
	 vertices_inner_Alv[0] = 0.000999999999999997;	 vertices_inner_Alv[1] = -0.000999999999999997; 
	 vertices_Alv[2] = 6.016; vertices_Alv[3] = -0; 
	 vertices_inner_Alv[2] = 6.015;	 vertices_inner_Alv[3] = -0.000999999999999997; 
	 vertices_Alv[4] = 5.96052167; vertices_Alv[5] = -3.599572496; 
	 vertices_inner_Alv[4] = 5.95952167;	 vertices_inner_Alv[5] = -3.598572496; 
	 vertices_Alv[6] = 0.05547833; vertices_Alv[7] = -3.599572496; 
	 vertices_inner_Alv[6] = 0.05647833;	 vertices_inner_Alv[7] = -3.598572496; 
	 vertices_Alv[8] = -1.961110651; vertices_Alv[9] = 1.02300049; 
	 vertices_inner_Alv[8] = -1.960110651;	 vertices_inner_Alv[9] = 1.02200049; 
	 vertices_Alv[10] = 7.977110651; vertices_Alv[11] = 1.02300049; 
	 vertices_inner_Alv[10] = 7.976110651;	 vertices_inner_Alv[11] = 1.02200049; 
	 vertices_Alv[12] = 7.888418008; vertices_Alv[13] = -4.682508381; 
	 vertices_inner_Alv[12] = 7.887418008;	 vertices_inner_Alv[13] = -4.681508381; 
	 vertices_Alv[14] = -1.872418008; vertices_Alv[15] = -4.682508381; 
	 vertices_inner_Alv[14] = -1.871418008;	 vertices_inner_Alv[15] = -4.681508381; 
	 TGeoVolume *Alveolus_6=gGeoManager->MakeArb8("Alveolus_6", pCarbonFibreMedium,10, vertices_Alv); 
	 Alveolus_6->SetLineColor(col_barrel_alveolus); 

	 TGeoVolume *AlveolusInner_6=gGeoManager->MakeArb8("AlveolusInner_6", pAirMedium,9.97, vertices_inner_Alv); 
	 AlveolusInner_6->SetLineColor(col_barrel_wrapping); 

	 vertices_Alv[0] = 0; vertices_Alv[1] = -0; 
	 vertices_inner_Alv[0] = 0.000999999999999997;	 vertices_inner_Alv[1] = -0.000999999999999997; 
	 vertices_Alv[2] = 5.943; vertices_Alv[3] = -0; 
	 vertices_inner_Alv[2] = 5.942;	 vertices_inner_Alv[3] = -0.000999999999999997; 
	 vertices_Alv[4] = 5.923522221; vertices_Alv[5] = -3.599947307; 
	 vertices_inner_Alv[4] = 5.922522221;	 vertices_inner_Alv[5] = -3.598947307; 
	 vertices_Alv[6] = 0.019477779; vertices_Alv[7] = -3.599947307; 
	 vertices_inner_Alv[6] = 0.020477779;	 vertices_inner_Alv[7] = -3.598947307; 
	 vertices_Alv[8] = -1.97214963; vertices_Alv[9] = 1.042554934; 
	 vertices_inner_Alv[8] = -1.97114963;	 vertices_inner_Alv[9] = 1.041554934; 
	 vertices_Alv[10] = 7.91514963; vertices_Alv[11] = 1.042554934; 
	 vertices_inner_Alv[10] = 7.91414963;	 vertices_inner_Alv[11] = 1.041554934; 
	 vertices_Alv[12] = 7.884841237; vertices_Alv[13] = -4.663787466; 
	 vertices_inner_Alv[12] = 7.883841237;	 vertices_inner_Alv[13] = -4.662787466; 
	 vertices_Alv[14] = -1.941841237; vertices_Alv[15] = -4.663787466; 
	 vertices_inner_Alv[14] = -1.940841237;	 vertices_inner_Alv[15] = -4.662787466; 
	 TGeoVolume *Alveolus_7=gGeoManager->MakeArb8("Alveolus_7", pCarbonFibreMedium,10, vertices_Alv); 
	 Alveolus_7->SetLineColor(col_barrel_alveolus); 

	 TGeoVolume *AlveolusInner_7=gGeoManager->MakeArb8("AlveolusInner_7", pAirMedium,9.97, vertices_inner_Alv); 
	 AlveolusInner_7->SetLineColor(col_barrel_wrapping); 

	 vertices_Alv[0] = 0; vertices_Alv[1] = -0; 
	 vertices_inner_Alv[0] = 0.000999999999999997;	 vertices_inner_Alv[1] = -0.000999999999999997; 
	 vertices_Alv[2] = 5.905; vertices_Alv[3] = -0; 
	 vertices_inner_Alv[2] = 5.904;	 vertices_inner_Alv[3] = -0.000999999999999997; 
	 vertices_Alv[4] = 5.89147374; vertices_Alv[5] = -3.09997049; 
	 vertices_inner_Alv[4] = 5.89047374;	 vertices_inner_Alv[5] = -3.09897049; 
	 vertices_Alv[6] = 0.013526259; vertices_Alv[7] = -3.09997049; 
	 vertices_inner_Alv[6] = 0.014526259;	 vertices_inner_Alv[7] = -3.09897049; 
	 vertices_Alv[8] = -1.970980365; vertices_Alv[9] = 0.868840899; 
	 vertices_inner_Alv[8] = -1.969980365;	 vertices_inner_Alv[9] = 0.867840899; 
	 vertices_Alv[10] = 7.875980366; vertices_Alv[11] = 0.868840899; 
	 vertices_inner_Alv[10] = 7.874980366;	 vertices_inner_Alv[11] = 0.867840899; 
	 vertices_Alv[12] = 7.856178119; vertices_Alv[13] = -3.985990067; 
	 vertices_inner_Alv[12] = 7.855178119;	 vertices_inner_Alv[13] = -3.984990067; 
	 vertices_Alv[14] = -1.951178119; vertices_Alv[15] = -3.985990067; 
	 vertices_inner_Alv[14] = -1.950178119;	 vertices_inner_Alv[15] = -3.984990067; 
	 TGeoVolume *Alveolus_8=gGeoManager->MakeArb8("Alveolus_8", pCarbonFibreMedium,10, vertices_Alv); 
	 Alveolus_8->SetLineColor(col_barrel_alveolus); 

	 TGeoVolume *AlveolusInner_8=gGeoManager->MakeArb8("AlveolusInner_8", pAirMedium,9.97, vertices_inner_Alv); 
	 AlveolusInner_8->SetLineColor(col_barrel_wrapping); 

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
	 Alveolus_9->SetLineColor(col_barrel_alveolus); 

	 TGeoVolume *AlveolusInner_9=gGeoManager->MakeArb8("AlveolusInner_9", pAirMedium,9.97, vertices_inner_Alv); 
	 AlveolusInner_9->SetLineColor(col_barrel_wrapping); 

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
	 Alveolus_10->SetLineColor(col_barrel_alveolus); 

	 TGeoVolume *AlveolusInner_10=gGeoManager->MakeArb8("AlveolusInner_10", pAirMedium,9.97, vertices_inner_Alv); 
	 AlveolusInner_10->SetLineColor(col_barrel_wrapping); 

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
	 Alveolus_11->SetLineColor(col_barrel_alveolus); 

	 TGeoVolume *AlveolusInner_11=gGeoManager->MakeArb8("AlveolusInner_11", pAirMedium,9.97, vertices_inner_Alv); 
	 AlveolusInner_11->SetLineColor(col_barrel_wrapping); 

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
	 Alveolus_12->SetLineColor(col_barrel_alveolus); 

	 TGeoVolume *AlveolusInner_12=gGeoManager->MakeArb8("AlveolusInner_12", pAirMedium,9.97, vertices_inner_Alv); 
	 AlveolusInner_12->SetLineColor(col_barrel_wrapping); 

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
	 Alveolus_13->SetLineColor(col_barrel_alveolus); 

	 TGeoVolume *AlveolusInner_13=gGeoManager->MakeArb8("AlveolusInner_13", pAirMedium,9.97, vertices_inner_Alv); 
	 AlveolusInner_13->SetLineColor(col_barrel_wrapping); 

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
	 Alveolus_14->SetLineColor(col_barrel_alveolus); 

	 TGeoVolume *AlveolusInner_14=gGeoManager->MakeArb8("AlveolusInner_14", pAirMedium,11.97, vertices_inner_Alv); 
	 AlveolusInner_14->SetLineColor(col_barrel_wrapping); 

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
	 Alveolus_15->SetLineColor(col_barrel_alveolus); 

	 TGeoVolume *AlveolusInner_15=gGeoManager->MakeArb8("AlveolusInner_15", pAirMedium,11.97, vertices_inner_Alv); 
	 AlveolusInner_15->SetLineColor(col_barrel_wrapping); 

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
	 Alveolus_16->SetLineColor(col_barrel_alveolus); 

	 TGeoVolume *AlveolusInner_16=gGeoManager->MakeArb8("AlveolusInner_16", pAirMedium,11.97, vertices_inner_Alv); 
	 AlveolusInner_16->SetLineColor(col_barrel_wrapping); 



 //Crystals with and without wrapping
	 Double_t* vertices=new Double_t[16]; 
	 Double_t* vertices2=new Double_t[16]; 
	 vertices[0] = 0; vertices[1] = -0; 				 vertices2[0] = 0.03; vertices2[1] = -0.03; 
	 vertices[2] = 6.777; vertices[3] = -0; 				 vertices2[2] = 6.747; vertices2[3] = -0.03; 
	 vertices[4] = 6.324846802; vertices[5] = -6.954316536; 				 vertices2[4] = 6.294846802; vertices2[5] = -6.924316536; 
	 vertices[6] = 0.452153198; vertices[7] = -6.954316536; 				 vertices2[6] = 0.482153198; vertices2[7] = -6.924316536; 
	 vertices[8] = -0.9561059226; vertices[9] = 0.9928044486; 				 vertices2[8] = -0.9261059226; vertices2[9] = 0.9628044486; 
	 vertices[10] = 7.7331059226; vertices[11] = 0.9928044486; 				 vertices2[10] = 7.7031059226; vertices2[11] = 0.9628044486; 
	 vertices[12] = 7.1455614508; vertices[13] = -8.0626535484; 				 vertices2[12] = 7.1155614508; vertices2[13] = -8.0326535484; 
	 vertices[14] = -0.3685614508; vertices[15] = -8.0626535484; 				 vertices2[14] = -0.3385614508; vertices2[15] = -8.0326535484; 
	 TGeoVolume *CrystalWithWrapping_1_1 = gGeoManager->MakeArb8("CrystalWithWrapping_1_1", pWrappingMedium,6, vertices); 
	 CrystalWithWrapping_1_1->SetLineColor(kGreen); 
	 TGeoVolume *Crystal_1_1 = gGeoManager->MakeArb8("Crystal_1_1", pCsIMedium,5.97, vertices2); 
	 Crystal_1_1->SetLineColor(col_barrel_crystal); 

	 vertices[0] = 2.9365; vertices[1] = -0; 				 vertices2[0] = 2.9665; vertices2[1] = -0.03; 
	 vertices[2] = 5.873; vertices[3] = -0; 				 vertices2[2] = 5.843; vertices2[3] = -0.03; 
	 vertices[4] = 5.873; vertices[5] = -2.48; 				 vertices2[4] = 5.843; vertices2[5] = -2.45; 
	 vertices[6] = 2.9365; vertices[7] = -2.48; 				 vertices2[6] = 2.9665; vertices2[7] = -2.45; 
	 vertices[8] = 2.9365; vertices[9] = 0.6721329237; 				 vertices2[8] = 2.9665; vertices2[9] = 0.6421329237; 
	 vertices[10] = 6.9859392204; vertices[11] = 0.6721329237; 				 vertices2[10] = 6.9559392204; vertices2[11] = 0.6421329237; 
	 vertices[12] = 6.9859392204; vertices[13] = -2.48; 				 vertices2[12] = 6.9559392204; vertices2[13] = -2.45; 
	 vertices[14] = 2.9365; vertices[15] = -2.48; 				 vertices2[14] = 2.9665; vertices2[15] = -2.45; 
	 TGeoVolume *CrystalWithWrapping_2_1 = gGeoManager->MakeArb8("CrystalWithWrapping_2_1", pWrappingMedium,7, vertices); 
	 CrystalWithWrapping_2_1->SetLineColor(kGreen); 
	 TGeoVolume *Crystal_2_1 = gGeoManager->MakeArb8("Crystal_2_1", pCsIMedium,6.97, vertices2); 
	 Crystal_2_1->SetLineColor(col_barrel_crystal); 

	 vertices[0] = 2.9365; vertices[1] = -2.48; 				 vertices2[0] = 2.9665; vertices2[1] = -2.51; 
	 vertices[2] = 5.873; vertices[3] = -2.48; 				 vertices2[2] = 5.843; vertices2[3] = -2.51; 
	 vertices[4] = 5.873; vertices[5] = -4.96; 				 vertices2[4] = 5.843; vertices2[5] = -4.93; 
	 vertices[6] = 2.9365; vertices[7] = -4.96; 				 vertices2[6] = 2.9665; vertices2[7] = -4.93; 
	 vertices[8] = 2.9365; vertices[9] = -2.48; 				 vertices2[8] = 2.9665; vertices2[9] = -2.51; 
	 vertices[10] = 6.9859392204; vertices[11] = -2.48; 				 vertices2[10] = 6.9559392204; vertices2[11] = -2.51; 
	 vertices[12] = 6.9859392204; vertices[13] = -5.6321329237; 				 vertices2[12] = 6.9559392204; vertices2[13] = -5.6021329237; 
	 vertices[14] = 2.9365; vertices[15] = -5.6321329237; 				 vertices2[14] = 2.9665; vertices2[15] = -5.6021329237; 
	 TGeoVolume *CrystalWithWrapping_2_2 = gGeoManager->MakeArb8("CrystalWithWrapping_2_2", pWrappingMedium,7, vertices); 
	 CrystalWithWrapping_2_2->SetLineColor(kGreen); 
	 TGeoVolume *Crystal_2_2 = gGeoManager->MakeArb8("Crystal_2_2", pCsIMedium,6.97, vertices2); 
	 Crystal_2_2->SetLineColor(col_barrel_crystal); 

	 vertices[0] = 0; vertices[1] = -0; 				 vertices2[0] = 0.03; vertices2[1] = -0.03; 
	 vertices[2] = 2.9365; vertices[3] = -0; 				 vertices2[2] = 2.9065; vertices2[3] = -0.03; 
	 vertices[4] = 2.9365; vertices[5] = -2.48; 				 vertices2[4] = 2.9065; vertices2[5] = -2.45; 
	 vertices[6] = 0; vertices[7] = -2.48; 				 vertices2[6] = 0.03; vertices2[7] = -2.45; 
	 vertices[8] = -1.1129392204; vertices[9] = 0.6721329237; 				 vertices2[8] = -1.0829392204; vertices2[9] = 0.6421329237; 
	 vertices[10] = 2.9365; vertices[11] = 0.6721329237; 				 vertices2[10] = 2.9065; vertices2[11] = 0.6421329237; 
	 vertices[12] = 2.9365; vertices[13] = -2.48; 				 vertices2[12] = 2.9065; vertices2[13] = -2.45; 
	 vertices[14] = -1.1129392204; vertices[15] = -2.48; 				 vertices2[14] = -1.0829392204; vertices2[15] = -2.45; 
	 TGeoVolume *CrystalWithWrapping_2_3 = gGeoManager->MakeArb8("CrystalWithWrapping_2_3", pWrappingMedium,7, vertices); 
	 CrystalWithWrapping_2_3->SetLineColor(kGreen); 
	 TGeoVolume *Crystal_2_3 = gGeoManager->MakeArb8("Crystal_2_3", pCsIMedium,6.97, vertices2); 
	 Crystal_2_3->SetLineColor(col_barrel_crystal); 

	 vertices[0] = 0; vertices[1] = -2.48; 				 vertices2[0] = 0.03; vertices2[1] = -2.51; 
	 vertices[2] = 2.9365; vertices[3] = -2.48; 				 vertices2[2] = 2.9065; vertices2[3] = -2.51; 
	 vertices[4] = 2.9365; vertices[5] = -4.96; 				 vertices2[4] = 2.9065; vertices2[5] = -4.93; 
	 vertices[6] = 0; vertices[7] = -4.96; 				 vertices2[6] = 0.03; vertices2[7] = -4.93; 
	 vertices[8] = -1.1129392204; vertices[9] = -2.48; 				 vertices2[8] = -1.0829392204; vertices2[9] = -2.51; 
	 vertices[10] = 2.9365; vertices[11] = -2.48; 				 vertices2[10] = 2.9065; vertices2[11] = -2.51; 
	 vertices[12] = 2.9365; vertices[13] = -5.6321329237; 				 vertices2[12] = 2.9065; vertices2[13] = -5.6021329237; 
	 vertices[14] = -1.1129392204; vertices[15] = -5.6321329237; 				 vertices2[14] = -1.0829392204; vertices2[15] = -5.6021329237; 
	 TGeoVolume *CrystalWithWrapping_2_4 = gGeoManager->MakeArb8("CrystalWithWrapping_2_4", pWrappingMedium,7, vertices); 
	 CrystalWithWrapping_2_4->SetLineColor(kGreen); 
	 TGeoVolume *Crystal_2_4 = gGeoManager->MakeArb8("Crystal_2_4", pCsIMedium,6.97, vertices2); 
	 Crystal_2_4->SetLineColor(col_barrel_crystal); 

	 vertices[0] = 2.937; vertices[1] = -0; 				 vertices2[0] = 2.967; vertices2[1] = -0.03; 
	 vertices[2] = 5.874; vertices[3] = -0; 				 vertices2[2] = 5.844; vertices2[3] = -0.03; 
	 vertices[4] = 5.874; vertices[5] = -1.7845; 				 vertices2[4] = 5.844; vertices2[5] = -1.7545; 
	 vertices[6] = 2.937; vertices[7] = -1.7845; 				 vertices2[6] = 2.967; vertices2[7] = -1.7545; 
	 vertices[8] = 2.937; vertices[9] = 0.8199062992; 				 vertices2[8] = 2.967; vertices2[9] = 0.7899062992; 
	 vertices[10] = 7.4160818352; vertices[11] = 0.8199062992; 				 vertices2[10] = 7.3860818352; vertices2[11] = 0.7899062992; 
	 vertices[12] = 7.4160818352; vertices[13] = -1.7845; 				 vertices2[12] = 7.3860818352; vertices2[13] = -1.7545; 
	 vertices[14] = 2.937; vertices[15] = -1.7845; 				 vertices2[14] = 2.967; vertices2[15] = -1.7545; 
	 TGeoVolume *CrystalWithWrapping_3_1 = gGeoManager->MakeArb8("CrystalWithWrapping_3_1", pWrappingMedium,8, vertices); 
	 CrystalWithWrapping_3_1->SetLineColor(kGreen); 
	 TGeoVolume *Crystal_3_1 = gGeoManager->MakeArb8("Crystal_3_1", pCsIMedium,7.97, vertices2); 
	 Crystal_3_1->SetLineColor(col_barrel_crystal); 

	 vertices[0] = 2.937; vertices[1] = -1.7845; 				 vertices2[0] = 2.967; vertices2[1] = -1.8145; 
	 vertices[2] = 5.874; vertices[3] = -1.7845; 				 vertices2[2] = 5.844; vertices2[3] = -1.8145; 
	 vertices[4] = 5.874; vertices[5] = -3.569; 				 vertices2[4] = 5.844; vertices2[5] = -3.539; 
	 vertices[6] = 2.937; vertices[7] = -3.569; 				 vertices2[6] = 2.967; vertices2[7] = -3.539; 
	 vertices[8] = 2.937; vertices[9] = -1.7845; 				 vertices2[8] = 2.967; vertices2[9] = -1.8145; 
	 vertices[10] = 7.4160818352; vertices[11] = -1.7845; 				 vertices2[10] = 7.3860818352; vertices2[11] = -1.8145; 
	 vertices[12] = 7.4160818352; vertices[13] = -4.3889062992; 				 vertices2[12] = 7.3860818352; vertices2[13] = -4.3589062992; 
	 vertices[14] = 2.937; vertices[15] = -4.3889062992; 				 vertices2[14] = 2.967; vertices2[15] = -4.3589062992; 
	 TGeoVolume *CrystalWithWrapping_3_2 = gGeoManager->MakeArb8("CrystalWithWrapping_3_2", pWrappingMedium,8, vertices); 
	 CrystalWithWrapping_3_2->SetLineColor(kGreen); 
	 TGeoVolume *Crystal_3_2 = gGeoManager->MakeArb8("Crystal_3_2", pCsIMedium,7.97, vertices2); 
	 Crystal_3_2->SetLineColor(col_barrel_crystal); 

	 vertices[0] = 0; vertices[1] = -0; 				 vertices2[0] = 0.03; vertices2[1] = -0.03; 
	 vertices[2] = 2.937; vertices[3] = -0; 				 vertices2[2] = 2.907; vertices2[3] = -0.03; 
	 vertices[4] = 2.937; vertices[5] = -1.7845; 				 vertices2[4] = 2.907; vertices2[5] = -1.7545; 
	 vertices[6] = 0; vertices[7] = -1.7845; 				 vertices2[6] = 0.03; vertices2[7] = -1.7545; 
	 vertices[8] = -1.5420818352; vertices[9] = 0.8199062992; 				 vertices2[8] = -1.5120818352; vertices2[9] = 0.7899062992; 
	 vertices[10] = 2.937; vertices[11] = 0.8199062992; 				 vertices2[10] = 2.907; vertices2[11] = 0.7899062992; 
	 vertices[12] = 2.937; vertices[13] = -1.7845; 				 vertices2[12] = 2.907; vertices2[13] = -1.7545; 
	 vertices[14] = -1.5420818352; vertices[15] = -1.7845; 				 vertices2[14] = -1.5120818352; vertices2[15] = -1.7545; 
	 TGeoVolume *CrystalWithWrapping_3_3 = gGeoManager->MakeArb8("CrystalWithWrapping_3_3", pWrappingMedium,8, vertices); 
	 CrystalWithWrapping_3_3->SetLineColor(kGreen); 
	 TGeoVolume *Crystal_3_3 = gGeoManager->MakeArb8("Crystal_3_3", pCsIMedium,7.97, vertices2); 
	 Crystal_3_3->SetLineColor(col_barrel_crystal); 

	 vertices[0] = 0; vertices[1] = -1.7845; 				 vertices2[0] = 0.03; vertices2[1] = -1.8145; 
	 vertices[2] = 2.937; vertices[3] = -1.7845; 				 vertices2[2] = 2.907; vertices2[3] = -1.8145; 
	 vertices[4] = 2.937; vertices[5] = -3.569; 				 vertices2[4] = 2.907; vertices2[5] = -3.539; 
	 vertices[6] = 0; vertices[7] = -3.569; 				 vertices2[6] = 0.03; vertices2[7] = -3.539; 
	 vertices[8] = -1.5420818352; vertices[9] = -1.7845; 				 vertices2[8] = -1.5120818352; vertices2[9] = -1.8145; 
	 vertices[10] = 2.937; vertices[11] = -1.7845; 				 vertices2[10] = 2.907; vertices2[11] = -1.8145; 
	 vertices[12] = 2.937; vertices[13] = -4.3889062992; 				 vertices2[12] = 2.907; vertices2[13] = -4.3589062992; 
	 vertices[14] = -1.5420818352; vertices[15] = -4.3889062992; 				 vertices2[14] = -1.5120818352; vertices2[15] = -4.3589062992; 
	 TGeoVolume *CrystalWithWrapping_3_4 = gGeoManager->MakeArb8("CrystalWithWrapping_3_4", pWrappingMedium,8, vertices); 
	 CrystalWithWrapping_3_4->SetLineColor(kGreen); 
	 TGeoVolume *Crystal_3_4 = gGeoManager->MakeArb8("Crystal_3_4", pCsIMedium,7.97, vertices2); 
	 Crystal_3_4->SetLineColor(col_barrel_crystal); 

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
	 TGeoVolume *Crystal_4_1 = gGeoManager->MakeArb8("Crystal_4_1", pCsIMedium,8.47, vertices2); 
	 Crystal_4_1->SetLineColor(col_barrel_crystal); 

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
	 TGeoVolume *Crystal_4_2 = gGeoManager->MakeArb8("Crystal_4_2", pCsIMedium,8.47, vertices2); 
	 Crystal_4_2->SetLineColor(col_barrel_crystal); 

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
	 TGeoVolume *Crystal_4_3 = gGeoManager->MakeArb8("Crystal_4_3", pCsIMedium,8.47, vertices2); 
	 Crystal_4_3->SetLineColor(col_barrel_crystal); 

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
	 TGeoVolume *Crystal_4_4 = gGeoManager->MakeArb8("Crystal_4_4", pCsIMedium,8.47, vertices2); 
	 Crystal_4_4->SetLineColor(col_barrel_crystal); 

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
	 TGeoVolume *Crystal_5_1 = gGeoManager->MakeArb8("Crystal_5_1", pCsIMedium,8.97, vertices2); 
	 Crystal_5_1->SetLineColor(col_barrel_crystal); 

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
	 TGeoVolume *Crystal_5_2 = gGeoManager->MakeArb8("Crystal_5_2", pCsIMedium,8.97, vertices2); 
	 Crystal_5_2->SetLineColor(col_barrel_crystal); 

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
	 TGeoVolume *Crystal_5_3 = gGeoManager->MakeArb8("Crystal_5_3", pCsIMedium,8.97, vertices2); 
	 Crystal_5_3->SetLineColor(col_barrel_crystal); 

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
	 TGeoVolume *Crystal_5_4 = gGeoManager->MakeArb8("Crystal_5_4", pCsIMedium,8.97, vertices2); 
	 Crystal_5_4->SetLineColor(col_barrel_crystal); 

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
	 TGeoVolume *Crystal_6_1 = gGeoManager->MakeArb8("Crystal_6_1", pCsIMedium,10.97, vertices2); 
	 Crystal_6_1->SetLineColor(col_barrel_crystal); 

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
	 TGeoVolume *Crystal_6_2 = gGeoManager->MakeArb8("Crystal_6_2", pCsIMedium,10.97, vertices2); 
	 Crystal_6_2->SetLineColor(col_barrel_crystal); 

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
	 TGeoVolume *Crystal_6_3 = gGeoManager->MakeArb8("Crystal_6_3", pCsIMedium,10.97, vertices2); 
	 Crystal_6_3->SetLineColor(col_barrel_crystal); 

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
	 TGeoVolume *Crystal_6_4 = gGeoManager->MakeArb8("Crystal_6_4", pCsIMedium,10.97, vertices2); 
	 Crystal_6_4->SetLineColor(col_barrel_crystal); 

 //Some common geometrical operations 
	 TGeoTranslation* noTrans=new TGeoTranslation("noTrans",0.,0.,0.); 
	 TGeoRotation *rotSymmetric = new TGeoRotation(); //Symmetric crystal 
	 rotSymmetric->RotateZ(180); 
 //The right transformation describing the difference in the coordinate system in CAD and simulation 
	 TGeoRotation *rotDef = new TGeoRotation(); 
	 rotDef->RotateX(-90); 
	 TGeoTranslation* shiftDef=new TGeoTranslation("shiftDef",0.,10.,0.); 
	 TGeoCombiTrans* transDef = new TGeoCombiTrans(*shiftDef,*rotDef); 

	 TGeoRotation *rotDef1 = new TGeoRotation(); 
	 rotDef1->RotateZ(-36.*1); 
	 TGeoCombiTrans* transDefRot1 = new TGeoCombiTrans(*noTrans,*rotDef1); 

	 TGeoRotation *rotDef2 = new TGeoRotation(); 
	 rotDef2->RotateZ(-36.*2); 
	 TGeoCombiTrans* transDefRot2 = new TGeoCombiTrans(*noTrans,*rotDef2); 

	 TGeoRotation *rotDef3 = new TGeoRotation(); 
	 rotDef3->RotateZ(-36.*3); 
	 TGeoCombiTrans* transDefRot3 = new TGeoCombiTrans(*noTrans,*rotDef3); 

	 TGeoRotation *rotDef4 = new TGeoRotation(); 
	 rotDef4->RotateZ(-36.*4); 
	 TGeoCombiTrans* transDefRot4 = new TGeoCombiTrans(*noTrans,*rotDef4); 

	 TGeoRotation *rotDef5 = new TGeoRotation(); 
	 rotDef5->RotateZ(-36.*5); 
	 TGeoCombiTrans* transDefRot5 = new TGeoCombiTrans(*noTrans,*rotDef5); 

	 TGeoRotation *rotDef6 = new TGeoRotation(); 
	 rotDef6->RotateZ(-36.*6); 
	 TGeoCombiTrans* transDefRot6 = new TGeoCombiTrans(*noTrans,*rotDef6); 

	 TGeoRotation *rotDef7 = new TGeoRotation(); 
	 rotDef7->RotateZ(-36.*7); 
	 TGeoCombiTrans* transDefRot7 = new TGeoCombiTrans(*noTrans,*rotDef7); 

	 TGeoRotation *rotDef8 = new TGeoRotation(); 
	 rotDef8->RotateZ(-36.*8); 
	 TGeoCombiTrans* transDefRot8 = new TGeoCombiTrans(*noTrans,*rotDef8); 

	 TGeoRotation *rotDef9 = new TGeoRotation(); 
	 rotDef9->RotateZ(-36.*9); 
	 TGeoCombiTrans* transDefRot9 = new TGeoCombiTrans(*noTrans,*rotDef9); 

	 CrystalWithWrapping_1_1->AddNode(Crystal_1_1,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 //AddSensitiveVolume(Crystal_1_1);  
	 CrystalWithWrapping_2_1->AddNode(Crystal_2_1,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 //AddSensitiveVolume(Crystal_2_1);  
	 CrystalWithWrapping_2_2->AddNode(Crystal_2_2,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 //AddSensitiveVolume(Crystal_2_2);  
	 CrystalWithWrapping_2_3->AddNode(Crystal_2_3,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 //AddSensitiveVolume(Crystal_2_3);  
	 CrystalWithWrapping_2_4->AddNode(Crystal_2_4,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 //AddSensitiveVolume(Crystal_2_4);  
	 CrystalWithWrapping_3_1->AddNode(Crystal_3_1,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 //AddSensitiveVolume(Crystal_3_1);  
	 CrystalWithWrapping_3_2->AddNode(Crystal_3_2,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 //AddSensitiveVolume(Crystal_3_2);  
	 CrystalWithWrapping_3_3->AddNode(Crystal_3_3,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 //AddSensitiveVolume(Crystal_3_3);  
	 CrystalWithWrapping_3_4->AddNode(Crystal_3_4,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 //AddSensitiveVolume(Crystal_3_4);  
	 CrystalWithWrapping_4_1->AddNode(Crystal_4_1,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 //AddSensitiveVolume(Crystal_4_1);  
	 CrystalWithWrapping_4_2->AddNode(Crystal_4_2,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 //AddSensitiveVolume(Crystal_4_2);  
	 CrystalWithWrapping_4_3->AddNode(Crystal_4_3,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 //AddSensitiveVolume(Crystal_4_3);  
	 CrystalWithWrapping_4_4->AddNode(Crystal_4_4,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 //AddSensitiveVolume(Crystal_4_4);  
	 CrystalWithWrapping_5_1->AddNode(Crystal_5_1,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 //AddSensitiveVolume(Crystal_5_1);  
	 CrystalWithWrapping_5_2->AddNode(Crystal_5_2,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 //AddSensitiveVolume(Crystal_5_2);  
	 CrystalWithWrapping_5_3->AddNode(Crystal_5_3,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 //AddSensitiveVolume(Crystal_5_3);  
	 CrystalWithWrapping_5_4->AddNode(Crystal_5_4,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 //AddSensitiveVolume(Crystal_5_4);  
	 CrystalWithWrapping_6_1->AddNode(Crystal_6_1,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 //AddSensitiveVolume(Crystal_6_1);  
	 CrystalWithWrapping_6_2->AddNode(Crystal_6_2,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 //AddSensitiveVolume(Crystal_6_2);  
	 CrystalWithWrapping_6_3->AddNode(Crystal_6_3,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 //AddSensitiveVolume(Crystal_6_3);  
	 CrystalWithWrapping_6_4->AddNode(Crystal_6_4,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 //AddSensitiveVolume(Crystal_6_4);  
 //Crystals inside each alveoli 
	 AlveolusInner_1->AddNode(CrystalWithWrapping_1_1,0,new TGeoCombiTrans(0,0,-3.97,rotUni)); 
	 Alveolus_1->AddNode(AlveolusInner_1,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 AlveolusInner_2->AddNode(CrystalWithWrapping_2_1,0,new TGeoCombiTrans(0.284,-0.0164004289999999,-2.97,rotUni)); 
	 AlveolusInner_2->AddNode(CrystalWithWrapping_2_2,1,new TGeoCombiTrans(0.284,-0.0164004289999999,-2.97,rotUni)); 
	 AlveolusInner_2->AddNode(CrystalWithWrapping_2_3,2,new TGeoCombiTrans(0.284,-0.0164004289999999,-2.97,rotUni)); 
	 AlveolusInner_2->AddNode(CrystalWithWrapping_2_4,3,new TGeoCombiTrans(0.284,-0.0164004289999999,-2.97,rotUni)); 
	 Alveolus_2->AddNode(AlveolusInner_2,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 AlveolusInner_3->AddNode(CrystalWithWrapping_2_1,0,new TGeoCombiTrans(0.239,-0.0174952349999997,-2.97,rotUni)); 
	 AlveolusInner_3->AddNode(CrystalWithWrapping_2_2,1,new TGeoCombiTrans(0.239,-0.0174952349999997,-2.97,rotUni)); 
	 AlveolusInner_3->AddNode(CrystalWithWrapping_2_3,2,new TGeoCombiTrans(0.239,-0.0174952349999997,-2.97,rotUni)); 
	 AlveolusInner_3->AddNode(CrystalWithWrapping_2_4,3,new TGeoCombiTrans(0.239,-0.0174952349999997,-2.97,rotUni)); 
	 Alveolus_3->AddNode(AlveolusInner_3,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 AlveolusInner_4->AddNode(CrystalWithWrapping_2_1,0,new TGeoCombiTrans(0.19225,-0.0184387605,-2.97,rotUni)); 
	 AlveolusInner_4->AddNode(CrystalWithWrapping_2_2,1,new TGeoCombiTrans(0.19225,-0.0184387605,-2.97,rotUni)); 
	 AlveolusInner_4->AddNode(CrystalWithWrapping_2_3,2,new TGeoCombiTrans(0.19225,-0.0184387605,-2.97,rotUni)); 
	 AlveolusInner_4->AddNode(CrystalWithWrapping_2_4,3,new TGeoCombiTrans(0.19225,-0.0184387605,-2.97,rotUni)); 
	 Alveolus_4->AddNode(AlveolusInner_4,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 AlveolusInner_5->AddNode(CrystalWithWrapping_2_1,0,new TGeoCombiTrans(0.1435,-0.0191883909999997,-2.97,rotUni)); 
	 AlveolusInner_5->AddNode(CrystalWithWrapping_2_2,1,new TGeoCombiTrans(0.1435,-0.0191883909999997,-2.97,rotUni)); 
	 AlveolusInner_5->AddNode(CrystalWithWrapping_2_3,2,new TGeoCombiTrans(0.1435,-0.0191883909999997,-2.97,rotUni)); 
	 AlveolusInner_5->AddNode(CrystalWithWrapping_2_4,3,new TGeoCombiTrans(0.1435,-0.0191883909999997,-2.97,rotUni)); 
	 Alveolus_5->AddNode(AlveolusInner_5,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 AlveolusInner_6->AddNode(CrystalWithWrapping_3_1,0,new TGeoCombiTrans(0.0709999999999997,-0.0152862479999998,-1.97,rotUni)); 
	 AlveolusInner_6->AddNode(CrystalWithWrapping_3_2,1,new TGeoCombiTrans(0.0709999999999997,-0.0152862479999998,-1.97,rotUni)); 
	 AlveolusInner_6->AddNode(CrystalWithWrapping_3_3,2,new TGeoCombiTrans(0.0709999999999997,-0.0152862479999998,-1.97,rotUni)); 
	 AlveolusInner_6->AddNode(CrystalWithWrapping_3_4,3,new TGeoCombiTrans(0.0709999999999997,-0.0152862479999998,-1.97,rotUni)); 
	 Alveolus_6->AddNode(AlveolusInner_6,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 AlveolusInner_7->AddNode(CrystalWithWrapping_3_1,0,new TGeoCombiTrans(0.0344999999999995,-0.0154736535,-1.97,rotUni)); 
	 AlveolusInner_7->AddNode(CrystalWithWrapping_3_2,1,new TGeoCombiTrans(0.0344999999999995,-0.0154736535,-1.97,rotUni)); 
	 AlveolusInner_7->AddNode(CrystalWithWrapping_3_3,2,new TGeoCombiTrans(0.0344999999999995,-0.0154736535,-1.97,rotUni)); 
	 AlveolusInner_7->AddNode(CrystalWithWrapping_3_4,3,new TGeoCombiTrans(0.0344999999999995,-0.0154736535,-1.97,rotUni)); 
	 Alveolus_7->AddNode(AlveolusInner_7,1,new TGeoCombiTrans(0,0,0,rotUni)); 
	 AlveolusInner_8->AddNode(CrystalWithWrapping_4_1,0,new TGeoCombiTrans(0.0272499999999996,-0.0149852450000001,-1.47,rotUni)); 
	 AlveolusInner_8->AddNode(CrystalWithWrapping_4_2,1,new TGeoCombiTrans(0.0272499999999996,-0.0149852450000001,-1.47,rotUni)); 
	 AlveolusInner_8->AddNode(CrystalWithWrapping_4_3,2,new TGeoCombiTrans(0.0272499999999996,-0.0149852450000001,-1.47,rotUni)); 
	 AlveolusInner_8->AddNode(CrystalWithWrapping_4_4,3,new TGeoCombiTrans(0.0272499999999996,-0.0149852450000001,-1.47,rotUni)); 
	 Alveolus_8->AddNode(AlveolusInner_8,1,new TGeoCombiTrans(0,0,0,rotUni)); 
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
	 thetaEuler = 180.*TMath::ATan2(-0.6560740922919081600,-0.7546964856307127900)/TMath::Pi(); 
	 psiEuler = 0; 

	 TGeoRotation *rotAlv_1 = new TGeoRotation("rotAlv1",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_1_0 = new TGeoCombiTrans(3.40349999999993,34.5829460840929,-30.9210174523951,rotAlv_1); 
	 *transAlv_1_0 =  (*transAlv_1_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_1,0,transAlv_1_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_1 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_1 =  (*transAlv_1_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,1,transAlv_1_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_2 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_2 =  (*transAlv_1_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,2,transAlv_1_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_3 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_3 =  (*transAlv_1_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,3,transAlv_1_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_4 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_4 =  (*transAlv_1_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,4,transAlv_1_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_5 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_5 =  (*transAlv_1_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,5,transAlv_1_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_6 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_6 =  (*transAlv_1_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,6,transAlv_1_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_7 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_7 =  (*transAlv_1_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,7,transAlv_1_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_8 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_8 =  (*transAlv_1_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,8,transAlv_1_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_9 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_9 =  (*transAlv_1_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,9,transAlv_1_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_10 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_10 =  (*transAlv_1_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,10,transAlv_1_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_11 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_11 =  (*transAlv_1_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,11,transAlv_1_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_12 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_12 =  (*transAlv_1_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,12,transAlv_1_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_13 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_13 =  (*transAlv_1_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,13,transAlv_1_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_14 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_14 =  (*transAlv_1_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,14,transAlv_1_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_15 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_15 =  (*transAlv_1_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,15,transAlv_1_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_16 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_16 =  (*transAlv_1_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,16,transAlv_1_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_17 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_17 =  (*transAlv_1_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,17,transAlv_1_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_18 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_18 =  (*transAlv_1_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,18,transAlv_1_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_19 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_19 =  (*transAlv_1_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,19,transAlv_1_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_20 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_20 =  (*transAlv_1_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,20,transAlv_1_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_21 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_21 =  (*transAlv_1_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,21,transAlv_1_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_22 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_22 =  (*transAlv_1_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,22,transAlv_1_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_23 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_23 =  (*transAlv_1_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,23,transAlv_1_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_24 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_24 =  (*transAlv_1_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,24,transAlv_1_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_25 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_25 =  (*transAlv_1_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,25,transAlv_1_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_26 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_26 =  (*transAlv_1_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,26,transAlv_1_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_27 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_27 =  (*transAlv_1_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,27,transAlv_1_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_28 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_28 =  (*transAlv_1_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,28,transAlv_1_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_29 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_29 =  (*transAlv_1_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,29,transAlv_1_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_30 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_30 =  (*transAlv_1_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,30,transAlv_1_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_1->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_1_31 = new TGeoCombiTrans(3.40349999999993*cos(rotAngle)+34.5829460840929*sin(rotAngle),-3.40349999999993*sin(rotAngle)+34.5829460840929*cos(rotAngle),-30.9210174523951,rotAlv_1); 
	 *transAlv_1_31 =  (*transAlv_1_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_1,31,transAlv_1_31); 

	 phiEuler = 180; 
	 thetaEuler = 180.*TMath::ATan2(-0.5448084181316358300,-0.8385605449416906300)/TMath::Pi(); 
	 psiEuler = 0; 

	 TGeoRotation *rotAlv_2 = new TGeoRotation("rotAlv2",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_2_0 = new TGeoCombiTrans(3.22049999999993,32.7201199375079,-23.3652176279389,rotAlv_2); 
	 *transAlv_2_0 =  (*transAlv_2_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_2,0,transAlv_2_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_1 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_1 =  (*transAlv_2_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,1,transAlv_2_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_2 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_2 =  (*transAlv_2_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,2,transAlv_2_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_3 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_3 =  (*transAlv_2_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,3,transAlv_2_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_4 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_4 =  (*transAlv_2_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,4,transAlv_2_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_5 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_5 =  (*transAlv_2_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,5,transAlv_2_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_6 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_6 =  (*transAlv_2_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,6,transAlv_2_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_7 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_7 =  (*transAlv_2_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,7,transAlv_2_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_8 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_8 =  (*transAlv_2_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,8,transAlv_2_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_9 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_9 =  (*transAlv_2_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,9,transAlv_2_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_10 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_10 =  (*transAlv_2_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,10,transAlv_2_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_11 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_11 =  (*transAlv_2_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,11,transAlv_2_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_12 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_12 =  (*transAlv_2_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,12,transAlv_2_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_13 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_13 =  (*transAlv_2_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,13,transAlv_2_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_14 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_14 =  (*transAlv_2_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,14,transAlv_2_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_15 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_15 =  (*transAlv_2_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,15,transAlv_2_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_16 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_16 =  (*transAlv_2_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,16,transAlv_2_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_17 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_17 =  (*transAlv_2_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,17,transAlv_2_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_18 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_18 =  (*transAlv_2_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,18,transAlv_2_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_19 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_19 =  (*transAlv_2_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,19,transAlv_2_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_20 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_20 =  (*transAlv_2_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,20,transAlv_2_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_21 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_21 =  (*transAlv_2_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,21,transAlv_2_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_22 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_22 =  (*transAlv_2_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,22,transAlv_2_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_23 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_23 =  (*transAlv_2_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,23,transAlv_2_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_24 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_24 =  (*transAlv_2_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,24,transAlv_2_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_25 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_25 =  (*transAlv_2_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,25,transAlv_2_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_26 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_26 =  (*transAlv_2_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,26,transAlv_2_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_27 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_27 =  (*transAlv_2_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,27,transAlv_2_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_28 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_28 =  (*transAlv_2_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,28,transAlv_2_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_29 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_29 =  (*transAlv_2_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,29,transAlv_2_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_30 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_30 =  (*transAlv_2_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,30,transAlv_2_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_2->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_2_31 = new TGeoCombiTrans(3.22049999999993*cos(rotAngle)+32.7201199375079*sin(rotAngle),-3.22049999999993*sin(rotAngle)+32.7201199375079*cos(rotAngle),-23.3652176279389,rotAlv_2); 
	 *transAlv_2_31 =  (*transAlv_2_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_2,31,transAlv_2_31); 

	 phiEuler = 180; 
	 thetaEuler = 180.*TMath::ATan2(-0.4543918745633551100,-0.8908018995999054100)/TMath::Pi(); 
	 psiEuler = 0; 

	 TGeoRotation *rotAlv_3 = new TGeoRotation("rotAlv3",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_3_0 = new TGeoCombiTrans(3.17549999999993,32.2696830832608,-17.323018303327,rotAlv_3); 
	 *transAlv_3_0 =  (*transAlv_3_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_3,0,transAlv_3_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_1 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_1 =  (*transAlv_3_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,1,transAlv_3_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_2 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_2 =  (*transAlv_3_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,2,transAlv_3_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_3 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_3 =  (*transAlv_3_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,3,transAlv_3_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_4 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_4 =  (*transAlv_3_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,4,transAlv_3_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_5 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_5 =  (*transAlv_3_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,5,transAlv_3_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_6 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_6 =  (*transAlv_3_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,6,transAlv_3_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_7 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_7 =  (*transAlv_3_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,7,transAlv_3_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_8 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_8 =  (*transAlv_3_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,8,transAlv_3_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_9 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_9 =  (*transAlv_3_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,9,transAlv_3_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_10 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_10 =  (*transAlv_3_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,10,transAlv_3_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_11 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_11 =  (*transAlv_3_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,11,transAlv_3_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_12 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_12 =  (*transAlv_3_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,12,transAlv_3_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_13 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_13 =  (*transAlv_3_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,13,transAlv_3_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_14 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_14 =  (*transAlv_3_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,14,transAlv_3_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_15 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_15 =  (*transAlv_3_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,15,transAlv_3_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_16 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_16 =  (*transAlv_3_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,16,transAlv_3_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_17 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_17 =  (*transAlv_3_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,17,transAlv_3_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_18 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_18 =  (*transAlv_3_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,18,transAlv_3_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_19 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_19 =  (*transAlv_3_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,19,transAlv_3_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_20 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_20 =  (*transAlv_3_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,20,transAlv_3_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_21 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_21 =  (*transAlv_3_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,21,transAlv_3_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_22 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_22 =  (*transAlv_3_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,22,transAlv_3_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_23 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_23 =  (*transAlv_3_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,23,transAlv_3_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_24 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_24 =  (*transAlv_3_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,24,transAlv_3_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_25 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_25 =  (*transAlv_3_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,25,transAlv_3_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_26 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_26 =  (*transAlv_3_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,26,transAlv_3_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_27 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_27 =  (*transAlv_3_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,27,transAlv_3_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_28 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_28 =  (*transAlv_3_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,28,transAlv_3_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_29 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_29 =  (*transAlv_3_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,29,transAlv_3_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_30 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_30 =  (*transAlv_3_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,30,transAlv_3_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_3->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_3_31 = new TGeoCombiTrans(3.17549999999993*cos(rotAngle)+32.2696830832608*sin(rotAngle),-3.17549999999993*sin(rotAngle)+32.2696830832608*cos(rotAngle),-17.323018303327,rotAlv_3); 
	 *transAlv_3_31 =  (*transAlv_3_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_3,31,transAlv_3_31); 

	 phiEuler = 180; 
	 thetaEuler = 180.*TMath::ATan2(-0.3591065806722531000,-0.9332965572195595100)/TMath::Pi(); 
	 psiEuler = 0; 

	 TGeoRotation *rotAlv_4 = new TGeoRotation("rotAlv4",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_4_0 = new TGeoCombiTrans(3.12874999999993,31.7944116004365,-11.6319526859721,rotAlv_4); 
	 *transAlv_4_0 =  (*transAlv_4_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_4,0,transAlv_4_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_1 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_1 =  (*transAlv_4_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,1,transAlv_4_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_2 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_2 =  (*transAlv_4_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,2,transAlv_4_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_3 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_3 =  (*transAlv_4_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,3,transAlv_4_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_4 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_4 =  (*transAlv_4_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,4,transAlv_4_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_5 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_5 =  (*transAlv_4_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,5,transAlv_4_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_6 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_6 =  (*transAlv_4_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,6,transAlv_4_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_7 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_7 =  (*transAlv_4_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,7,transAlv_4_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_8 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_8 =  (*transAlv_4_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,8,transAlv_4_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_9 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_9 =  (*transAlv_4_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,9,transAlv_4_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_10 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_10 =  (*transAlv_4_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,10,transAlv_4_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_11 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_11 =  (*transAlv_4_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,11,transAlv_4_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_12 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_12 =  (*transAlv_4_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,12,transAlv_4_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_13 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_13 =  (*transAlv_4_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,13,transAlv_4_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_14 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_14 =  (*transAlv_4_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,14,transAlv_4_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_15 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_15 =  (*transAlv_4_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,15,transAlv_4_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_16 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_16 =  (*transAlv_4_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,16,transAlv_4_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_17 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_17 =  (*transAlv_4_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,17,transAlv_4_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_18 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_18 =  (*transAlv_4_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,18,transAlv_4_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_19 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_19 =  (*transAlv_4_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,19,transAlv_4_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_20 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_20 =  (*transAlv_4_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,20,transAlv_4_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_21 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_21 =  (*transAlv_4_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,21,transAlv_4_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_22 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_22 =  (*transAlv_4_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,22,transAlv_4_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_23 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_23 =  (*transAlv_4_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,23,transAlv_4_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_24 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_24 =  (*transAlv_4_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,24,transAlv_4_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_25 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_25 =  (*transAlv_4_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,25,transAlv_4_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_26 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_26 =  (*transAlv_4_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,26,transAlv_4_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_27 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_27 =  (*transAlv_4_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,27,transAlv_4_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_28 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_28 =  (*transAlv_4_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,28,transAlv_4_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_29 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_29 =  (*transAlv_4_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,29,transAlv_4_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_30 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_30 =  (*transAlv_4_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,30,transAlv_4_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_4->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_4_31 = new TGeoCombiTrans(3.12874999999993*cos(rotAngle)+31.7944116004365*sin(rotAngle),-3.12874999999993*sin(rotAngle)+31.7944116004365*cos(rotAngle),-11.6319526859721,rotAlv_4); 
	 *transAlv_4_31 =  (*transAlv_4_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_4,31,transAlv_4_31); 

	 phiEuler = 180; 
	 thetaEuler = 180.*TMath::ATan2(-0.2599860908684709600,-0.9656123614344068700)/TMath::Pi(); 
	 psiEuler = 0; 

	 TGeoRotation *rotAlv_5 = new TGeoRotation("rotAlv5",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_5_0 = new TGeoCombiTrans(3.07999999999993,31.2995084401373,-6.20952340260598,rotAlv_5); 
	 *transAlv_5_0 =  (*transAlv_5_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_5,0,transAlv_5_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_1 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_1 =  (*transAlv_5_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,1,transAlv_5_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_2 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_2 =  (*transAlv_5_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,2,transAlv_5_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_3 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_3 =  (*transAlv_5_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,3,transAlv_5_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_4 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_4 =  (*transAlv_5_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,4,transAlv_5_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_5 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_5 =  (*transAlv_5_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,5,transAlv_5_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_6 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_6 =  (*transAlv_5_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,6,transAlv_5_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_7 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_7 =  (*transAlv_5_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,7,transAlv_5_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_8 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_8 =  (*transAlv_5_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,8,transAlv_5_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_9 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_9 =  (*transAlv_5_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,9,transAlv_5_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_10 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_10 =  (*transAlv_5_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,10,transAlv_5_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_11 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_11 =  (*transAlv_5_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,11,transAlv_5_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_12 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_12 =  (*transAlv_5_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,12,transAlv_5_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_13 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_13 =  (*transAlv_5_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,13,transAlv_5_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_14 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_14 =  (*transAlv_5_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,14,transAlv_5_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_15 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_15 =  (*transAlv_5_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,15,transAlv_5_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_16 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_16 =  (*transAlv_5_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,16,transAlv_5_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_17 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_17 =  (*transAlv_5_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,17,transAlv_5_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_18 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_18 =  (*transAlv_5_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,18,transAlv_5_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_19 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_19 =  (*transAlv_5_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,19,transAlv_5_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_20 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_20 =  (*transAlv_5_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,20,transAlv_5_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_21 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_21 =  (*transAlv_5_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,21,transAlv_5_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_22 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_22 =  (*transAlv_5_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,22,transAlv_5_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_23 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_23 =  (*transAlv_5_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,23,transAlv_5_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_24 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_24 =  (*transAlv_5_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,24,transAlv_5_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_25 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_25 =  (*transAlv_5_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,25,transAlv_5_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_26 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_26 =  (*transAlv_5_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,26,transAlv_5_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_27 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_27 =  (*transAlv_5_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,27,transAlv_5_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_28 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_28 =  (*transAlv_5_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,28,transAlv_5_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_29 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_29 =  (*transAlv_5_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,29,transAlv_5_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_30 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_30 =  (*transAlv_5_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,30,transAlv_5_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_5->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_5_31 = new TGeoCombiTrans(3.07999999999993*cos(rotAngle)+31.2995084401373*sin(rotAngle),-3.07999999999993*sin(rotAngle)+31.2995084401373*cos(rotAngle),-6.20952340260598,rotAlv_5); 
	 *transAlv_5_31 =  (*transAlv_5_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_5,31,transAlv_5_31); 

	 phiEuler = 180; 
	 thetaEuler = 180.*TMath::ATan2(-0.1580858802177538500,-0.9874253665344933100)/TMath::Pi(); 
	 psiEuler = 0; 

	 TGeoRotation *rotAlv_6 = new TGeoRotation("rotAlv6",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_6_0 = new TGeoCombiTrans(3.00799999999993,30.5690415864581,-2.36924983179562,rotAlv_6); 
	 *transAlv_6_0 =  (*transAlv_6_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_6,0,transAlv_6_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_1 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_1 =  (*transAlv_6_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,1,transAlv_6_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_2 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_2 =  (*transAlv_6_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,2,transAlv_6_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_3 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_3 =  (*transAlv_6_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,3,transAlv_6_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_4 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_4 =  (*transAlv_6_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,4,transAlv_6_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_5 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_5 =  (*transAlv_6_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,5,transAlv_6_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_6 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_6 =  (*transAlv_6_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,6,transAlv_6_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_7 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_7 =  (*transAlv_6_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,7,transAlv_6_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_8 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_8 =  (*transAlv_6_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,8,transAlv_6_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_9 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_9 =  (*transAlv_6_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,9,transAlv_6_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_10 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_10 =  (*transAlv_6_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,10,transAlv_6_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_11 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_11 =  (*transAlv_6_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,11,transAlv_6_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_12 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_12 =  (*transAlv_6_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,12,transAlv_6_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_13 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_13 =  (*transAlv_6_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,13,transAlv_6_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_14 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_14 =  (*transAlv_6_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,14,transAlv_6_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_15 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_15 =  (*transAlv_6_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,15,transAlv_6_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_16 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_16 =  (*transAlv_6_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,16,transAlv_6_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_17 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_17 =  (*transAlv_6_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,17,transAlv_6_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_18 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_18 =  (*transAlv_6_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,18,transAlv_6_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_19 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_19 =  (*transAlv_6_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,19,transAlv_6_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_20 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_20 =  (*transAlv_6_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,20,transAlv_6_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_21 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_21 =  (*transAlv_6_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,21,transAlv_6_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_22 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_22 =  (*transAlv_6_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,22,transAlv_6_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_23 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_23 =  (*transAlv_6_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,23,transAlv_6_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_24 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_24 =  (*transAlv_6_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,24,transAlv_6_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_25 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_25 =  (*transAlv_6_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,25,transAlv_6_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_26 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_26 =  (*transAlv_6_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,26,transAlv_6_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_27 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_27 =  (*transAlv_6_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,27,transAlv_6_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_28 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_28 =  (*transAlv_6_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,28,transAlv_6_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_29 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_29 =  (*transAlv_6_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,29,transAlv_6_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_30 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_30 =  (*transAlv_6_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,30,transAlv_6_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_6->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_6_31 = new TGeoCombiTrans(3.00799999999993*cos(rotAngle)+30.5690415864581*sin(rotAngle),-3.00799999999993*sin(rotAngle)+30.5690415864581*cos(rotAngle),-2.36924983179562,rotAlv_6); 
	 *transAlv_6_31 =  (*transAlv_6_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_6,31,transAlv_6_31); 

	 phiEuler = 180; 
	 thetaEuler = 180.*TMath::ATan2(-0.05447746084201386000,-0.9985150005183732700)/TMath::Pi(); 
	 psiEuler = 0; 

	 TGeoRotation *rotAlv_7 = new TGeoRotation("rotAlv7",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_7_0 = new TGeoCombiTrans(2.97149999999993,30.1961159884726,1.29187363150737,rotAlv_7); 
	 *transAlv_7_0 =  (*transAlv_7_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_7,0,transAlv_7_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_1 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_1 =  (*transAlv_7_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,1,transAlv_7_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_2 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_2 =  (*transAlv_7_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,2,transAlv_7_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_3 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_3 =  (*transAlv_7_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,3,transAlv_7_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_4 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_4 =  (*transAlv_7_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,4,transAlv_7_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_5 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_5 =  (*transAlv_7_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,5,transAlv_7_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_6 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_6 =  (*transAlv_7_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,6,transAlv_7_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_7 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_7 =  (*transAlv_7_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,7,transAlv_7_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_8 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_8 =  (*transAlv_7_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,8,transAlv_7_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_9 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_9 =  (*transAlv_7_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,9,transAlv_7_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_10 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_10 =  (*transAlv_7_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,10,transAlv_7_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_11 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_11 =  (*transAlv_7_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,11,transAlv_7_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_12 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_12 =  (*transAlv_7_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,12,transAlv_7_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_13 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_13 =  (*transAlv_7_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,13,transAlv_7_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_14 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_14 =  (*transAlv_7_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,14,transAlv_7_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_15 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_15 =  (*transAlv_7_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,15,transAlv_7_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_16 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_16 =  (*transAlv_7_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,16,transAlv_7_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_17 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_17 =  (*transAlv_7_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,17,transAlv_7_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_18 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_18 =  (*transAlv_7_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,18,transAlv_7_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_19 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_19 =  (*transAlv_7_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,19,transAlv_7_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_20 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_20 =  (*transAlv_7_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,20,transAlv_7_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_21 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_21 =  (*transAlv_7_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,21,transAlv_7_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_22 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_22 =  (*transAlv_7_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,22,transAlv_7_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_23 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_23 =  (*transAlv_7_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,23,transAlv_7_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_24 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_24 =  (*transAlv_7_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,24,transAlv_7_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_25 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_25 =  (*transAlv_7_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,25,transAlv_7_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_26 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_26 =  (*transAlv_7_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,26,transAlv_7_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_27 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_27 =  (*transAlv_7_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,27,transAlv_7_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_28 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_28 =  (*transAlv_7_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,28,transAlv_7_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_29 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_29 =  (*transAlv_7_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,29,transAlv_7_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_30 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_30 =  (*transAlv_7_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,30,transAlv_7_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_7->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_7_31 = new TGeoCombiTrans(2.97149999999993*cos(rotAngle)+30.1961159884726*sin(rotAngle),-2.97149999999993*sin(rotAngle)+30.1961159884726*cos(rotAngle),1.29187363150737,rotAlv_7); 
	 *transAlv_7_31 =  (*transAlv_7_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_7,31,transAlv_7_31); 

	 phiEuler = 180; 
	 thetaEuler = 180.*TMath::ATan2(-0.04097943424329152800,0.9991599901762978600)/TMath::Pi(); 
	 psiEuler = -180; 

	 TGeoRotation *rotAlv_8 = new TGeoRotation("rotAlv8",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_8_0 = new TGeoCombiTrans(-2.95249999999993,30,1.29734899998037,rotAlv_8); 
	 *transAlv_8_0 =  (*transAlv_8_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_8,0,transAlv_8_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_1 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_1 =  (*transAlv_8_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,1,transAlv_8_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_2 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_2 =  (*transAlv_8_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,2,transAlv_8_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_3 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_3 =  (*transAlv_8_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,3,transAlv_8_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_4 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_4 =  (*transAlv_8_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,4,transAlv_8_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_5 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_5 =  (*transAlv_8_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,5,transAlv_8_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_6 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_6 =  (*transAlv_8_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,6,transAlv_8_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_7 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_7 =  (*transAlv_8_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,7,transAlv_8_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_8 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_8 =  (*transAlv_8_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,8,transAlv_8_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_9 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_9 =  (*transAlv_8_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,9,transAlv_8_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_10 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_10 =  (*transAlv_8_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,10,transAlv_8_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_11 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_11 =  (*transAlv_8_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,11,transAlv_8_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_12 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_12 =  (*transAlv_8_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,12,transAlv_8_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_13 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_13 =  (*transAlv_8_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,13,transAlv_8_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_14 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_14 =  (*transAlv_8_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,14,transAlv_8_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_15 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_15 =  (*transAlv_8_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,15,transAlv_8_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_16 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_16 =  (*transAlv_8_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,16,transAlv_8_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_17 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_17 =  (*transAlv_8_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,17,transAlv_8_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_18 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_18 =  (*transAlv_8_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,18,transAlv_8_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_19 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_19 =  (*transAlv_8_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,19,transAlv_8_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_20 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_20 =  (*transAlv_8_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,20,transAlv_8_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_21 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_21 =  (*transAlv_8_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,21,transAlv_8_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_22 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_22 =  (*transAlv_8_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,22,transAlv_8_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_23 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_23 =  (*transAlv_8_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,23,transAlv_8_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_24 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_24 =  (*transAlv_8_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,24,transAlv_8_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_25 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_25 =  (*transAlv_8_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,25,transAlv_8_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_26 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_26 =  (*transAlv_8_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,26,transAlv_8_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_27 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_27 =  (*transAlv_8_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,27,transAlv_8_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_28 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_28 =  (*transAlv_8_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,28,transAlv_8_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_29 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_29 =  (*transAlv_8_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,29,transAlv_8_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_30 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_30 =  (*transAlv_8_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,30,transAlv_8_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_8->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_8_31 = new TGeoCombiTrans(-2.95249999999993*cos(rotAngle)+30*sin(rotAngle),2.95249999999993*sin(rotAngle)+30*cos(rotAngle),1.29734899998037,rotAlv_8); 
	 *transAlv_8_31 =  (*transAlv_8_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_8,31,transAlv_8_31); 

	 phiEuler = 180; 
	 thetaEuler = 180.*TMath::ATan2(-0.1275567287250409800,0.9918312764563167500)/TMath::Pi(); 
	 psiEuler = -180; 

	 TGeoRotation *rotAlv_9 = new TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_9_0 = new TGeoCombiTrans(-2.98999999999993,30.3953961794098,4.44438599416416,rotAlv_9); 
	 *transAlv_9_0 =  (*transAlv_9_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_9,0,transAlv_9_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_1 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_1 =  (*transAlv_9_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,1,transAlv_9_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_2 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_2 =  (*transAlv_9_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,2,transAlv_9_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_3 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_3 =  (*transAlv_9_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,3,transAlv_9_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_4 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_4 =  (*transAlv_9_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,4,transAlv_9_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_5 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_5 =  (*transAlv_9_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,5,transAlv_9_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_6 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_6 =  (*transAlv_9_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,6,transAlv_9_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_7 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_7 =  (*transAlv_9_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,7,transAlv_9_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_8 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_8 =  (*transAlv_9_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,8,transAlv_9_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_9 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_9 =  (*transAlv_9_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,9,transAlv_9_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_10 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_10 =  (*transAlv_9_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,10,transAlv_9_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_11 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_11 =  (*transAlv_9_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,11,transAlv_9_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_12 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_12 =  (*transAlv_9_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,12,transAlv_9_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_13 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_13 =  (*transAlv_9_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,13,transAlv_9_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_14 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_14 =  (*transAlv_9_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,14,transAlv_9_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_15 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_15 =  (*transAlv_9_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,15,transAlv_9_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_16 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_16 =  (*transAlv_9_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,16,transAlv_9_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_17 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_17 =  (*transAlv_9_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,17,transAlv_9_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_18 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_18 =  (*transAlv_9_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,18,transAlv_9_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_19 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_19 =  (*transAlv_9_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,19,transAlv_9_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_20 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_20 =  (*transAlv_9_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,20,transAlv_9_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_21 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_21 =  (*transAlv_9_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,21,transAlv_9_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_22 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_22 =  (*transAlv_9_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,22,transAlv_9_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_23 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_23 =  (*transAlv_9_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,23,transAlv_9_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_24 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_24 =  (*transAlv_9_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,24,transAlv_9_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_25 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_25 =  (*transAlv_9_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,25,transAlv_9_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_26 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_26 =  (*transAlv_9_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,26,transAlv_9_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_27 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_27 =  (*transAlv_9_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,27,transAlv_9_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_28 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_28 =  (*transAlv_9_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,28,transAlv_9_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_29 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_29 =  (*transAlv_9_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,29,transAlv_9_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_30 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_30 =  (*transAlv_9_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,30,transAlv_9_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_9->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_9_31 = new TGeoCombiTrans(-2.98999999999993*cos(rotAngle)+30.3953961794098*sin(rotAngle),2.98999999999993*sin(rotAngle)+30.3953961794098*cos(rotAngle),4.44438599416416,rotAlv_9); 
	 *transAlv_9_31 =  (*transAlv_9_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_9,31,transAlv_9_31); 

	 phiEuler = 180; 
	 thetaEuler = 180.*TMath::ATan2(-0.2131210329617397300,0.9770258058563863200)/TMath::Pi(); 
	 psiEuler = -180; 

	 TGeoRotation *rotAlv_10 = new TGeoRotation("rotAlv10",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_10_0 = new TGeoCombiTrans(-3.01649999999993,30.6605329493663,7.63929090825073,rotAlv_10); 
	 *transAlv_10_0 =  (*transAlv_10_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_10,0,transAlv_10_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_1 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_1 =  (*transAlv_10_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,1,transAlv_10_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_2 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_2 =  (*transAlv_10_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,2,transAlv_10_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_3 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_3 =  (*transAlv_10_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,3,transAlv_10_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_4 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_4 =  (*transAlv_10_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,4,transAlv_10_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_5 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_5 =  (*transAlv_10_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,5,transAlv_10_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_6 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_6 =  (*transAlv_10_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,6,transAlv_10_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_7 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_7 =  (*transAlv_10_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,7,transAlv_10_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_8 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_8 =  (*transAlv_10_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,8,transAlv_10_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_9 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_9 =  (*transAlv_10_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,9,transAlv_10_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_10 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_10 =  (*transAlv_10_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,10,transAlv_10_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_11 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_11 =  (*transAlv_10_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,11,transAlv_10_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_12 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_12 =  (*transAlv_10_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,12,transAlv_10_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_13 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_13 =  (*transAlv_10_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,13,transAlv_10_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_14 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_14 =  (*transAlv_10_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,14,transAlv_10_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_15 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_15 =  (*transAlv_10_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,15,transAlv_10_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_16 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_16 =  (*transAlv_10_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,16,transAlv_10_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_17 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_17 =  (*transAlv_10_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,17,transAlv_10_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_18 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_18 =  (*transAlv_10_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,18,transAlv_10_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_19 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_19 =  (*transAlv_10_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,19,transAlv_10_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_20 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_20 =  (*transAlv_10_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,20,transAlv_10_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_21 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_21 =  (*transAlv_10_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,21,transAlv_10_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_22 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_22 =  (*transAlv_10_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,22,transAlv_10_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_23 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_23 =  (*transAlv_10_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,23,transAlv_10_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_24 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_24 =  (*transAlv_10_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,24,transAlv_10_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_25 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_25 =  (*transAlv_10_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,25,transAlv_10_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_26 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_26 =  (*transAlv_10_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,26,transAlv_10_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_27 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_27 =  (*transAlv_10_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,27,transAlv_10_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_28 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_28 =  (*transAlv_10_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,28,transAlv_10_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_29 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_29 =  (*transAlv_10_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,29,transAlv_10_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_30 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_30 =  (*transAlv_10_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,30,transAlv_10_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_10->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_10_31 = new TGeoCombiTrans(-3.01649999999993*cos(rotAngle)+30.6605329493663*sin(rotAngle),3.01649999999993*sin(rotAngle)+30.6605329493663*cos(rotAngle),7.63929090825073,rotAlv_10); 
	 *transAlv_10_31 =  (*transAlv_10_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_10,31,transAlv_10_31); 

	 phiEuler = 180; 
	 thetaEuler = 180.*TMath::ATan2(-0.2971184379757390000,0.9548406326790127600)/TMath::Pi(); 
	 psiEuler = -180; 

	 TGeoRotation *rotAlv_11 = new TGeoRotation("rotAlv11",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_11_0 = new TGeoCombiTrans(-3.04199999999993,30.9206806111464,10.9180918399028,rotAlv_11); 
	 *transAlv_11_0 =  (*transAlv_11_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_11,0,transAlv_11_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_1 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_1 =  (*transAlv_11_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,1,transAlv_11_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_2 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_2 =  (*transAlv_11_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,2,transAlv_11_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_3 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_3 =  (*transAlv_11_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,3,transAlv_11_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_4 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_4 =  (*transAlv_11_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,4,transAlv_11_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_5 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_5 =  (*transAlv_11_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,5,transAlv_11_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_6 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_6 =  (*transAlv_11_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,6,transAlv_11_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_7 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_7 =  (*transAlv_11_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,7,transAlv_11_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_8 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_8 =  (*transAlv_11_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,8,transAlv_11_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_9 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_9 =  (*transAlv_11_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,9,transAlv_11_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_10 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_10 =  (*transAlv_11_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,10,transAlv_11_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_11 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_11 =  (*transAlv_11_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,11,transAlv_11_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_12 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_12 =  (*transAlv_11_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,12,transAlv_11_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_13 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_13 =  (*transAlv_11_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,13,transAlv_11_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_14 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_14 =  (*transAlv_11_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,14,transAlv_11_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_15 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_15 =  (*transAlv_11_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,15,transAlv_11_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_16 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_16 =  (*transAlv_11_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,16,transAlv_11_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_17 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_17 =  (*transAlv_11_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,17,transAlv_11_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_18 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_18 =  (*transAlv_11_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,18,transAlv_11_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_19 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_19 =  (*transAlv_11_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,19,transAlv_11_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_20 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_20 =  (*transAlv_11_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,20,transAlv_11_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_21 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_21 =  (*transAlv_11_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,21,transAlv_11_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_22 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_22 =  (*transAlv_11_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,22,transAlv_11_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_23 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_23 =  (*transAlv_11_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,23,transAlv_11_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_24 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_24 =  (*transAlv_11_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,24,transAlv_11_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_25 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_25 =  (*transAlv_11_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,25,transAlv_11_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_26 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_26 =  (*transAlv_11_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,26,transAlv_11_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_27 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_27 =  (*transAlv_11_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,27,transAlv_11_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_28 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_28 =  (*transAlv_11_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,28,transAlv_11_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_29 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_29 =  (*transAlv_11_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,29,transAlv_11_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_30 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_30 =  (*transAlv_11_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,30,transAlv_11_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_11->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_11_31 = new TGeoCombiTrans(-3.04199999999993*cos(rotAngle)+30.9206806111464*sin(rotAngle),3.04199999999993*sin(rotAngle)+30.9206806111464*cos(rotAngle),10.9180918399028,rotAlv_11); 
	 *transAlv_11_31 =  (*transAlv_11_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_11,31,transAlv_11_31); 

	 phiEuler = 180; 
	 thetaEuler = 180.*TMath::ATan2(-0.3788871076768108300,0.9254428991765514300)/TMath::Pi(); 
	 psiEuler = -180; 

	 TGeoRotation *rotAlv_12 = new TGeoRotation("rotAlv12",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_12_0 = new TGeoCombiTrans(-3.06749999999993,31.1737346881315,14.3082258908461,rotAlv_12); 
	 *transAlv_12_0 =  (*transAlv_12_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_12,0,transAlv_12_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_1 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_1 =  (*transAlv_12_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,1,transAlv_12_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_2 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_2 =  (*transAlv_12_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,2,transAlv_12_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_3 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_3 =  (*transAlv_12_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,3,transAlv_12_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_4 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_4 =  (*transAlv_12_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,4,transAlv_12_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_5 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_5 =  (*transAlv_12_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,5,transAlv_12_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_6 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_6 =  (*transAlv_12_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,6,transAlv_12_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_7 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_7 =  (*transAlv_12_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,7,transAlv_12_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_8 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_8 =  (*transAlv_12_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,8,transAlv_12_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_9 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_9 =  (*transAlv_12_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,9,transAlv_12_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_10 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_10 =  (*transAlv_12_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,10,transAlv_12_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_11 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_11 =  (*transAlv_12_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,11,transAlv_12_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_12 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_12 =  (*transAlv_12_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,12,transAlv_12_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_13 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_13 =  (*transAlv_12_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,13,transAlv_12_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_14 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_14 =  (*transAlv_12_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,14,transAlv_12_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_15 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_15 =  (*transAlv_12_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,15,transAlv_12_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_16 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_16 =  (*transAlv_12_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,16,transAlv_12_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_17 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_17 =  (*transAlv_12_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,17,transAlv_12_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_18 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_18 =  (*transAlv_12_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,18,transAlv_12_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_19 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_19 =  (*transAlv_12_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,19,transAlv_12_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_20 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_20 =  (*transAlv_12_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,20,transAlv_12_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_21 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_21 =  (*transAlv_12_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,21,transAlv_12_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_22 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_22 =  (*transAlv_12_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,22,transAlv_12_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_23 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_23 =  (*transAlv_12_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,23,transAlv_12_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_24 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_24 =  (*transAlv_12_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,24,transAlv_12_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_25 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_25 =  (*transAlv_12_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,25,transAlv_12_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_26 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_26 =  (*transAlv_12_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,26,transAlv_12_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_27 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_27 =  (*transAlv_12_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,27,transAlv_12_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_28 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_28 =  (*transAlv_12_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,28,transAlv_12_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_29 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_29 =  (*transAlv_12_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,29,transAlv_12_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_30 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_30 =  (*transAlv_12_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,30,transAlv_12_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_12->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_12_31 = new TGeoCombiTrans(-3.06749999999993*cos(rotAngle)+31.1737346881315*sin(rotAngle),3.06749999999993*sin(rotAngle)+31.1737346881315*cos(rotAngle),14.3082258908461,rotAlv_12); 
	 *transAlv_12_31 =  (*transAlv_12_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_12,31,transAlv_12_31); 

	 phiEuler = 180; 
	 thetaEuler = 180.*TMath::ATan2(-0.4578676224535794800,0.8890203823921623000)/TMath::Pi(); 
	 psiEuler = -180; 

	 TGeoRotation *rotAlv_13 = new TGeoRotation("rotAlv13",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_13_0 = new TGeoCombiTrans(-3.09099999999993,31.417956426523,17.8408437262792,rotAlv_13); 
	 *transAlv_13_0 =  (*transAlv_13_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_13,0,transAlv_13_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_1 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_1 =  (*transAlv_13_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,1,transAlv_13_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_2 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_2 =  (*transAlv_13_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,2,transAlv_13_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_3 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_3 =  (*transAlv_13_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,3,transAlv_13_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_4 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_4 =  (*transAlv_13_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,4,transAlv_13_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_5 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_5 =  (*transAlv_13_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,5,transAlv_13_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_6 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_6 =  (*transAlv_13_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,6,transAlv_13_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_7 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_7 =  (*transAlv_13_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,7,transAlv_13_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_8 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_8 =  (*transAlv_13_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,8,transAlv_13_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_9 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_9 =  (*transAlv_13_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,9,transAlv_13_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_10 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_10 =  (*transAlv_13_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,10,transAlv_13_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_11 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_11 =  (*transAlv_13_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,11,transAlv_13_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_12 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_12 =  (*transAlv_13_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,12,transAlv_13_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_13 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_13 =  (*transAlv_13_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,13,transAlv_13_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_14 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_14 =  (*transAlv_13_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,14,transAlv_13_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_15 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_15 =  (*transAlv_13_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,15,transAlv_13_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_16 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_16 =  (*transAlv_13_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,16,transAlv_13_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_17 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_17 =  (*transAlv_13_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,17,transAlv_13_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_18 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_18 =  (*transAlv_13_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,18,transAlv_13_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_19 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_19 =  (*transAlv_13_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,19,transAlv_13_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_20 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_20 =  (*transAlv_13_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,20,transAlv_13_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_21 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_21 =  (*transAlv_13_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,21,transAlv_13_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_22 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_22 =  (*transAlv_13_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,22,transAlv_13_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_23 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_23 =  (*transAlv_13_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,23,transAlv_13_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_24 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_24 =  (*transAlv_13_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,24,transAlv_13_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_25 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_25 =  (*transAlv_13_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,25,transAlv_13_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_26 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_26 =  (*transAlv_13_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,26,transAlv_13_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_27 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_27 =  (*transAlv_13_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,27,transAlv_13_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_28 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_28 =  (*transAlv_13_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,28,transAlv_13_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_29 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_29 =  (*transAlv_13_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,29,transAlv_13_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_30 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_30 =  (*transAlv_13_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,30,transAlv_13_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_13->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_13_31 = new TGeoCombiTrans(-3.09099999999993*cos(rotAngle)+31.417956426523*sin(rotAngle),3.09099999999993*sin(rotAngle)+31.417956426523*cos(rotAngle),17.8408437262792,rotAlv_13); 
	 *transAlv_13_31 =  (*transAlv_13_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_13,31,transAlv_13_31); 

	 phiEuler = 180; 
	 thetaEuler = 180.*TMath::ATan2(-0.5334425026911959700,0.8458363295121305000)/TMath::Pi(); 
	 psiEuler = -180; 

	 TGeoRotation *rotAlv_14 = new TGeoRotation("rotAlv14",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_14_0 = new TGeoCombiTrans(-3.11449999999993,33.343093197755,22.6194116972268,rotAlv_14); 
	 *transAlv_14_0 =  (*transAlv_14_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_14,0,transAlv_14_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_1 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_1 =  (*transAlv_14_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,1,transAlv_14_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_2 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_2 =  (*transAlv_14_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,2,transAlv_14_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_3 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_3 =  (*transAlv_14_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,3,transAlv_14_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_4 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_4 =  (*transAlv_14_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,4,transAlv_14_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_5 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_5 =  (*transAlv_14_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,5,transAlv_14_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_6 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_6 =  (*transAlv_14_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,6,transAlv_14_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_7 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_7 =  (*transAlv_14_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,7,transAlv_14_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_8 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_8 =  (*transAlv_14_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,8,transAlv_14_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_9 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_9 =  (*transAlv_14_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,9,transAlv_14_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_10 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_10 =  (*transAlv_14_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,10,transAlv_14_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_11 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_11 =  (*transAlv_14_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,11,transAlv_14_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_12 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_12 =  (*transAlv_14_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,12,transAlv_14_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_13 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_13 =  (*transAlv_14_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,13,transAlv_14_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_14 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_14 =  (*transAlv_14_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,14,transAlv_14_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_15 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_15 =  (*transAlv_14_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,15,transAlv_14_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_16 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_16 =  (*transAlv_14_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,16,transAlv_14_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_17 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_17 =  (*transAlv_14_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,17,transAlv_14_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_18 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_18 =  (*transAlv_14_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,18,transAlv_14_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_19 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_19 =  (*transAlv_14_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,19,transAlv_14_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_20 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_20 =  (*transAlv_14_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,20,transAlv_14_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_21 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_21 =  (*transAlv_14_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,21,transAlv_14_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_22 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_22 =  (*transAlv_14_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,22,transAlv_14_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_23 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_23 =  (*transAlv_14_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,23,transAlv_14_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_24 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_24 =  (*transAlv_14_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,24,transAlv_14_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_25 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_25 =  (*transAlv_14_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,25,transAlv_14_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_26 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_26 =  (*transAlv_14_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,26,transAlv_14_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_27 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_27 =  (*transAlv_14_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,27,transAlv_14_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_28 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_28 =  (*transAlv_14_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,28,transAlv_14_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_29 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_29 =  (*transAlv_14_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,29,transAlv_14_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_30 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_30 =  (*transAlv_14_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,30,transAlv_14_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_14->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_14_31 = new TGeoCombiTrans(-3.11449999999993*cos(rotAngle)+33.343093197755*sin(rotAngle),3.11449999999993*sin(rotAngle)+33.343093197755*cos(rotAngle),22.6194116972268,rotAlv_14); 
	 *transAlv_14_31 =  (*transAlv_14_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_14,31,transAlv_14_31); 

	 phiEuler = 180; 
	 thetaEuler = 180.*TMath::ATan2(-0.6050038521676790500,0.7962225435531633500)/TMath::Pi(); 
	 psiEuler = -180; 

	 TGeoRotation *rotAlv_15 = new TGeoRotation("rotAlv15",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_15_0 = new TGeoCombiTrans(-3.13549999999993,33.4646558050777,26.6976862352101,rotAlv_15); 
	 *transAlv_15_0 =  (*transAlv_15_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_15,0,transAlv_15_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_1 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_1 =  (*transAlv_15_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,1,transAlv_15_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_2 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_2 =  (*transAlv_15_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,2,transAlv_15_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_3 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_3 =  (*transAlv_15_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,3,transAlv_15_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_4 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_4 =  (*transAlv_15_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,4,transAlv_15_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_5 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_5 =  (*transAlv_15_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,5,transAlv_15_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_6 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_6 =  (*transAlv_15_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,6,transAlv_15_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_7 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_7 =  (*transAlv_15_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,7,transAlv_15_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_8 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_8 =  (*transAlv_15_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,8,transAlv_15_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_9 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_9 =  (*transAlv_15_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,9,transAlv_15_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_10 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_10 =  (*transAlv_15_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,10,transAlv_15_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_11 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_11 =  (*transAlv_15_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,11,transAlv_15_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_12 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_12 =  (*transAlv_15_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,12,transAlv_15_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_13 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_13 =  (*transAlv_15_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,13,transAlv_15_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_14 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_14 =  (*transAlv_15_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,14,transAlv_15_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_15 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_15 =  (*transAlv_15_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,15,transAlv_15_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_16 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_16 =  (*transAlv_15_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,16,transAlv_15_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_17 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_17 =  (*transAlv_15_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,17,transAlv_15_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_18 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_18 =  (*transAlv_15_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,18,transAlv_15_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_19 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_19 =  (*transAlv_15_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,19,transAlv_15_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_20 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_20 =  (*transAlv_15_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,20,transAlv_15_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_21 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_21 =  (*transAlv_15_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,21,transAlv_15_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_22 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_22 =  (*transAlv_15_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,22,transAlv_15_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_23 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_23 =  (*transAlv_15_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,23,transAlv_15_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_24 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_24 =  (*transAlv_15_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,24,transAlv_15_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_25 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_25 =  (*transAlv_15_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,25,transAlv_15_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_26 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_26 =  (*transAlv_15_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,26,transAlv_15_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_27 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_27 =  (*transAlv_15_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,27,transAlv_15_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_28 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_28 =  (*transAlv_15_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,28,transAlv_15_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_29 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_29 =  (*transAlv_15_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,29,transAlv_15_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_30 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_30 =  (*transAlv_15_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,30,transAlv_15_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_15->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_15_31 = new TGeoCombiTrans(-3.13549999999993*cos(rotAngle)+33.4646558050777*sin(rotAngle),3.13549999999993*sin(rotAngle)+33.4646558050777*cos(rotAngle),26.6976862352101,rotAlv_15); 
	 *transAlv_15_31 =  (*transAlv_15_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_15,31,transAlv_15_31); 

	 phiEuler = 180; 
	 thetaEuler = 180.*TMath::ATan2(-0.6720370049050432600,0.7405175649761855900)/TMath::Pi(); 
	 psiEuler = -180; 

	 TGeoRotation *rotAlv_16 = new TGeoRotation("rotAlv16",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_16_0 = new TGeoCombiTrans(-3.15649999999993,33.559817680364,31.0463924828861,rotAlv_16); 
	 *transAlv_16_0 =  (*transAlv_16_0) * (*transDef) ; 
	 pWorld->AddNode(Alveolus_16,0,transAlv_16_0); 

	 rotAngle = TMath::Pi()*11.25*1/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_1 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_1 =  (*transAlv_16_1) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,1,transAlv_16_1); 

	 rotAngle = TMath::Pi()*11.25*2/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_2 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_2 =  (*transAlv_16_2) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,2,transAlv_16_2); 

	 rotAngle = TMath::Pi()*11.25*3/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_3 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_3 =  (*transAlv_16_3) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,3,transAlv_16_3); 

	 rotAngle = TMath::Pi()*11.25*4/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_4 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_4 =  (*transAlv_16_4) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,4,transAlv_16_4); 

	 rotAngle = TMath::Pi()*11.25*5/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_5 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_5 =  (*transAlv_16_5) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,5,transAlv_16_5); 

	 rotAngle = TMath::Pi()*11.25*6/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_6 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_6 =  (*transAlv_16_6) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,6,transAlv_16_6); 

	 rotAngle = TMath::Pi()*11.25*7/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_7 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_7 =  (*transAlv_16_7) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,7,transAlv_16_7); 

	 rotAngle = TMath::Pi()*11.25*8/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_8 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_8 =  (*transAlv_16_8) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,8,transAlv_16_8); 

	 rotAngle = TMath::Pi()*11.25*9/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_9 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_9 =  (*transAlv_16_9) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,9,transAlv_16_9); 

	 rotAngle = TMath::Pi()*11.25*10/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_10 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_10 =  (*transAlv_16_10) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,10,transAlv_16_10); 

	 rotAngle = TMath::Pi()*11.25*11/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_11 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_11 =  (*transAlv_16_11) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,11,transAlv_16_11); 

	 rotAngle = TMath::Pi()*11.25*12/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_12 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_12 =  (*transAlv_16_12) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,12,transAlv_16_12); 

	 rotAngle = TMath::Pi()*11.25*13/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_13 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_13 =  (*transAlv_16_13) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,13,transAlv_16_13); 

	 rotAngle = TMath::Pi()*11.25*14/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_14 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_14 =  (*transAlv_16_14) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,14,transAlv_16_14); 

	 rotAngle = TMath::Pi()*11.25*15/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_15 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_15 =  (*transAlv_16_15) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,15,transAlv_16_15); 

	 rotAngle = TMath::Pi()*11.25*16/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_16 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_16 =  (*transAlv_16_16) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,16,transAlv_16_16); 

	 rotAngle = TMath::Pi()*11.25*17/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_17 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_17 =  (*transAlv_16_17) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,17,transAlv_16_17); 

	 rotAngle = TMath::Pi()*11.25*18/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_18 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_18 =  (*transAlv_16_18) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,18,transAlv_16_18); 

	 rotAngle = TMath::Pi()*11.25*19/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_19 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_19 =  (*transAlv_16_19) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,19,transAlv_16_19); 

	 rotAngle = TMath::Pi()*11.25*20/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_20 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_20 =  (*transAlv_16_20) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,20,transAlv_16_20); 

	 rotAngle = TMath::Pi()*11.25*21/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_21 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_21 =  (*transAlv_16_21) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,21,transAlv_16_21); 

	 rotAngle = TMath::Pi()*11.25*22/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_22 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_22 =  (*transAlv_16_22) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,22,transAlv_16_22); 

	 rotAngle = TMath::Pi()*11.25*23/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_23 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_23 =  (*transAlv_16_23) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,23,transAlv_16_23); 

	 rotAngle = TMath::Pi()*11.25*24/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_24 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_24 =  (*transAlv_16_24) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,24,transAlv_16_24); 

	 rotAngle = TMath::Pi()*11.25*25/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_25 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_25 =  (*transAlv_16_25) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,25,transAlv_16_25); 

	 rotAngle = TMath::Pi()*11.25*26/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_26 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_26 =  (*transAlv_16_26) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,26,transAlv_16_26); 

	 rotAngle = TMath::Pi()*11.25*27/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_27 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_27 =  (*transAlv_16_27) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,27,transAlv_16_27); 

	 rotAngle = TMath::Pi()*11.25*28/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_28 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_28 =  (*transAlv_16_28) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,28,transAlv_16_28); 

	 rotAngle = TMath::Pi()*11.25*29/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_29 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_29 =  (*transAlv_16_29) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,29,transAlv_16_29); 

	 rotAngle = TMath::Pi()*11.25*30/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_30 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_30 =  (*transAlv_16_30) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,30,transAlv_16_30); 

	 rotAngle = TMath::Pi()*11.25*31/180.;  
	 rotAlv_16->RotateZ(-11.25);     
	 TGeoCombiTrans* transAlv_16_31 = new TGeoCombiTrans(-3.15649999999993*cos(rotAngle)+33.559817680364*sin(rotAngle),3.15649999999993*sin(rotAngle)+33.559817680364*cos(rotAngle),31.0463924828861,rotAlv_16); 
	 *transAlv_16_31 =  (*transAlv_16_31) * (*transDef); 
	 pWorld->AddNode(Alveolus_16,31,transAlv_16_31); 

#endif

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
