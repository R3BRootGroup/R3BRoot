
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
#include "TGeoTube.h"
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
fGeometryVersion=1;
fMedium=0;
}

void R3BMagnet::ConstructGeometry(){

  if(fGeometryVersion==2){
                          ConstructGeometry2();   
                         }else{
                               ConstructGeometry1();
                              }

}


void R3BMagnet::ConstructGeometry1(){

    Double_t degrad = TMath::Pi()/180.;
    Double_t w       =        0.;
    Double_t a       =        0.;
    Double_t z       =        0.;
    Double_t density =        0.;
    Double_t radl    =        0.;
    Double_t absl    =        0.;
    Int_t numed      =        0 ;
    Double_t dx,dy,dz;
    Double_t pDx1,pDx2,pDx3,pDx4,pDy1,pDy2,pDz,pTheta,pPhi,pAlp1,pAlp2;
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
       par[1]  = 1.000000; // ifield
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
      
      
    // -- Mixture: Steel
    TGeoMedium * pMedSteel=NULL;
   if (gGeoManager->GetMedium("Steel") ){
       pMedSteel=gGeoManager->GetMedium("Steel");
   }else{
     Int_t nel     = 3;
     density = 7.9;
     TGeoMixture*
	 pMatSteel = new TGeoMixture("Steel", nel,density);
     a = 55.845000;   z = 26.000000;  w = 0.710000;  // Fe
     pMatSteel->DefineElement(0,a,z,w);
     a = 51.996100;   z = 24.000000;   w = 0.180000;  // Cr
     pMatSteel->DefineElement(1,a,z,w);
     a = 58.693400;   z = 28.000000;   w = 0.110000;  // Ni
     pMatSteel->DefineElement(2,a,z,w);
     pMatSteel->SetIndex(2);
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
     pMedSteel = new TGeoMedium("Steel", numed,pMatSteel, par );
   }
   
   
   // -- Mixture: Mylar: C10H8O4
    TGeoMedium * pMedMylar=NULL;
   if (gGeoManager->GetMedium("Mylar") ){
       pMedMylar=gGeoManager->GetMedium("Mylar");
   }else{
     Int_t nel     = 3;
     density = 1.4;
     TGeoMixture*
	 pMatMylar = new TGeoMixture("Mylar", nel,density);
     a = 12.010700;   z = 6.000000;  w = 0.454545;  // C from formula 10C is 45.4545%
     pMatMylar->DefineElement(0,a,z,w);
     a = 1.007940;   z = 1.000000;   w = 0.363636;  // H from formula 8H is 36.33636% 
     pMatMylar->DefineElement(1,a,z,w);
     a = 15.999400;   z = 8.000000;   w = 0.181819;  // O from formula 4O is 18.1818%
     pMatMylar->DefineElement(2,a,z,w);
     pMatMylar->SetIndex(17);
     numed   = 31;  // medium number
     Double_t par[8];
     par[0]  = 0.000000; // isvol
     par[1]  = 0.000000; // ifield
     par[2]  = 0.000000; // fieldm
     par[3]  = 0.000000; // tmaxfd
     par[4]  = 0.000000; // stemax
     par[5]  = 0.000000; // deemax
     par[6]  = 0.000100; // epsil
     par[7]  = 0.000000; // stmin
     pMedMylar = new TGeoMedium("Mylar", numed,pMatMylar, par );
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
   dz = 374.5;
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
   ab[4][0] = 78.55;	ab[4][1] = -30.0;
   ab[5][0] = 78.55;	ab[5][1] = 30.0;
   ab[6][0] = -78.55;	ab[6][1] = 30.0;
   ab[7][0] = -78.55;	ab[7][1] = -30.0;
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
    
       
   
   pAWorld->AddNode(pHeliumAladinChamberLog1,1,pMatrix61);
   pAWorld->AddNode(pHeliumAladinChamberLog2,1,pMatrix62);
   pAWorld->AddNode(pHeliumAladinChamberLog3,1,pMatrix63);
   
//   pAWorld->AddNode(pAladinChamberLog1,1,pMatrix58);
//   pAWorld->AddNode(pAladinChamberLog2,1,pMatrix59);
//   pAWorld->AddNode(pAladinChamberLog3,1,pMatrix60);
 
   pAWorld->AddNode(pinWINLog,1,pMatrix64);
   pAWorld->AddNode(poutWINLog,1,pMatrix65);
   
   pAWorld->AddNode(pinHELLog,1,pMatrix66);
   
}


void R3BMagnet::ConstructGeometry2(){

    cout<<"-I- R3BMagnet::ConstructGeometry2()"  <<endl;


    Double_t dx,dy,dz;
    Double_t thx, phx, thy, phy, thz, phz;
    Double_t h1, bl1, tl1, alpha1, h2, bl2, tl2, alpha2, ddz;
    Double_t phi1, phi2,theta,phi,rmax,rmin;
    Double_t degrad = TMath::Pi()/180.;
    Double_t w       =        0.;
    Double_t a       =        0.;
    Double_t z       =        0.;
    Double_t density =        0.;
    Double_t radl    =        0.;
    Double_t absl    =        0.;
    Int_t numed      =        0 ;

   // Specific Material definition
   // --  Material: Iron
   TGeoMedium * pMedFe=NULL;
   if (gGeoManager->GetMedium("Iron") ){
       cout << "-I- TGeoManager: Iron Medium already defined " << endl;
       pMedFe=gGeoManager->GetMedium("Iron");
   }else{
    w       =        0.;
    a       = 55.850000;
    z       = 26.000000;
    density = 7.870000;
    TGeoMaterial*
	pMatFe = new TGeoMaterial("Iron", a,z,density);
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


   // -- Material: Aluminium
   TGeoMedium   * pMedAl=NULL;
   TGeoMaterial * pMatAl=NULL;
   if (gGeoManager->GetMedium("aluminium") ){
       cout << "-I- TGeoManager: Aluminium Medium already defined " << endl;
       pMedAl = gGeoManager->GetMedium("aluminium");
   }else{
    a       = 26.980000;
    z       = 13.000000;
    density = 2.7000000;
    pMatAl = new TGeoMaterial("aluminium", a,z,density);
    pMatAl->SetIndex(601);
    numed   = 27;  // medium number
    Double_t par[8];
    par[0]  = 0.000000; // isvol
    par[1]  = 0.000000; // ifield
    par[2]  = 0.000000; // fieldm
    par[3]  = 0.000000; // tmaxfd
    par[4]  = 0.000000; // stemax
    par[5]  = 0.000000; // deemax
    par[6]  = 0.000100; // epsil
    par[7]  = 0.000000; // stmin
    pMedAl  = new TGeoMedium("aluminium", numed,pMatAl, par);
   }

    // -- Mixture: Mylar
    TGeoMedium * pMedMylar=NULL;
    if (gGeoManager->GetMedium("mylar") ){
           cout << "-I- TGeoManager: Mylar Medium already defined " << endl;
	   pMedMylar=gGeoManager->GetMedium("mylar");
	}else{ 
		int nel     = 3;
		density = 1.390000;
		TGeoMixture*
		pMylarMaterial = new TGeoMixture("Mylar", nel,density);
		a = 12.010700;   z = 6.000000;   w = 0.363636;  // C
		pMylarMaterial->DefineElement(0,a,z,w);
		a = 1.007940;   z = 1.000000;   w = 0.454545;  // H
		pMylarMaterial->DefineElement(1,a,z,w);
		a = 15.999400;   z = 8.000000;   w = 0.181818;  // O
		pMylarMaterial->DefineElement(2,a,z,w);
		// Medium: Mylar
		numed   = 805;  // medium number
		pMylarMaterial->SetIndex(numed);
		Double_t par[8];
		par[0]  = 0.000000; // isvol
		par[1]  = 0.000000; // ifield
		par[2]  = 0.000000; // fieldm
		par[3]  = 0.000000; // tmaxfd
		par[4]  = 0.000000; // stemax
		par[5]  = 0.000000; // deemax
		par[6]  = 0.000100; // epsil
		par[7]  = 0.000000; // stmin
		pMedMylar = new TGeoMedium("Mylar", numed, pMylarMaterial, par);
	}

   // -- Mixture: Air
   TGeoMedium * pMed2=NULL;
   if (gGeoManager->GetMedium("Air") ){
       cout << "-I- TGeoManager: Air Medium already defined " << endl;
       pMed2=gGeoManager->GetMedium("Air");
   }else{
     Int_t nel     = 2;
     density = 0.0012900;
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
   
  
   // -- Material: HeliumGas
   TGeoMedium * pMed4=NULL;
   if (gGeoManager->GetMedium("HeliumGas") ){
       cout << "-I- TGeoManager: Helium Gas Medium already defined " << endl;
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
       par[1]  = 1.000000; // ifield
       par[2]  = 0.000000; // fieldm
       par[3]  = 0.000000; // tmaxfd
       par[4]  = 0.000000; // stemax
       par[5]  = 0.000000; // deemax
       par[6]  = 0.000100; // epsil
       par[7]  = 0.000000; // stmin

     pMed4 = new TGeoMedium("HeliumGas", numed,pMat4,par);
   }   

   //-- Material: Vacuum
   TGeoMedium * pMedVa=NULL;
   if (gGeoManager->GetMedium("vacuum") ){
       cout << "-I- TGeoManager: Vacuum Medium already defined " << endl;
       pMedVa=gGeoManager->GetMedium("vacuum");
   }else{
    a       = 1.e-16;
    z       = 1.e-16;
    density = 1.e-16;
    TGeoMaterial*
    pMatVa = new TGeoMaterial("vacuum", a,z,density);
    pMatVa->SetIndex(1212);
    numed   =1212;  // medium number
    Double_t par[8];
       par[0]  = 0.000000; // isvol
       par[1]  = 0.000000; // ifield
       par[2]  = 0.000000; // fieldm
       par[3]  = 0.000000; // tmaxfd
       par[4]  = 0.000000; // stemax
       par[5]  = 0.000000; // deemax
       par[6]  = 0.000100; // epsil
       par[7]  = 0.000000; // stmin
       pMedVa = new TGeoMedium("vacuum", numed,pMatVa,par);
   }

   // -- Dimensions and distances --
   Double_t Aladin_width = 156.0;    // cm
   Double_t Aladin_length = 176.0;   // cm, Field length is 1.4 m
   Double_t Aladin_gap = 50.;        // cm
   Double_t Aladin_angle = -7.0;     // degree
   Double_t DistanceToTarget = 350.0;// cm
   Double_t Yoke_thickness = 50.;    // cm
   Double_t Correction = -95.0;      // cm
   Double_t DistanceFromtargetToAladinCenter
	     = DistanceToTarget + Correction;

   // Transformations
   TGeoRotation *rot_aladin = new TGeoRotation("Aladinrot");
   rot_aladin->RotateY(Aladin_angle);


   // -- Select Gap medium --
   cout<<"-I- R3BMagnet::SetGapMedium ";
   TGeoMedium* GapMedium=NULL;

   if(fMedium==2){ 
                  GapMedium=pMed4;
                  cout<< " Helium" <<endl;
                  }else{
                        if(fMedium==1){
                                        GapMedium=pMedVa;
                                        cout<< " Vacuum" <<endl;
                                        }else{
                                             GapMedium=pMed2;
                                             cout<< " Air" <<endl;
                                             }
                       }  


   // -- Geometry description --
   TGeoVolume *pAWorld =  gGeoManager->GetTopVolume();

   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = DistanceToTarget+Correction;

   TGeoCombiTrans* pMatrix = new TGeoCombiTrans("", dx,dy,dz,rot_aladin);

   // Shape: pCWorld type: TGeoBBox
   dx = Aladin_width+2.0*Yoke_thickness;
   dy = Aladin_gap+2.0*Yoke_thickness;
   dz = Aladin_length+64.0;
   TGeoShape *pCWorld = new TGeoBBox("Aladin", dx/2.,dy/2.,dz/2.);
   // Volume:
   TGeoVolume* pWorld = new TGeoVolume("Aladin Box",pCWorld, GapMedium);
   // Position Volume
   TGeoCombiTrans* pGlobal = GetGlobalPosition(pMatrix);
   pAWorld->AddNodeOverlap(pWorld, 0, pGlobal);
   

   
   // -- Aladin: Iron constrains of the magnet --

   // Transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 0.000000;
   TGeoCombiTrans* pMatrix1 = new TGeoCombiTrans("", dx,dy,dz,0);

   // Shape: TGeoBBox
   dx = 156.0+144.0+100.0;
   dy = 150.0+150.0;
   dz = Aladin_length;
   TGeoShape *iron = new TGeoBBox("iron", dx/2.,dy/2.,dz/2.);

   // Shape: TGeoBBox
   dx = Aladin_width;
   dy = Aladin_gap;
   dz = Aladin_length+2.0;
   TGeoShape *iron2 = new TGeoBBox("iron2", dx/2.,dy/2.,dz/2.);

   // Subtraction:
   TGeoSubtraction* sub20 = new TGeoSubtraction(iron,iron2,pMatrix1,pMatrix1);
   TGeoShape *sub20s = new TGeoCompositeShape("GE", sub20);

   // Transformation: 
   dx = 0.000000;
   dy = 130.5000;
   dz = 0.000000;
   TGeoCombiTrans* pMatrix2 = new TGeoCombiTrans("", dx,dy,dz,0);

   // Shape: TGeoTrap
   ddz    = 180.000;
   theta  = 0.0;
   phi    = 0.0;
   h1     = 40.000;
   bl1    = 60.000;
   tl1    = 80.000;
   alpha1 = 0.000000;
   h2     = 40.000;
   bl2    = 60.0000;
   tl2    = 80.0000;
   alpha2 = 0.000000;
   TGeoShape *iron3 = new TGeoTrap("iron3", ddz/2.0,theta,phi,h1/2.0,bl1/2.0,tl1/2.0,alpha1,h2/2.0,bl2/2.0,tl2/2.0,alpha2);

   // Subtraction:
   TGeoSubtraction* sub21 = new TGeoSubtraction(sub20s,iron3,pMatrix1,pMatrix2);
   TGeoShape *sub21s = new TGeoCompositeShape("GE", sub21);

   // Transformation: 
   dx = 200.0000;
   dy = 122.0000;
   dz = 0.000000;
   TGeoCombiTrans* pMatrix3 = new TGeoCombiTrans("", dx,dy,dz,0);

   // Shape: TGeoTrap
   ddz    = 180.000;
   theta  = 0.0;
   phi    = 0.0;
   h1     = 60.000;
   bl1    = 1.000;
   tl1    = 100.000;
   alpha1 = 0.000000;
   h2     = 60.000;
   bl2    = 1.0000;
   tl2    = 100.0000;
   alpha2 = 0.000000;
   TGeoShape *iron4 = new TGeoTrap("iron4", ddz/2.0,theta,phi,h1/2.0,bl1/2.0,tl1/2.0,alpha1,h2/2.0,bl2/2.0,tl2/2.0,alpha2);

   // Subtraction:
   TGeoSubtraction* sub22 = new TGeoSubtraction(sub21s,iron4,pMatrix1,pMatrix3);
   TGeoShape *sub22s = new TGeoCompositeShape("GE", sub22);

   // Transformation: 
   dx = -200.0000;
   dy = 122.0000;
   dz = 0.000000;
   TGeoCombiTrans* pMatrix4 = new TGeoCombiTrans("", dx,dy,dz,0);

   // Shape: TGeoTrap
   ddz    = 180.000;
   theta  = 0.0;
   phi    = 0.0;
   h1     = 60.000;
   bl1    = 1.000;
   tl1    = 100.000;
   alpha1 = 0.000000;
   h2     = 60.000;
   bl2    = 1.0000;
   tl2    = 100.0000;
   alpha2 = 0.000000;
   TGeoShape *sub4 = new TGeoTrap("sub4", ddz/2.0,theta,phi,h1/2.0,bl1/2.0,tl1/2.0,alpha1,h2/2.0,bl2/2.0,tl2/2.0,alpha2);

   // Subtraction:
   TGeoSubtraction* sub23 = new TGeoSubtraction(sub22s,sub4,pMatrix1,pMatrix4);
   TGeoShape *sub23s = new TGeoCompositeShape("GE", sub23);

   // Volume:
   TGeoVolume* magnet_log = new TGeoVolume("magnet_log",sub23s, pMedFe);
   magnet_log->SetVisLeaves(kTRUE);
   magnet_log->SetLineColor(4);

   // Position Volume
   TGeoCombiTrans* pGlobal1 = GetGlobalPosition(pMatrix1);
   pWorld->AddNode(magnet_log, 0, pGlobal1);
   
   


   // -- Aladin: Chamber --

   // Combi transformation: 
   dx = 120.8/2.0+2.4+1.0;
   dy = 0.000000;
   dz = -48.750000+17.45;
   TGeoCombiTrans* pMatrix5 = new TGeoCombiTrans("", dx,dy,dz,0);
 
   // Shape: TGeoTrap
   ddz    = 178.900;
   theta  = 2.74;
   phi    = 0.0;
   h1     = 42.000;
   bl1    = 1.000;
   tl1    = 1.000;
   alpha1 = 0.000000;
   h2     = 43.6000;
   bl2    = 1.0000;
   tl2    = 1.0000;
   alpha2 = 0.000000;
   dx = 2.0;
   dy = 42.000;
   dz = 144.00;
   TGeoShape *cham1 = new TGeoTrap("cham1", ddz/2.0,theta,phi,h1/2.0,bl1/2.0,tl1/2.0,alpha1,h2/2.0,bl2/2.0,tl2/2.0,alpha2);
   
   // Volume:
   TGeoVolume*
   cham_log1 = new TGeoVolume("cham_log1",cham1, pMedFe);
   cham_log1->SetVisLeaves(kTRUE);
   cham_log1->SetLineColor(17);

   // Position Volume
   TGeoCombiTrans* pGlobal5 = GetGlobalPosition(pMatrix5);
   pWorld->AddNode(cham_log1, 0, pGlobal5);

   // Combi transformation: 
   dx = -120.8/2.0-2.4-1.0;
   dy = 0.000000;
   dz = -48.750000+17.45;
   TGeoCombiTrans* pMatrix6 = new TGeoCombiTrans("", dx,dy,dz,0);

   // Shape: TGeoTrap
   ddz     = 178.900;
   theta  = -2.74;
   phi    = 0.0;
   h1     = 42.000;
   bl1    = 1.000;
   tl1    = 1.000;
   alpha1 = 0.000000;
   h2     = 43.6000;
   bl2    = 1.0000;
   tl2    = 1.0000;
   alpha2 = 0.000000;
   dx = 2.0;
   dy = 42.000;
   dz = 144.00;
   TGeoShape *cham2 = new TGeoTrap("cham2", ddz/2.0,theta,phi,h1/2.0,bl1/2.0,tl1/2.0,alpha1,h2/2.0,bl2/2.0,tl2/2.0,alpha2);

   // Volume:
   TGeoVolume*
   cham_log2 = new TGeoVolume("cham_log2",cham2, pMedFe);
   cham_log2->SetVisLeaves(kTRUE);
   cham_log2->SetLineColor(17);

   // Position Volume
   TGeoCombiTrans* pGlobal6 = GetGlobalPosition(pMatrix6);
   pWorld->AddNode(cham_log2, 0, pGlobal6);

   // Combi transformation: 
   dx = 0.0;
   dy = 22.00-1.0;
   dz = -48.75000+34.9/2.;
   TGeoCombiTrans* pMatrix7 = new TGeoCombiTrans("", dx,dy,dz,0);

   // Shape: TGeoTrap
   ddz    = 178.900;
   theta  = 0.31;
   phi    = 90.0;
   h1     = 1.000;
   bl1    = 118.000;
   tl1    = 118.000;
   alpha1 = 0.000000;
   h2     = 1.000;
   bl2    = 135.2000;
   tl2    = 135.2000;
   alpha2 = 0.000000;
   TGeoShape *cham3 = new TGeoTrap("cham3", ddz/2.0,theta,phi,h1/2.0,bl1/2.0,tl1/2.0,alpha1,h2/2.0,bl2/2.0,tl2/2.0,alpha2);

   // Volume:
   TGeoVolume*
   cham_log3 = new TGeoVolume("cham_log3",cham3, pMedFe);
   cham_log3->SetVisLeaves(kTRUE);
   cham_log3->SetLineColor(17);

   // Position Volume
   TGeoCombiTrans* pGlobal7 = GetGlobalPosition(pMatrix7);
   pWorld->AddNode(cham_log3, 0,  pGlobal7);

   // Combi transformation: 
   dx = 0.0;
   dy = -22.00+1.0;
   dz = -48.75000+34.9/2.;
   TGeoCombiTrans* pMatrix8 = new TGeoCombiTrans("", dx,dy,dz,0);

   // Shape: TGeoTrap
   ddz    = 178.900;
   theta  = -0.31;
   phi    = 90.0;
   h1     = 1.000;
   bl1    = 118.000;
   tl1    = 118.000;
   alpha1 = 0.000000;
   h2     = 1.000;
   bl2    = 135.2000;
   tl2    = 135.2000;
   alpha2 = 0.000000;
   TGeoShape *cham4 = new TGeoTrap("cham4", ddz/2.0,theta,phi,h1/2.0,bl1/2.0,tl1/2.0,alpha1,h2/2.0,bl2/2.0,tl2/2.0,alpha2);

   // Volume:
   TGeoVolume*
   cham_log4 = new TGeoVolume("cham_log4",cham4, pMedFe);
   cham_log4->SetVisLeaves(kTRUE);
   cham_log4->SetLineColor(17);

   // Position Volume
   TGeoCombiTrans* pGlobal8 = GetGlobalPosition(pMatrix8);
   pWorld->AddNode(cham_log4, 0,  pGlobal8);

   // Combi transformation: 
   dx = 0.0;
   dy = -22.00-2.1;
   dz = 88.45;
   TGeoCombiTrans* pMatrix9 = new TGeoCombiTrans("", dx,dy,dz,0);

   // Shape: TGeoTrap
   ddz    = 60.600;
   theta  = -4.8;
   phi    = 90.0;
   h1     = 1.00;
   bl1    = 137.200;
   tl1    = 137.200;
   alpha1 = 0.000000;
   h2     = 1.00;
   bl2    = 157.1000;
   tl2    = 157.1000;
   alpha2 = 0.000000;
   TGeoShape *cham5 = new TGeoTrap("cham5", ddz/2.0,theta,phi,h1/2.0,bl1/2.0,tl1/2.0,alpha1,h2/2.0,bl2/2.0,tl2/2.0,alpha2);

   // Volume:
   TGeoVolume*
   cham_log5 = new TGeoVolume("cham_log5",cham5, pMedFe);
   cham_log5->SetVisLeaves(kTRUE);
   cham_log5->SetLineColor(17);

   // Position Volume
   TGeoCombiTrans* pGlobal9 = GetGlobalPosition(pMatrix9);
   pWorld->AddNode(cham_log5, 0,  pGlobal9);

   // Combi transformation: 
   dx = 0.0;
   dy = 22.00+2.1;
   dz = 88.45;
   TGeoCombiTrans* pMatrix10 = new TGeoCombiTrans("", dx,dy,dz,0);

   // Shape: TGeoTrap
   ddz    = 60.600;
   theta  = 4.8;
   phi    = 90.0;
   h1     = 1.00;
   bl1    = 137.200;
   tl1    = 137.200;
   alpha1 = 0.000000;
   h2     = 1.00;
   bl2    = 157.1000;
   tl2    = 157.1000;
   alpha2 = 0.000000;
   TGeoShape *cham6 = new TGeoTrap("cham6", ddz/2.0,theta,phi,h1/2.0,bl1/2.0,tl1/2.0,alpha1,h2/2.0,bl2/2.0,tl2/2.0,alpha2);

   // Volume:
   TGeoVolume*
   cham_log6 = new TGeoVolume("cham_log6",cham6, pMedFe);
   cham_log6->SetVisLeaves(kTRUE);
   cham_log6->SetLineColor(17);

   // Position Volume
   TGeoCombiTrans* pGlobal10 = GetGlobalPosition(pMatrix10);
   pWorld->AddNode(cham_log6, 0,  pGlobal10);

   // Combi transformation: 
   dx = 139.0/2.0+2.6+1.0;
   dy = 0.000000;
   dz = 71.0000+35.1/2.0;
   TGeoCombiTrans* pMatrix11 = new TGeoCombiTrans("", dx,dy,dz,0);
 
   // Shape: TGeoTrap
   ddz    = 60.600;
   theta  = 9.3;
   phi    = 0.0;
   h1     = 42.000;
   bl1    = 1.000;
   tl1    = 1.000;
   alpha1 = 0.000000;
   h2     = 52.4000;
   bl2    = 1.0000;
   tl2    = 1.0000;
   alpha2 = 0.000000;
   TGeoShape *cham7 = new TGeoTrap("cham7", ddz/2.0,theta,phi,h1/2.0,bl1/2.0,tl1/2.0,alpha1,h2/2.0,bl2/2.0,tl2/2.0,alpha2);

   // Volume:
   TGeoVolume*
   cham_log7 = new TGeoVolume("cham_log7",cham7, pMedFe);
   cham_log7->SetVisLeaves(kTRUE);
   cham_log7->SetLineColor(17);

   // Position Volume
   TGeoCombiTrans* pGlobal11 = GetGlobalPosition(pMatrix11);
   pWorld->AddNode(cham_log7, 0, pGlobal11);

   // Combi transformation: 
   dx = -139.0/2.0-2.6-1.0;
   dy = 0.000000;
   dz = 71.0000+35.1/2.0;
   TGeoCombiTrans* pMatrix12 = new TGeoCombiTrans("", dx,dy,dz,0);
 
   // Shape: TGeoTrap
   ddz    = 60.600;
   theta  = -9.3;
   phi    = 0.0;
   h1     = 42.000;
   bl1    = 1.000;
   tl1    = 1.000;
   alpha1 = 0.000000;
   h2     = 52.4000;
   bl2    = 1.0000;
   tl2    = 1.0000;
   alpha2 = 0.000000;
   TGeoShape *cham8 = new TGeoTrap("cham8", ddz/2.0,theta,phi,h1/2.0,bl1/2.0,tl1/2.0,alpha1,h2/2.0,bl2/2.0,tl2/2.0,alpha2);

   // Volume:
   TGeoVolume*
   cham_log8 = new TGeoVolume("cham_log8",cham8, pMedFe);
   cham_log8->SetVisLeaves(kTRUE);
   cham_log8->SetLineColor(17);

   // Position Volume
   TGeoCombiTrans* pGlobal12 = GetGlobalPosition(pMatrix12);
   pWorld->AddNode(cham_log8, 0, pGlobal12);

   // Combi transformation: 
   dx = 0.0;
   dy = 0.00000;
   dz = -122.75;
   TGeoCombiTrans* pMatrix13 = new TGeoCombiTrans("", dx,dy,dz,0);

   // Shape: TGeoBBox
   dx = 128.8;
   dy = 50.0;
   dz = 4.00;
   TGeoShape *cham9 = new TGeoBBox("cham9", dx/2.,dy/2.,dz/2.);

    // Combi transformation: 
   dx = 0.0;
   dy = 0.00000;
   dz = -122.75;
   TGeoCombiTrans* pMatrix14 = new TGeoCombiTrans("", dx,dy,dz,0);

   // Shape: TGeoBBox
   dx = 116.8;
   dy = 40.0;
   dz = 8.00;
   TGeoShape *cham10 = new TGeoBBox("cham10", dx/2.,dy/2.,dz/2.);

    // Subtraction:
   TGeoSubtraction* subcham11 = new TGeoSubtraction(cham9,cham10,pMatrix13,pMatrix14);
   TGeoShape *cham11 = new TGeoCompositeShape("GE", subcham11);

   // Volume:
   TGeoVolume*
   cham_log11 = new TGeoVolume("cham_log11",cham11, pMedFe);
   cham_log11->SetVisLeaves(kTRUE);
   cham_log11->SetLineColor(17);

   // Position Volume
   pWorld->AddNode(cham_log11, 0, 0);

   
   // Combi transformation: 
   dx = 0.0;
   dy = 0.00000;
   dz = 119.75;
   TGeoCombiTrans* pMatrix15 = new TGeoCombiTrans("", dx,dy,dz,0);

   // Shape: TGeoBBox
   dx = 197.1;
   dy = 100.0;
   dz = 2.00;
   TGeoShape *cham12 = new TGeoBBox("cham12", dx/2.,dy/2.,dz/2.);

    // Combi transformation: 
   dx = 0.0;
   dy = 0.00000;
   dz = 119.75;
   TGeoCombiTrans* pMatrix16 = new TGeoCombiTrans("", dx,dy,dz,0);

   // Shape: TGeoBBox
   dx = 155.1;
   dy = 52.4;
   dz = 4.00;
   TGeoShape *cham13 = new TGeoBBox("cham13", dx/2.,dy/2.,dz/2.);

   // Subtraction:
   TGeoSubtraction* subcham14 = new TGeoSubtraction(cham12,cham13,pMatrix15,pMatrix16);
   TGeoShape *cham14 = new TGeoCompositeShape("GE", subcham14);

   // Volume:
   TGeoVolume*
   cham_log14 = new TGeoVolume("cham_log14",cham14, pMedFe);
   cham_log14->SetVisLeaves(kTRUE);
   cham_log14->SetLineColor(17);

   // Position Volume
   pWorld->AddNode(cham_log14, 0, 0);


   // Combi transformation: 
   dx = 0.0;
   dy = 0.00000;
   dz = -120.75;
   TGeoCombiTrans* pMatrix17 = new TGeoCombiTrans("", dx,dy,dz,0);

   // Shape: TGeoBBox
   dx = 260.0;
   dy = 150.0;
   dz = 6.00;
   TGeoShape *cham15 = new TGeoBBox("cham15", dx/2.,dy/2.,dz/2.);

   // Combi transformation: 
   dx = 0.0;
   dy = 0.00000;
   dz = -120.75;
   TGeoCombiTrans* pMatrix18 = new TGeoCombiTrans("", dx,dy,dz,0);

   // Shape: TGeoBBox
   dx = 128.8;
   dy = 50.0;
   dz = 10.00;
   TGeoShape *cham16 = new TGeoBBox("cham16", dx/2.,dy/2.,dz/2.);

   // Subtraction:
   TGeoSubtraction* subcham17 = new TGeoSubtraction(cham15,cham16,pMatrix17,pMatrix18);
   TGeoShape *cham17 = new TGeoCompositeShape("GE", subcham17);

   // Volume:
   TGeoVolume*
   cham_log17 = new TGeoVolume("cham_log17",cham17, pMedFe);
   cham_log17->SetVisLeaves(kTRUE);
   cham_log17->SetLineColor(5);

   // Position Volume
   pWorld->AddNode(cham_log17, 0, 0);


    // Combi transformation: 
   dx = 0.0;
   dy = 0.00000;
   dz = 116.75-1.0;
   TGeoCombiTrans* pMatrix19 = new TGeoCombiTrans("", dx,dy,dz,0);

   // Shape: TGeoBBox
   dx = 260.0;
   dy = 150.0;
   dz = 6.00;
   TGeoShape *cham18 = new TGeoBBox("cham18", dx/2.,dy/2.,dz/2.);

   // Combi transformation: 
   dx = 0.0;
   dy = 0.00000;
   dz = 116.75-1.0;
   TGeoCombiTrans* pMatrix20 = new TGeoCombiTrans("", dx,dy,dz,0);

   // Shape: TGeoBBox
   dx = 180.0;
   dy = 58.0;
   dz = 10.00;
   TGeoShape *cham19 = new TGeoBBox("cham19", dx/2.,dy/2.,dz/2.);

   // Subtraction:
   TGeoSubtraction* subcham20 = new TGeoSubtraction(cham18,cham19,pMatrix19,pMatrix20);
   TGeoShape *cham20 = new TGeoCompositeShape("GE", subcham20);

   // Volume:
   TGeoVolume*
   cham_log20 = new TGeoVolume("cham_log20",cham20, pMedFe);
   cham_log20->SetVisLeaves(kTRUE);
   cham_log20->SetLineColor(5);

   // Position Volume
   pWorld->AddNode(cham_log20, 0, 0);




   // -- Aladin: Windows --
   //
   // Iron      ->    4 cm
   // Aluminium ->    2 cm
   // Kapton    -> 0.05 mm
 
   // Combi transformation: 
   dx = -17.30000;
   dy = 0.00000;
   dz = -142.75;
   TGeoRotation *rot_t1 = new TGeoRotation("rot1");
   rot_t1->RotateY(7.0);
   TGeoCombiTrans* pMatrix1w = new TGeoCombiTrans("", dx,dy,dz,rot_t1);

   // Shape: TGeoTubeSeg
   rmin = 12.50;
   rmax = 67.00;
   dz   = 4.00000;
   phi1 = 0.000000;
   phi2 = 360.0000;
   TGeoShape *windows1 = new TGeoTubeSeg("windows1",rmin,rmax,dz/2.,phi1,phi2);

   // Volume:
   TGeoVolume*
   Windows_log1 = new TGeoVolume("Windows_log1",windows1, pMedFe);
   Windows_log1->SetVisLeaves(kTRUE);
   Windows_log1->SetLineColor(15);

   // Position Volume
   TGeoCombiTrans* pGlobal1w = GetGlobalPosition(pMatrix1w);
   pWorld->AddNode(Windows_log1, 0, pGlobal1w);

   // Combi transformation: 
   dx = -17.30000;
   dy = 0.00000;
   dz = -142.75-3.0;
   TGeoCombiTrans* pMatrix2w = new TGeoCombiTrans("", dx,dy,dz,rot_t1);

   // Shape: TGeoTubeSeg
   rmin = 12.50;
   rmax = 15.00;
   dz   = 2.00000;
   phi1 = 0.00000;
   phi2 = 360.000;
   TGeoShape *windows2 = new TGeoTubeSeg("windows2",rmin,rmax,dz/2.,phi1,phi2);

   // Volume:
   TGeoVolume*
   Windows_log2 = new TGeoVolume("Windows_log2",windows2, pMedAl);
   Windows_log2->SetVisLeaves(kTRUE);
   Windows_log2->SetLineColor(19);

   // Position Volume
   TGeoCombiTrans* pGlobal2w = GetGlobalPosition(pMatrix2w);
   pWorld->AddNode(Windows_log2, 0, pGlobal2w);

   // Combi transformation: 
   dx = -17.30000;
   dy = 0.00000;
   dz = -142.75-2.0;
   TGeoCombiTrans* pMatrix3w = new TGeoCombiTrans("", dx,dy,dz,rot_t1);

   // Shape: TGeoTubeSeg
   rmin = 0.000;
   rmax = 12.50;
   dz   = 0.00500;
   phi1 = 0.00000;
   phi2 = 360.000;
   TGeoShape *windows3 = new TGeoTubeSeg("windows3",rmin,rmax,dz/2.,phi1,phi2);

   // Volume:
   TGeoVolume*
   Windows_log3 = new TGeoVolume("Windows_log3",windows3, pMedMylar);
   Windows_log3->SetVisLeaves(kTRUE);
   Windows_log3->SetLineColor(28);

   // Position Volume
   TGeoCombiTrans* pGlobal3w = GetGlobalPosition(pMatrix3w);
   pWorld->AddNode(Windows_log3, 0, pGlobal3w);



}// closed ConstructGeometry2()


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














