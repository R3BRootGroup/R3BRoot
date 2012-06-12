
#include "R3BMagnet.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairGeoRootBuilder.h"
#include "FairRuntimeDb.h"
#include "TObjArray.h"
#include "FairRun.h"
#include "FairGeoVolume.h"
#include "FairGeoNode.h"

#include "R3BGeoMagnet.h"
#include "R3BGeoPassivePar.h"
#include "TGeoMatrix.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoBBox.h"
#include "TGeoPgon.h"
#include "TGeoSphere.h"
#include "TGeoArb8.h"
#include "TGeoCone.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"
#include "TGeoManager.h"

#include <iostream>

using namespace std;

R3BMagnet::~R3BMagnet()
{
}
R3BMagnet::R3BMagnet()
{
}

R3BMagnet::R3BMagnet(const char * name, const char *Title)
  : R3BModule(name ,Title)
{
}



void R3BMagnet::ConstructGeometry(){

    Double_t degrad = TMath::Pi()/180.;
    Double_t w       =        0.;
    Double_t a       =        0.;
    Double_t z       =        0.;
    Double_t density =        0.;
    Double_t radl    =        0.;
    Double_t absl    =        0.;
    Int_t numed      =        0 ;
    Double_t dx,dy,dz;
    Double_t fDx1, fDx2, fDy1, fDy2, fDz;
    Double_t thx, phx, thy, phy, thz, phz;
    
 // Specific Material definition
 // --  Material: Iron
   TGeoMedium * pMedFe=NULL;
   if (gGeoManager->GetMedium("Iron") ){
       pMedFe=gGeoManager->GetMedium("Iron");
   }else{
    w       =        0.;
    a       = 55.850000;
    z       = 26.000000;
    density = 7.870000;
    radl    = 1.757717;
    absl    = 169.994418;
    TGeoMaterial*
	pMatFe = new TGeoMaterial("Iron", a,z,density,radl,absl);
    pMatFe->SetIndex(701);
    numed   = 23;  // medium number
    Double_t par[8];
    par[0]  = 0.000000; // isvol
    par[1]  = 0.000000; // ifield
    par[2]  = 0.000000; // fieldm
    par[3]  = 0.000000; // tmaxfd
    par[4]  = 0.000000; // stemax
    par[5]  = 0.000000; // deemax
    par[6]  = 0.000100; // epsil
    par[7]  = 0.000000; // stmin
    pMedFe = new TGeoMedium("Iron", numed,pMatFe, par);
   }
   
   
   
      // Material: HeliumGas
   TGeoMedium * pMed4=NULL;
   if (gGeoManager->GetMedium("HeliumGas") ){
       pMed4=gGeoManager->GetMedium("HeliumGas");
   }else{
       w       =        0;
       a       = 4.000000;
       z       = 2.000000;
       density = 0.000125;
       
       TGeoMaterial*
	   pMat4 = new TGeoMaterial("HeliumGas", a,z,density);
       pMat4->SetIndex(702);
       numed   = 20;  // medium number
       Double_t par[8];
       par[0]  = 0.000000; // isvol
       par[1]  = 0.000000; // ifield
       par[2]  = 0.000000; // fieldm
       par[3]  = 0.000000; // tmaxfd
       par[4]  = 0.000000; // stemax
       par[5]  = 0.000000; // deemax
       par[6]  = 0.000100; // epsil
       par[7]  = 0.000000; // stmin

     pMed4 = new TGeoMedium("HeliumGas", numed,pMat4,par);
   }   
   
   
   // Material: Aluminum
   TGeoMedium * pMed21=NULL;
   if (gGeoManager->GetMedium("Aluminum") ){
       pMed21=gGeoManager->GetMedium("Aluminum");
   }else{
     a       = 26.980000;
     z       = 13.000000;
     density = 2.700000;
     radl    = 8.875105;
     absl    = 388.793113;
     TGeoMaterial*
	 pMat21 = new TGeoMaterial("Aluminum", a,z,density,radl,absl);
     pMat21->SetIndex(20);
     // Medium: Aluminum
     numed   = 20;  // medium number
     Double_t par[8];
     par[0]  = 0.000000; // isvol
     par[1]  = 0.000000; // ifield
     par[2]  = 0.000000; // fieldm
     par[3]  = 0.000000; // tmaxfd
     par[4]  = 0.000000; // stemax
     par[5]  = 0.000000; // deemax
     par[6]  = 0.000100; // epsil
     par[7]  = 0.000000; // stmin
     pMed21 = new TGeoMedium("Aluminum", numed,pMat21, par);
   }
   
      

 // -- Mixture: Air
    TGeoMedium * pMed2=NULL;
   if (gGeoManager->GetMedium("Air") ){
       pMed2=gGeoManager->GetMedium("Air");
   }else{
     Int_t nel     = 2;
     density = 0.001290;
     TGeoMixture*
	 pMat2 = new TGeoMixture("Air", nel,density);
     a = 14.006740;   z = 7.000000;  w = 0.700000;  // N
     pMat2->DefineElement(0,a,z,w);
     a = 15.999400;   z = 8.000000;   w = 0.300000;  // O
     pMat2->DefineElement(1,a,z,w);
     pMat2->SetIndex(700);
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
     pMed2 = new TGeoMedium("Air", numed,pMat2, par );
   }
      
    
 // <D.Bertini@gsi.de>
 // version adapted from ALADIN magnet
 // parameters

    Double_t Aladin_width = 156.0;//1.250*m; 
    Double_t Aladin_length = 176.0; //1.70*m; Field length is 1.4 m
    Double_t Aladin_gap = 50.;    //cm y-directory
    // Angle / beam axis check me ! (-7.3 deg , +7.2 deg ?)
    //Double_t Aladin_angle = +7.3; // degree
    Double_t Aladin_angle = -7.0; // degree
    Double_t DistanceToTarget = 350.0;  //cm
    Double_t Yoke_thickness = 50.;     //cm thicknes of lead
//    Double_t Correction = -119.94;   //cm
    Double_t Correction = -95.0;   //cm
// Define distance  from target ??? FIXME
//    Double_t DistanceFromtargetToAladinCenter
//	     = DistanceToTarget + Aladin_length/2.0;
//  R3BSim
    Double_t DistanceFromtargetToAladinCenter
	     = DistanceToTarget + Correction;
// Transformations
    TGeoRotation *rot_aladin = new TGeoRotation("Aladinrot");
    rot_aladin->RotateY(Aladin_angle);
    TGeoRotation *rot_mirror = new TGeoRotation("mirror");
    rot_mirror->RotateZ(180.0);
    rot_mirror->RotateY(Aladin_angle);
// Geometry description
   TGeoVolume *pAWorld =  gGeoManager->GetTopVolume();

   //Enveloppe
   TGeoShape *aladin_box = new TGeoBBox("Aladin_Box",
                                            Aladin_length/2.0,
					    Aladin_length/2.0,
					    Aladin_length/2.0);


   TGeoVolume* pWorld = new TGeoVolume("Aladin_box",aladin_box, pMed2);
   TGeoCombiTrans *t0 = new TGeoCombiTrans();
   TGeoCombiTrans *pGlobal = GetGlobalPosition(t0);
   //pAWorld->AddNodeOverlap(pWorld, 0, pGlobal );



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



   pAWorld->AddNode(pVolFeYoke_up,   1, t1);
   pAWorld->AddNode(pVolFeYoke_down, 2, t2);
   pAWorld->AddNode(pVolFeYoke_left, 3, t3);
   pAWorld->AddNode(pVolFeYoke_right,4, t4);
   
   
   
//-----------------------------------------------------------   


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
   dz = 343.5;
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
   dz = 343.5;
   TGeoRotation *gRot6 = new TGeoRotation();
   gRot6->RotateX(0.);
   gRot6->RotateY(-7.000000);
   gRot6->RotateZ(0.);
   TGeoCombiTrans *pMatrix63 = new TGeoCombiTrans("", dx,dy,dz,gRot6);   
      


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
   xy[4][0] = 79.05;	xy[4][1] = -30.5;
   xy[5][0] = 79.05;	xy[5][1] = 30.5;
   xy[6][0] = -79.05;	xy[6][1] = 30.5;
   xy[7][0] = -79.05;	xy[7][1] = -30.5;
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
   TGeoVolume* pHeliumAladinChamberLog1 = new TGeoVolume("HeliumAladinChamberLog1",pHeliumAladinChamber1, pMed2);
   
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
   TGeoVolume* pHeliumAladinChamberLog2 = new TGeoVolume("HeliumAladinChamberLog2",pHeliumAladinChamber2, pMed2);
   
   // Helium-Aladin chamber part3
   ab[0][0] = 68.6;	ab[0][1] = -22.9;
   ab[1][0] = 68.6;	ab[1][1] = 22.9;
   ab[2][0] = -68.6;	ab[2][1] = 22.9;
   ab[3][0] = -68.6;	ab[3][1] = -22.9;
   ab[4][0] = 78.55;	ab[4][1] = -30.0;
   ab[5][0] = 78.55;	ab[5][1] = 30.0;
   ab[6][0] = -78.55;	ab[6][1] = 30.0;
   ab[7][0] = -78.55;	ab[7][1] = -30.0;
   TGeoShape *pHeliumAladinChamber3 = new TGeoArb8("HeliumAladinChamber3", 30.95, &ab[0][0]);
   TGeoVolume* pHeliumAladinChamberLog3 = new TGeoVolume("HeliumAladinChamberLog3",pHeliumAladinChamber3, pMed2);        
   
   
   
   
   pAWorld->AddNode(pHeliumAladinChamberLog1,1,pMatrix61);
   pAWorld->AddNode(pHeliumAladinChamberLog2,1,pMatrix62);
   pAWorld->AddNode(pHeliumAladinChamberLog3,1,pMatrix63);
   
   pAWorld->AddNode(pAladinChamberLog1,1,pMatrix58);
   pAWorld->AddNode(pAladinChamberLog2,1,pMatrix59);
   pAWorld->AddNode(pAladinChamberLog3,1,pMatrix60);
 

   
}

