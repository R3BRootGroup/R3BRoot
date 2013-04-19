//*-- AUTHOR : Marc Labiche
//*-- Created : 28/06/2012

#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairGeoRootBuilder.h"
#include "FairRuntimeDb.h"
#include "TObjArray.h"
#include "FairRun.h"
#include "FairGeoVolume.h"
#include "FairGeoNode.h"
#include "FairGeoMedia.h"
#include "R3BVacVesselCool.h"
#include "R3BGeoPassivePar.h"
#include "R3BGeoVacVesselCool.h"

// includes for modeling
#include "TGeoManager.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoMatrix.h"
#include "TGeoMaterial.h"
#include "TGeoCompositeShape.h"
#include "TGeoMedium.h"
#include "TGeoBBox.h"
#include "TGeoPara.h"
#include "TGeoTube.h"
#include "TGeoPgon.h"
#include "TGeoCone.h"
#include "TGeoTorus.h"
#include "TGeoBoolNode.h"


#include <iostream>
#include <TMath.h>

using namespace std;

R3BVacVesselCool::~R3BVacVesselCool()
{
if (fVacVesselCoolName ) delete fVacVesselCoolName;
}


R3BVacVesselCool::R3BVacVesselCool()
{
fVacVesselCoolName=NULL;
}

R3BVacVesselCool::R3BVacVesselCool(const char * name,  const char * title)
  : R3BModule(name ,title)
{

fVacVesselCoolName =  new TString(name);

}


void R3BVacVesselCool::ConstructGeometry(){

  if (*fVacVesselCoolName == "LiH") return  ConstructGeometry1();  // ie: For Liquid Hydrogen target only (targt4)

}

