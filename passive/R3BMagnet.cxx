
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
#include <iostream.h>

R3BMagnet::~R3BMagnet()
{
}
R3BMagnet::R3BMagnet()
{
}

R3BMagnet::R3BMagnet(const char * name, const char *Title)
  : FairModule(name ,Title)
{
}





void R3BMagnet::ConstructGeometry(){

    Double_t degrad = TMath::Pi()/180.;

 // Specific Material definition
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
   numed   = 1;  // medium number
   TGeoMedium*
   pMed2 = new TGeoMedium("Air", numed,pMat2);


 // <D.Bertini@gsi.de>
 // version adapted from ALADIN magnet
 // parameters

    Double_t Aladin_width = 156.0;//1.250*m;
    Double_t Aladin_length = 176.0; //1.70*m; Field length is 1.4 m
    Double_t Aladin_gap = 50.;    //cm
    // Angle / beam axis check me ! (-7.3 deg , +7.2 deg ?)
    Double_t Aladin_angle = +7.3; // degree
    Double_t DistanceToTarget = 350.0;  //cm
    Double_t Yoke_thickness = 50.;     //cm
    Double_t Correction = -119.94;   //cm
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
   TGeoVolume *pWorld =  gGeoManager->GetTopVolume();
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

   pWorld->AddNode(pVolFeYoke_up,   1, t1);
   pWorld->AddNode(pVolFeYoke_down, 2, t2);
   pWorld->AddNode(pVolFeYoke_left, 3, t3);
   pWorld->AddNode(pVolFeYoke_right,4, t4);

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