/*
void R3BMagnet::ConstructGeometry(){
// Material definition

// --  Material: Iron
   Double_t w       =        0.;
   Double_t a       = 55.850000;
   Double_t z       = 26.000000;
   Double_t density = 7.870000;
   Double_t radl    = 1.757717;
   Double_t absl    = 169.994418;
   TGeoMaterial*
   pMatFe = new TGeoMaterial("Iron", a,z,density,radl,absl);
   pMatFe->SetIndex(701);
   Int_t numed   = 23;  // medium number
   TGeoMedium*
   pMedFe = new TGeoMedium("Iron", numed,pMatFe);

// -- Mixture: Air
   Int_t nel     = 2;
   density = 0.001290;
   TGeoMixture*
   pMat2 = new TGeoMixture("Air", nel,density);
      a = 14.006740;   z = 7.000000;  w = 0.700000;  // N
   pMat2->DefineElement(0,a,z,w);
      a = 15.999400;   z = 8.000000;   w = 0.300000;  // O
   pMat2->DefineElement(1,a,z,w);
   pMat2->SetIndex(700);
// Medium: Air
   numed   = 1;  // medium number
   TGeoMedium*
   pMed2 = new TGeoMedium("Air", numed,pMat2);


// <D.Bertini@gsi.de>
// version adapted from R3bSim

    Double_t offsetTarget = 350.;
    Double_t solidDipole_x = 75.;
    Double_t solidDipole_y = 25.;
    Double_t solidDipole_z = 108.;
    Double_t solidYoke_x = 36.;
    Double_t solidYoke_y = 25.;
    Double_t solidYoke_z = 3.6;

   // Global  Translation
    TGeoTranslation *posDipole = new TGeoTranslation(0.,0.,-119.94 + offsetTarget);
   // Global Y rotation
    TGeoRotation    *rotDipole = new TGeoRotation("global");
    rotDipole->RotateY(7.2);
   // 3D Transform
    TGeoCombiTrans *transDipole
	           = new TGeoCombiTrans(*posDipole,*rotDipole);


    Double_t ironYoke_x=  (solidDipole_x+solidYoke_x)*TMath::Cos(7.2)
	                 +(solidDipole_z-solidYoke_z)*TMath::Sin(7.2);

    Double_t ironYoke_y=0;

    Double_t ironYoke_z=  -119.94
	                  + offsetTarget-(solidDipole_x+solidYoke_x)
	                  * TMath::Sin(7.2)
	                  + (solidDipole_z-solidYoke_z)
	                  * TMath::Cos(7.2);
    
    TGeoTranslation *positionYoke =
	            new TGeoTranslation(ironYoke_x,ironYoke_y,ironYoke_z);
    TGeoCombiTrans  *yokeTransform =
	            new TGeoCombiTrans(*positionYoke,*rotDipole);



    Double_t ironYoke_x1=  (-solidDipole_x-solidYoke_x)*TMath::Cos(7.2)
	                  +(solidDipole_z-solidYoke_z)*TMath::Sin(7.2);

    Double_t ironYoke_y1=0;

    Double_t ironYoke_z1= -119.94
	                  + offsetTarget-(-solidDipole_x-solidYoke_x)
	                  * TMath::Sin(7.2)
	                  + (solidDipole_z-solidYoke_z)
	                  * TMath::Cos(7.2);
    
    TGeoTranslation *positionYoke1 =
	             new TGeoTranslation(ironYoke_x1,ironYoke_y1,ironYoke_z1);
    TGeoCombiTrans* yokeTransform1 =
	             new TGeoCombiTrans(*positionYoke1,*rotDipole);



   // Geometry description
   TGeoVolume *pWorld =  gGeoManager->GetTopVolume();

   TGeoShape *pFeYoke = new TGeoBBox("FeYoke", solidYoke_x,solidYoke_y,solidYoke_z);
   TGeoShape *pFeDipole = new TGeoBBox("FeDipole", solidDipole_x,solidDipole_y,solidDipole_z);
   // Volume: Feyoke_up
   TGeoVolume* pVolFeyoke = new TGeoVolume("FeYokeVol",pFeYoke, pMedFe);
   TGeoVolume* pVolDipole = new TGeoVolume("FeDipoleVol",pFeDipole, pMed2);
   pVolFeyoke->SetVisLeaves(kTRUE);
   pVolDipole->SetVisLeaves(kTRUE);

   pWorld->AddNode(pVolDipole, 1, transDipole);
   pWorld->AddNode(pVolFeyoke, 2, yokeTransform);
   pWorld->AddNode(pVolFeyoke, 3, yokeTransform1);

}
*/