void R3BVacVesselCool::ConstructGeometry1(){

    cout << endl;
    cout << "-I- R3BVacVesselCool:: ConstructGeometry() "<< endl;
    //cout << "-I- R3BVacVesselCool VacVesselCool type: LiH target "<< endl;
    cout << "-I- R3BVacVesselCool VacVesselCool type: LiH target"<< endl;
    cout << endl;

  // test of out-of-file geometry definition

  Double_t dx, dy, dz;
  Double_t thx, thy, thz;
  Double_t phx, phy, phz;
  Double_t a, z, density;
  Double_t radl, absl; 
  //Double_t par[20];
  Int_t numed;

  TGeoMaterial *pMat=NULL;
  TGeoMedium   *pMed=NULL;

 

  if (gGeoManager->GetMedium("Copper") ){
       cout << "-I- TGeoManager: Copper Medium already defined " << endl;
       pMed = gGeoManager->GetMedium("Copper");
   }else{
    
  // Material definition
  // Material: Copper
   a       = 63.54;
   z       = 29.000000;
   density = 8.960000;
   pMat = new TGeoMaterial("Copper", a,z,density);
   pMat->SetIndex(600);

   // Medium: Copper
   numed   = 29;  // medium number
   Double_t par[8];
   par[0]  = 0.000000; // isvol
   par[1]  = 0.000000; // ifield
   par[2]  = 0.000000; // fieldm
   par[3]  = 0.000000; // tmaxfd
   par[4]  = 0.000000; // stemax
   par[5]  = 0.000000; // deemax
   par[6]  = 0.000100; // epsil
   par[7]  = 0.000000; // stmin
   pMed  = new TGeoMedium("Copper", numed,pMat, par);
   }

  // Material: Aluminium
   TGeoMaterial *pMa2t=NULL;
   TGeoMedium * pMed2=NULL;
   if (gGeoManager->GetMedium("Aluminum") ){
       pMed2=gGeoManager->GetMedium("Aluminum");
   }else{
     a       = 26.980000;
     z       = 13.000000;
     density = 2.700000;
     radl    = 8.875105;
     absl    = 388.793113;
     TGeoMaterial*
	 pMat2 = new TGeoMaterial("Aluminum", a,z,density,radl,absl);
     pMat2->SetIndex(20);
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
     pMed2 = new TGeoMedium("Aluminum", numed,pMat2, par);
   }

  // Material: StainSteel
   TGeoMedium * pMed3=NULL;
   if (gGeoManager->GetMedium("Steel") ){
       pMed3=gGeoManager->GetMedium("Steel");
   }else{
    // inox stainless steel, defined as a mixture
    // used for all metallic parts
    TGeoMixture *mix = new TGeoMixture("INOX", 9, 8.03 );
    mix->SetIndex(21);
     numed   = 21;  // medium number
    mix->DefineElement(0, 12.0107,  6., .0003);  // C  by fractional weight
    mix->DefineElement(1, 54.9380, 25., .02);    // Fe by fractional weight
    mix->DefineElement(2, 28.0855, 14., .01);    // Na by fractional weight
    mix->DefineElement(3, 30.9738, 15., .00045); // P  by fractional weight
    mix->DefineElement(4, 32.066 , 16., .0003);  // S  by fractional weight
    mix->DefineElement(5, 58.6928, 28., .12);    // Ni by fractional weight
    mix->DefineElement(6, 55.9961, 24., .17);    //    by fractional weight
    mix->DefineElement(7, 95.84  , 42., .025);   //    by fractional weight
    mix->DefineElement(8, 55.845 , 26., .654);   //    by fractional weight
    //mix->SetPressure();
    //mix->SetTemperature(290.);
    //mix->SetState(TGeoMaterial::kMatStateSolid);
    Double_t par[8];
    par[3] = 0.;
    par[4] = 0.;
    par[5] = 0.;
    par[6] = 0.;
    par[7] = 0.;
    pMed3 = new TGeoMedium("ITSspdStainlessSteel", numed, mix, par);
   }


   // TRANSFORMATION MATRICES
 
   TGeoRotation *pRot0 = new TGeoRotation("pRot0");
   pRot0->RotateZ(-15.);
   pRot0->RegisterYourself(); 

   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
    // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pRot1 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);

   dz=-11.0000; // z position of ring for Outer Si
   TGeoCombiTrans* pMatrix1 =
                   new TGeoCombiTrans("", dx,dy,dz,pRot1);
 
   TGeoRotation *pRot2 = new TGeoRotation("pRot2");
   pRot2->RotateZ(180.);
   pRot2->RegisterYourself(); 
   TGeoCombiTrans* pMatrix2 =
                   new TGeoCombiTrans("", dx,dy,dz,pRot2);


   dz=-13.0000;   // z position of ring for Middle Si
   TGeoRotation *pRot3 = new TGeoRotation("pRot3");
   pRot3->RotateZ(90.);
   pRot3->RegisterYourself(); 
   TGeoCombiTrans* pMatrix3 =
                   new TGeoCombiTrans("", dx,dy,dz,pRot3);

   TGeoRotation *pRot4 = new TGeoRotation("pRot4");
   pRot4->RotateZ(270.);
   pRot4->RegisterYourself(); 
   TGeoCombiTrans* pMatrix4 =
                   new TGeoCombiTrans("", dx,dy,dz,pRot4);

   dz = -8.200000; // z position of ring for Inner Si     
   TGeoCombiTrans* pMatrix5 =
                   new TGeoCombiTrans("", dx,dy,dz,pRot3);



    // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;     phz = 0.000000;
   TGeoRotation *pRot5 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);

   dx = -.3500000*cos(-15*3.14159/180)-7.75*sin(-15*3.14159/180); //    
   dy = -0.35*sin(-15*3.14159/180)+7.75*cos(-15*3.14159/180); //     
   //dz = -13.80000-0.5; //      // for end plate shadowing CALIFA
   dz = -18.80000-0.5; //      // for end plate at edge of CALIFA
   //dz = -28.80000-0.5; //        // for end plate position just behind califa
   TGeoCombiTrans* pMatrix6 =
                   new TGeoCombiTrans("", dx,dy,dz,pRot5);

   dx = -.350000*cos(-15*3.14159/180)-7.75*sin(-15*3.14159/180)-0.5; // x position of ring for Inner Si     
   dy = -0.35*sin(-15*3.14159/180)+7.75*cos(-15*3.14159/180)+0.1; // y position of ring for Inner Si     
   dz = -8.70000; // z position of ring for Inner Si
   TGeoRotation *pRotXZ0 = new TGeoRotation("pRotXZ0");
   pRotXZ0->RotateX(90.);
   pRotXZ0->RotateZ(-15.);
   pRotXZ0->RegisterYourself(); 
   TGeoCombiTrans* pMatrix16 =
                   new TGeoCombiTrans("", dx,dy,dz,pRotXZ0);

   dx = 0.3500000*cos(-15*3.14159/180)-7.75*sin(-15*3.14159/180); //   
   dy = 0.35*sin(-15*3.14159/180)+7.75*cos(-15*3.14159/180); //     
   //dz = -13.8000000-0.5;      // for end plate shadowing CALIFA
   dz = -18.8000000-0.5;      // for end plate at edge of CALIFA
   //dz = -28.8000000-0.5;       // for end plate position just behind califa
   TGeoCombiTrans* pMatrix7 =
                   new TGeoCombiTrans("", dx,dy,dz,pRot5);

   dx = 0.3500000*cos(-15*3.14159/180)-7.75*sin(-15*3.14159/180)+0.5; // x position of ring for Inner Si     
   dy = 0.35*sin(-15*3.14159/180)+7.75*cos(-15*3.14159/180)-0.15; // y position of ring for Inner Si     
   dz = -8.7000000; // z position of ring for Inner Si     
   TGeoRotation *pRotXYZ0 = new TGeoRotation("pRotXYZ0");
   pRotXYZ0->RotateX(90.);
   pRotXYZ0->RotateY(270.);
   pRotXYZ0->RotateZ(-15.);
   pRotXYZ0->RegisterYourself(); 
   TGeoCombiTrans* pMatrix17 =
                   new TGeoCombiTrans("", dx,dy,dz,pRotXYZ0);
 

   //////////////////////////////////////////////////////////////////////////
   dx = 24.50000; // x position of ring for Outer Si     
   dy = 2.21000; // y position of ring for Outer Si     
   //dz = -16.500000-0.5; //      // for end plate shadowing CALIFA 
   dz = -21.500000-0.5; //      // for end plate at edge of CALIFA 
   //dz = -31.500000-0.5; //       // for end plate position just behind califa
   TGeoCombiTrans* pMatrix12 =
                   new TGeoCombiTrans("", dx,dy,dz,pRot5);
   dy = 1.250000;
   dz= -12.;
   TGeoRotation *pRotY0 = new TGeoRotation("pRotY0");
   pRotY0->RotateY(90.);
   pRotY0->RegisterYourself(); 
   TGeoCombiTrans* pMatrix18 =
                   new TGeoCombiTrans("", dx,dy,dz,pRotY0);
   dz= -14.;
   TGeoRotation *pRotYX1 = new TGeoRotation("pRotYX1");
   pRotYX1->RotateY(90.);
   pRotYX1->RotateX(90.);
   pRotYX1->RegisterYourself(); 
   TGeoCombiTrans* pMatrix20 =
                   new TGeoCombiTrans("", dx,dy,dz,pRotYX1);
   dy = -1.250000; 
   dz= -12.;
   TGeoRotation *pRotYX0 = new TGeoRotation("pRotYX0");
   pRotYX0->RotateY(90.);
   pRotYX0->RotateX(180.);
   pRotYX0->RegisterYourself(); 
   TGeoCombiTrans* pMatrix19 =
                   new TGeoCombiTrans("", dx,dy,dz,pRotYX0);
   dz= -14.;
   TGeoCombiTrans* pMatrix21 =
                   new TGeoCombiTrans("", dx,dy,dz,pRotYX0);
 
   dx = 24.50000; // x position of ring for Outer Si     
   dy = -2.21000; // y position of ring for Outer Si     
   //dz = -16.500000-0.5; // z position of ring for Outer Si      // for end plate edge shadowing CALIFA 
   dz = -21.500000-0.5; // z position of ring for Outer Si     // for end plate at edge of CALIFA  
   //dz = -31.500000-0.5; // z position of ring for Outer Si      // for end plate position just behind califa
   TGeoCombiTrans* pMatrix13 =
                   new TGeoCombiTrans("", dx,dy,dz,pRot5);

   ////////////////////////////////////////////////////////////////////
   dx = -24.50000; // x position of ring for Outer Si     
   dy = 2.21000; // y position of ring for Outer Si     
   //dz = -16.500000-0.5; // z position of ring for Outer Si    // for end plate edge shadowing CALIFA
   dz = -21.500000-0.5; // z position of ring for Outer Si   // for end plate at edge of CALIFA  
   //dz = -31.500000-0.5; // z position of ring for Outer Si     // for end plate position just behind califa
   TGeoCombiTrans* pMatrix14 =
                   new TGeoCombiTrans("", dx,dy,dz,pRot5);
   dy = 1.250000; 
   dz= -12.;
   TGeoCombiTrans* pMatrix22 =
                   new TGeoCombiTrans("", dx,dy,dz,pRotY0);
   dz= -14.;
    TGeoCombiTrans* pMatrix24 =
                   new TGeoCombiTrans("", dx,dy,dz,pRotYX1);
   dy = -1.250000;
   dz= -12.;
   TGeoCombiTrans* pMatrix23 =
                   new TGeoCombiTrans("", dx,dy,dz,pRotYX0);
   dz= -14.;
   TGeoCombiTrans* pMatrix25 =
                   new TGeoCombiTrans("", dx,dy,dz,pRotYX0);

   dx = -24.50000; // x position of ring for Outer Si     
   dy = -2.210000; // y position of ring for Outer Si     
   //dz = -16.500000-0.5; // z position of ring for Outer Si    // for end plate edge shadowing CALIFA 
   dz = -21.500000-0.5; // z position of ring for Outer Si   // for end plate at edge of CALIFA 
   //dz = -31.500000-0.5; // z position of ring for Outer Si     // for end plate position just behind califa
   TGeoCombiTrans* pMatrix15 =
                   new TGeoCombiTrans("", dx,dy,dz,pRot5);
   ////////////////////////////////////////////////////////////////////////


   //////////////////////////////////////////////////////////////////////////
   dx = 2.21000; // x position of ring for Middle Si     
   dy = 23.000000; // y position of ring for Middle Si     
   //dz = -17.50000-0.5; // z position of ring for Middle Si      // for end plate edge shadowing CALIFA
   dz = -22.50000-0.5; // z position of ring for Middle Si     // for end plate at edge of CALIFA
   //dz = -32.50000-0.5; // z position of ring for Middle Si      // for end plate position just behind califa
   TGeoCombiTrans* pMatrix8 =
                   new TGeoCombiTrans("", dx,dy,dz,pRot5);
   dx = 1.250000;
   dz= -14.;
   TGeoRotation *pRotXY0 = new TGeoRotation("pRotXY0");
   pRotXY0->RotateX(90.);
   pRotXY0->RotateY(90.);
   pRotXY0->RegisterYourself(); 
   TGeoCombiTrans* pMatrix26 =
                   new TGeoCombiTrans("", dx,dy,dz,pRotXY0);
   dz= -16.;
   TGeoRotation *pRotX0 = new TGeoRotation("pRotX0");
   pRotX0->RotateX(90.);
   pRotX0->RegisterYourself(); 
   TGeoCombiTrans* pMatrix28 =
                   new TGeoCombiTrans("", dx,dy,dz,pRotX0);
   dx = -1.250000; 
   dz= -14.;
   TGeoRotation *pRotXY1 = new TGeoRotation("pRotXY1");
   pRotXY1->RotateX(90.);
   pRotXY1->RotateY(270.);
   pRotXY1->RegisterYourself(); 
   TGeoCombiTrans* pMatrix27 =
                   new TGeoCombiTrans("", dx,dy,dz,pRotXY1);
   dz= -16.;
   TGeoCombiTrans* pMatrix29 =
                   new TGeoCombiTrans("", dx,dy,dz,pRotXY1);
   
   dx = -2.21000; // x position of ring for Middle Si     
   dy = 23.000000; // y position of ring for Middle Si     
   //dz = -17.50000-0.5; // z position of ring for Middle Si       // for end plate edge shadowing CALIFA
   dz = -22.50000-0.5; // z position of ring for Middle Si      // for end plate at edge of CALIFA
   //dz = -32.50000-0.5; // z position of ring for Middle Si       // for end plate position just behind califa
   TGeoCombiTrans* pMatrix9 =
                   new TGeoCombiTrans("", dx,dy,dz,pRot5);
  
   //////////////////////////////////////////////////////////////////////////
   dx = 2.21000; // x position of ring for Middle Si     
   dy = -23.000000; // y position of ring for Middle Si     
   //dz = -17.500000-0.5; // z position of ring for Middle Si       // for end plate edge shadowing CALIFA
   dz = -22.500000-0.5; // z position of ring for Middle Si      // for end plate at edge of CALIFA
   //dz = -32.500000-0.5; // z position of ring for Middle Si     // for end plate position just behind califa
  TGeoCombiTrans* pMatrix10 =
                   new TGeoCombiTrans("", dx,dy,dz,pRot5);

   dx = 1.250000; 
   dz= -14.;
   TGeoCombiTrans* pMatrix30 =
                   new TGeoCombiTrans("", dx,dy,dz,pRotXY0);
   dz= -16.;
    TGeoCombiTrans* pMatrix32 =
                   new TGeoCombiTrans("", dx,dy,dz,pRotX0);
   dx = -1.250000;
   dz= -14.;
   TGeoCombiTrans* pMatrix31 =
                   new TGeoCombiTrans("", dx,dy,dz,pRotXY1);
   dz= -16.;
   TGeoCombiTrans* pMatrix33 =
                   new TGeoCombiTrans("", dx,dy,dz,pRotXY1);

   dx = -2.21000; // x position of ring for Middle Si     
   dy = -23.000000; // y position of ring for Middle Si     
   //dz = -17.500000-0.5; // z position of ring for Middle Si       // for end plate edge shadowing CALIFA
   dz = -22.500000-0.5; // z position of ring for Middle Si      // for end plate at edge of CALIFA
   //dz = -32.500000-0.5; // z position of ring for Middle Si     // for end plate position just behind califa
   TGeoCombiTrans* pMatrix11 =
                   new TGeoCombiTrans("", dx,dy,dz,pRot5);





  TGeoRotation *pRot10 = new TGeoRotation("pRot10");
   pRot10->RotateZ(-75.);
   pRot10->RegisterYourself(); 
  TGeoRotation *pRot11 = new TGeoRotation("pRot11");
   pRot11->RotateZ(-135.);
   pRot11->RegisterYourself(); 
  TGeoRotation *pRot12 = new TGeoRotation("pRot12");
   pRot12->RotateZ(-195.);
   pRot12->RegisterYourself(); 
  TGeoRotation *pRot13 = new TGeoRotation("pRot13");
   pRot13->RotateZ(-255.);
   pRot13->RegisterYourself(); 
  TGeoRotation *pRot14 = new TGeoRotation("pRot14");
   pRot14->RotateZ(-315.);
   pRot14->RegisterYourself(); 

   TGeoRotation *pRot15 = new TGeoRotation("pRot15");
   pRot15->RotateZ(-45.);
   pRot15->RegisterYourself(); 
  TGeoRotation *pRot16 = new TGeoRotation("pRot16");
   pRot16->RotateZ(-105.);
   pRot16->RegisterYourself(); 
  TGeoRotation *pRot17 = new TGeoRotation("pRot17");
   pRot17->RotateZ(-165.);
   pRot17->RegisterYourself(); 
  TGeoRotation *pRot18 = new TGeoRotation("pRot18");
   pRot18->RotateZ(-225.);
   pRot18->RegisterYourself(); 
  TGeoRotation *pRot19 = new TGeoRotation("pRot19");
   pRot19->RotateZ(-285.);
   pRot19->RegisterYourself(); 
  TGeoRotation *pRot20 = new TGeoRotation("pRot20");
   pRot20->RotateZ(-345.);
   pRot20->RegisterYourself(); 


   // Chamber:
	
	// Combi transformation: 
	dx = 0.000000;
	dy = 0.000000;
	//dz = -10.00000;  // for end plate edge shadowing CALIFA
	dz = -15.00000; // for end plate edge of CALIFA
	//dz = -25.00000;    // for end plate position behind califa
	// Rotation: 
	thx = 90.000000;    phx = 0.000000;
	thy = 90.000000;    phy = 90.000000;
	thz = 0.000000;    phz = 0.000000;
	TGeoRotation *pMatrix171 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans*
	pMatrix170 = new TGeoCombiTrans("", dx,dy,dz,pMatrix171);
	
	// Combi transformation: 
	dz = 20.000000;
	TGeoCombiTrans*
	pMatrix172 = new TGeoCombiTrans("", dx,dy,dz,pMatrix171);
	
        // Combi transformation: 
	//dz = -20.0000-0.75;    // for end plate edge shadowing CALIFA
	dz = -30.0000-0.75; // for end plate edge of CALIFA
	//dz = -50.0000-0.75;   // for end plate position behind califa
	TGeoCombiTrans*
	pMatrix174 = new TGeoCombiTrans("", dx,dy,dz,pMatrix171);

   // Beam Pipes:
	
	// Combi transformation: 
	//dz = -20.-1.5-5.00000; // for end plate shadowing CALIFA
	dz = -30.-1.5-5.00000;  // for end plate edge of CALIFA
	//dz = -50.-1.5-5.00000; // for end plate position behind califa
	TGeoCombiTrans*
	pMatrix176 = new TGeoCombiTrans("", dx,dy,dz,pMatrix171);
	
	//dz = -20.+5.50; // for end plate position behind califa
	dz = -30.+10.50; // for end plate edge of CALIFA
	//dz = -50.+20.50; // for end plate position behind califa
	TGeoCombiTrans*
	pMatrix178 = new TGeoCombiTrans("", dx,dy,dz,pMatrix171);

	dz = -29.+20.0+2.5;
	TGeoCombiTrans*
	pMatrix180 = new TGeoCombiTrans("", dx,dy,dz,pMatrix171);

	dz = -29.+20.0+5.0-0.5+0.25;
	TGeoCombiTrans*
	pMatrix182 = new TGeoCombiTrans("", dx,dy,dz,pMatrix171);

	dz = -29.+20.0+5.0-0.5+0.50+1.;
	TGeoCombiTrans*
	pMatrix184 = new TGeoCombiTrans("", dx,dy,dz,pMatrix171);


   // Geometry Parameter for volumes 

   Double_t parCoolRingOut[5] = { 24.5, 0.25, 0.35,
				 2.5,  175.};
   Double_t parCoolRingMid[5] = { 23., 0.25, 0.35,
				 2.5,  175.};
   Double_t parCoolRingIn[5] = { 7.75, 0.25, 0.35,
				351.,  349.};

   //Double_t parCoolPipeIn[5] = { 0.25, 0.35, 5.6 };   // for end plate position shadowing califa
   Double_t parCoolPipeIn[5] = { 0.25, 0.35, 10.6 }; // for end plate edge of CALIFA
   //Double_t parCoolPipeIn[5] = { 0.25, 0.35, 20.6 };   // for end plate position just behind califa
   Double_t parCoolPipeCurveIn[5] = { 0.5, 0.25, 0.35,
				      0, 90 };
   //Double_t parCoolPipeMid[5] = { 0.25, 0.35, 2. };  // for end plate position shadowing califa
   Double_t parCoolPipeMid[5] = { 0.25, 0.35, 7. }; // for end plate edge of CALIFA
   //Double_t parCoolPipeMid[5] = { 0.25, 0.35, 17. };  // for end plate position just behind califa

   //Double_t parCoolPipeOut[5] = { 0.25, 0.35, 3. };  // for end plate position shadowing califa
   Double_t parCoolPipeOut[5] = { 0.25, 0.35, 8. }; // for end plate edge of CALIFA
   //Double_t parCoolPipeOut[5] = { 0.25, 0.35, 18. };  // for end plate position just behind califa

   Double_t parCoolPipeCurveOut1[5] = { 1, 0.25, 0.35,
				      180, 180 };
   Double_t parCoolPipeCurveOut2[5] = { 1, 0.25, 0.35,
				      0, 90 };

   Double_t parCoolStructIn[10] = { 0., 360., 6, 2,
				    (-8.+ 0.5), 6.3, 7.9,
				    (-7.+ 0.5), 7.7, 7.7};
   Double_t parCoolStructMid[10] = { 0., 360., 12, 2,
				     (-13.+ 0.5), (22.75-1.1), (23.2),
				     (-12.+ 0.5), 23.2, 23.2};
   Double_t parCoolStructOut[10] = { 0., 360., 12, 2,
				     (-11.+ 0.5), (23.75-1.1), (23.75+.8),
				     (-10.+ 0.5), 23.75, 23.75};

   Double_t parCoolAsicIn[10] = { .5, 59., 1, 2,
				  -8., 8, 8.2,
				  -2.2, 7, 7.2};
   Double_t parCoolAsicMid[10] = { .5, 29., 1, 2,
				   -11.5, 23., 23.2,
				   -4.8, 20., 20.2};
   Double_t parCoolAsicOut[10] = { .5, 29., 1, 2,
				   -9.5, 23.6, 23.8,
				   -3.9, 20.8, 21.};


   Double_t parLinkStruct[22] = { 358.5, 3., 1, 6,
				 (-12.75-2.5), 21., 25.,
				 (-12.75-2.5 + 1.5), 17.5, 25.,
				 (-12.75-2.5 + 1.5), 17.5, 21.,
				 (-6.75-3.5), 9.5, 12.75,
				 (-6.75-3.5), 7.5, 12.75,
				 (-6.75-3.5 + 0.05 + 1.5), 7.5, 9.5};


   Double_t parChamberCone[5] = { 20., 
				   26.20000, 26.40000,
				   3.00000,  3.2000};
   //				   2.450000,  2.650000};

   //Double_t parChamberBarrel[3] = { 26.0, 26.2, 10.}; // for end plate edge shadowing CALIFA
   Double_t parChamberBarrel[3] = { 26.0, 26.2, 15.}; // for end plate edge of CALIFA
   //Double_t parChamberBarrel[3] = { 26.0, 26.2, 25.};  // for end plate edge at then end of CALIFA

   //Double_t parChamberEndPlate[3] = { 5.50, 26.2, 1.}; // 2x1 cm Aluminium
     Double_t parChamberEndPlate[3] = { 5.50, 26.2, 0.75}; // 2*0.75 cm of stainless steel


     //Double_t parPipeIn1[3] = { 5.50, 6.0, 5.5}; // for end plate edge shadowing CALIFA
   Double_t parPipeIn1[3] = { 5.50, 6.0, 10.5}; // for end plate edge of CALIFA
   //Double_t parPipeIn1[3] = { 5.50, 6.0, 20.5}; // for end plate at the end of CALIFA

   Double_t parPipeIn2[3] = { 5.50, 5.7, 2.5};
   Double_t parPipeOut[3] = { 5.50, 6.5, 5.};
   Double_t parPipeInSteelRing[3] = { 3., 5.50, 0.25};
   Double_t parPipeInSteelPipe[3] = { 3., 4.0, 1.};

   // Volumes :
   TGeoTorus *pCoolRingIn = new TGeoTorus(parCoolRingIn);
   TGeoTorus *pCoolRingMid = new TGeoTorus(parCoolRingMid);
   TGeoTorus *pCoolRingOut = new TGeoTorus(parCoolRingOut);
   TGeoTube *pCoolPipeIn = new TGeoTube(parCoolPipeIn);
   TGeoTorus *pCoolPipeCurveIn = new TGeoTorus(parCoolPipeCurveIn);
   TGeoTube *pCoolPipeMid = new TGeoTube(parCoolPipeMid);
   TGeoTube *pCoolPipeOut = new TGeoTube(parCoolPipeOut);
   TGeoTorus *pCoolPipeCurveOut1 = new TGeoTorus(parCoolPipeCurveOut1);
   TGeoTorus *pCoolPipeCurveOut2 = new TGeoTorus(parCoolPipeCurveOut2);

   TGeoPgon * pCoolStructIn = new TGeoPgon(parCoolStructIn);
   TGeoPgon * pCoolStructMid = new TGeoPgon(parCoolStructMid);
   TGeoPgon * pCoolStructOut = new TGeoPgon(parCoolStructOut);
   TGeoPgon * pCoolAsicIn = new TGeoPgon(parCoolAsicIn);
   TGeoPgon * pCoolAsicMid = new TGeoPgon(parCoolAsicMid);
   TGeoPgon * pCoolAsicOut = new TGeoPgon(parCoolAsicOut);

   TGeoPgon * pLinkStruct = new TGeoPgon(parLinkStruct);

   // chamber
   TGeoTube *pChamberBarrel = new TGeoTube(parChamberBarrel);
   TGeoCone *pChamberCone = new TGeoCone(parChamberCone);
   TGeoTube *pChamberEndPlate = new TGeoTube(parChamberEndPlate);

   // beam pipe
   TGeoTube *pPipeIn1 = new TGeoTube(parPipeIn1);
   TGeoTube *pPipeIn2 = new TGeoTube(parPipeIn2);
   TGeoTube *pPipeOut = new TGeoTube(parPipeOut);
   // target holders
   TGeoTube *pPipeInSteelRing = new TGeoTube(parPipeInSteelRing);
   TGeoTube *pPipeInSteelPipe = new TGeoTube(parPipeInSteelPipe);

   // Material of volumes
   TGeoVolume* pCoolRingIn_log
               = new TGeoVolume("CoolRingIn",pCoolRingIn, pMed);
   pCoolRingIn_log->SetVisLeaves(kTRUE);
   TGeoVolume* pCoolRingMid_log
               = new TGeoVolume("CoolRingMid",pCoolRingMid, pMed);
   pCoolRingMid_log->SetVisLeaves(kTRUE);
   TGeoVolume* pCoolRingOut_log
               = new TGeoVolume("CoolRingOut",pCoolRingOut, pMed);
   pCoolRingOut_log->SetVisLeaves(kTRUE);

   TGeoVolume* pCoolPipeIn_log
               = new TGeoVolume("CoolPipeIn",pCoolPipeIn, pMed);
   pCoolPipeIn_log->SetVisLeaves(kTRUE);
   TGeoVolume* pCoolPipeCurveIn_log
               = new TGeoVolume("CoolPipeCurveIn",pCoolPipeCurveIn, pMed);
   pCoolPipeCurveIn_log->SetVisLeaves(kTRUE);

   TGeoVolume* pCoolPipeMid_log
               = new TGeoVolume("CoolPipeMid",pCoolPipeMid, pMed);
   pCoolPipeMid_log->SetVisLeaves(kTRUE);
   TGeoVolume* pCoolPipeOut_log
               = new TGeoVolume("CoolPipeOut",pCoolPipeOut, pMed);
   pCoolPipeOut_log->SetVisLeaves(kTRUE);
   TGeoVolume* pCoolPipeCurveOut1_log
               = new TGeoVolume("CoolPipeCurveOut1",pCoolPipeCurveOut1, pMed);
   pCoolPipeCurveOut1_log->SetVisLeaves(kTRUE);
   TGeoVolume* pCoolPipeCurveOut2_log
               = new TGeoVolume("CoolPipeCurveOut2",pCoolPipeCurveOut2, pMed);
   pCoolPipeCurveOut2_log->SetVisLeaves(kTRUE);
 
   TGeoVolume* pCoolStructIn_log
               = new TGeoVolume("CoolStructIn",pCoolStructIn, pMed);
   pCoolStructIn_log->SetVisLeaves(kTRUE);
   TGeoVolume* pCoolStructMid_log
               = new TGeoVolume("CoolStructMid",pCoolStructMid, pMed);
   pCoolStructMid_log->SetVisLeaves(kTRUE);
   TGeoVolume* pCoolStructOut_log
               = new TGeoVolume("CoolStructOut",pCoolStructOut, pMed);
   pCoolStructOut_log->SetVisLeaves(kTRUE);

   TGeoVolume* pCoolAsicIn_log
               = new TGeoVolume("CoolAsicIn",pCoolAsicIn, pMed);
   pCoolAsicIn_log->SetVisLeaves(kTRUE);
   TGeoVolume* pCoolAsicMid_log
               = new TGeoVolume("CoolAsicMid",pCoolAsicMid, pMed);
   pCoolAsicMid_log->SetVisLeaves(kTRUE);
   TGeoVolume* pCoolAsicOut_log
               = new TGeoVolume("CoolAsicOut",pCoolAsicOut, pMed);
   pCoolAsicOut_log->SetVisLeaves(kTRUE);

   TGeoVolume* pLinkStruct_log
               = new TGeoVolume("LinkStruct",pLinkStruct, pMed2);
   pLinkStruct_log->SetVisLeaves(kTRUE);

   TGeoVolume* pChamberBarrelLog = new TGeoVolume("ChamberBarrelLog",pChamberBarrel, pMed2);
   pChamberBarrelLog->SetVisLeaves(kTRUE);

   TGeoVolume* pChamberConeLog = new TGeoVolume("ChamberConeLog",pChamberCone, pMed2);
   pChamberConeLog->SetVisLeaves(kTRUE);

   TGeoVolume* pChamberEndPlateLog = new TGeoVolume("ChamberEndPlateLog",pChamberEndPlate, pMed3);  // Stainless Steel
   //TGeoVolume* pChamberEndPlateLog = new TGeoVolume("ChamberEndPlateLog",pChamberEndPlate, pMed2);  // Alu
   pChamberEndPlateLog->SetVisLeaves(kTRUE);

   TGeoVolume* pPipeIn1Log = new TGeoVolume("PipeIn1Log", pPipeIn1, pMed2);
   pPipeIn1Log->SetVisLeaves(kTRUE);

   TGeoVolume* pPipeIn2Log = new TGeoVolume("PipeIn2Log", pPipeIn2, pMed2);
   pPipeIn2Log->SetVisLeaves(kTRUE);

   TGeoVolume* pPipeOutLog = new TGeoVolume("PipeOutLog", pPipeOut, pMed2);
   pPipeOutLog->SetVisLeaves(kTRUE);

   TGeoVolume* pPipeInSteelRingLog = new TGeoVolume("PipeInSteelRingLog", pPipeInSteelRing, pMed3);
   pPipeInSteelRingLog->SetVisLeaves(kTRUE);

   TGeoVolume* pPipeInSteelPipeLog = new TGeoVolume("PipeInSteelPipeLog", pPipeInSteelPipe, pMed3);
   pPipeInSteelPipeLog->SetVisLeaves(kTRUE);

   TGeoVolume *top =  gGeoManager->GetTopVolume();
   TGeoCombiTrans* t0 = new TGeoCombiTrans();;
   TGeoCombiTrans* pGlobal2 = GetGlobalPosition(t0);

   //TGeoCombiTrans* pGlobal1 = GetGlobalPosition(pMatrix1);
   //TGeoCombiTrans* pGlobal2 = GetGlobalPosition(pMatrix2);

   
   top->AddNode(pCoolRingOut_log, 0, pMatrix1);  // top
   top->AddNode(pCoolRingOut_log, 1, pMatrix2); // bottom
   top->AddNode(pCoolRingMid_log, 0, pMatrix3);  // right
   top->AddNode(pCoolRingMid_log, 1, pMatrix4);  // leftt
   top->AddNode(pCoolRingIn_log, 0, pMatrix5);

   top->AddNode(pCoolPipeIn_log, 0, pMatrix6);
   top->AddNode(pCoolPipeIn_log, 1, pMatrix7);
   top->AddNode(pCoolPipeCurveIn_log, 0, pMatrix16);
   top->AddNode(pCoolPipeCurveIn_log, 1, pMatrix17);

   top->AddNode(pCoolPipeMid_log, 0, pMatrix8);
   top->AddNode(pCoolPipeMid_log, 1, pMatrix9);
   top->AddNode(pCoolPipeMid_log, 2, pMatrix10);
   top->AddNode(pCoolPipeMid_log, 3, pMatrix11);

   top->AddNode(pCoolPipeOut_log, 0, pMatrix12);
   top->AddNode(pCoolPipeOut_log, 1, pMatrix13);
   top->AddNode(pCoolPipeOut_log, 2, pMatrix14);
   top->AddNode(pCoolPipeOut_log, 3, pMatrix15);

   top->AddNode(pCoolPipeCurveOut1_log, 0, pMatrix18);
   top->AddNode(pCoolPipeCurveOut1_log, 1, pMatrix19);
   top->AddNode(pCoolPipeCurveOut2_log, 0, pMatrix20);
   top->AddNode(pCoolPipeCurveOut2_log, 1, pMatrix21);
   top->AddNode(pCoolPipeCurveOut1_log, 2, pMatrix22);
   top->AddNode(pCoolPipeCurveOut1_log, 3, pMatrix23);
   top->AddNode(pCoolPipeCurveOut2_log, 2, pMatrix24);
   top->AddNode(pCoolPipeCurveOut2_log, 3, pMatrix25);
   top->AddNode(pCoolPipeCurveOut1_log, 4, pMatrix26);
   top->AddNode(pCoolPipeCurveOut1_log, 5, pMatrix27);
   top->AddNode(pCoolPipeCurveOut2_log, 4, pMatrix28);
   top->AddNode(pCoolPipeCurveOut2_log, 5, pMatrix29);

   top->AddNode(pCoolPipeCurveOut1_log, 6, pMatrix30);
   top->AddNode(pCoolPipeCurveOut1_log, 7, pMatrix31);
   top->AddNode(pCoolPipeCurveOut2_log, 6, pMatrix32);
   top->AddNode(pCoolPipeCurveOut2_log, 7, pMatrix33);

   top->AddNode(pCoolStructOut_log, 0, pRot0);
   top->AddNode(pCoolStructMid_log, 0, pRot0);
   top->AddNode(pCoolStructIn_log, 0, pRot0);

   top->AddNode(pCoolAsicIn_log, 0, pRot0);
   top->AddNode(pCoolAsicIn_log, 1, pRot10);
   top->AddNode(pCoolAsicIn_log, 2, pRot11);
   top->AddNode(pCoolAsicIn_log, 3, pRot12);
   top->AddNode(pCoolAsicIn_log, 4, pRot13);
   top->AddNode(pCoolAsicIn_log, 5, pRot14);

   top->AddNode(pCoolAsicMid_log, 0, pRot0);
   top->AddNode(pCoolAsicMid_log, 1, pRot15);
   top->AddNode(pCoolAsicMid_log, 2, pRot10);
   top->AddNode(pCoolAsicMid_log, 3, pRot16);
   top->AddNode(pCoolAsicMid_log, 4, pRot11);
   top->AddNode(pCoolAsicMid_log, 5, pRot17);
   top->AddNode(pCoolAsicMid_log, 6, pRot12);
   top->AddNode(pCoolAsicMid_log, 7, pRot18);
   top->AddNode(pCoolAsicMid_log, 8, pRot13);
   top->AddNode(pCoolAsicMid_log, 9, pRot19);
   top->AddNode(pCoolAsicMid_log, 10, pRot14);
   top->AddNode(pCoolAsicMid_log, 11, pRot20);

   top->AddNode(pCoolAsicOut_log, 0, pRot0);
   top->AddNode(pCoolAsicOut_log, 1, pRot15);
   top->AddNode(pCoolAsicOut_log, 2, pRot10);
   top->AddNode(pCoolAsicOut_log, 3, pRot16);
   top->AddNode(pCoolAsicOut_log, 4, pRot11);
   top->AddNode(pCoolAsicOut_log, 5, pRot17);
   top->AddNode(pCoolAsicOut_log, 6, pRot12);
   top->AddNode(pCoolAsicOut_log, 7, pRot18);
   top->AddNode(pCoolAsicOut_log, 8, pRot13);
   top->AddNode(pCoolAsicOut_log, 9, pRot19);
   top->AddNode(pCoolAsicOut_log, 10, pRot14);
   top->AddNode(pCoolAsicOut_log, 11, pRot20);

   top->AddNode(pLinkStruct_log, 0,pRot0);  // 30deg
   top->AddNode(pLinkStruct_log, 1,pRot10);   // 90
   top->AddNode(pLinkStruct_log, 2,pRot11);  // 150
   top->AddNode(pLinkStruct_log, 3,pRot12);   // 210
   top->AddNode(pLinkStruct_log, 4,pRot13);  // 270
   top->AddNode(pLinkStruct_log, 5,pRot14);  // 330 

   top->AddNode(pPipeOutLog,0,pMatrix176);
   top->AddNode(pPipeIn1Log,0,pMatrix178);
   top->AddNode(pPipeIn2Log,0,pMatrix180);
   top->AddNode(pPipeInSteelRingLog,0,pMatrix182);
   top->AddNode(pPipeInSteelPipeLog,0,pMatrix184);
   
   top->AddNode(pChamberBarrelLog,0,pMatrix170);
   top->AddNode(pChamberConeLog,0,pMatrix172);
   top->AddNode(pChamberEndPlateLog,0,pMatrix174);


}


ClassImp(R3BVacVesselCool)



