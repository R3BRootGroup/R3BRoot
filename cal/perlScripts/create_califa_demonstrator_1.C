#include <iomanip>
#include <iostream>
#include "TGeoManager.h"
#include "TMath.h"

#define Barrel 1


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
Bool_t fLabTrans = kFALSE;




void create_califa_geo(const char* geoTag)
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

  TGeoShape *pCBCone = new TGeoCone("Califa_Cone",20.,0.,26.4.,0.,3.2);
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

#if 0

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
    if(i == 4 || i == 8 || i == 10 || i == 12) {
      Double_t thick_alv_theta = thick_alv;
    } else {
      Double_t thick_alv_theta = 0.;
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

#endif

#if Barrel