// Correct geometry Self/made from
// Parameter definition
// need better description for ALADIN parameters
// <D.Bertini@gsi.de>

/*
void R3BMagnet::ConstructGeometry(){
// off-file definition
   Double_t dx,dy,dz;
   Double_t dx1, dx2, dy1, dy2;
   Double_t vert[20], par[20];
   Double_t theta, phi, h1, bl1, tl1, alpha1, h2, bl2, tl2, alpha2;
   Double_t twist;
   Double_t origin[3];
   Double_t rmin, rmax, rmin1, rmax1, rmin2, rmax2;
   Double_t r, rlo, rhi;
   Double_t a,b;
   Double_t point[3], norm[3];
   Double_t rin, stin, rout, stout;
   Double_t thx, phx, thy, phy, thz, phz;
   Double_t alpha, theta1, theta2, phi1, phi2, dphi;
   Double_t tr[3], rot[9];
   Double_t z, density, radl, absl, w;
   Double_t lx,ly,lz,tx,ty,tz;
   Double_t xvert[50], yvert[50];
   Double_t zsect,x0,y0,scale0;
   Int_t nel, numed, nz, nedges, nvert;
   TGeoBoolNode *pBoolNode = 0;

   // Mixture: Air
   nel     = 2;
   density = 0.001290;
   TGeoMixture*
   pMat2 = new TGeoMixture("Air", nel,density);
      a = 14.006740;   z = 7.000000;   w = 0.700000;  // N
   pMat2->DefineElement(0,a,z,w);
      a = 15.999400;   z = 8.000000;   w = 0.300000;  // O
   pMat2->DefineElement(1,a,z,w);
   pMat2->SetIndex(700);
// Medium: Air
   numed   = 1;  // medium number
   par[0]  = 0.000000; // isvol
   par[1]  = 0.000000; // ifield
   par[2]  = 0.000000; // fieldm
   par[3]  = 0.000000; // tmaxfd
   par[4]  = 0.000000; // stemax
   par[5]  = 0.000000; // deemax
   par[6]  = 0.000000; // epsil
   par[7]  = 0.000000; // stmin
   TGeoMedium*
   pMed2 = new TGeoMedium("Air", numed,pMat2, par);
// Material: Iron
   a       = 55.850000;
   z       = 26.000000;
   density = 7.870000;
   radl    = 1.757717;
   absl    = 169.994418;
   TGeoMaterial*
   pMat24 = new TGeoMaterial("Iron", a,z,density,radl,absl);
   pMat24->SetIndex(701);
// Medium: Iron
   numed   = 23;  // medium number
   par[0]  = 0.000000; // isvol
   par[1]  = 0.000000; // ifield
   par[2]  = 0.000000; // fieldm
   par[3]  = 0.000000; // tmaxfd
   par[4]  = 0.000000; // stemax
   par[5]  = 0.000000; // deemax
   par[6]  = 0.000000; // epsil
   par[7]  = 0.000000; // stmin
   TGeoMedium*
   pMed24 = new TGeoMedium("Iron", numed,pMat24, par);

   // TRANSFORMATION MATRICES
   // Combi transformation: 
   dx = 0.000000;
   dy = 50.000000;
   dz = 230.060000;
   // Rotation: 
   thx = 97.300000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 7.300000;    phz = 0.000000;
   TGeoRotation *pMatrix3 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix2 = new TGeoCombiTrans("", dx,dy,dz,pMatrix3);
   // Combi transformation: 
   dx = 0.000000;
   dy = -50.000000;
   dz = 230.060000;
   // Rotation: 
   thx = 97.300000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 7.300000;    phz = 0.000000;
   TGeoRotation *pMatrix5 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix4 = new TGeoCombiTrans("", dx,dy,dz,pMatrix5);
   // Combi transformation: 
   dx = 102.165128;
   dy = 0.000000;
   dz = 216.972345;
   // Rotation: 
   thx = 97.300000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 7.300000;    phz = 0.000000;
   TGeoRotation *pMatrix7 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix6 = new TGeoCombiTrans("", dx,dy,dz,pMatrix7);
   // Combi transformation: 
   dx = -102.165128;
   dy = 0.000000;
   dz = 243.147655;
   // Rotation: 
   thx = 97.300000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 7.300000;    phz = 0.000000;
   TGeoRotation *pMatrix9 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix8 = new TGeoCombiTrans("", dx,dy,dz,pMatrix9);


   // Geometry description

   TGeoVolume *top =  gGeoManager->GetTopVolume();

   // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
   // Shape: FeYoke_up type: TGeoBBox
   dx = 78.000000;
   dy = 25.000000;
   dz = 88.000000;
   TGeoShape *pFeYoke_up_2 = new TGeoBBox("FeYoke_up", dx,dy,dz);
   // Volume: Feyoke_up
   TGeoVolume*
   pFeyoke_up_82ab1a8 = new TGeoVolume("Feyoke_up",pFeYoke_up_2, pMed24);
   pFeyoke_up_82ab1a8->SetVisLeaves(kTRUE);
   top->AddNode(pFeyoke_up_82ab1a8, 1, pMatrix2);
   // Shape: FeYoke_down type: TGeoBBox
   dx = 78.000000;
   dy = 25.000000;
   dz = 88.000000;
   TGeoShape *pFeYoke_down_3 = new TGeoBBox("FeYoke_down", dx,dy,dz);
   // Volume: Feyoke_down
   TGeoVolume*
   pFeyoke_down_82ab348 = new TGeoVolume("Feyoke_down",pFeYoke_down_3, pMed24);
   pFeyoke_down_82ab348->SetVisLeaves(kTRUE);
   top->AddNode(pFeyoke_down_82ab348, 2, pMatrix4);
   // Shape: FeYoke_left type: TGeoBBox
   dx = 25.000000;
   dy = 75.000000;
   dz = 88.000000;
   TGeoShape *pFeYoke_left_4 = new TGeoBBox("FeYoke_left", dx,dy,dz);
   // Volume: Feyoke_left
   TGeoVolume*
   pFeyoke_left_82ab4e8 = new TGeoVolume("Feyoke_left",pFeYoke_left_4, pMed24);
   pFeyoke_left_82ab4e8->SetVisLeaves(kTRUE);
   top->AddNode(pFeyoke_left_82ab4e8, 3, pMatrix6);
   // Shape: FeYoke_right type: TGeoBBox
   dx = 25.000000;
   dy = 75.000000;
   dz = 88.000000;
   TGeoShape *pFeYoke_right_5 = new TGeoBBox("FeYoke_right", dx,dy,dz);
   // Volume: Feyoke_right
   TGeoVolume*
   pFeyoke_right_82ab690 = new TGeoVolume("Feyoke_right",pFeYoke_right_5, pMed24);
   pFeyoke_right_82ab690->SetVisLeaves(kTRUE);
   top->AddNode(pFeyoke_right_82ab690, 4, pMatrix8);

}

*/

