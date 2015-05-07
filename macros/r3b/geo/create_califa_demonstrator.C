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

 //Generated 4/30/2015 15:36:20 from macro createGeometryCLF811.pl by Hector Alvarez Pol (hector.alvarez.es) 


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
	 Alveolus_1->SetLineColor(kBlue); 

	 TGeoVolume *AlveolusInner_1=gGeoManager->MakeArb8("AlveolusInner_1", pAirMedium,9.97, vertices_inner_Alv); 
	 AlveolusInner_1->SetLineColor(kYellow); 

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
	 Alveolus_2->SetLineColor(kBlue); 

	 TGeoVolume *AlveolusInner_2=gGeoManager->MakeArb8("AlveolusInner_2", pAirMedium,9.97, vertices_inner_Alv); 
	 AlveolusInner_2->SetLineColor(kYellow); 

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
	 Alveolus_3->SetLineColor(kBlue); 

	 TGeoVolume *AlveolusInner_3=gGeoManager->MakeArb8("AlveolusInner_3", pAirMedium,9.97, vertices_inner_Alv); 
	 AlveolusInner_3->SetLineColor(kYellow); 

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
	 Alveolus_4->SetLineColor(kBlue); 

	 TGeoVolume *AlveolusInner_4=gGeoManager->MakeArb8("AlveolusInner_4", pAirMedium,9.97, vertices_inner_Alv); 
	 AlveolusInner_4->SetLineColor(kYellow); 

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
	 Alveolus_5->SetLineColor(kBlue); 

	 TGeoVolume *AlveolusInner_5=gGeoManager->MakeArb8("AlveolusInner_5", pAirMedium,9.97, vertices_inner_Alv); 
	 AlveolusInner_5->SetLineColor(kYellow); 

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
	 Alveolus_6->SetLineColor(kBlue); 

	 TGeoVolume *AlveolusInner_6=gGeoManager->MakeArb8("AlveolusInner_6", pAirMedium,9.97, vertices_inner_Alv); 
	 AlveolusInner_6->SetLineColor(kYellow); 

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
	 Alveolus_7->SetLineColor(kBlue); 

	 TGeoVolume *AlveolusInner_7=gGeoManager->MakeArb8("AlveolusInner_7", pAirMedium,9.97, vertices_inner_Alv); 
	 AlveolusInner_7->SetLineColor(kYellow); 

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
	 Alveolus_8->SetLineColor(kBlue); 

	 TGeoVolume *AlveolusInner_8=gGeoManager->MakeArb8("AlveolusInner_8", pAirMedium,9.97, vertices_inner_Alv); 
	 AlveolusInner_8->SetLineColor(kYellow); 

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
	 Double_t originOffset_1_1[3]; memset(originOffset_1_1, 0, 3*sizeof(Double_t));
	 for (int tmpi = 0; tmpi < 8; tmpi++) { originOffset_1_1[0] += vertices2[2*tmpi]/8.0; originOffset_1_1[1] += vertices2[2*tmpi+1]/8.0;}
	 for(int tmpi = 0; tmpi < 8; tmpi++) { vertices2[2*tmpi] -= originOffset_1_1[0]; vertices2[2*tmpi+1] -= originOffset_1_1[1]; }
	 TGeoVolume *Crystal_1_1 = gGeoManager->MakeArb8("Crystal_1_1", pCsIMedium,5.97, vertices2); 
	 Crystal_1_1->SetLineColor(kRed); 

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
	 Double_t originOffset_2_1[3]; memset(originOffset_2_1, 0, 3*sizeof(Double_t));
	 for (int tmpi = 0; tmpi < 8; tmpi++) { originOffset_2_1[0] += vertices2[2*tmpi]/8.0; originOffset_2_1[1] += vertices2[2*tmpi+1]/8.0;}
	 for(int tmpi = 0; tmpi < 8; tmpi++) { vertices2[2*tmpi] -= originOffset_2_1[0]; vertices2[2*tmpi+1] -= originOffset_2_1[1]; }
	 TGeoVolume *Crystal_2_1 = gGeoManager->MakeArb8("Crystal_2_1", pCsIMedium,6.97, vertices2); 
	 Crystal_2_1->SetLineColor(kRed); 

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
	 Double_t originOffset_2_2[3]; memset(originOffset_2_2, 0, 3*sizeof(Double_t));
	 for (int tmpi = 0; tmpi < 8; tmpi++) { originOffset_2_2[0] += vertices2[2*tmpi]/8.0; originOffset_2_2[1] += vertices2[2*tmpi+1]/8.0;}
	 for(int tmpi = 0; tmpi < 8; tmpi++) { vertices2[2*tmpi] -= originOffset_2_2[0]; vertices2[2*tmpi+1] -= originOffset_2_2[1]; }
	 TGeoVolume *Crystal_2_2 = gGeoManager->MakeArb8("Crystal_2_2", pCsIMedium,6.97, vertices2); 
	 Crystal_2_2->SetLineColor(kRed); 

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
	 Double_t originOffset_2_3[3]; memset(originOffset_2_3, 0, 3*sizeof(Double_t));
	 for (int tmpi = 0; tmpi < 8; tmpi++) { originOffset_2_3[0] += vertices2[2*tmpi]/8.0; originOffset_2_3[1] += vertices2[2*tmpi+1]/8.0;}
	 for(int tmpi = 0; tmpi < 8; tmpi++) { vertices2[2*tmpi] -= originOffset_2_3[0]; vertices2[2*tmpi+1] -= originOffset_2_3[1]; }
	 TGeoVolume *Crystal_2_3 = gGeoManager->MakeArb8("Crystal_2_3", pCsIMedium,6.97, vertices2); 
	 Crystal_2_3->SetLineColor(kRed); 

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
	 Double_t originOffset_2_4[3]; memset(originOffset_2_4, 0, 3*sizeof(Double_t));
	 for (int tmpi = 0; tmpi < 8; tmpi++) { originOffset_2_4[0] += vertices2[2*tmpi]/8.0; originOffset_2_4[1] += vertices2[2*tmpi+1]/8.0;}
	 for(int tmpi = 0; tmpi < 8; tmpi++) { vertices2[2*tmpi] -= originOffset_2_4[0]; vertices2[2*tmpi+1] -= originOffset_2_4[1]; }
	 TGeoVolume *Crystal_2_4 = gGeoManager->MakeArb8("Crystal_2_4", pCsIMedium,6.97, vertices2); 
	 Crystal_2_4->SetLineColor(kRed); 

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
	 Double_t originOffset_3_1[3]; memset(originOffset_3_1, 0, 3*sizeof(Double_t));
	 for (int tmpi = 0; tmpi < 8; tmpi++) { originOffset_3_1[0] += vertices2[2*tmpi]/8.0; originOffset_3_1[1] += vertices2[2*tmpi+1]/8.0;}
	 for(int tmpi = 0; tmpi < 8; tmpi++) { vertices2[2*tmpi] -= originOffset_3_1[0]; vertices2[2*tmpi+1] -= originOffset_3_1[1]; }
	 TGeoVolume *Crystal_3_1 = gGeoManager->MakeArb8("Crystal_3_1", pCsIMedium,7.97, vertices2); 
	 Crystal_3_1->SetLineColor(kRed); 

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
	 Double_t originOffset_3_2[3]; memset(originOffset_3_2, 0, 3*sizeof(Double_t));
	 for (int tmpi = 0; tmpi < 8; tmpi++) { originOffset_3_2[0] += vertices2[2*tmpi]/8.0; originOffset_3_2[1] += vertices2[2*tmpi+1]/8.0;}
	 for(int tmpi = 0; tmpi < 8; tmpi++) { vertices2[2*tmpi] -= originOffset_3_2[0]; vertices2[2*tmpi+1] -= originOffset_3_2[1]; }
	 TGeoVolume *Crystal_3_2 = gGeoManager->MakeArb8("Crystal_3_2", pCsIMedium,7.97, vertices2); 
	 Crystal_3_2->SetLineColor(kRed); 

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
	 Double_t originOffset_3_3[3]; memset(originOffset_3_3, 0, 3*sizeof(Double_t));
	 for (int tmpi = 0; tmpi < 8; tmpi++) { originOffset_3_3[0] += vertices2[2*tmpi]/8.0; originOffset_3_3[1] += vertices2[2*tmpi+1]/8.0;}
	 for(int tmpi = 0; tmpi < 8; tmpi++) { vertices2[2*tmpi] -= originOffset_3_3[0]; vertices2[2*tmpi+1] -= originOffset_3_3[1]; }
	 TGeoVolume *Crystal_3_3 = gGeoManager->MakeArb8("Crystal_3_3", pCsIMedium,7.97, vertices2); 
	 Crystal_3_3->SetLineColor(kRed); 

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
	 Double_t originOffset_3_4[3]; memset(originOffset_3_4, 0, 3*sizeof(Double_t));
	 for (int tmpi = 0; tmpi < 8; tmpi++) { originOffset_3_4[0] += vertices2[2*tmpi]/8.0; originOffset_3_4[1] += vertices2[2*tmpi+1]/8.0;}
	 for(int tmpi = 0; tmpi < 8; tmpi++) { vertices2[2*tmpi] -= originOffset_3_4[0]; vertices2[2*tmpi+1] -= originOffset_3_4[1]; }
	 TGeoVolume *Crystal_3_4 = gGeoManager->MakeArb8("Crystal_3_4", pCsIMedium,7.97, vertices2); 
	 Crystal_3_4->SetLineColor(kRed); 

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

	 CrystalWithWrapping_1_1->AddNode(Crystal_1_1,1,new TGeoCombiTrans(originOffset_1_1[0], originOffset_1_1[1],0.0,rotUni)); 
	 CrystalWithWrapping_2_1->AddNode(Crystal_2_1,1,new TGeoCombiTrans(originOffset_2_1[0], originOffset_2_1[1],0.0,rotUni)); 
	 CrystalWithWrapping_2_2->AddNode(Crystal_2_2,1,new TGeoCombiTrans(originOffset_2_2[0], originOffset_2_2[1],0.0,rotUni)); 
	 CrystalWithWrapping_2_3->AddNode(Crystal_2_3,1,new TGeoCombiTrans(originOffset_2_3[0], originOffset_2_3[1],0.0,rotUni)); 
	 CrystalWithWrapping_2_4->AddNode(Crystal_2_4,1,new TGeoCombiTrans(originOffset_2_4[0], originOffset_2_4[1],0.0,rotUni)); 
	 CrystalWithWrapping_3_1->AddNode(Crystal_3_1,1,new TGeoCombiTrans(originOffset_3_1[0], originOffset_3_1[1],0.0,rotUni)); 
	 CrystalWithWrapping_3_2->AddNode(Crystal_3_2,1,new TGeoCombiTrans(originOffset_3_2[0], originOffset_3_2[1],0.0,rotUni)); 
	 CrystalWithWrapping_3_3->AddNode(Crystal_3_3,1,new TGeoCombiTrans(originOffset_3_3[0], originOffset_3_3[1],0.0,rotUni)); 
	 CrystalWithWrapping_3_4->AddNode(Crystal_3_4,1,new TGeoCombiTrans(originOffset_3_4[0], originOffset_3_4[1],0.0,rotUni)); 
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
	 TGeoCombiTrans matDemo(0, 1.92093, -1.75703, new TGeoRotation("rotDemo", 0, 20.3304, 0));
	 phiEuler = 180; 
	 thetaEuler = 180.*TMath::ATan2(-0.1275567287250409800,0.9918312764563167500)/TMath::Pi(); 
	 psiEuler = -180; 

	 TGeoRotation *rotAlv_9 = new TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans *transAlvBase = new TGeoCombiTrans();
	 TGeoRotation *rotPlace_9 = new TGeoRotation();
	 *rotAlv_9 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_9_0 = new TGeoCombiTrans(-2.98999999999993,30.3953961794098,4.44438599416416,rotAlv_9); 
	 *transAlvBase =  (*transAlv_9_0) * (*transDef) ; 
	 rotPlace_9->SetAngles(-120, 0., 0.);
	 *transAlv_9_0 =  (*rotPlace_9) * matDemo * (*transAlvBase); 
	 rotAlv_9->SetAngles(-11.25, 0., 0.);     
	 pWorld->AddNode(Alveolus_9,0,transAlv_9_0); 

	 TGeoCombiTrans* transAlv_9_1 = new TGeoCombiTrans();
	 *transAlv_9_1 = (*rotPlace_9) * matDemo * (*rotAlv_9) * (*transAlvBase);
	 pWorld->AddNode(Alveolus_9,1,transAlv_9_1); 

	 *rotAlv_9 = TGeoRotation("rotAlv9",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_9_2 = new TGeoCombiTrans(-2.98999999999993,30.3953961794098,4.44438599416416,rotAlv_9); 
	 *transAlvBase =  (*transAlv_9_2) * (*transDef) ; 
	 rotPlace_9->SetAngles(-300, 0., 0.);
	 *transAlv_9_2 =  (*rotPlace_9) * matDemo * (*transAlvBase); 
	 rotAlv_9->SetAngles(-11.25, 0., 0.);     
	 pWorld->AddNode(Alveolus_9,2,transAlv_9_2); 

	 TGeoCombiTrans* transAlv_9_3 = new TGeoCombiTrans();
	 *transAlv_9_3 = (*rotPlace_9) * matDemo * (*rotAlv_9) * (*transAlvBase);
	 pWorld->AddNode(Alveolus_9,3,transAlv_9_3); 

	 phiEuler = 180; 
	 thetaEuler = 180.*TMath::ATan2(-0.2131210329617397300,0.9770258058563863200)/TMath::Pi(); 
	 psiEuler = -180; 

	 TGeoRotation *rotAlv_10 = new TGeoRotation("rotAlv10",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans *transAlvBase = new TGeoCombiTrans();
	 TGeoRotation *rotPlace_10 = new TGeoRotation();
	 *rotAlv_10 = TGeoRotation("rotAlv10",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_10_0 = new TGeoCombiTrans(-3.01649999999993,30.6605329493663,7.63929090825073,rotAlv_10); 
	 *transAlvBase =  (*transAlv_10_0) * (*transDef) ; 
	 rotPlace_10->SetAngles(-120, 0., 0.);
	 *transAlv_10_0 =  (*rotPlace_10) * matDemo * (*transAlvBase); 
	 rotAlv_10->SetAngles(-11.25, 0., 0.);     
	 pWorld->AddNode(Alveolus_10,0,transAlv_10_0); 

	 TGeoCombiTrans* transAlv_10_1 = new TGeoCombiTrans();
	 *transAlv_10_1 = (*rotPlace_10) * matDemo * (*rotAlv_10) * (*transAlvBase);
	 pWorld->AddNode(Alveolus_10,1,transAlv_10_1); 

	 *rotAlv_10 = TGeoRotation("rotAlv10",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_10_2 = new TGeoCombiTrans(-3.01649999999993,30.6605329493663,7.63929090825073,rotAlv_10); 
	 *transAlvBase =  (*transAlv_10_2) * (*transDef) ; 
	 rotPlace_10->SetAngles(-300, 0., 0.);
	 *transAlv_10_2 =  (*rotPlace_10) * matDemo * (*transAlvBase); 
	 rotAlv_10->SetAngles(-11.25, 0., 0.);     
	 pWorld->AddNode(Alveolus_10,2,transAlv_10_2); 

	 TGeoCombiTrans* transAlv_10_3 = new TGeoCombiTrans();
	 *transAlv_10_3 = (*rotPlace_10) * matDemo * (*rotAlv_10) * (*transAlvBase);
	 pWorld->AddNode(Alveolus_10,3,transAlv_10_3); 

	 phiEuler = 180; 
	 thetaEuler = 180.*TMath::ATan2(-0.2971184379757390000,0.9548406326790127600)/TMath::Pi(); 
	 psiEuler = -180; 

	 TGeoRotation *rotAlv_11 = new TGeoRotation("rotAlv11",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans *transAlvBase = new TGeoCombiTrans();
	 TGeoRotation *rotPlace_11 = new TGeoRotation();
	 *rotAlv_11 = TGeoRotation("rotAlv11",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_11_0 = new TGeoCombiTrans(-3.04199999999993,30.9206806111464,10.9180918399028,rotAlv_11); 
	 *transAlvBase =  (*transAlv_11_0) * (*transDef) ; 
	 rotPlace_11->SetAngles(-120, 0., 0.);
	 *transAlv_11_0 =  (*rotPlace_11) * matDemo * (*transAlvBase); 
	 rotAlv_11->SetAngles(-11.25, 0., 0.);     
	 pWorld->AddNode(Alveolus_11,0,transAlv_11_0); 

	 TGeoCombiTrans* transAlv_11_1 = new TGeoCombiTrans();
	 *transAlv_11_1 = (*rotPlace_11) * matDemo * (*rotAlv_11) * (*transAlvBase);
	 pWorld->AddNode(Alveolus_11,1,transAlv_11_1); 

	 *rotAlv_11 = TGeoRotation("rotAlv11",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_11_2 = new TGeoCombiTrans(-3.04199999999993,30.9206806111464,10.9180918399028,rotAlv_11); 
	 *transAlvBase =  (*transAlv_11_2) * (*transDef) ; 
	 rotPlace_11->SetAngles(-300, 0., 0.);
	 *transAlv_11_2 =  (*rotPlace_11) * matDemo * (*transAlvBase); 
	 rotAlv_11->SetAngles(-11.25, 0., 0.);     
	 pWorld->AddNode(Alveolus_11,2,transAlv_11_2); 

	 TGeoCombiTrans* transAlv_11_3 = new TGeoCombiTrans();
	 *transAlv_11_3 = (*rotPlace_11) * matDemo * (*rotAlv_11) * (*transAlvBase);
	 pWorld->AddNode(Alveolus_11,3,transAlv_11_3); 

	 phiEuler = 180; 
	 thetaEuler = 180.*TMath::ATan2(-0.3788871076768108300,0.9254428991765514300)/TMath::Pi(); 
	 psiEuler = -180; 

	 TGeoRotation *rotAlv_12 = new TGeoRotation("rotAlv12",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans *transAlvBase = new TGeoCombiTrans();
	 TGeoRotation *rotPlace_12 = new TGeoRotation();
	 *rotAlv_12 = TGeoRotation("rotAlv12",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_12_0 = new TGeoCombiTrans(-3.06749999999993,31.1737346881315,14.3082258908461,rotAlv_12); 
	 *transAlvBase =  (*transAlv_12_0) * (*transDef) ; 
	 rotPlace_12->SetAngles(-120, 0., 0.);
	 *transAlv_12_0 =  (*rotPlace_12) * matDemo * (*transAlvBase); 
	 rotAlv_12->SetAngles(-11.25, 0., 0.);     
	 pWorld->AddNode(Alveolus_12,0,transAlv_12_0); 

	 TGeoCombiTrans* transAlv_12_1 = new TGeoCombiTrans();
	 *transAlv_12_1 = (*rotPlace_12) * matDemo * (*rotAlv_12) * (*transAlvBase);
	 pWorld->AddNode(Alveolus_12,1,transAlv_12_1); 

	 *rotAlv_12 = TGeoRotation("rotAlv12",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_12_2 = new TGeoCombiTrans(-3.06749999999993,31.1737346881315,14.3082258908461,rotAlv_12); 
	 *transAlvBase =  (*transAlv_12_2) * (*transDef) ; 
	 rotPlace_12->SetAngles(-300, 0., 0.);
	 *transAlv_12_2 =  (*rotPlace_12) * matDemo * (*transAlvBase); 
	 rotAlv_12->SetAngles(-11.25, 0., 0.);     
	 pWorld->AddNode(Alveolus_12,2,transAlv_12_2); 

	 TGeoCombiTrans* transAlv_12_3 = new TGeoCombiTrans();
	 *transAlv_12_3 = (*rotPlace_12) * matDemo * (*rotAlv_12) * (*transAlvBase);
	 pWorld->AddNode(Alveolus_12,3,transAlv_12_3); 

	 phiEuler = 180; 
	 thetaEuler = 180.*TMath::ATan2(-0.4578676224535794800,0.8890203823921623000)/TMath::Pi(); 
	 psiEuler = -180; 

	 TGeoRotation *rotAlv_13 = new TGeoRotation("rotAlv13",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans *transAlvBase = new TGeoCombiTrans();
	 TGeoRotation *rotPlace_13 = new TGeoRotation();
	 *rotAlv_13 = TGeoRotation("rotAlv13",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_13_0 = new TGeoCombiTrans(-3.09099999999993,31.417956426523,17.8408437262792,rotAlv_13); 
	 *transAlvBase =  (*transAlv_13_0) * (*transDef) ; 
	 rotPlace_13->SetAngles(-120, 0., 0.);
	 *transAlv_13_0 =  (*rotPlace_13) * matDemo * (*transAlvBase); 
	 rotAlv_13->SetAngles(-11.25, 0., 0.);     
	 pWorld->AddNode(Alveolus_13,0,transAlv_13_0); 

	 TGeoCombiTrans* transAlv_13_1 = new TGeoCombiTrans();
	 *transAlv_13_1 = (*rotPlace_13) * matDemo * (*rotAlv_13) * (*transAlvBase);
	 pWorld->AddNode(Alveolus_13,1,transAlv_13_1); 

	 *rotAlv_13 = TGeoRotation("rotAlv13",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_13_2 = new TGeoCombiTrans(-3.09099999999993,31.417956426523,17.8408437262792,rotAlv_13); 
	 *transAlvBase =  (*transAlv_13_2) * (*transDef) ; 
	 rotPlace_13->SetAngles(-300, 0., 0.);
	 *transAlv_13_2 =  (*rotPlace_13) * matDemo * (*transAlvBase); 
	 rotAlv_13->SetAngles(-11.25, 0., 0.);     
	 pWorld->AddNode(Alveolus_13,2,transAlv_13_2); 

	 TGeoCombiTrans* transAlv_13_3 = new TGeoCombiTrans();
	 *transAlv_13_3 = (*rotPlace_13) * matDemo * (*rotAlv_13) * (*transAlvBase);
	 pWorld->AddNode(Alveolus_13,3,transAlv_13_3); 

	 phiEuler = 180; 
	 thetaEuler = 180.*TMath::ATan2(-0.5334425026911959700,0.8458363295121305000)/TMath::Pi(); 
	 psiEuler = -180; 

	 TGeoRotation *rotAlv_14 = new TGeoRotation("rotAlv14",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans *transAlvBase = new TGeoCombiTrans();
	 TGeoRotation *rotPlace_14 = new TGeoRotation();
	 *rotAlv_14 = TGeoRotation("rotAlv14",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_14_0 = new TGeoCombiTrans(-3.11449999999993,33.343093197755,22.6194116972268,rotAlv_14); 
	 *transAlvBase =  (*transAlv_14_0) * (*transDef) ; 
	 rotPlace_14->SetAngles(-120, 0., 0.);
	 *transAlv_14_0 =  (*rotPlace_14) * matDemo * (*transAlvBase); 
	 rotAlv_14->SetAngles(-11.25, 0., 0.);     
	 pWorld->AddNode(Alveolus_14,0,transAlv_14_0); 

	 TGeoCombiTrans* transAlv_14_1 = new TGeoCombiTrans();
	 *transAlv_14_1 = (*rotPlace_14) * matDemo * (*rotAlv_14) * (*transAlvBase);
	 pWorld->AddNode(Alveolus_14,1,transAlv_14_1); 

	 *rotAlv_14 = TGeoRotation("rotAlv14",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_14_2 = new TGeoCombiTrans(-3.11449999999993,33.343093197755,22.6194116972268,rotAlv_14); 
	 *transAlvBase =  (*transAlv_14_2) * (*transDef) ; 
	 rotPlace_14->SetAngles(-300, 0., 0.);
	 *transAlv_14_2 =  (*rotPlace_14) * matDemo * (*transAlvBase); 
	 rotAlv_14->SetAngles(-11.25, 0., 0.);     
	 pWorld->AddNode(Alveolus_14,2,transAlv_14_2); 

	 TGeoCombiTrans* transAlv_14_3 = new TGeoCombiTrans();
	 *transAlv_14_3 = (*rotPlace_14) * matDemo * (*rotAlv_14) * (*transAlvBase);
	 pWorld->AddNode(Alveolus_14,3,transAlv_14_3); 

	 phiEuler = 180; 
	 thetaEuler = 180.*TMath::ATan2(-0.6050038521676790500,0.7962225435531633500)/TMath::Pi(); 
	 psiEuler = -180; 

	 TGeoRotation *rotAlv_15 = new TGeoRotation("rotAlv15",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans *transAlvBase = new TGeoCombiTrans();
	 TGeoRotation *rotPlace_15 = new TGeoRotation();
	 *rotAlv_15 = TGeoRotation("rotAlv15",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_15_0 = new TGeoCombiTrans(-3.13549999999993,33.4646558050777,26.6976862352101,rotAlv_15); 
	 *transAlvBase =  (*transAlv_15_0) * (*transDef) ; 
	 rotPlace_15->SetAngles(-120, 0., 0.);
	 *transAlv_15_0 =  (*rotPlace_15) * matDemo * (*transAlvBase); 
	 rotAlv_15->SetAngles(-11.25, 0., 0.);     
	 pWorld->AddNode(Alveolus_15,0,transAlv_15_0); 

	 TGeoCombiTrans* transAlv_15_1 = new TGeoCombiTrans();
	 *transAlv_15_1 = (*rotPlace_15) * matDemo * (*rotAlv_15) * (*transAlvBase);
	 pWorld->AddNode(Alveolus_15,1,transAlv_15_1); 

	 *rotAlv_15 = TGeoRotation("rotAlv15",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans* transAlv_15_2 = new TGeoCombiTrans(-3.13549999999993,33.4646558050777,26.6976862352101,rotAlv_15); 
	 *transAlvBase =  (*transAlv_15_2) * (*transDef) ; 
	 rotPlace_15->SetAngles(-300, 0., 0.);
	 *transAlv_15_2 =  (*rotPlace_15) * matDemo * (*transAlvBase); 
	 rotAlv_15->SetAngles(-11.25, 0., 0.);     
	 pWorld->AddNode(Alveolus_15,2,transAlv_15_2); 

	 TGeoCombiTrans* transAlv_15_3 = new TGeoCombiTrans();
	 *transAlv_15_3 = (*rotPlace_15) * matDemo * (*rotAlv_15) * (*transAlvBase);
	 pWorld->AddNode(Alveolus_15,3,transAlv_15_3); 

	 phiEuler = 180; 
	 thetaEuler = 180.*TMath::ATan2(-0.6720370049050432600,0.7405175649761855900)/TMath::Pi(); 
	 psiEuler = -180; 

	 TGeoRotation *rotAlv_16 = new TGeoRotation("rotAlv16",phiEuler,thetaEuler,psiEuler); 
	 TGeoCombiTrans *transAlvBase = new TGeoCombiTrans();
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