Bool_t R3BMagnet::CheckIfSensitive(std::string name){
	// just to get rid of the warrning during run, not need this is a passive element! 
	return kTRUE;
}

void R3BMagnet::ConstructASCIIGeometry(){
	FairGeoLoader *loader=FairGeoLoader::Instance();
	FairGeoInterface *GeoInterface =loader->getGeoInterface();
	R3BGeoMagnet *MGeo=new R3BGeoMagnet();
	MGeo->setGeomFile(GetGeometryFileName());
	GeoInterface->addGeoModule(MGeo);
	Bool_t rc = GeoInterface->readSet(MGeo);
	if ( rc ) MGeo->create(loader->getGeoBuilder());

        TList* volList = MGeo->getListOfVolumes();
        // store geo parameter
        FairRun *fRun = FairRun::Instance();
        FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
        R3BGeoPassivePar* par=(R3BGeoPassivePar*)(rtdb->getContainer("R3BGeoPassivePar"));
        TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
        TObjArray *fPassNodes = par->GetGeoPassiveNodes();

        TListIter iter(volList);
        FairGeoNode* node   = NULL;
        FairGeoVolume *aVol=NULL;

        while( (node = (FairGeoNode*)iter.Next()) ) {
            aVol = dynamic_cast<FairGeoVolume*> ( node );
            if ( node->isSensitive()  ) {
                fSensNodes->AddLast( aVol );
            }else{
                fPassNodes->AddLast( aVol );
            }
        }
	ProcessNodes( volList );
        par->setChanged();
        par->setInputVersion(fRun->GetRunId(),1);	
}


ClassImp(R3BMagnet)














