#include <iomanip>
#include <iostream>
#include "TGeoManager.h"
#include "TMath.h"




// Create Matrix Unity
TGeoRotation *fGlobalRot = new TGeoRotation();

// Create a null translation
//TGeoTranslation *fGlobalTrans = new TGeoTranslation();
TGeoTranslation *fGlobalTrans = NULL;

//fGlobalTrans->SetTranslation(0.0,0.0,0.0);
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
Bool_t fLabTrans = kTRUE;

TGeoCombiTrans* GetGlobalPosition(TGeoCombiTrans *fRef);

void create_startra_geo_v16(const char* geoTag)
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
  TString geoFileName = geoPath + "/geometry/startrack_";
  geoFileName = geoFileName + geoTag + ".geo.root";
  // --------------------------------------------------------------------------



  // -----------------   Get and create the required media    -----------------
  FairGeoMedia*   geoMedia = geoFace->getMedia();
  FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

  FairGeoMedium* mAir      = geoMedia->getMedium("Air");
  if ( ! mAir ) Fatal("Main", "FairMedium Air not found");
  geoBuild->createMedium(mAir);
  TGeoMedium* pAirMed = gGeoMan->GetMedium("Air");
  if ( ! pAirMed ) Fatal("Main", "Medium Air not found");

  FairGeoMedium* mVac      = geoMedia->getMedium("vacuum");
  if ( ! mVac ) Fatal("Main", "FairMedium vacuum not found");
  geoBuild->createMedium(mVac);
  TGeoMedium* pVacuumMedium = gGeoMan->GetMedium("vacuum");
  if ( ! pVacuumMedium ) Fatal("Main", "Medium vacuum not found");
  
  FairGeoMedium* mSi      = geoMedia->getMedium("silicon");
  if ( ! mSi ) Fatal("Main", "FairMedium silicon not found");
  geoBuild->createMedium(mSi);
  TGeoMedium* pSiMed = gGeoMan->GetMedium("silicon");
  if ( ! pSiMed ) Fatal("Main", "Medium silicon not found");

  FairGeoMedium* mCopper      = geoMedia->getMedium("copper");
  if ( ! mCopper ) Fatal("Main", "FairMedium copper not found");
  geoBuild->createMedium(mCopper);
  TGeoMedium* pCopperMed = gGeoMan->GetMedium("copper");
  if ( ! pCopperMed ) Fatal("Main", "Medium copper not found");
  
  FairGeoMedium* mAl      = geoMedia->getMedium("aluminium");
  if ( ! mAl ) Fatal("Main", "FairMedium aluminium not found");
  geoBuild->createMedium(mAl);
  TGeoMedium* pAluMed = gGeoMan->GetMedium("aluminium");
  if ( ! pAluMed ) Fatal("Main", "Medium aluminium not found");
  
  FairGeoMedium* mCar      = geoMedia->getMedium("CarbonFibre");
  if ( ! mCar ) Fatal("Main", "FairMedium CarbonFibre not found");
  geoBuild->createMedium(mCar);
  TGeoMedium* pCarbonFibreMedium = gGeoMan->GetMedium("CarbonFibre");
  if ( ! pCarbonFibreMedium ) Fatal("Main", "Medium CarbonFibre not found");

  FairGeoMedium* mAu      = geoMedia->getMedium("gold");
  if ( ! mAu ) Fatal("Main", "FairMedium gold not found");
  geoBuild->createMedium(mAu);
  TGeoMedium* pMed79 = gGeoMan->GetMedium("gold");
  if ( ! pMed79 ) Fatal("Main", "Medium gold not found");
  // --------------------------------------------------------------------------



  // --------------   Create geometry and top volume  -------------------------
  gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoMan->SetName("STARTRAgeom");
  TGeoVolume* top = new TGeoVolumeAssembly("TOP");
  gGeoMan->SetTopVolume(top);
  // --------------------------------------------------------------------------

  // WORLD
  //  TGeoVolume* pAWorld = gGeoManager->GetTopVolume();
  TGeoVolume* pAWorld = top;
   pAWorld->SetVisLeaves(kTRUE);
  
  gGeoManager->SetVisLevel(6);


  //Double_t WorldHalfLength=24.813;
  Double_t WorldHalfLength=25.;
  Double_t ShiftToWorldEdge=20;

  // StarTrack world (mother volume to have vacuum for Si tracker and air for CALIFA)  !!
  
   TGeoShape *pSTaRTrackWorld_A =new TGeoTube("STarTrack_Tube_A",
					     0.,   // rmin // put it to 4 for delat e- and protons; TODO: modify geo to leave it to 0 all the time
					    //1.,   // rmin // put it to 4 for delat e- and protons; TODO: modify geo to leave it to 0 all the time
					     26.,   // rmax 
					     WorldHalfLength  // half length
					    );
   // Remove target area 
   TGeoShape *pSTaRTrackWorld_B =new TGeoTube("STarTrack_Tube_B",
					     0.,   // rmin // put it to 4 for delat e- and protons; TODO: modify geo to leave it to 0 all the time
					    //1.,  // rmin // put it to 4 for delat e- and protons; TODO: modify geo to leave it to 0 all the time
					     3.5250,  // rmax 
					     6     // half length
					    );

   //TGeoCombiTrans *t_targ=new TGeoCombiTrans("t_targ", 0.,0.,5-WorldHalfLength,fRefRot);
   TGeoCombiTrans *t_targ=new TGeoCombiTrans("t_targ", 0.,0.,4.813-WorldHalfLength,fRefRot);
   t_targ->RegisterYourself();

   TGeoCompositeShape *pSTaRTrackWorld = new TGeoCompositeShape("STarTrack_Tube","STarTrack_Tube_A - (STarTrack_Tube_B:t_targ)");

   TGeoVolume*
   pTraWorld  = new TGeoVolume("STaRTrackWorld",pSTaRTrackWorld, pVacuumMedium);
   //TGeoCombiTrans *t0 = new TGeoCombiTrans();
   TGeoCombiTrans *t0 = new TGeoCombiTrans("",0.,0.,ShiftToWorldEdge,fRefRot);
   TGeoCombiTrans *pGlobalc = GetGlobalPosition(t0);
   
   pTraWorld->SetLineColor(41);

   // add the sphere as Mother Volume
   pAWorld->AddNode(pTraWorld, 0, pGlobalc);

  
   // All the other volumes:

  // out-of-file geometry definition
  Double_t dx,dy,dz;
  // Double_t par[20];
  //   Double_t rmin, rmax;
  Double_t a;
  Double_t thx, phx, thy, phy, thz, phz;
  //   Double_t  phi1, phi2;
  Double_t z, density, radl, absl, w;
  Double_t tx,ty,tz;
  Int_t nel, numed;
  
  Double_t Frame_Width=0.1; // =0.1/2=0.05cm on each side
  Double_t Frame_Length=Frame_Width;
  Double_t Frame_Depth=0.1;  // = half of the frame Depth (ie frame full Depth=2.mm)
    
  
  Double_t PI= 3.141592653589793;
  Double_t NSide1= 6;             // Nb of detector in inner Layer
  //Double_t WidthMax1= 7.945;   // Max width of detector (cm)
  Double_t WidthMax1= 8.1912;   // Max width of detector (cm) as final specs
  //Double_t WidthMin1= 2.25;    // Max width of detector (cm)
  Double_t WidthMin1= 1.971;    // Max width of detector (cm) as final specs
  Double_t Thickness1= 0.005;  // Half thickness of detector (cm) as final specs  -> total thickness: 100um
  //Double_t Thickness1= 0.0005;  // Half thickness of detector (cm) as final specs  -> total thickness: 100um
  //Double_t Thickness1= 0.015;  // Half thickness of detector (cm)   -> total thickness: 300um
  Double_t Length1=21.794+0.015;      // length of detector (cm) as final specs ( +150um to count for the gap between sensor)
  //Double_t Length1=19.03;      // length of detector (cm
  //Double_t InclAng1=14.9;      // angle d'inclinaison with respect to z axis (deg)
  Double_t InclAng1=14.3;      // angle d'inclinaison with respect to z axis (deg)  As final specs
  //Double_t Rmin1=2.;           // beam clearance 2cm radius
  Double_t Rmin1=1.75;           // beam clearance 2cm radius as final specs
  //Double_t AngRangeMin1=7;     // Min theta angle covered (deg)
  Double_t AngRangeMin1=5.26;     // Min theta angle covered (deg) as final specs
  //   Double_t AngTrap1=atan((WidthMax1 /2 - WidthMin1 /2)/Length1); // (rad)
  //   Double_t WidthHalf1= WidthMax1 - (Length1*tan(AngTrap1)); // width of detector at Length/2
  
  Int_t NSide2= 12;            // Nb of detector in intermediate Layer
  //Double_t WidthMax2= 10.4;    // Max width of detector (cm)
  Double_t WidthMax2= 10.80295;    // Max width of detector (cm) as final specs
  //Double_t WidthMin2= 1.3;     // Max width of detector (cm)
  Double_t WidthMin2= 1.1406;     // Max width of detector (cm)  as final specs
  Double_t Thickness2= 0.015;  // half thickness of detector (cm)
  //Double_t Length2=30.6;       // length of detector (cm
  Double_t Length2=33.83875 + 2*0.015;       // length of detector (cm) as final specs  ( + 2*150um to count for the gap between sensor)
  //Double_t InclAng2=33.7;      // angle d'inclinaison with respect to z axis (deg) as final specs to be checked
  Double_t InclAng2=32.155;      // angle d'inclinaison with respect to z axis (deg) as final specs to be checked
  //Double_t Rmin2=2.5;        // beam clearance 3cm radius
  Double_t Rmin2=2.30657; // 2.22;        // beam clearance 3cm radius as final specs
  //Double_t AngRangeMin2=7.;    // Min theta angle covered (deg)
  Double_t AngRangeMin2=5.537946491; // 5.306;    // Min theta angle covered (deg) as final specs
  //   Double_t AngTrap2=atan((WidthMax2 /2 - WidthMin2 /2)/Length2); // (rad)
  //   Double_t WidthHalf2= WidthMax2 - (Length2*tan(AngTrap2)); // width of detector at Length/2
  
  Int_t NSide3= 12;            // Nb of detector in outer Layer
  //Double_t WidthMax3= 10.4;    // Max width of detector (cm)
  Double_t WidthMax3= 10.80295;    // Max width of detector (cm) as final specs
  //Double_t WidthMin3= 1.3;     // Max width of detector (cm)
  Double_t WidthMin3= 1.1406;     // Max width of detector (cm) as final specs
  Double_t Thickness3= 0.015;  // half thickness of detector (cm)
  //Double_t Length3=30.6;       // length of detector (cm
  Double_t Length3=33.83875 + 2*0.015;       // length of detector (cm) as final specs  ( + 2*150um to count for the gap between sensor)
  //Double_t InclAng3=33.7;      // angle d'inclinaison with respect to z axis (deg) as final specs to be checked !!
  Double_t InclAng3=32.155;      // angle d'inclinaison with respect to z axis (deg) as final specs to be checked !!
  //Double_t Rmin3=2.685;          // beam clearance 3cm radius
  Double_t Rmin3=3.03196; //2.95;          // beam clearance 3cm radius as final specs
  //Double_t AngRangeMin3=7;     // Min theta angle covered (deg)
  Double_t AngRangeMin3=6.98815; //6.7657;     // Min theta angle covered (deg) as final specs
  //   Double_t AngTrap3=atan((WidthMax3 /2 - WidthMin3 /2)/Length2); // (rad)
  //   Double_t WidthHalf3= WidthMax3 - (Length3*tan(AngTrap3)); // width of detector at Length/2
  
  
  
  // TRANSFORMATION MATRICES
  
  // Detectors
  
  
  //
  // First layer
  //
  //########################
  
  // Combi transformation:
  //dx = 0.000;
  //dy = -((Length1/2  + (WidthHalf1/2)*tan(AngTrap1) )*sin(InclAng1*PI/180.)+ Rmin1); // considering  real barycentre position
  //dy = -((Length1/2)*sin(InclAng1*PI/180.)+ Rmin1);   // considering  intersection of 2 medianes
  
  // +rotation of 30 degrees:

  Double_t ZRotAngle = 30.;

  dx = (-(( (Length1)/2)*sin(InclAng1*PI/180.)+ Rmin1))*sin((ZRotAngle)*PI/180.);
  dy = (-(( (Length1)/2)*sin(InclAng1*PI/180.)+ Rmin1))*cos((ZRotAngle)*PI/180.);;   // considering  intersection of 2 medianes
  dz = -(Length1)*cos(InclAng1*PI/180.)/2 + ((Rmin1)/tan(AngRangeMin1*PI/180.)) - ShiftToWorldEdge;
  
  
  // Rotation:
  //thx = 90.000000;        phx = 0.000000;   //  0 degrees rotation to span over 2 detectors of middle layers
  //thy = 90.+ InclAng1;    phy = 90.000000;
  //thz = InclAng1;         phz = 90.000000;
  thx = 90.000000;        phx = 0.000000-ZRotAngle;   // 30 degrees rotation to span over 2 detectors of middle layers
  thy = 90.+ InclAng1;    phy = 90.000000-ZRotAngle;
  thz = InclAng1;         phz = 90.000000-ZRotAngle;
  TGeoRotation *pMatrix3 = new TGeoRotation("",thx,phx,thy,phy,thz,phz); // geant3 angles
  //thx = -InclAng1;
  //thy = 0.;
  //thz = 0;
  //TGeoRotation *pMatrix3 = new TGeoRotation("",thz,thx,thy); // Euler phi (rot/z), theta (rot/x), psi (rot/ new z) angles
  TGeoCombiTrans*
  pMatrix2 = new TGeoCombiTrans("", dx,dy,dz,pMatrix3);  // bottom inner detector
  
  dx = (-(( (Length1+Frame_Length)/2)*sin(InclAng1*PI/180.)+ Rmin1 + (Thickness1*2+Frame_Depth)))*sin((ZRotAngle)*PI/180.);
  dy = (-(( (Length1+Frame_Length)/2)*sin(InclAng1*PI/180.)+ Rmin1 + (Thickness1*2+Frame_Depth)))*cos((ZRotAngle)*PI/180.);;   // considering  intersection of 2 medianes
  dz = -(Length1+Frame_Length)*cos(InclAng1*PI/180.)/2 + ((Rmin1)/tan(AngRangeMin1*PI/180.))- ShiftToWorldEdge;
  TGeoCombiTrans*
  pMatrix2b = new TGeoCombiTrans("", dx,dy,dz,pMatrix3);  // bottom inner detector
  
  //########################
  
  // Combi transformation:
  //dx =  dx -(WidthHalf1/2)*(1/(cos(AngTrap1)*cos(AngTrap1)))*(1 + cos((360./NSide1)*PI/180.)); // considering  real barycentre position;
  // dx =  -(WidthHalf1/2)*(1 + cos((360./NSide1)*PI/180.));  // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf1/2)*(1/(cos(AngTrap1)*cos(AngTrap1)))*sin((360./NSide1)*PI/180.)*;      // considering  real barycentre position;
  //dy = dy +(WidthHalf1/2)*sin((360./NSide1)*PI/180.);      // considering  intersection of 2 medianes
  
  // +rotation of ZRotAngle degrees:
  dx=  (-(( (Length1)/2)*sin(InclAng1*PI/180.)+ Rmin1))*sin((ZRotAngle+360/NSide1)*PI/180.);  // rotation by 60+30 deg/ z axis
  dy=  (-(( (Length1)/2)*sin(InclAng1*PI/180.)+ Rmin1))*cos((ZRotAngle+360/NSide1)*PI/180.);  // rotation by 60+30 deg/ z axis
  dz = -(Length1)*cos(InclAng1*PI/180.)/2 + ((Rmin1)/tan(AngRangeMin1*PI/180.))- ShiftToWorldEdge;
  
  // Rotation:
  //thx = 90.000000;        phx = 0.000000-(360./NSide1);
  //thy = 90.+ InclAng1;    phy = 90.000000-(360./NSide1);
  //thz = InclAng1;         phz = 90.000000-(360./NSide1);
  thx = 90.000000;        phx = 0.000000-(ZRotAngle+360./NSide1);
  thy = 90.+ InclAng1;    phy = 90.000000-(ZRotAngle+360./NSide1);
  thz = InclAng1;         phz = 90.000000-(ZRotAngle+360./NSide1);
  TGeoRotation *pMatrix5 = new TGeoRotation("",thx,phx,thy,phy,thz,phz); // geant3 angles
  //thx = -InclAng1;
  //thy = 0.;
  //thz = -360./NSide1;
  //TGeoRotation *pMatrix5 = new TGeoRotation("",thz,thx,thy); // Euler phi (rot/z), theta (rot/x), psi (rot/ new z) angles
  TGeoCombiTrans*
  pMatrix4 = new TGeoCombiTrans("", dx,dy,dz,pMatrix5);
  
  
  dx=  (-(( (Length1+Frame_Length)/2)*sin(InclAng1*PI/180.)+ Rmin1 + (Thickness1*2+Frame_Depth)))*sin((ZRotAngle+360/NSide1)*PI/180.);  // rotation by 60+15 deg/ z axis
  dy=  (-(( (Length1+Frame_Length)/2)*sin(InclAng1*PI/180.)+ Rmin1 + (Thickness1*2+Frame_Depth)))*cos((ZRotAngle+360/NSide1)*PI/180.);  // rotation by 60+15 deg/ z axis
  //dz = -(Length1+Frame_Length)*cos(InclAng1*PI/180.)/2 + ((Rmin1)/tan(AngRangeMin1*PI/180.));
  TGeoCombiTrans*
  pMatrix4b = new TGeoCombiTrans("", dx,dy,dz,pMatrix5);
  
  //########################
  
  // Combi transformation:
  //dx =  dx -(WidthHalf1/2)*(1/(cos(AngTrap1)*cos(AngTrap1)))*(cos((360./NSide1)*PI/180.)+cos(2*(360./NSide1)*PI/180.));   // considering  real barycentre position;
  //dx =  dx -(WidthHalf1/2)*(cos((360./NSide1)*PI/180.) + cos(2*(360./NSide1)*PI/180.)); // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf1/2)*(1/(cos(AngTrap1)*cos(AngTrap1)))*(sin((360./NSide1)*PI/180.)+sin(2*(360./NSide1)*PI/180.));  // considering  real barycentre position;
  //dy =  dy +(WidthHalf1/2)*(sin((360./NSide1)*PI/180.) + sin(2*(360./NSide1)*PI/180.)); // considering  intersection of 2 medianes
  
  // +rotation of 30 degrees:
  dx= -(( (Length1)/2)*sin(InclAng1*PI/180.)+ Rmin1)*sin((ZRotAngle+2*(360/NSide1))*PI/180.);  // rotation by 120+30 deg / z axis
  dy= -(( (Length1)/2)*sin(InclAng1*PI/180.)+ Rmin1)*cos((ZRotAngle+2*(360/NSide1))*PI/180.);  // rotation by 120+30 deg / z axis
  dz = -(Length1)*cos(InclAng1*PI/180.)/2 + ((Rmin1)/tan(AngRangeMin1*PI/180.)) - ShiftToWorldEdge  ;
  
  
  // Rotation:
  //thx = 90.000000;        phx = 0.000000-(2.*360./NSide1);
  //thy = 90.+ InclAng1;    phy = 90.000000-(2.*360./NSide1);
  //thz = InclAng1;         phz = 90.000000-(2.*360./NSide1);
  thx = 90.000000;        phx = 0.000000-(ZRotAngle+2.*360./NSide1);
  thy = 90.+ InclAng1;    phy = 90.000000-(ZRotAngle+2.*360./NSide1);
  thz = InclAng1;         phz = 90.000000-(ZRotAngle+2.*360./NSide1);
  TGeoRotation *pMatrix7 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix6 = new TGeoCombiTrans("", dx,dy,dz,pMatrix7);
  
  
  dx= -(( (Length1+Frame_Length)/2)*sin(InclAng1*PI/180.)+ Rmin1 + (Thickness1*2+Frame_Depth))*sin((ZRotAngle+2*(360/NSide1))*PI/180.);  // rotation by 120+15 deg / z axis
  dy= -(( (Length1+Frame_Length)/2)*sin(InclAng1*PI/180.)+ Rmin1 + (Thickness1*2+Frame_Depth))*cos((ZRotAngle+2*(360/NSide1))*PI/180.);  // rotation by 120+15 deg / z axis
  //dz = -(Length1+Frame_Length)*cos(InclAng1*PI/180.)/2 + ((Rmin1)/tan(AngRangeMin1*PI/180.));
  TGeoCombiTrans*
  pMatrix6b = new TGeoCombiTrans("", dx,dy,dz,pMatrix7);
  
  //########################
  
  // Combi transformation:
  //dy = ((Length1/2  + (WidthHalf1/2)*tan(AngTrap1) )*sin(InclAng1*PI/180.)+ Rmin1); // considering  real barycentre position
  //dx =  dx -(WidthHalf1/2)*(1/(cos(AngTrap1)*cos(AngTrap1)))*(cos(2*(360./NSide1)*PI/180.)+cos(3*(360./NSide1)*PI/180.));              // considering  real barycentre position;
  //dx =  dx -(WidthHalf1/2)*(cos(2*(360./NSide1)*PI/180.) + cos(3*(360./NSide1)*PI/180.));  // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf1/2)*(1/(cos(AngTrap1)*cos(AngTrap1)))*(sin(2*(360./NSide1)*PI/180.)+sin(3*(360./NSide1)*PI/180.));              // considering  real barycentre position;
  //dy =  dy +(WidthHalf1/2)*(sin(2*(360./NSide1)*PI/180.) + sin(3*(360./NSide1)*PI/180.));  // considering  intersection of 2 medianes
  
  // +rotation of 30 degrees:
  dx= -(( (Length1)/2)*sin(InclAng1*PI/180.)+ Rmin1)*sin((ZRotAngle+3*(360/NSide1))*PI/180.);     // rotation by 180+30 deg / z axis
  dy= -(( (Length1)/2)*sin(InclAng1*PI/180.)+ Rmin1)*cos((ZRotAngle+3*(360/NSide1))*PI/180.);     // rotation by 180+30 deg / z axis
  dz = -(Length1)*cos(InclAng1*PI/180.)/2 + ((Rmin1)/tan(AngRangeMin1*PI/180.))- ShiftToWorldEdge  ;
  
  // Rotation:
  //thx = 90.000000;        phx = 0.000000-(3.*360./NSide1);
  //thy = 90.+ InclAng1;    phy = 90.000000-(3.*360./NSide1);
  //thz = InclAng1;         phz = 90.000000-(3.*360./NSide1);
  thx = 90.000000;        phx = 0.000000-(ZRotAngle+3.*360./NSide1);
  thy = 90.+ InclAng1;    phy = 90.000000-(ZRotAngle+3.*360./NSide1);
  thz = InclAng1;         phz = 90.000000-(ZRotAngle+3.*360./NSide1);
  TGeoRotation *pMatrix9 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix8 = new TGeoCombiTrans("", dx,dy,dz,pMatrix9);
  
  
  dx= -(( (Length1+Frame_Length)/2)*sin(InclAng1*PI/180.)+ Rmin1 +(Thickness1*2+ Frame_Depth))*sin((ZRotAngle+3*(360/NSide1))*PI/180.);     // rotation by 180+15 deg / z axis
  dy= -(( (Length1+Frame_Length)/2)*sin(InclAng1*PI/180.)+ Rmin1 +(Thickness1*2+ Frame_Depth))*cos((ZRotAngle+3*(360/NSide1))*PI/180.);     // rotation by 180+15 deg / z axis
  //dz = -(Length1+Frame_Length)*cos(InclAng1*PI/180.)/2 + ((Rmin1)/tan(AngRangeMin1*PI/180.));
  TGeoCombiTrans*
  pMatrix8b = new TGeoCombiTrans("", dx,dy,dz,pMatrix9);
  
  
  
  
  //########################
  
  // Combi transformation:
  //dx =  dx -(WidthHalf1/2)*(1/(cos(AngTrap1)*cos(AngTrap1)))*(cos(3*(360./NSide1)*PI/180.)+cos(4*(360./NSide1)*PI/180.));    // considering  real barycentre position;
  //dx =  dx -(WidthHalf1/2)*(cos(3*(360./NSide1)*PI/180.) + cos(4*(360./NSide1)*PI/180.));  // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf1/2)*(1/(cos(AngTrap1)*cos(AngTrap1)))*(sin(3*(360./NSide1)*PI/180.)+sin(4*(360./NSide1)*PI/180.));   // considering  real barycentre position;
  //dy =  dy +(WidthHalf1/2)*(sin(3*(360./NSide1)*PI/180.) + sin(4*(360./NSide1)*PI/180.));  // considering  intersection of 2 medianes
  
  // +rotation of 30 degrees:
  dx= -(( (Length1)/2)*sin(InclAng1*PI/180.)+ Rmin1)*sin((ZRotAngle+4*(360/NSide1))*PI/180.);    // rotation by 240+30 deg/ z axis
  dy= -(( (Length1)/2)*sin(InclAng1*PI/180.)+ Rmin1)*cos((ZRotAngle+4*(360/NSide1))*PI/180.);    // rotation by 240+30 deg/ z axis
  dz = -(Length1)*cos(InclAng1*PI/180.)/2 + ((Rmin1)/tan(AngRangeMin1*PI/180.)) - ShiftToWorldEdge ;
  
  // Rotation:
  //thx = 90.000000;        phx = 0.000000-(4.*360./NSide1);
  //thy = 90.+ InclAng1;    phy = 90.000000-(4.*360./NSide1);
  //thz = InclAng1;         phz = 90.000000-(4.*360./NSide1);
  thx = 90.000000;        phx = 0.000000-(ZRotAngle+4.*360./NSide1);
  thy = 90.+ InclAng1;    phy = 90.000000-(ZRotAngle+4.*360./NSide1);
  thz = InclAng1;         phz = 90.000000-(ZRotAngle+4.*360./NSide1);
  TGeoRotation *pMatrix11 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix10 = new TGeoCombiTrans("", dx,dy,dz,pMatrix11);
  
  
  dx= -(( (Length1+Frame_Length)/2)*sin(InclAng1*PI/180.)+ Rmin1 + (Thickness1*2+Frame_Depth))*sin((ZRotAngle+4*(360/NSide1))*PI/180.);    // rotation by 240+15 deg/ z axis
  dy= -(( (Length1+Frame_Length)/2)*sin(InclAng1*PI/180.)+ Rmin1 + (Thickness1*2+Frame_Depth))*cos((ZRotAngle+4*(360/NSide1))*PI/180.);    // rotation by 240+15 deg/ z axis
  //dz = -(Length1+Frame_Length)*cos(InclAng1*PI/180.)/2 + ((Rmin1)/tan(AngRangeMin1*PI/180.));
  TGeoCombiTrans*
  pMatrix10b = new TGeoCombiTrans("", dx,dy,dz,pMatrix11);
  
  
  //########################
  
  // Combi transformation:
  //dx =  dx -(WidthHalf1/2)*(1/(cos(AngTrap1)*cos(AngTrap1)))*(cos(4*(360./NSide1)*PI/180.)+cos(5*(360./NSide1)*PI/180.));   // considering  real barycentre position;
  //dx =  dx -(WidthHalf1/2)*(cos(4*(360./NSide1)*PI/180.) + cos(5*(360./NSide1)*PI/180.)); // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf1/2)*(1/(cos(AngTrap1)*cos(AngTrap1)))*(sin(4*(360./NSide1)*PI/180.)
  //                                                             +sin(5*(360./NSide1)*PI/180.));  // considering  real barycentre position;
  //dy =  dy +(WidthHalf1/2)*(sin(4*(360./NSide1)*PI/180.) + sin(5*(360./NSide1)*PI/180));      // considering  intersection of 2 medianes
  
  // +rotation of 30 degrees:
  dx= -(( (Length1)/2)*sin(InclAng1*PI/180.)+ Rmin1)*sin((ZRotAngle+5*(360/NSide1))*PI/180.);  // rotation by 300+30 deg/ z axis
  dy= -(( (Length1)/2)*sin(InclAng1*PI/180.)+ Rmin1)*cos((ZRotAngle+5*(360/NSide1))*PI/180.);  // rotation by 300+30 deg/ z axis
  dz = -(Length1)*cos(InclAng1*PI/180.)/2 + ((Rmin1)/tan(AngRangeMin1*PI/180.)) - ShiftToWorldEdge ;
  
  
  // Rotation:
  //thx = 90.000000;        phx = 0.000000-(5.*360./NSide1);
  //thy = 90.+ InclAng1;    phy = 90.000000-(5.*360./NSide1);
  //thz = InclAng1;         phz = 90.000000-(5.*360./NSide1);
  thx = 90.000000;        phx = 0.000000-(ZRotAngle+5.*360./NSide1);
  thy = 90.+ InclAng1;    phy = 90.000000-(ZRotAngle+5.*360./NSide1);
  thz = InclAng1;         phz = 90.000000-(ZRotAngle+5.*360./NSide1);
  TGeoRotation *pMatrix13 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix12 = new TGeoCombiTrans("", dx,dy,dz,pMatrix13);
  
  
  dx= -(( (Length1+Frame_Length)/2)*sin(InclAng1*PI/180.)+ Rmin1 + (Thickness1*2+Frame_Depth))*sin((ZRotAngle+5*(360/NSide1))*PI/180.);  // rotation by 300+15 deg/ z axis
  dy= -(( (Length1+Frame_Length)/2)*sin(InclAng1*PI/180.)+ Rmin1 + (Thickness1*2+Frame_Depth))*cos((ZRotAngle+5*(360/NSide1))*PI/180.);  // rotation by 300+15 deg/ z axis
  //dz = -(Length1+Frame_Length)*cos(InclAng1*PI/180.)/2 + ((Rmin1)/tan(AngRangeMin1*PI/180.));
  TGeoCombiTrans*
  pMatrix12b = new TGeoCombiTrans("", dx,dy,dz,pMatrix13);
  
  
  
  //###############################################################################
  //
  // Outer layer
  //
  //###############################################################################


  Double_t ZRotAngleOL = 15.;  // degrees

  
  // Combi transformation:
  //dy = -((Length3/2  + (WidthHalf3/2)*tan(AngTrap3) )*sin(InclAng3*PI/180.)+ Rmin3); // considering  real barycentre position
  //dx = 0.000;
  //dy = -(( Length3/2)*sin(InclAng3*PI/180.)+ Rmin3 );                               // considering  intersection of 2 medianes
  //dz = -Length3*cos(InclAng3*PI/180.)/2 + (Rmin3/tan(AngRangeMin3*PI/180.)) - ShiftToWorldEdge ;
  dx = (-(( (Length3)/2)*sin(InclAng3*PI/180.)+ Rmin3))*sin((ZRotAngleOL)*PI/180.);
  dy = (-(( (Length3)/2)*sin(InclAng3*PI/180.)+ Rmin3))*cos((ZRotAngleOL)*PI/180.);;   // considering  intersection of 2 medianes
  dz = -(Length3)*cos(InclAng3*PI/180.)/2 + ((Rmin3)/tan(AngRangeMin3*PI/180.)) - ShiftToWorldEdge;

 
  // Rotation:
  thx = 90.000000;        phx = 0.000000-ZRotAngleOL;
  thy = 90.+ InclAng3;    phy = 90.000000-ZRotAngleOL;
  thz = InclAng3;         phz = 90.000000-ZRotAngleOL;
  TGeoRotation *pMatrix67 = new TGeoRotation("",thx,phx,thy,phy,thz,phz); // geant3 angles
  TGeoCombiTrans*
  pMatrix66 = new TGeoCombiTrans("", dx,dy,dz,pMatrix67);
  
  
  //dy = -(( (Length3+Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3 + (Thickness3*2+Frame_Depth));  // considering  intersection of 2 medianes and CBFrame
  //// dz = -(Length3+Frame_Length)*cos(InclAng3*PI/180.)/2 + (Rmin3/tan(AngRangeMin3*PI/180.));
 
  dx = (-(( (Length3+Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3 + (Thickness3*2+Frame_Depth)))*sin((ZRotAngleOL)*PI/180.);
  dy = (-(( (Length3+Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3 + (Thickness3*2+Frame_Depth)))*cos((ZRotAngleOL)*PI/180.);;   // considering  intersection of 2 medianes
  //  dz = -(Length1+Frame_Length)*cos(InclAng1*PI/180.)/2 + ((Rmin1)/tan(AngRangeMin1*PI/180.))- ShiftToWorldEdge;

 TGeoCombiTrans*
  pMatrix66b = new TGeoCombiTrans("", dx,dy,dz,pMatrix67);
  
  //########################
  
  // Combi transformation:
  //dx =  dx -(WidthHalf3/2)*(1/(cos(AngTrap3)*cos(AngTrap3)))*(1 + cos((360./NSide3)*PI/180.)); // considering  real barycentre position;
  //dx =  -(WidthHalf3/2)*(1 + cos((360./NSide3)*PI/180));                                     // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf3/2)*(1/(cos(AngTrap3)*cos(AngTrap3)))*sin((360./NSide3)*PI/180.);      // considering  real barycentre position;
  //dy = dy +(WidthHalf3/2)*sin((360./NSide3)*PI/180);                                         // considering  intersection of 2 medianes
  
  dx=  (-(( (Length3)/2)*sin(InclAng3*PI/180.)+ Rmin3))*sin((360/NSide3 + ZRotAngleOL)*PI/180.);  // rotation by 360/12=30 deg/ z axis
  dy=  (-(( (Length3)/2)*sin(InclAng3*PI/180.)+ Rmin3))*cos((360/NSide3 + ZRotAngleOL)*PI/180.);  // rotation by 360/12=30 deg/ z axis
  dz = -Length3*cos(InclAng3*PI/180.)/2 + (Rmin3/tan(AngRangeMin3*PI/180.)) - ShiftToWorldEdge ;
  // Rotation:
  thx = 90.000000;        phx = 0.000000-(360/NSide3 + ZRotAngleOL);
  thy = 90.+ InclAng3;    phy = 90.000000-(360/NSide3 + ZRotAngleOL);
  thz = InclAng3;         phz = 90.000000-(360/NSide3 + ZRotAngleOL);
  TGeoRotation *pMatrix69 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix68 = new TGeoCombiTrans("", dx,dy,dz,pMatrix69);
  
 
  dx = -(( (Length3+Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3 + (Thickness3*2+Frame_Depth))*sin((ZRotAngleOL+360/NSide3)*PI/180.) ;  // considering  intersection of 2 medianes and CBFrame
  dy=  -(( (Length3+Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3 + (Thickness3*2+Frame_Depth))*cos((ZRotAngleOL+360/NSide3)*PI/180.);  // rotation by 60+15 deg/ z axis
 //dz = -(Length3+Frame_Length)*cos(InclAng3*PI/180.)/2 + (Rmin3/tan(AngRangeMin3*PI/180.));
  TGeoCombiTrans*
  pMatrix68b = new TGeoCombiTrans("", dx,dy,dz,pMatrix69);
  
  //########################
  
  
  // Combi transformation:
  //dx =  dx -(WidthHalf3/2)*(1/(cos(AngTrap3)*cos(AngTrap3)))*(cos((360/NSide3 + ZRotAngleOL)*PI/180.)+cos(2*(360/NSide3 + ZRotAngleOL)*PI/180.));     // considering  real barycentre position;
  //dx =  dx -(WidthHalf3/2)*(cos((360/NSide3 + ZRotAngleOL)*PI/180.) + cos(2*(360/NSide3 + ZRotAngleOL)*3.14159/180));   // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf3/2)*(1/(cos(AngTrap3)*cos(AngTrap3)))*(sin((360/NSide3 + ZRotAngleOL)*PI/180.)+sin(2*(360./NSide2)*PI/180.));    // considering  real barycentre position;
  //dy =  dy +(WidthHalf3/2)*(sin((360/NSide3 + ZRotAngleOL)*PI/180.) + sin(2*(360/NSide3 + ZRotAngleOL)*PI/180));        // considering  intersection of 2 medianes
  
  dx=  -(( (Length3)/2)*sin(InclAng3*PI/180.)+ Rmin3)*sin((2*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 60 deg/ z axis
  dy=  -(( (Length3)/2)*sin(InclAng3*PI/180.)+ Rmin3 )*cos((2*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 60 deg/ z axis
  dz = -Length3*cos(InclAng3*PI/180.)/2 + (Rmin3/tan(AngRangeMin3*PI/180.)) - ShiftToWorldEdge ;
  // Rotation:
  thx = 90.000000;        phx = 0.000000-(2*360/NSide3 + ZRotAngleOL);
  thy = 90.+ InclAng3;    phy = 90.000000-(2*360/NSide3 + ZRotAngleOL);
  thz = InclAng3;         phz = 90.000000-(2*360/NSide3 + ZRotAngleOL);
  TGeoRotation *pMatrix71 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix70 = new TGeoCombiTrans("", dx,dy,dz,pMatrix71);
  
  
  dx=  -(( (Length3+ Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3 + (Thickness3*2+Frame_Depth))*sin((2*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 60 deg/ z axis
  dy=  -(( (Length3+ Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3 + (Thickness3*2+Frame_Depth))*cos((2*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 60 deg/ z axis
  dz= -(Length3+Frame_Length)*cos(InclAng3*PI/180.)/2 + (Rmin3/tan(AngRangeMin3*PI/180.))- ShiftToWorldEdge;
  TGeoCombiTrans*
  pMatrix70b = new TGeoCombiTrans("", dx,dy,dz,pMatrix71);
  
  //########################
  
  // Combi transformation:
  //dx =  dx -(WidthHalf3/2)*(1/(cos(AngTrap3)*cos(AngTrap3)))*(cos(2*(360/NSide3 + ZRotAngleOL)*PI/180.)+cos(3*(360/NSide3 + ZRotAngleOL)*PI/180.));     // considering  real barycentre position;
  //dx =  dx -(WidthHalf3/2)*(cos(2*(360/NSide3 + ZRotAngleOL)*PI/180.) + cos(3*(360/NSide3 + ZRotAngleOL)*3.14159/180)); // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf3/2)*(1/(cos(AngTrap3)*cos(AngTrap3)))*(sin(2*(360/NSide3 + ZRotAngleOL)*PI/180.)+sin(3*(360/NSide3 + ZRotAngleOL)*PI/180.));    // considering  real barycentre position;
  //dy =  dy +(WidthHalf3/2)*(sin(2*(360/NSide3 + ZRotAngleOL)*PI/180.) + sin(3*(360/NSide3 + ZRotAngleOL)*PI/180));      // considering  intersection of 2 medianes
  
  dx=  -(( (Length3)/2)*sin(InclAng3*PI/180.)+ Rmin3)*sin((3*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 90 deg/ z axis
  dy=  -(( (Length3)/2)*sin(InclAng3*PI/180.)+ Rmin3)*cos((3*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 90 deg/ z axis
  dz = -Length3*cos(InclAng3*PI/180.)/2 + (Rmin3/tan(AngRangeMin3*PI/180.)) - ShiftToWorldEdge ;
  // Rotation:
  thx = 90.000000;        phx = 0.000000-(3*360/NSide3 + ZRotAngleOL);
  thy = 90.+ InclAng3;    phy = 90.000000-(3*360/NSide3 + ZRotAngleOL);
  thz = InclAng3;         phz = 90.000000-(3*360/NSide3 + ZRotAngleOL);
  TGeoRotation *pMatrix73 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix72 = new TGeoCombiTrans("", dx,dy,dz,pMatrix73);
  
  dx=  -(( (Length3+ Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3 + (Thickness3*2+Frame_Depth))*sin((3*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 90 deg/ z axis
  dy=  -(( (Length3+ Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3 + (Thickness3*2+Frame_Depth))*cos((3*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 90 deg/ z axis
  //dz= -(Length3+Frame_Length)*cos(InclAng3*PI/180.)/2 + (Rmin3/tan(AngRangeMin3*PI/180.));
  TGeoCombiTrans*
  pMatrix72b = new TGeoCombiTrans("", dx,dy,dz,pMatrix73);
  
  //########################
  
  // Combi transformation:
  //dx =  dx -(WidthHalf3/2)*(1/(cos(AngTrap3)*cos(AngTrap3)))*(cos(3*(360/NSide3 + ZRotAngleOL)*PI/180.)+cos(4*(360/NSide3 + ZRotAngleOL)*PI/180.));      // considering  real barycentre position;
  //dx =  dx -(WidthHalf3/2)*(cos(3*(360/NSide3 + ZRotAngleOL)*PI/180.) + cos(4*(360/NSide3 + ZRotAngleOL)*3.14159/180));  // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf3/2)*(1/(cos(AngTrap3)*cos(AngTrap3)))*(sin(3*(360/NSide3 + ZRotAngleOL)*PI/180.)+sin(4*(360/NSide3 + ZRotAngleOL)*PI/180.));     // considering  real barycentre position;
  //dy =  dy +(WidthHalf3/2)*(sin(3*(360/NSide3 + ZRotAngleOL)*PI/180.) + sin(4*(360/NSide3 + ZRotAngleOL)*PI/180));       // considering  intersection of 2 medianes
  
  dx=  -(( (Length3)/2)*sin(InclAng3*PI/180.)+ Rmin3)*sin((4*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 120 deg/ z axis
  dy=  -(( (Length3)/2)*sin(InclAng3*PI/180.)+ Rmin3)*cos((4*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 120 deg/ z axis
  dz = -Length3*cos(InclAng2*PI/180.)/2 + (Rmin3/tan(AngRangeMin3*PI/180.)) - ShiftToWorldEdge ;
  
  // Rotation:
  thx = 90.000000;        phx = 0.000000-(4*360/NSide3 + ZRotAngleOL);
  thy = 90.+ InclAng3;    phy = 90.000000-(4*360/NSide3 + ZRotAngleOL);
  thz = InclAng3;         phz = 90.000000-(4*360/NSide3 + ZRotAngleOL);
  TGeoRotation *pMatrix75 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix74 = new TGeoCombiTrans("", dx,dy,dz,pMatrix75);
  
  dx=  -(( (Length3+ Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3+ (Thickness3*2+Frame_Depth))*sin((4*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 120 deg/ z axis
  dy=  -(( (Length3+ Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3+ (Thickness3*2+Frame_Depth))*cos((4*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 120 deg/ z axis
  //dz= -(Length3+Frame_Length)*cos(InclAng3*PI/180.)/2 + (Rmin3/tan(AngRangeMin3*PI/180.));
  TGeoCombiTrans*
  pMatrix74b = new TGeoCombiTrans("", dx,dy,dz,pMatrix75);
  
  //########################
  
  
  // Combi transformation:
  //dx =  dx -(WidthHalf3/2)*(1/(cos(AngTrap3)*cos(AngTrap3)))*(cos(4*(360/NSide3 + ZRotAngleOL)*PI/180.)+cos(5*(360/NSide3 + ZRotAngleOL)*PI/180.));      // considering  real barycentre position;
  //dx =  dx -(WidthHalf3/2)*(cos(4*(360/NSide3 + ZRotAngleOL)*PI/180.) + cos(5*(360/NSide3 + ZRotAngleOL)*3.14159/180));  // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf3/2)*(1/(cos(AngTrap3)*cos(AngTrap3)))*(sin(4*(360/NSide3 + ZRotAngleOL)*PI/180.)+sin(5*(360/NSide3 + ZRotAngleOL)*PI/180.));     // considering  real barycentre position;
  //dy =  dy +(WidthHalf3/2)*(sin(4*(360/NSide3 + ZRotAngleOL)*PI/180.) + sin(5*(360/NSide3 + ZRotAngleOL)*PI/180));       // considering  intersection of 2 medianes
  
  dx=  -(( (Length3)/2)*sin(InclAng3*PI/180.)+ Rmin3)*sin((5*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 150 deg/ z axis
  dy=  -(( (Length3)/2)*sin(InclAng3*PI/180.)+ Rmin3)*cos((5*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 150 deg/ z axis
  dz = -Length3*cos(InclAng3*PI/180.)/2 + (Rmin3/tan(AngRangeMin3*PI/180.))- ShiftToWorldEdge  ;
  
  // Rotation:
  thx = 90.000000;        phx = 0.000000-(5*360/NSide3 + ZRotAngleOL);
  thy = 90.+ InclAng3;    phy = 90.000000-(5*360/NSide3 + ZRotAngleOL);
  thz = InclAng3;         phz = 90.000000-(5*360/NSide3 + ZRotAngleOL);
  TGeoRotation *pMatrix77 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix76 = new TGeoCombiTrans("", dx,dy,dz,pMatrix77);
  
  dx=  -(( (Length3+ Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3+ (Thickness3*2+Frame_Depth))*sin((5*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 150 deg/ z axis
  dy=  -(( (Length3+ Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3+ (Thickness3*2+Frame_Depth))*cos((5*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 150 deg/ z axis
  //dz= -(Length3+Frame_Length)*cos(InclAng3*PI/180.)/2 + (Rmin3/tan(AngRangeMin3*PI/180.));
  
  TGeoCombiTrans*
  pMatrix76b = new TGeoCombiTrans("", dx,dy,dz,pMatrix77);
  
  //########################
  
  // Combi transformation:
  //dx =  dx -(WidthHalf3/2)*(1/(cos(AngTrap3)*cos(AngTrap3)))*(cos(5*(360/NSide3 + ZRotAngleOL)*PI/180.)+cos(6*(360/NSide3 + ZRotAngleOL)*PI/180.));      // considering  real barycentre position;
  //dx =  dx -(WidthHalf3/2)*(cos(5*(360/NSide3 + ZRotAngleOL)*PI/180.) + cos(6*(360/NSide3 + ZRotAngleOL)*3.14159/180));  // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf3/2)*(1/(cos(AngTrap3)*cos(AngTrap3)))*(sin(5*(360/NSide3 + ZRotAngleOL)*PI/180.)+sin(6*(360/NSide3 + ZRotAngleOL)*PI/180.));     // considering  real barycentre position;
  //dy =  dy +(WidthHalf3/2)*(sin(5*(360/NSide3 + ZRotAngleOL)*PI/180.) + sin(6*(360/NSide3 + ZRotAngleOL)*PI/180));       // considering  intersection of 2 medianes
  
  dx=  -(( (Length3)/2)*sin(InclAng3*PI/180.)+ Rmin3)*sin((6*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 180 deg/ z axis
  dy=  -(( (Length3)/2)*sin(InclAng3*PI/180.)+ Rmin3)*cos((6*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 180 deg/ z axis
  dz = -Length3*cos(InclAng3*PI/180.)/2 + (Rmin3/tan(AngRangeMin3*PI/180.)) - ShiftToWorldEdge ;
  
  // Rotation:
  thx = 90.000000;        phx = 0.000000-(6*360/NSide3 + ZRotAngleOL);
  thy = 90.+ InclAng3;    phy = 90.000000-(6*360/NSide3 + ZRotAngleOL);
  thz = InclAng3;         phz = 90.000000-(6*360/NSide3 + ZRotAngleOL);
  TGeoRotation *pMatrix79 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix78 = new TGeoCombiTrans("", dx,dy,dz,pMatrix79);
  
  dx=  -(( (Length3+ Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3 + (Thickness3*2+Frame_Depth))*sin((6*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 180 deg/ z axis
  dy=  -(( (Length3+ Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3 + (Thickness3*2+Frame_Depth))*cos((6*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 180 deg/ z axis
  //dz= -(Length3+Frame_Length)*cos(InclAng3*PI/180.)/2 + (Rmin3/tan(AngRangeMin3*PI/180.));
  
  TGeoCombiTrans*
  pMatrix78b = new TGeoCombiTrans("", dx,dy,dz,pMatrix79);
  
  //########################
  
  // Combi transformation:
  //dx =  dx -(WidthHalf3/2)*(1/(cos(AngTrap3)*cos(AngTrap3)))*(cos(6*(360/NSide3 + ZRotAngleOL)*PI/180.)+cos(7*(360/NSide3 + ZRotAngleOL)*PI/180.));     // considering  real barycentre position;
  //dx =  dx -(WidthHalf3/2)*(cos(6*(360/NSide3 + ZRotAngleOL)*PI/180.) + cos(7*(360/NSide3 + ZRotAngleOL)*3.14159/180)); // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf3/2)*(1/(cos(AngTrap3)*cos(AngTrap3)))*(sin(6*(360/NSide3 + ZRotAngleOL)*PI/180.)+sin(7*(360/NSide3 + ZRotAngleOL)*PI/180.));    // considering  real barycentre position;
  //dy =  dy +(WidthHalf3/2)*(sin(6*(360/NSide3 + ZRotAngleOL)*PI/180.) + sin(7*(360/NSide3 + ZRotAngleOL)*PI/180));      // considering  intersection of 2 medianes
  
  dx=  -(( (Length3)/2)*sin(InclAng3*PI/180.)+ Rmin3)*sin((7*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 210 deg/ z axis
  dy=  -(( (Length3)/2)*sin(InclAng3*PI/180.)+ Rmin3)*cos((7*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 210 deg/ z axis
  dz = -Length3*cos(InclAng3*PI/180.)/2 + (Rmin3/tan(AngRangeMin3*PI/180.)) - ShiftToWorldEdge ;
  
  // Rotation:
  thx = 90.000000;        phx = 0.000000-(7*360/NSide3 + ZRotAngleOL);
  thy = 90.+ InclAng3;    phy = 90.000000-(7*360/NSide3 + ZRotAngleOL);
  thz = InclAng3;         phz = 90.000000-(7*360/NSide3 + ZRotAngleOL);
  TGeoRotation *pMatrix81 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix80 = new TGeoCombiTrans("", dx,dy,dz,pMatrix81);
  
  dx=  -(( (Length3+ Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3+ (Thickness3*2+Frame_Depth))*sin((7*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 210 deg/ z axis
  dy=  -(( (Length3+ Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3+ (Thickness3*2+Frame_Depth))*cos((7*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 210 deg/ z axis
  //dz= -(Length2+Frame_Length)*cos(InclAng3*PI/180.)/2 + (Rmin3/tan(AngRangeMin3*PI/180.));
  
  TGeoCombiTrans*
  pMatrix80b = new TGeoCombiTrans("", dx,dy,dz,pMatrix81);
  
  //########################
  
  
  // Combi transformation:
  //dx =  dx -(WidthHalf3/2)*(1/(cos(AngTrap3)*cos(AngTrap3)))*(cos(7*(360/NSide3 + ZRotAngleOL)*PI/180.)+cos(8*(360/NSide3 + ZRotAngleOL)*PI/180.));     // considering  real barycentre position;
  //dx =  dx -(WidthHalf3/2)*(cos(7*(360/NSide3 + ZRotAngleOL)*PI/180.) + cos(8*(360/NSide3 + ZRotAngleOL)*3.14159/180)); // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf3/2)*(1/(cos(AngTrap3)*cos(AngTrap3)))*(sin(7*(360/NSide3 + ZRotAngleOL)*PI/180.)+sin(8*(360/NSide3 + ZRotAngleOL)*PI/180.));    // considering  real barycentre position;
  //dy =  dy +(WidthHalf3/2)*(sin(7*(360/NSide3 + ZRotAngleOL)*PI/180.) + sin(8*(360/NSide3 + ZRotAngleOL)*PI/180));      // considering  intersection of 2 medianes
  
  dx=  -(( (Length3)/2)*sin(InclAng3*PI/180.)+ Rmin3)*sin((8*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 240 deg/ z axis
  dy=  -(( (Length3)/2)*sin(InclAng3*PI/180.)+ Rmin3)*cos((8*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 240 deg/ z axis
  dz = -Length3*cos(InclAng3*PI/180.)/2 + (Rmin3/tan(AngRangeMin3*PI/180.)) - ShiftToWorldEdge ;
  // Rotation:
  thx = 90.000000;        phx = 0.000000-(8*360/NSide3 + ZRotAngleOL);
  thy = 90.+ InclAng3;    phy = 90.000000-(8*360/NSide3 + ZRotAngleOL);
  thz = InclAng3;         phz = 90.000000-(8*360/NSide3 + ZRotAngleOL);
  TGeoRotation *pMatrix83 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix82 = new TGeoCombiTrans("", dx,dy,dz,pMatrix83);
  
  
  dx=  -(( (Length3+ Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3 + (Thickness3*2+Frame_Depth))*sin((8*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 240 deg/ z axis
  dy=  -(( (Length3+ Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3 + (Thickness3*2+Frame_Depth))*cos((8*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 240 deg/ z axis
  //dz= -(Length3+Frame_Length)*cos(InclAng3*PI/180.)/2 + (Rmin3/tan(AngRangeMin3*PI/180.));
  
  TGeoCombiTrans*
  pMatrix82b = new TGeoCombiTrans("", dx,dy,dz,pMatrix83);
  
  //########################
  
  
  // Combi transformation:
  //dx =  dx -(WidthHalf3/2)*(1/(cos(AngTrap3)*cos(AngTrap3)))*(cos(8*(360/NSide3 + ZRotAngleOL)*PI/180.)+cos(9*(360/NSide3 + ZRotAngleOL)*PI/180.));      // considering  real barycentre position;
  //dx =  dx -(WidthHalf3/2)*(cos(8*(360/NSide3 + ZRotAngleOL)*PI/180.) + cos(9*(360/NSide3 + ZRotAngleOL)*3.14159/180));  // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf3/2)*(1/(cos(AngTrap3)*cos(AngTrap3)))*(sin(8*(360/NSide3 + ZRotAngleOL)*PI/180.)+sin(9*(360/NSide3 + ZRotAngleOL)*PI/180.));     // considering  real barycentre position;
  //dy =  dy +(WidthHalf3/2)*(sin(8*(360/NSide3 + ZRotAngleOL)*PI/180.) + sin(9*(360/NSide3 + ZRotAngleOL)*PI/180));       // considering  intersection of 2 medianes
  
  dx=  -(( (Length3)/2)*sin(InclAng3*PI/180.)+ Rmin3)*sin((9*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 270 deg/ z axis
  dy=  -(( (Length3)/2)*sin(InclAng3*PI/180.)+ Rmin3)*cos((9*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 270 deg/ z axis
  dz = -Length3*cos(InclAng3*PI/180.)/2 + (Rmin3/tan(AngRangeMin3*PI/180.))  - ShiftToWorldEdge;
  
  // Rotation:
  thx = 90.000000;        phx = 0.000000-(9*360/NSide3 + ZRotAngleOL);
  thy = 90.+ InclAng3;    phy = 90.000000-(9*360/NSide3 + ZRotAngleOL);
  thz = InclAng3;         phz = 90.000000-(9*360/NSide3 + ZRotAngleOL);
  TGeoRotation *pMatrix85 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix84 = new TGeoCombiTrans("", dx,dy,dz,pMatrix85);
  
  dx=  -(( (Length3+ Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3+ (Thickness3*2+Frame_Depth) )*sin((9*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 270 deg/ z axis
  dy=  -(( (Length3+ Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3+ (Thickness3*2+Frame_Depth))*cos((9*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 270 deg/ z axis
  //dz= -(Length3+Frame_Length)*cos(InclAng3*PI/180.)/2 + (Rmin3/tan(AngRangeMin3*PI/180.));
  
  
  TGeoCombiTrans*
  pMatrix84b = new TGeoCombiTrans("", dx,dy,dz,pMatrix85);
  
  //########################
  
  // Combi transformation:
  //dx =  dx -(WidthHalf3/2)*(1/(cos(AngTrap3)*cos(AngTrap3)))*(cos(9*(360/NSide3 + ZRotAngleOL)*PI/180.)+cos(10*(360/NSide3 + ZRotAngleOL)*PI/180.));     // considering  real barycentre position;
  // dx =  dx -(WidthHalf3/2)*(cos(9*(360/NSide3 + ZRotAngleOL)*PI/180.) + cos(10*(360/NSide3 + ZRotAngleOL)*3.14159/180)); // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf3/2)*(1/(cos(AngTrap3)*cos(AngTrap3)))*(sin(9*(360/NSide3 + ZRotAngleOL)*PI/180.)+sin(10*(360/NSide3 + ZRotAngleOL)*PI/180.));    // considering  real barycentre position;
  //dy =  dy +(WidthHalf3/2)*(sin(9*(360/NSide3 + ZRotAngleOL)*PI/180.) + sin(10*(360/NSide3 + ZRotAngleOL)*PI/180));     // considering  intersection of 2 medianes
  
  dx=  -(( (Length3)/2)*sin(InclAng3*PI/180.)+ Rmin3)*sin((10*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 300 deg/ z axis
  dy=  -(( (Length3)/2)*sin(InclAng3*PI/180.)+ Rmin3)*cos((10*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 300 deg/ z axis
  dz = -Length3*cos(InclAng3*PI/180.)/2 + (Rmin3/tan(AngRangeMin3*PI/180.)) - ShiftToWorldEdge ;
  
  // Rotation:
  thx = 90.000000;        phx = 0.000000-(10*360/NSide3 + ZRotAngleOL);
  thy = 90.+ InclAng3;    phy = 90.000000-(10*360/NSide3 + ZRotAngleOL);
  thz = InclAng3;         phz = 90.000000-(10*360/NSide3 + ZRotAngleOL);
  TGeoRotation *pMatrix87 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix86 = new TGeoCombiTrans("", dx,dy,dz,pMatrix87);
  
  dx=  -(( (Length3+ Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3+ (Thickness3*2+Frame_Depth))*sin((10*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 300 deg/ z axis
  dy=  -(( (Length3+ Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3+ (Thickness3*2+Frame_Depth))*cos((10*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 300 deg/ z axis
  //dz= -(Length3+Frame_Length)*cos(InclAng3*PI/180.)/2 + (Rmin3/tan(AngRangeMin3*PI/180.));
  
  TGeoCombiTrans*
  pMatrix86b = new TGeoCombiTrans("", dx,dy,dz,pMatrix87);
  
  //########################
  
  
  // Combi transformation:
  //dx =  dx -(WidthHalf3/2)*(1/(cos(AngTrap3)*cos(AngTrap3)))*(cos(10*(360/NSide3 + ZRotAngleOL)*PI/180.)+cos(11*(360/NSide3 + ZRotAngleOL)*PI/180.));      // considering  real barycentre position;
  //dx =  dx -(WidthHalf3/2)*(cos(10*(360/NSide3 + ZRotAngleOL)*PI/180.) + cos(11*(360/NSide3 + ZRotAngleOL)*3.14159/180)); // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf3/2)*(1/(cos(AngTrap3)*cos(AngTrap3)))*(sin(10*(360/NSide3 + ZRotAngleOL)*PI/180.)+sin(11*(360/NSide3 + ZRotAngleOL)*PI/180.));     // considering  real barycentre position;
  //dy =  dy +(WidthHalf3/2)*(sin(10*(360/NSide3 + ZRotAngleOL)*PI/180.) + sin(11*(360/NSide3 + ZRotAngleOL)*PI/180));      // considering  intersection of 2 medianes
  
  dx=  -(((Length3)/2)*sin(InclAng3*PI/180.)+ Rmin3)*sin((11*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 330 deg/ z axis
  dy=  -(((Length3)/2)*sin(InclAng3*PI/180.)+ Rmin3)*cos((11*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 330 deg/ z axis
  dz = -Length3*cos(InclAng3*PI/180.)/2 + (Rmin3/tan(AngRangeMin3*PI/180.)) - ShiftToWorldEdge ;
  
  // Rotation:
  thx = 90.000000;        phx = 0.000000-(11*360/NSide3 + ZRotAngleOL);
  thy = 90.+ InclAng3;    phy = 90.000000-(11*360/NSide3 + ZRotAngleOL);
  thz = InclAng3;         phz = 90.000000-(11*360/NSide3 + ZRotAngleOL);
  TGeoRotation *pMatrix89 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix88 = new TGeoCombiTrans("", dx,dy,dz,pMatrix89);
  
  dx=  -(((Length3+ Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3+ (Thickness3*2+Frame_Depth))*sin((11*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 330 deg/ z axis
  dy=  -(((Length3+ Frame_Length)/2)*sin(InclAng3*PI/180.)+ Rmin3+ (Thickness3*2+Frame_Depth))*cos((11*(360/NSide3) + ZRotAngleOL)*PI/180.);  // rotation by 330 deg/ z axis
  //dz= -(Length3+Frame_Length)*cos(InclAng3*PI/180.)/2 + (Rmin3/tan(AngRangeMin3*PI/180.));
  
  
  TGeoCombiTrans*
  pMatrix88b = new TGeoCombiTrans("", dx,dy,dz,pMatrix89);
  
  //########################
  
  
  // Combi transformation:
  dx = 0.000000;
  dy = 10.8600;
  dz = 18.60000;
  // Rotation:
  thx = 180.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 0.000000;
  thz = 90.000000;    phz = 90.000000;
  //   TGeoRotation *pMatrix91 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  //   TGeoCombiTrans*
  //     pMatrix90 = new TGeoCombiTrans("", dx,dy,dz,pMatrix91);
  
  // Combi transformation:
  dx = 7.67000;
  dy = 7.67000;
  dz = 18.60000;
  // Rotation:
  thx = 0.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 315.000000;
  thz = 90.000000;    phz = 45.000000;
  //   TGeoRotation *pMatrix93 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  //   TGeoCombiTrans*
  //     pMatrix92 = new TGeoCombiTrans("", dx,dy,dz,pMatrix93);
  
  // Combi transformation:
  dx = 10.86000;
  dy = 0.00000;
  dz = 18.60000;
  // Rotation:
  thx = 180.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = 90.000000;    phz = 0.000000;
  //   TGeoRotation *pMatrix95 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  //   TGeoCombiTrans*
  //     pMatrix94 = new TGeoCombiTrans("", dx,dy,dz,pMatrix95);
  
  // Combi transformation:
  dx = 7.67000;
  dy = -7.67000;
  dz = 18.60000;
  // Rotation:
  thx = 0.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 45.000000;
  thz = 90.000000;    phz = 315.000000;
  //   TGeoRotation *pMatrix97 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  //   TGeoCombiTrans*
  //     pMatrix96 = new TGeoCombiTrans("", dx,dy,dz,pMatrix97);
  
  
  
  
  //
  // Intermediate (Middle) layer
  //
  
  //########################
  
  // Combi transformation:
  //dy = -((Length2/2  + (WidthHalf2/2)*tan(AngTrap2) )*sin(InclAng2*PI/180.)+ Rmin2); // considering  real barycentre position
  //dy = -(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2);  // considering  intersection of 2 medianes
  //dz = -Length2*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.));
  
  //dx = 0.000;
  //dy = -(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2);  // considering  intersection of 2 medianes
  //dz = -(Length2)*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.)) - ShiftToWorldEdge ;
  dx = (-(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2))*sin((ZRotAngleOL)*PI/180.);
  dy = (-(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2))*cos((ZRotAngleOL)*PI/180.);;   // considering  intersection of 2 medianes
  dz = -(Length2)*cos(InclAng2*PI/180.)/2 + ((Rmin2)/tan(AngRangeMin2*PI/180.)) - ShiftToWorldEdge;
 
  // Rotation:
  thx = 90.000000;        phx = 0.000000-ZRotAngleOL;
  thy = 90.+ InclAng2;    phy = 90.000000-ZRotAngleOL;
  thz = InclAng2;         phz = 90.000000-ZRotAngleOL;
  TGeoRotation *pMatrix35 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);   // geant3 angles
  TGeoCombiTrans*
  pMatrix34 = new TGeoCombiTrans("", dx,dy,dz,pMatrix35);
  
  //dy = -(( (Length2+ Frame_Length)/2)*sin(InclAng2*PI/180.)+ Rmin2+ (Thickness2*2+Frame_Depth));  // considering  intersection of 2 medianes
  ////dz = -(Length2+Frame_Length)*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.));

  dx = (-(( (Length2+Frame_Length)/2)*sin(InclAng2*PI/180.)+ Rmin2 + (Thickness2*2+Frame_Depth)))*sin((ZRotAngleOL)*PI/180.);
  dy = (-(( (Length2+Frame_Length)/2)*sin(InclAng2*PI/180.)+ Rmin2 + (Thickness2*2+Frame_Depth)))*cos((ZRotAngleOL)*PI/180.);;   // considering  intersection of 2 medianes

   TGeoCombiTrans*
  pMatrix34b = new TGeoCombiTrans("", dx,dy,dz,pMatrix35);
  
  //########################
  
  
  // Combi transformation:
  
  //dx =  dx -(WidthHalf2/2)*(1/(cos(AngTrap2)*cos(AngTrap2)))*(1 + cos((360./NSide3)*PI/180.)); // considering  real barycentre position;
  //dx =  -(WidthHalf2/2)*(1 + cos((360./NSide3)*PI/180));                                     // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf2/2)*(1/(cos(AngTrap2)*cos(AngTrap2)))*sin((360./NSide3)*PI/180.);      // considering  real barycentre position;
  //dy = dy +(WidthHalf2/2)*sin((360./NSide3)*PI/180);                                         // considering  intersection of 2 medianes
  
  //dy = -(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2);  // considering  intersection of 2 medianes
  //dx=  dy*sin((360/NSide2)*PI/180.);  // rotation by 30 deg/ z axis
  //dy=  dy*cos((360/NSide2)*PI/180.);  // rotation by 30 deg/ z axis
  //dz = -(Length2)*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.)) - ShiftToWorldEdge ;

  dx=  (-(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2))*sin((360/NSide2 + ZRotAngleOL)*PI/180.);  // rotation by 360/12=30 deg/ z axis
  dy=  (-(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2))*cos((360/NSide2 + ZRotAngleOL)*PI/180.);  // rotation by 360/12=30 deg/ z axis
  dz = -Length2*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.)) - ShiftToWorldEdge ;

  // Rotation:
  thx = 90.000000;        phx = 0.000000-(360./NSide2+ZRotAngleOL);
  thy = 90.+ InclAng2;    phy = 90.000000-(360./NSide2+ZRotAngleOL);
  thz = InclAng2;         phz = 90.000000-(360./NSide2+ZRotAngleOL);
  TGeoRotation *pMatrix37 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix36 = new TGeoCombiTrans("", dx,dy,dz,pMatrix37);
  
  dy = -(( (Length2+ Frame_Length)/2)*sin(InclAng2*PI/180.)+ Rmin2+ (Thickness2*2+Frame_Depth));  // considering  intersection of 2 medianes
  dx=  dy*sin((360/NSide2+ZRotAngleOL)*PI/180.);  // rotation by 30 deg/ z axis
  dy=  dy*cos((360/NSide2+ZRotAngleOL)*PI/180.);  // rotation by 30 deg/ z axis
  //dz = -(Length2+Frame_Length)*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.));
  TGeoCombiTrans*
  pMatrix36b = new TGeoCombiTrans("", dx,dy,dz,pMatrix37);
  
  //########################
  
  
  
  // Combi transformation:
  //dx =  dx -(WidthHalf2/2)*(1/(cos(AngTrap2)*cos(AngTrap2)))*(cos((360./NSide3)*PI/180.)+cos(2*(360./NSide3)*PI/180.));     // considering  real barycentre position;
  //dx =  dx -(WidthHalf2/2)*(cos((360./NSide3)*PI/180.) + cos(2*(360./NSide3)*3.14159/180));   // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf2/2)*(1/(cos(AngTrap2)*cos(AngTrap2)))*(sin((360./NSide3)*PI/180.)+sin(2*(360./NSide3)*PI/180.));    // considering  real barycentre position;
  //dy =  dy +(WidthHalf2/2)*(sin((360./NSide3)*PI/180.) + sin(2*(360./NSide3)*PI/180));        // considering  intersection of 2 medianes
  
  dx=  -(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2)*sin((2*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 60 deg/ z axis
  dy=  -(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2)*cos((2*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 60 deg/ z axis
  dz = -(Length2)*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.)) - ShiftToWorldEdge ;
  
  // Rotation:
  thx = 90.000000;        phx = 0.000000-(2*360./NSide2+ZRotAngleOL);
  thy = 90.+ InclAng2;    phy = 90.000000-(2*360./NSide2+ZRotAngleOL);
  thz = InclAng2;         phz = 90.000000-(2*360./NSide2+ZRotAngleOL);
  TGeoRotation *pMatrix39 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix38 = new TGeoCombiTrans("", dx,dy,dz,pMatrix39);
  
  dx=  -(( (Length2+ Frame_Length)/2)*sin(InclAng2*PI/180.)+ Rmin2+ (Thickness2*2+Frame_Depth))*sin((2*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 60 deg/ z axis
  dy=  -(( (Length2+ Frame_Length)/2)*sin(InclAng2*PI/180.)+ Rmin2+ (Thickness2*2+Frame_Depth))*cos((2*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 60 deg/ z axis
  //dz = -(Length2+Frame_Length)*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.));
  TGeoCombiTrans*
  pMatrix38b = new TGeoCombiTrans("", dx,dy,dz,pMatrix39);
  
  //########################
  
  
  // Combi transformation:
  //dx =  dx -(WidthHalf2/2)*(1/(cos(AngTrap2)*cos(AngTrap2)))*(cos(2*(360./NSide3)*PI/180.)+cos(3*(360./NSide3)*PI/180.));     // considering  real barycentre position;
  //dx =  dx -(WidthHalf2/2)*(cos(2*(360./NSide3)*PI/180.) + cos(3*(360./NSide3)*3.14159/180)); // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf2/2)*(1/(cos(AngTrap2)*cos(AngTrap2)))*(sin(2*(360./NSide3)*PI/180.)+sin(3*(360./NSide3)*PI/180.));    // considering  real barycentre position;
  //dy =  dy +(WidthHalf2/2)*(sin(2*(360./NSide3)*PI/180.) + sin(3*(360./NSide3)*PI/180));      // considering  intersection of 2 medianes
  
  dx=  -(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2)*sin((3*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 90 deg/ z axis
  dy=  -(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2)*cos((3*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 90 deg/ z axis
  dz = -(Length2)*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.)) - ShiftToWorldEdge ;
  
  // Rotation:
  thx = 90.000000;        phx = 0.000000-(3*360./NSide2+ZRotAngleOL);
  thy = 90.+ InclAng2;    phy = 90.000000-(3*360./NSide2+ZRotAngleOL);
  thz = InclAng2;         phz = 90.000000-(3*360./NSide2+ZRotAngleOL);
  TGeoRotation *pMatrix41 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix40 = new TGeoCombiTrans("", dx,dy,dz,pMatrix41);
  
  dx=  -(( (Length2+ Frame_Length)/2)*sin(InclAng2*PI/180.)+ Rmin2+ (Thickness2*2+Frame_Depth))*sin((3*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 90 deg/ z axis
  dy=  -(( (Length2+ Frame_Length)/2)*sin(InclAng2*PI/180.)+ Rmin2+ (Thickness2*2+Frame_Depth))*cos((3*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 90 deg/ z axis
  //dz = -(Length2+Frame_Length)*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.));
  TGeoCombiTrans*
  pMatrix40b = new TGeoCombiTrans("", dx,dy,dz,pMatrix41);
  
  
  //########################
  
  // Combi transformation:
  //dx =  dx -(WidthHalf2/2)*(1/(cos(AngTrap2)*cos(AngTrap2)))*(cos(3*(360./NSide3)*PI/180.)+cos(4*(360./NSide3)*PI/180.));     // considering  real barycentre position;
  //dx =  dx -(WidthHalf2/2)*(cos(3*(360./NSide3)*PI/180.) + cos(4*(360./NSide3)*3.14159/180)); // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf2/2)*(1/(cos(AngTrap2)*cos(AngTrap2)))*(sin(3*(360./NSide3)*PI/180.)+sin(4*(360./NSide3)*PI/180.));    // considering  real barycentre position;
  //dy =  dy +(WidthHalf2/2)*(sin(3*(360./NSide3)*PI/180.) + sin(4*(360./NSide3)*PI/180));      // considering  intersection of 2 medianes
  
  dx=  -(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2)*sin((4*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 120 deg/ z axis
  dy=  -(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2)*cos((4*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 120 deg/ z axis
  dz = -(Length2)*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.)) - ShiftToWorldEdge ;
  
  // Rotation:
  thx = 90.000000;        phx = 0.000000-(4*360./NSide2+ZRotAngleOL);
  thy = 90.+ InclAng2;    phy = 90.000000-(4*360./NSide2+ZRotAngleOL);
  thz = InclAng2;         phz = 90.000000-(4*360./NSide2+ZRotAngleOL);
  TGeoRotation *pMatrix43 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix42 = new TGeoCombiTrans("", dx,dy,dz,pMatrix43);
  
  dx=  -(( (Length2+ Frame_Length)/2)*sin(InclAng2*PI/180.)+ Rmin2+ (Thickness2*2+Frame_Depth))*sin((4*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 120 deg/ z axis
  dy=  -(( (Length2+ Frame_Length)/2)*sin(InclAng2*PI/180.)+ Rmin2+ (Thickness2*2+Frame_Depth))*cos((4*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 120 deg/ z axis
  //dz = -(Length2+Frame_Length)*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.));
  TGeoCombiTrans*
  pMatrix42b = new TGeoCombiTrans("", dx,dy,dz,pMatrix43);
  
  //########################
  
  // Combi transformation:
  //dx =  dx -(WidthHalf2/2)*(1/(cos(AngTrap2)*cos(AngTrap2)))*(cos(4*(360./NSide3)*PI/180.)+cos(5*(360./NSide3)*PI/180.));     // considering  real barycentre position;
  //dx =  dx -(WidthHalf2/2)*(cos(4*(360./NSide3)*PI/180.) + cos(5*(360./NSide3)*3.14159/180)); // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf2/2)*(1/(cos(AngTrap2)*cos(AngTrap2)))*(sin(4*(360./NSide3)*PI/180.)+sin(5*(360./NSide3)*PI/180.));    // considering  real barycentre position;
  //dy =  dy +(WidthHalf2/2)*(sin(4*(360./NSide3)*PI/180.) + sin(5*(360./NSide3)*PI/180));      // considering  intersection of 2 medianes
  
  dx=  -(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2)*sin((5*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 130 deg/ z axis
  dy=  -(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2)*cos((5*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 150 deg/ z axis
  dz = -(Length2)*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.)) - ShiftToWorldEdge ;
  
  // Rotation:
  thx = 90.000000;        phx = 0.000000-(5*360./NSide2+ZRotAngleOL);
  thy = 90.+ InclAng2;    phy = 90.000000-(5*360./NSide2+ZRotAngleOL);
  thz = InclAng2;         phz = 90.000000-(5*360./NSide2+ZRotAngleOL);
  TGeoRotation *pMatrix45 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix44 = new TGeoCombiTrans("", dx,dy,dz,pMatrix45);
  
  dx=  -(( (Length2+ Frame_Length)/2)*sin(InclAng2*PI/180.)+ Rmin2+ (Thickness2*2+Frame_Depth))*sin((5*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 130 deg/ z axis
  dy=  -(( (Length2+ Frame_Length)/2)*sin(InclAng2*PI/180.)+ Rmin2+ (Thickness2*2+Frame_Depth))*cos((5*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 150 deg/ z axis
  //dz = -(Length2+Frame_Length)*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.));
  TGeoCombiTrans*
  pMatrix44b = new TGeoCombiTrans("", dx,dy,dz,pMatrix45);
  
  //########################
  
  
  // Combi transformation:
  //dx =  dx -(WidthHalf2/2)*(1/(cos(AngTrap2)*cos(AngTrap2)))*(cos(5*(360./NSide3)*PI/180.)+cos(6*(360./NSide3)*PI/180.));     // considering  real barycentre position;
  //dx =  dx -(WidthHalf2/2)*(cos(5*(360./NSide3)*PI/180.) + cos(6*(360./NSide3)*3.14159/180)); // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf2/2)*(1/(cos(AngTrap2)*cos(AngTrap2)))*(sin(5*(360./NSide3)*PI/180.)+sin(6*(360./NSide3)*PI/180.));    // considering  real barycentre position;
  //dy =  dy +(WidthHalf2/2)*(sin(5*(360./NSide3)*PI/180.) + sin(6*(360./NSide3)*PI/180));      // considering  intersection of 2 medianes
  
  dx=  -(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2)*sin((6*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 180 deg/ z axis
  dy=  -(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2)*cos((6*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 180 deg/ z axis
  dz = -(Length2)*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.)) - ShiftToWorldEdge ;
  
  // Rotation:
  thx = 90.000000;        phx = 0.000000-(6*360./NSide2+ZRotAngleOL);
  thy = 90.+ InclAng2;    phy = 90.000000-(6*360./NSide2+ZRotAngleOL);
  thz = InclAng2;         phz = 90.000000-(6*360./NSide2+ZRotAngleOL);
  TGeoRotation *pMatrix47 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix46 = new TGeoCombiTrans("", dx,dy,dz,pMatrix47);
  
  dx=  -(( (Length2+ Frame_Length)/2)*sin(InclAng2*PI/180.)+ Rmin2+ (Thickness2*2+Frame_Depth))*sin((6*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 180 deg/ z axis
  dy=  -(( (Length2+ Frame_Length)/2)*sin(InclAng2*PI/180.)+ Rmin2+ (Thickness2*2+Frame_Depth))*cos((6*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 180 deg/ z axis
  //dz = -(Length2+Frame_Length)*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.));
  TGeoCombiTrans*
  pMatrix46b = new TGeoCombiTrans("", dx,dy,dz,pMatrix47);
  
  //########################
  
  // Combi transformation:
  //dx =  dx -(WidthHalf2/2)*(1/(cos(AngTrap2)*cos(AngTrap2)))*(cos(6*(360./NSide3)*PI/180.)+cos(7*(360./NSide3)*PI/180.));     // considering  real barycentre position;
  //dx =  dx -(WidthHalf2/2)*(cos(6*(360./NSide3)*PI/180.) + cos(7*(360./NSide3)*3.14159/180)); // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf2/2)*(1/(cos(AngTrap2)*cos(AngTrap2)))*(sin(6*(360./NSide3)*PI/180.)+sin(7*(360./NSide3)*PI/180.));    // considering  real barycentre position;
  //dy =  dy +(WidthHalf2/2)*(sin(6*(360./NSide3)*PI/180.) + sin(7*(360./NSide3)*PI/180));      // considering  intersection of 2 medianes
  
  dx=  -(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2)*sin((7*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 210 deg/ z axis
  dy=  -(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2)*cos((7*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 210 deg/ z axis
  dz = -(Length2)*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.)) - ShiftToWorldEdge ;
  
  // Rotation:
  thx = 90.000000;        phx = 0.000000-(7*360./NSide2+ZRotAngleOL);
  thy = 90.+ InclAng2;    phy = 90.000000-(7*360./NSide2+ZRotAngleOL);
  thz = InclAng2;         phz = 90.000000-(7*360./NSide2+ZRotAngleOL);
  TGeoRotation *pMatrix49 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix48 = new TGeoCombiTrans("", dx,dy,dz,pMatrix49);
  
  dx=  -(( (Length2+ Frame_Length)/2)*sin(InclAng2*PI/180.)+ Rmin2+ (Thickness2*2+Frame_Depth))*sin((7*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 210 deg/ z axis
  dy=  -(( (Length2+ Frame_Length)/2)*sin(InclAng2*PI/180.)+ Rmin2+ (Thickness2*2+Frame_Depth))*cos((7*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 210 deg/ z axis
  //dz = -(Length2+Frame_Length)*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.));
  TGeoCombiTrans*
  pMatrix48b = new TGeoCombiTrans("", dx,dy,dz,pMatrix49);
  
  //########################
  
  
  // Combi transformation:
  //dx =  dx -(WidthHalf2/2)*(1/(cos(AngTrap2)*cos(AngTrap2)))*(cos(7*(360./NSide3)*PI/180.)+cos(8*(360./NSide3)*PI/180.));     // considering  real barycentre position;
  //dx =  dx -(WidthHalf2/2)*(cos(7*(360./NSide3)*PI/180.) + cos(8*(360./NSide3)*3.14159/180)); // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf2/2)*(1/(cos(AngTrap2)*cos(AngTrap2)))*(sin(7*(360./NSide3)*PI/180.)+sin(8*(360./NSide3)*PI/180.));    // considering  real barycentre position;
  //dy =  dy +(WidthHalf2/2)*(sin(7*(360./NSide3)*PI/180.) + sin(8*(360./NSide3)*PI/180));      // considering  intersection of 2 medianes
  
  dx=  -(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2)*sin((8*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 240 deg/ z axis
  dy=  -(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2)*cos((8*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 240 deg/ z axis
  dz = -(Length2)*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.)) - ShiftToWorldEdge ;
  
  // Rotation:
  thx = 90.000000;        phx = 0.000000-(8*360./NSide2+ZRotAngleOL);
  thy = 90.+ InclAng2;    phy = 90.000000-(8*360./NSide2+ZRotAngleOL);
  thz = InclAng2;         phz = 90.000000-(8*360./NSide2+ZRotAngleOL);
  TGeoRotation *pMatrix51 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix50 = new TGeoCombiTrans("", dx,dy,dz,pMatrix51);
  
  dx=  -(( (Length2+ Frame_Length)/2)*sin(InclAng2*PI/180.)+ Rmin2+ (Thickness2*2+Frame_Depth))*sin((8*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 240 deg/ z axis
  dy=  -(( (Length2+ Frame_Length)/2)*sin(InclAng2*PI/180.)+ Rmin2+ (Thickness2*2+Frame_Depth))*cos((8*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 240 deg/ z axis
  //dz = -(Length2+Frame_Length)*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.));
  
  TGeoCombiTrans*
  pMatrix50b = new TGeoCombiTrans("", dx,dy,dz,pMatrix51);
  
  //########################
  
  // Combi transformation:
  //dx =  dx -(WidthHalf2/2)*(1/(cos(AngTrap2)*cos(AngTrap2)))*(cos(8*(360./NSide3)*PI/180.)+cos(9*(360./NSide3)*PI/180.));     // considering  real barycentre position;
  //dx =  dx -(WidthHalf2/2)*(cos(8*(360./NSide3)*PI/180.) + cos(9*(360./NSide3)*3.14159/180)); // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf2/2)*(1/(cos(AngTrap2)*cos(AngTrap2)))*(sin(8*(360./NSide3)*PI/180.)+sin(9*(360./NSide3)*PI/180.));    // considering  real barycentre position;
  //dy =  dy +(WidthHalf2/2)*(sin(8*(360./NSide3)*PI/180.) + sin(9*(360./NSide3)*PI/180));      // considering  intersection of 2 medianes
  
  dx=  -(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2)*sin((9*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 270 deg/ z axis
  dy=  -(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2)*cos((9*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 270 deg/ z axis
  dz = -(Length2)*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.))- ShiftToWorldEdge  ;
  
  // Rotation:
  thx = 90.000000;        phx = 0.000000-(9*360./NSide2+ZRotAngleOL);
  thy = 90.+ InclAng2;    phy = 90.000000-(9*360./NSide2+ZRotAngleOL);
  thz = InclAng2;         phz = 90.000000-(9*360./NSide2+ZRotAngleOL);
  TGeoRotation *pMatrix53 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix52 = new TGeoCombiTrans("", dx,dy,dz,pMatrix53);
  
  dx=  -(( (Length2+ Frame_Length)/2)*sin(InclAng2*PI/180.)+ Rmin2+ (Thickness2*2+Frame_Depth))*sin((9*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 270 deg/ z axis
  dy=  -(( (Length2+ Frame_Length)/2)*sin(InclAng2*PI/180.)+ Rmin2+ (Thickness2*2+Frame_Depth))*cos((9*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 270 deg/ z axis
  //dz = -(Length2+Frame_Length)*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.));
  TGeoCombiTrans*
  pMatrix52b = new TGeoCombiTrans("", dx,dy,dz,pMatrix53);
  
  
  //########################
  
  // Combi transformation:
  //dx =  dx -(WidthHalf2/2)*(1/(cos(AngTrap2)*cos(AngTrap2)))*(cos(9*(360./NSide3)*PI/180.)+cos(10*(360./NSide3)*PI/180.));     // considering  real barycentre position;
  //dx =  dx -(WidthHalf2/2)*(cos(9*(360./NSide3)*PI/180.) + cos(10*(360./NSide3)*3.14159/180)); // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf2/2)*(1/(cos(AngTrap2)*cos(AngTrap2)))*(sin(9*(360./NSide3)*PI/180.)+sin(10*(360./NSide3)*PI/180.));    // considering  real barycentre position;
  //dy =  dy +(WidthHalf2/2)*(sin(9*(360./NSide3)*PI/180.) + sin(10*(360./NSide3)*PI/180));      // considering  intersection of 2 medianes
  
  dx=  -(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2)*sin((10*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 300 deg/ z axis
  dy=  -(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2)*cos((10*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 300 deg/ z axis
  dz = -(Length2)*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.)) - ShiftToWorldEdge;
  
  // Rotation:
  thx = 90.000000;        phx = 0.000000-(10*360./NSide2+ZRotAngleOL);
  thy = 90.+ InclAng2;    phy = 90.000000-(10*360./NSide2+ZRotAngleOL);
  thz = InclAng2;         phz = 90.000000-(10*360./NSide2+ZRotAngleOL);
  TGeoRotation *pMatrix55 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix54 = new TGeoCombiTrans("", dx,dy,dz,pMatrix55);
  
  dx=  -(( (Length2+ Frame_Length)/2)*sin(InclAng2*PI/180.)+ Rmin2+ (Thickness2*2+Frame_Depth))*sin((10*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 300 deg/ z axis
  dy=  -(( (Length2+ Frame_Length)/2)*sin(InclAng2*PI/180.)+ Rmin2+ (Thickness2*2+Frame_Depth))*cos((10*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 300 deg/ z axis
  //dz = -(Length2+Frame_Length)*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.));
  TGeoCombiTrans*
  pMatrix54b = new TGeoCombiTrans("", dx,dy,dz,pMatrix55);
  
  //########################
  
  // Combi transformation:
  //dx =  dx -(WidthHalf2/2)*(1/(cos(AngTrap2)*cos(AngTrap2)))*(cos(10*(360./NSide3)*PI/180.)+cos(11*(360./NSide3)*PI/180.));     // considering  real barycentre position;
  //dx =  dx -(WidthHalf2/2)*(cos(10*(360./NSide3)*PI/180.) + cos(11*(360./NSide3)*3.14159/180)); // considering intersection of 2 medianes
  //dy =  dy + (WidthHalf2/2)*(1/(cos(AngTrap2)*cos(AngTrap2)))*(sin(10*(360./NSide3)*PI/180.)+sin(11*(360./NSide3)*PI/180.));    // considering  real barycentre position;
  //dy =  dy +(WidthHalf2/2)*(sin(10*(360./NSide3)*PI/180.) + sin(11*(360./NSide3)*PI/180));      // considering  intersection of 2 medianes
  
  dx=  -(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2)*sin((11*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 330 deg/ z axis
  dy=  -(( (Length2)/2)*sin(InclAng2*PI/180.)+ Rmin2)*cos((11*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 330 deg/ z axis
  dz = -(Length2)*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.)) - ShiftToWorldEdge;
  
  // Rotation:
  thx = 90.000000;        phx = 0.000000-(11*360./NSide2+ZRotAngleOL);
  thy = 90.+ InclAng2;    phy = 90.000000-(11*360./NSide2+ZRotAngleOL);
  thz = InclAng2;         phz = 90.000000-(11*360./NSide2+ZRotAngleOL);
  TGeoRotation *pMatrix57 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  TGeoCombiTrans*
  pMatrix56 = new TGeoCombiTrans("", dx,dy,dz,pMatrix57);
  
  dx=  -(( (Length2+ Frame_Length)/2)*sin(InclAng2*PI/180.)+ Rmin2+ (Thickness2*2+Frame_Depth))*sin((11*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 330 deg/ z axis
  dy=  -(( (Length2+ Frame_Length)/2)*sin(InclAng2*PI/180.)+ Rmin2+ (Thickness2*2+Frame_Depth))*cos((11*(360/NSide2)+ZRotAngleOL)*PI/180.);  // rotation by 330 deg/ z axis
  //dz = -(Length2+Frame_Length)*cos(InclAng2*PI/180.)/2 + (Rmin2/tan(AngRangeMin2*PI/180.));
  
  TGeoCombiTrans*
  pMatrix56b = new TGeoCombiTrans("", dx,dy,dz,pMatrix57);
  
  //########################
  
  // Combi transformation:
  dx = 0.000000;
  dy = 13.2800;
  dz = 18.60000;
  // Rotation:
  thx = 180.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 0.000000;
  thz = 90.000000;    phz = 90.000000;
  //   TGeoRotation *pMatrix59 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  //   TGeoCombiTrans*
  //     pMatrix58 = new TGeoCombiTrans("", dx,dy,dz,pMatrix59);
  
  // Combi transformation:
  dx = 9.39000;
  dy = 9.39000;
  dz = 18.60000;
  // Rotation:
  thx = 0.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 315.000000;
  thz = 90.000000;    phz = 45.000000;
  //   TGeoRotation *pMatrix61 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  //   TGeoCombiTrans*
  //     pMatrix60 = new TGeoCombiTrans("", dx,dy,dz,pMatrix61);
  
  // Combi transformation:
  dx = 13.28000;
  dy = 0.00000;
  dz = 18.60000;
  // Rotation:
  thx = 180.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = 90.000000;    phz = 0.000000;
  //   TGeoRotation *pMatrix63 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  //   TGeoCombiTrans*
  //     pMatrix62 = new TGeoCombiTrans("", dx,dy,dz,pMatrix63);
  
  // Combi transformation:
  dx = 9.39000;
  dy = -9.39000;
  dz = 18.60000;
  // Rotation:
  thx = 0.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 45.000000;
  thz = 90.000000;    phz = 315.000000;
  //   TGeoRotation *pMatrix65 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  //   TGeoCombiTrans*
  //     pMatrix64 = new TGeoCombiTrans("", dx,dy,dz,pMatrix65);
  
  // End caps
  
  // Combi transformation:
  dx = 0.000000;
  dy = -7.150000;
  dz = 35.000000;
  // Rotation:
  thx = 90.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = 0.000000;    phz = 0.000000;
  //   TGeoRotation *pMatrix99 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  //   TGeoCombiTrans*
  //   pMatrix98 = new TGeoCombiTrans("", dx,dy,dz,pMatrix99);
  
  // Combi transformation:
  dx = 0.000000;
  dy = 7.150000;
  dz = 35.000000;
  // Rotation:
  thx = 90.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = 0.000000;    phz = 0.000000;
  //   TGeoRotation *pMatrix101 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  //   TGeoCombiTrans*
  //   pMatrix100 = new TGeoCombiTrans("", dx,dy,dz,pMatrix101);
  
  // Combi transformation:
  dx = 0.000000;
  dy = -8.350000;
  dz = 40.00000;
  // Rotation:
  thx = 90.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = 0.000000;    phz = 0.000000;
  //   TGeoRotation *pMatrix103 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  //   TGeoCombiTrans*
  //   pMatrix102 = new TGeoCombiTrans("", dx,dy,dz,pMatrix103);
  
  // Combi transformation:
  dx = 0.000000;
  dy = 8.350000;
  dz = 40.00000;
  // Rotation:
  thx = 90.000000;    phx = 0.000000;
  thy = 90.000000;    phy = 90.000000;
  thz = 0.000000;    phz = 0.000000;
  //   TGeoRotation *pMatrix105 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  //   TGeoCombiTrans*
  //     pMatrix104 = new TGeoCombiTrans("", dx,dy,dz,pMatrix105);
  
  
  // Chamber
	
	// Combi transformation:
	dx = 0.000000;
	dy = 0.000000;
	dz = -15.00000;
	// Rotation:
	thx = 90.000000;    phx = 0.000000;
	thy = 90.000000;    phy = 90.000000;
	thz = 0.000000;    phz = 0.000000;
  //	TGeoRotation *pMatrix171 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  //	TGeoCombiTrans*
  //	pMatrix170 = new TGeoCombiTrans("", dx,dy,dz,pMatrix171);
	
	// Combi transformation:
	dx = 0.000000;
	dy = 0.000000;
	dz = 20.000000;
	// Rotation:
	thx = 90.000000;    phx = 0.000000;
	thy = 90.000000;    phy = 90.000000;
	thz = 0.000000;    phz = 0.000000;
  //	TGeoRotation *pMatrix173 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  //	TGeoCombiTrans*
  //	pMatrix172 = new TGeoCombiTrans("", dx,dy,dz,pMatrix173);
	
  // Combi transformation:
	dx = 0.000000;
	dy = 0.000000;
	dz = -30.000000;
	// Rotation:
	thx = 90.000000;    phx = 0.000000;
	thy = 90.000000;    phy = 90.000000;
	thz = 0.000000;    phz = 0.000000;
  //	TGeoRotation *pMatrix175 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  //	TGeoCombiTrans*
  //	pMatrix174 = new TGeoCombiTrans("", dx,dy,dz,pMatrix175);
	
  //Gold Cone
	// Combi transformation:
	dx = 0.000000;
	dy = 0.000000;
	dz = 7.10000;
	// Rotation:
	thx = 90.000000;    phx = 0.000000;
	thy = 90.000000;    phy = 90.000000;
	thz = 0.000000;    phz = 0.000000;
  //	TGeoRotation *pMatrix177 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  //	TGeoCombiTrans*
  //	pMatrix176 = new TGeoCombiTrans("", dx,dy,dz,pMatrix177);
  
  // Beam pipe
  
  // Combi transformation:
  //dx = 0.000000;
  //dy = 0.000000;
  //dz = 15.000000;
  // Rotation:
  //thx = 90.000000;    phx = 0.000000;
  //thy = 90.000000;    phy = 90.000000;
  //thz = 0.000000;    phz = 0.000000;
  //TGeoRotation *pMatrix59 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  //TGeoCombiTrans*
  //  pMatrix58 = new TGeoCombiTrans("", dx,dy,dz,pMatrix59);
  
  
  // Mounting platform
  
  // Electronics modules
  
  // Target
  
  // Combi transformation:
  //dx = -6.240000;
  //dy = -3.900000;
  //dz = 0.000000;
  // Rotation:
  //thx = 90.000000;    phx = 0.000000;
  //thy = 90.000000;    phy = 90.000000;
  //thz = 0.000000;    phz = 0.000000;
  //TGeoRotation *pMatrix35 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
  //TGeoCombiTrans*
  //pMatrix34 = new TGeoCombiTrans("", dx,dy,dz,pMatrix35);
  
  
  
  
  TGeoRotation *rotg = new TGeoRotation();
  rotg->RotateX(0.);
  rotg->RotateY(0.);
  rotg->RotateZ(0.);
  dx=tx=0.0;
  dy=ty=0.0;
  dz=tz=0.0;
  
  TGeoCombiTrans *t1 = new TGeoCombiTrans(tx,ty,tz,rotg);
  
  
  dx=tx=0;
  //dy=ty=2*(Frame_Depth+Thickness1)/2 - Thickness1;
  // Since Frame_Depth and Thickness1 are half thicknesses
  dy=ty=Frame_Depth;
  // Since Frame_Depth and Thickness1 are half thicknesses
  dz=tz=0;
  
  //   TGeoCombiTrans *t2 = new TGeoCombiTrans(tx,ty,tz,rotg);
  
  dx=tx=0;
  dy=ty=-(Frame_Depth+Thickness1) + 0.00625;
  dz=tz= Length1/2 -9.9869;
  
  //   TGeoCombiTrans *t3 = new TGeoCombiTrans(tx,ty,tz,rotg);
  
  
  dx=tx=0;
  dy=ty=-(Thickness1*2);
  // Since Thickness1 is half of the detector Thickness
  dy=
  dz=tz= Length1/2 -9.9869-1;
  
  //   TGeoCombiTrans *t4 = new TGeoCombiTrans(tx,ty,tz,rotg);
  
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
   
  // STaRTracker
  
  // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
  // Shape: SiVacuumSphereWorld type: TGeoSphere
  // Si Shape & volume: TraBox type: TGeoBBox
  //dx = 15.00000;
  //dy = 2.000000;
  //dz = 0.005000;
  // Volume: STaRTraLog FOR INNER LAYER
  //TGeoVolume *STaRTraLog1 = gGeoManager->MakeBox("STaRTraLog1",pSiMed,dx,dy,dz);
  
  TGeoVolume *STaRTraLog1 = gGeoManager->MakeTrd1("STaRTraLog1",pSiMed,WidthMax1/2.,WidthMin1/2.,Thickness1,Length1/2.);
  
  Double_t Empty_widthMax, Empty_widthMin, Empty_Depth, Empty_Length;
  
  Double_t alpha1=2.5; //alpha1= is an arbitrary fraction of the total length of the empty trapeze whih is totalLength=(Length1-2*Frame_Length)
  Double_t alpha2= (Length1-2*Frame_Length)*alpha1/((Length1-2*Frame_Length)+0.15*alpha1); //   0.15cm is the width of the transversal support in the middle of the frame
  Double_t alpha3= 20.;//alpha3= is an arbitrary fraction of the total length of the empty trapeze whih is totalLength=(Length1-2*Frame_Length)
  Double_t alpha4= (alpha2*alpha3)/(alpha2+alpha3);
  Double_t alpha5= alpha4/(alpha4-1);
  
  TGeoTranslation *t_CBFrame_0 = new TGeoTranslation("T0_in",0.,0.,0.);
  t_CBFrame_0->RegisterYourself();
  TGeoTranslation *t_EmptyFrame_1 = new TGeoTranslation("T1_in",0.,0., -(Length1+Frame_Length)/2 + Frame_Length + ((Length1-2*Frame_Length)/alpha1)/2);
  t_EmptyFrame_1->RegisterYourself();
  TGeoTranslation *t_EmptyFrame_2 = new TGeoTranslation("T2_in",0.,0., -(Length1+Frame_Length)/2 + Frame_Length + ((Length1-2*Frame_Length)/alpha1) + 0.15 + ((Length1-2*Frame_Length)/alpha3)/2.);
  t_EmptyFrame_2->RegisterYourself();
  TGeoTranslation *t_EmptyFrame_3 = new TGeoTranslation("T3_in",0.,0., -(Length1+Frame_Length)/2 + Frame_Length + ((Length1-2*Frame_Length)/alpha1) + 0.15 + ((Length1-2*Frame_Length)/alpha3) + 0.15 +((Length1-2*Frame_Length)/alpha5)/2.);
  t_EmptyFrame_3->RegisterYourself();
  
  
  TGeoTrd1 *CB1_Trd1 = new TGeoTrd1("A", (WidthMax1+Frame_Width)/2., (WidthMin1+Frame_Width)/2.,Frame_Depth,(Length1+Frame_Length)/2.); // exteranl CBframe
  
  Empty_widthMax= (WidthMax1-Frame_Width)/2.;
  Empty_widthMin= ((WidthMax1-Frame_Width) -(WidthMax1-WidthMin1)/alpha1 )/2.;
  Empty_Depth= (Frame_Depth+Frame_Depth/10.);
  Empty_Length= ((Length1-2*Frame_Length)/alpha1)/2.;
  TGeoTrd1 *Empty1_Trd1 =new TGeoTrd1("B", Empty_widthMax, Empty_widthMin, Empty_Depth, Empty_Length);
  
  Empty_widthMax= ( (WidthMax1-Frame_Width)-(WidthMax1-WidthMin1)/alpha2 )/2.;
  Empty_widthMin= ( Empty_widthMax*2 - (WidthMax1-WidthMin1)/alpha3 )/2.;
  //Empty_widthMin= ( Empty_widthMax*2 - (Empty_widthMax*2/alpha3))/2.;
  Empty_Length= ((Length1-2*Frame_Length)/alpha3)/2.;
  TGeoTrd1 *Empty2_Trd1 =new TGeoTrd1("C", Empty_widthMax ,Empty_widthMin , Empty_Depth ,  Empty_Length);
  
  
  Empty_widthMax= ( (WidthMax1-Frame_Width)-(WidthMax1-WidthMin1)/alpha4 )/2.;
  Empty_widthMin= (WidthMin1-Frame_Length)/2;
  Empty_Length= ((Length1-2*Frame_Length) - (Length1-2*Frame_Length) /alpha4)/2.;
  TGeoTrd1 *Empty3_Trd1 =new TGeoTrd1("D", Empty_widthMax ,Empty_widthMin , Empty_Depth , Empty_Length);
  
  TGeoCompositeShape *CBFrame;
  CBFrame= new TGeoCompositeShape("CBFrame", "((A:T0_in-B:T1_in)-C:T2_in)-D:T3_in");
  
  TGeoVolume *STaRTraCBFrameLog1 = new TGeoVolume("CBFRAME",CBFrame, pCarbonFibreMedium);
  
  gGeoManager->AddVolume(STaRTraCBFrameLog1);
  
  //TGeoVolume *STaRTraCBFrameLog1 = gGeoManager->MakeCBFrame("STaRTraCBFrameLog1",pCarbonFibreMedium);
  
  /*
   TGeoVolume *STaRTraVideLog1 = gGeoManager->MakeTrd1("STaRTraVideLog1",pVacuumMedium,WidthMax1/2.,WidthMin1/2.,Frame_Depth,Length1/2.);
   TGeoVolume *STaRTraCBFrameLog1 = gGeoManager->MakeTrd1("STaRTraCBFrameLog1",pCarbonFibreMedium,(WidthMax1+Frame_Width)/2.,(WidthMin1+Frame_Width)/2.,Frame_Depth,(Length1+Frame_Length)/2.);
   TGeoVolume *CBFrameSupportBDLog1 = gGeoManager->MakeTrd1("CBFrameSupportBDLog1",pCarbonFibreMedium, 4.90/2, 4.88/2, 0.05/2, 0.15/2);
   TGeoVolume *CBFrameXSupportBDLog1 = gGeoManager->MakeTrd1("CBFrameXSupportBDLog1",pCarbonFibreMedium, 5.15/2, 5.13/2,  0.2/2, 0.1/2);
   // extra support
   
   STaRTraCBFrameLog1->SetLineColor(kCyan);
   CBFrameSupportBDLog1->SetLineColor(kCyan);
   CBFrameXSupportBDLog1->SetLineColor(kCyan);
   
   STaRTraVideLog1->AddNodeOverlap(CBFrameXSupportBDLog1,1,t4);
   STaRTraVideLog1->AddNodeOverlap(CBFrameSupportBDLog1,1,t3);
   //STaRTraVideLog1->AddNodeOverlap(STaRTraLog1,1,t2);
   STaRTraCBFrameLog1->AddNode(STaRTraVideLog1,1,t1);
   */
  
  dy=ty=-(Frame_Depth+Thickness2) + 0.00625;
  dz=tz= Length2/2 -12.898471;
  //     TGeoCombiTrans *t5 = new TGeoCombiTrans(tx,ty,tz,rotg);
  
  dy=ty=-(Thickness2*2);
  // Since Thickness1 is half of the detector Thickness
  dz=tz= Length2/2 -12.898471 -1;
  //     TGeoCombiTrans *t6 = new TGeoCombiTrans(tx,ty,tz,rotg);
  
  dz=tz= Length3/2 - (12.898471+11.8071);
  //     TGeoCombiTrans *t7 = new TGeoCombiTrans(tx,ty,tz,rotg);
  
  dy=ty=-(Thickness3*2);
  // Since Thickness1 is half of the detector Thickness
  dz=tz= Length3/2 -(12.898471+11.8071)-1;
  //     TGeoCombiTrans *t8 = new TGeoCombiTrans(tx,ty,tz,rotg);
  
  
  
  
  // Si Shape & volume: TraBox type: TGeoBBox
  //dx = 15.00000;
  //dy = 5.50000;
  //dz = 0.015000;
  // Volume: STaRTraLog   FOR OUTER LAYER
  
  //TGeoVolume *STaRTraLog3 = gGeoManager->MakeBox("STaRTraLog3",pSiMed,dx,dy,dz);
  TGeoVolume *STaRTraLog3 = gGeoManager->MakeTrd1("STaRTraLog3",pSiMed,WidthMax3/2,WidthMin3/2,Thickness3,Length3/2);
  
  
  alpha1=5.; //alpha1= is an arbitrary fraction of the total length of the empty trapeze whih is totalLength=(Length1-2*Frame_Length)
  alpha2= (Length3-2*Frame_Length)*alpha1/((Length3-2*Frame_Length)+0.15*alpha1); //   0.15cm is the width of the transversal support in the middle of the frame
  alpha3= 20.;//alpha3= is an arbitrary fraction of the total length of the empty trapeze whih is totalLength=(Length1-2*Frame_Length)
  alpha4= 1/((alpha2+alpha3)/(alpha2*alpha3) +0.15/(Length3-2*Frame_Length));
  alpha5= 3.;
  Double_t alpha6= 1/( (alpha4+alpha5)/(alpha4*alpha5) +0.15/(Length3-2*Frame_Length));
  Double_t alpha7= alpha3;
  Double_t alpha8= (alpha6*alpha7)/(alpha6+alpha7);
  Double_t alpha9= alpha8/(alpha8-1);
  
  
  TGeoTranslation *t_CBFrame_out0 = new TGeoTranslation("T0_out",0.,0.,0.);
  t_CBFrame_out0->RegisterYourself();
  TGeoTranslation *t_EmptyFrame_out1 = new TGeoTranslation("T1_out",0.,0., -(Length3+Frame_Length)/2 + Frame_Length + ((Length3-2*Frame_Length)/alpha1)/2);
  t_EmptyFrame_out1->RegisterYourself();
  TGeoTranslation *t_EmptyFrame_out2 = new TGeoTranslation("T2_out",0.,0., -(Length3+Frame_Length)/2 + Frame_Length + ((Length3-2*Frame_Length)/alpha1) + 0.15 + ((Length3-2*Frame_Length)/alpha3)/2.);
  t_EmptyFrame_out2->RegisterYourself();
  TGeoTranslation *t_EmptyFrame_out3 = new TGeoTranslation("T3_out",0.,0., -(Length3+Frame_Length)/2 + Frame_Length + ((Length3-2*Frame_Length)/alpha1) + 0.15 + ((Length3-2*Frame_Length)/alpha3) + 0.15 +((Length3-2*Frame_Length)/alpha5)/2.);
  t_EmptyFrame_out3->RegisterYourself();
  TGeoTranslation *t_EmptyFrame_out4 = new TGeoTranslation("T4_out",0.,0., -(Length3+Frame_Length)/2 + Frame_Length + ((Length3-2*Frame_Length)/alpha1) + 0.15 + ((Length3-2*Frame_Length)/alpha3) + 0.15 +((Length3-2*Frame_Length)/alpha5) + 0.15 +((Length3-2*Frame_Length)/alpha7)/2) ;
  t_EmptyFrame_out4->RegisterYourself();
  TGeoTranslation *t_EmptyFrame_out5 = new TGeoTranslation("T5_out",0.,0., -(Length3+Frame_Length)/2 + Frame_Length + ((Length3-2*Frame_Length)/alpha1) + 0.15 + ((Length3-2*Frame_Length)/alpha3) + 0.15 +((Length3-2*Frame_Length)/alpha5) + 0.15 +((Length3-2*Frame_Length)/alpha7) +0.15 + ((Length3-2*Frame_Length)/alpha9)/2);
  t_EmptyFrame_out5->RegisterYourself();
  
  
  TGeoTrd1 *CB1out_Trd1 = new TGeoTrd1("Aout", (WidthMax3+Frame_Width)/2., (WidthMin3+Frame_Width)/2.,Frame_Depth,(Length3+Frame_Length)/2.); // exteranl CBframe
  
  Empty_widthMax= (WidthMax3-Frame_Width)/2.;
  Empty_widthMin= ((WidthMax3-Frame_Width) -(WidthMax3-WidthMin3)/alpha1 )/2.;
  Empty_Depth= (Frame_Depth+Frame_Depth/10.);
  Empty_Length= ((Length3-2*Frame_Length)/alpha1)/2.;
  TGeoTrd1 *Empty1out_Trd1 =new TGeoTrd1("Bout", Empty_widthMax, Empty_widthMin, Empty_Depth, Empty_Length);
  
  Empty_widthMax= ( (WidthMax3-Frame_Width)-(WidthMax3-WidthMin3)/alpha2 )/2.;
  Empty_widthMin= ( Empty_widthMax*2 - (WidthMax3-WidthMin3)/alpha3)/2.;
  Empty_Length= ((Length3-2*Frame_Length)/alpha3)/2.;
  TGeoTrd1 *Empty2out_Trd1 =new TGeoTrd1("Cout", Empty_widthMax ,Empty_widthMin , Empty_Depth ,  Empty_Length);
  
  
  Empty_widthMax= ( (WidthMax3-Frame_Width)-(WidthMax3-WidthMin3)/alpha4 )/2.;
  Empty_widthMin=  ( Empty_widthMax*2 - (WidthMax3-WidthMin3)/alpha5)/2.;
  Empty_Length= ((Length3-2*Frame_Length)/alpha5)/2.;
  TGeoTrd1 *Empty3out_Trd1 =new TGeoTrd1("Dout", Empty_widthMax ,Empty_widthMin , Empty_Depth , Empty_Length);
  
  Empty_widthMax= ( (WidthMax3-Frame_Width)-(WidthMax3-WidthMin3)/alpha6 )/2.;
  Empty_widthMin=  ( Empty_widthMax*2 - (WidthMax3-WidthMin3)/alpha7)/2.;
  Empty_Length= ((Length3-2*Frame_Length)/alpha7)/2.;
  TGeoTrd1 *Empty4out_Trd1 =new TGeoTrd1("Eout", Empty_widthMax ,Empty_widthMin , Empty_Depth , Empty_Length);
  
  Empty_widthMax= ( (WidthMax3-Frame_Width)-(WidthMax3-WidthMin3)/alpha8 )/2.;
  Empty_widthMin= (WidthMin3-Frame_Length)/2;
  Empty_Length= ((Length3-2*Frame_Length) - (Length3-2*Frame_Length) /alpha8)/2.;
  TGeoTrd1 *Empty5out_Trd1 =new TGeoTrd1("Fout", Empty_widthMax ,Empty_widthMin , Empty_Depth , Empty_Length);
  
  
  TGeoCompositeShape *CBFrame_out;
  CBFrame_out= new TGeoCompositeShape("CBFrame_out", "((((Aout:T0_out-Bout:T1_out)-Cout:T2_out)-Dout:T3_out)-Eout:T4_out)-Fout:T5_out");
  
  TGeoVolume *STaRTraCBFrameLog3 = new TGeoVolume("CBFRAME_OUT",CBFrame_out, pCarbonFibreMedium);
  
  gGeoManager->AddVolume(STaRTraCBFrameLog3);
  
  
  /*
   
   TGeoVolume *STaRTraVideLog3 = gGeoManager->MakeTrd1("STaRTraVideLog3",pVacuumMedium,WidthMax3/2.,WidthMin3/2.,Frame_Depth,Length3/2.);
   TGeoVolume *STaRTraCBFrameLog3 = gGeoManager->MakeTrd1("STaRTraCBFrameLog3",pCarbonFibreMedium,(WidthMax3+Frame_Width)/2.,(WidthMin3+Frame_Width)/2.,Frame_Depth,(Length3+Frame_Length)/2.);
	 TGeoVolume *CBFrameSupportBDLog3 = gGeoManager->MakeTrd1("CBFrameSupportBDLog3",pCarbonFibreMedium, 4.90/2, 4.88/2 ,0.05/2, 0.15/2);
	 TGeoVolume *CBFrameSupport2BDLog3 = gGeoManager->MakeTrd1("CBFrameSupport2BDLog3",pCarbonFibreMedium, 8.28/2, 8.26/2 ,0.05/2, 0.15/2);
	 TGeoVolume *CBFrameXSupportBDLog3 = gGeoManager->MakeTrd1("CBFrameXSupportBDLog3",pCarbonFibreMedium, 5.15/2, 5.13/2, 0.2/2, 0.1/2);
	 TGeoVolume *CBFrameXSupport2BDLog3 = gGeoManager->MakeTrd1("CBFrameXSupport2BDLog3",pCarbonFibreMedium, 8.52/2, 8.50/2, 0.2/2, 0.1/2);
   
   
   STaRTraCBFrameLog3->SetLineColor(kCyan);
   CBFrameSupportBDLog3->SetLineColor(kCyan);
   CBFrameSupport2BDLog3->SetLineColor(kCyan);
   CBFrameXSupportBDLog3->SetLineColor(kCyan);
   CBFrameXSupport2BDLog3->SetLineColor(kCyan);
   
   
   STaRTraVideLog3->AddNodeOverlap(CBFrameXSupportBDLog3,1,t6);
   STaRTraVideLog3->AddNodeOverlap(CBFrameXSupport2BDLog3,1,t8);
   STaRTraVideLog3->AddNodeOverlap(CBFrameSupportBDLog3,1,t5);
   STaRTraVideLog3->AddNodeOverlap(CBFrameSupport2BDLog3,1,t7);
   //STaRTraVideLog3->AddNodeOverlap(STaRTraLog3,1,t2);
   STaRTraCBFrameLog3->AddNode(STaRTraVideLog3,1,t1);
   */
  
  
  // Si Shape & volume: TraBox type: TGeoBBox
  //dx = 15.00000;
  //dy = 4.500000;
  //dz = 0.015000;
  // Volume: STaRTraLog FOR Intermediate LAYER
  
  //TGeoVolume *STaRTraLog2 = gGeoManager->MakeBox("STaRTraLog2",pSiMed,dx,dy,dz);
  TGeoVolume *STaRTraLog2 = gGeoManager->MakeTrd1("STaRTraLog2",pSiMed,WidthMax2/2,WidthMin2/2,Thickness2,Length2/2);
  
  alpha1=5.; //alpha1= is an arbitrary fraction of the total length of the empty trapeze whih is totalLength=(Length1-2*Frame_Length)
  alpha2= (Length2-2*Frame_Length)*alpha1/((Length2-2*Frame_Length)+0.15*alpha1); //   0.15cm is the width of the transversal support in the middle of the frame
  alpha3= 20.;//alpha3= is an arbitrary fraction of the total length of the empty trapeze whih is totalLength=(Length1-2*Frame_Length)
  alpha4= 1/((alpha2+alpha3)/(alpha2*alpha3) +0.15/(Length2-2*Frame_Length));
  alpha5= 3.;
  alpha6= 1/( (alpha4+alpha5)/(alpha4*alpha5) +0.15/(Length2-2*Frame_Length));
  alpha7= alpha3;
  alpha8= (alpha6*alpha7)/(alpha6+alpha7);
  alpha9= alpha8/(alpha8-1);
  
  
  TGeoTranslation *t_CBFrame_mid0 = new TGeoTranslation("T0_mid",0.,0.,0.);
  t_CBFrame_mid0->RegisterYourself();
  TGeoTranslation *t_EmptyFrame_mid1 = new TGeoTranslation("T1_mid",0.,0., -(Length2+Frame_Length)/2 + Frame_Length + ((Length2-2*Frame_Length)/alpha1)/2);
  t_EmptyFrame_mid1->RegisterYourself();
  TGeoTranslation *t_EmptyFrame_mid2 = new TGeoTranslation("T2_mid",0.,0., -(Length2+Frame_Length)/2 + Frame_Length + ((Length2-2*Frame_Length)/alpha1) + 0.15 + ((Length2-2*Frame_Length)/alpha3)/2.);
  t_EmptyFrame_mid2->RegisterYourself();
  TGeoTranslation *t_EmptyFrame_mid3 = new TGeoTranslation("T3_mid",0.,0., -(Length2+Frame_Length)/2 + Frame_Length + ((Length2-2*Frame_Length)/alpha1) + 0.15 + ((Length2-2*Frame_Length)/alpha3) + 0.15 +((Length2-2*Frame_Length)/alpha5)/2.);
  t_EmptyFrame_mid3->RegisterYourself();
  TGeoTranslation *t_EmptyFrame_mid4 = new TGeoTranslation("T4_mid",0.,0., -(Length2+Frame_Length)/2 + Frame_Length + ((Length2-2*Frame_Length)/alpha1) + 0.15 + ((Length2-2*Frame_Length)/alpha3) + 0.15 +((Length2-2*Frame_Length)/alpha5) + 0.15 +((Length2-2*Frame_Length)/alpha7)/2) ;
  t_EmptyFrame_mid4->RegisterYourself();
  TGeoTranslation *t_EmptyFrame_mid5 = new TGeoTranslation("T5_mid",0.,0., -(Length2+Frame_Length)/2 + Frame_Length + ((Length2-2*Frame_Length)/alpha1) + 0.15 + ((Length2-2*Frame_Length)/alpha3) + 0.15 +((Length2-2*Frame_Length)/alpha5) + 0.15 +((Length2-2*Frame_Length)/alpha7) +0.15 + ((Length2-2*Frame_Length)/alpha9)/2);
  t_EmptyFrame_mid5->RegisterYourself();
  
  
  TGeoTrd1 *CB1mid_Trd1 = new TGeoTrd1("Amid", (WidthMax2+Frame_Width)/2., (WidthMin2+Frame_Width)/2.,Frame_Depth,(Length2+Frame_Length)/2.); // exteranl CBframe
  
  Empty_widthMax= (WidthMax2-Frame_Width)/2.;
  Empty_widthMin= ((WidthMax2-Frame_Width) -(WidthMax2-WidthMin2)/alpha1 )/2.;
  Empty_Depth= (Frame_Depth+Frame_Depth/10.);
  Empty_Length= ((Length2-2*Frame_Length)/alpha1)/2.;
  TGeoTrd1 *Empty1mid_Trd1 =new TGeoTrd1("Bmid", Empty_widthMax, Empty_widthMin, Empty_Depth, Empty_Length);
  
  Empty_widthMax= ( (WidthMax3-Frame_Width)-(WidthMax2-WidthMin2)/alpha2 )/2.;
  Empty_widthMin= ( Empty_widthMax*2 - (WidthMax2-WidthMin2)/alpha3)/2.;
  Empty_Length= ((Length2-2*Frame_Length)/alpha3)/2.;
  TGeoTrd1 *Empty2mid_Trd1 =new TGeoTrd1("Cmid", Empty_widthMax ,Empty_widthMin , Empty_Depth ,  Empty_Length);
  
  
  Empty_widthMax= ( (WidthMax2-Frame_Width)-(WidthMax2-WidthMin2)/alpha4 )/2.;
  Empty_widthMin=  ( Empty_widthMax*2 - (WidthMax2-WidthMin2)/alpha5)/2.;
  Empty_Length= ((Length2-2*Frame_Length)/alpha5)/2.;
  TGeoTrd1 *Empty3mid_Trd1 =new TGeoTrd1("Dmid", Empty_widthMax ,Empty_widthMin , Empty_Depth , Empty_Length);
  
  Empty_widthMax= ( (WidthMax2-Frame_Width)-(WidthMax2-WidthMin2)/alpha6 )/2.;
  Empty_widthMin=  ( Empty_widthMax*2 - (WidthMax2-WidthMin2)/alpha7)/2.;
  Empty_Length= ((Length2-2*Frame_Length)/alpha7)/2.;
  TGeoTrd1 *Empty4mid_Trd1 =new TGeoTrd1("Emid", Empty_widthMax ,Empty_widthMin , Empty_Depth , Empty_Length);
  
  Empty_widthMax= ( (WidthMax2-Frame_Width)-(WidthMax2-WidthMin2)/alpha8 )/2.;
  Empty_widthMin= (WidthMin2-Frame_Length)/2;
  Empty_Length= ((Length2-2*Frame_Length) - (Length2-2*Frame_Length) /alpha8)/2.;
  TGeoTrd1 *Empty5mid_Trd1 =new TGeoTrd1("Fmid", Empty_widthMax ,Empty_widthMin , Empty_Depth , Empty_Length);
  
  
  TGeoCompositeShape *CBFrame_mid;
  CBFrame_mid= new TGeoCompositeShape("CBFrame_mid", "((((Amid:T0_mid-Bmid:T1_mid)-Cmid:T2_mid)-Dmid:T3_mid)-Emid:T4_mid)-Fmid:T5_mid");
  
  TGeoVolume *STaRTraCBFrameLog2 = new TGeoVolume("CBFRAME_MID",CBFrame_mid, pCarbonFibreMedium);
  
  gGeoManager->AddVolume(STaRTraCBFrameLog2);
  
  
  /*
   
   TGeoVolume *STaRTraVideLog2 = gGeoManager->MakeTrd1("STaRTraVideLog2",pVacuumMedium,WidthMax2/2.,WidthMin2/2.,Frame_Depth,Length2/2.);
   TGeoVolume *STaRTraCBFrameLog2 = gGeoManager->MakeTrd1("STaRTraCBFrameLog2",pCarbonFibreMedium,(WidthMax2+Frame_Width)/2.,(WidthMin2+Frame_Width)/2.,Frame_Depth,(Length2+Frame_Length)/2.);
	 TGeoVolume *CBFrameSupportBDLog2 = gGeoManager->MakeTrd1("CBFrameSupportBDLog2",pCarbonFibreMedium, 4.90/2, 4.88/2, 0.05/2, 0.15/2);
	 TGeoVolume *CBFrameSupport2BDLog2 = gGeoManager->MakeTrd1("CBFrameSupport2BDLog2",pCarbonFibreMedium, 8.28/2, 8.26/2, 0.05/2, 0.15/2);
	 TGeoVolume *CBFrameXSupportBDLog2 = gGeoManager->MakeTrd1("CBFrameXSupportBDLog2",pCarbonFibreMedium, 5.15/2, 5.13/2, 0.2/2, 0.1/2);
	 TGeoVolume *CBFrameXSupport2BDLog2 = gGeoManager->MakeTrd1("CBFrameXSuppor2tBDLog2",pCarbonFibreMedium, 8.52/2, 8.50/2, 0.2/2, 0.1/2);
   
   STaRTraVideLog2->SetLineColor(0);
   STaRTraCBFrameLog2->SetLineColor(41);
   CBFrameSupportBDLog2->SetLineColor(kCyan);
   CBFrameSupport2BDLog2->SetLineColor(kCyan);
   CBFrameXSupportBDLog2->SetLineColor(kCyan);
   CBFrameXSupport2BDLog2->SetLineColor(kCyan);
   
   STaRTraVideLog2->AddNodeOverlap(CBFrameXSupportBDLog2,1,t6);
   STaRTraVideLog2->AddNodeOverlap(CBFrameXSupport2BDLog2,1,t8);
   STaRTraVideLog2->AddNodeOverlap(CBFrameSupportBDLog2,1,t5);
   STaRTraVideLog2->AddNodeOverlap(CBFrameSupport2BDLog2,1,t7);
   //STaRTraVideLog2->AddNodeOverlap(STaRTraLog2,1,t2);
   STaRTraCBFrameLog2->AddNode(STaRTraVideLog2,1,t1);
   */
  
  //
  // Make elementary assembly of the whole structure.
  //
  
  TGeoVolume *aTra = new TGeoVolumeAssembly("ATRA");
  TGeoVolume *aTraFrame = new TGeoVolumeAssembly("ATRAFRAME");
  
  /*
   aTra->AddNode(pChamberBarrelLog,1,pMatrix170);
   aTra->AddNode(pChamberEnd1Log,1,pMatrix172);
   aTra->AddNode(pChamberEnd2Log,1,pMatrix174);
   */
	//aTra->AddNode(pGoldConelog, 1, pMatrix176);
  
  
//  AddSensitiveVolume(STaRTraLog1);
//  AddSensitiveVolume(STaRTraLog2);
//  AddSensitiveVolume(STaRTraLog3);
  //   AddSensitiveVolume(STaRTraLog4);
  //   AddSensitiveVolume(STaRTraLog5);
//  fNbOfSensitiveVol+=1;
  
  
  // Inner layer
  
  
  //STaRTraLog1->SetLineColor(kRed);
  STaRTraLog1->SetLineColor(40);
  
  aTra->AddNode(STaRTraLog1,1, pMatrix2);
  aTra->AddNode(STaRTraLog1,2, pMatrix4);
  aTra->AddNode(STaRTraLog1,3, pMatrix6);
  aTra->AddNode(STaRTraLog1,4, pMatrix8);
  aTra->AddNode(STaRTraLog1,5, pMatrix10);
  aTra->AddNode(STaRTraLog1,6, pMatrix12);
  
  STaRTraCBFrameLog1->SetLineColor(41);
  
  aTraFrame->AddNode(STaRTraCBFrameLog1,1, pMatrix2b);
  aTraFrame->AddNode(STaRTraCBFrameLog1,2, pMatrix4b);
  aTraFrame->AddNode(STaRTraCBFrameLog1,3, pMatrix6b);
  aTraFrame->AddNode(STaRTraCBFrameLog1,4, pMatrix8b);
  aTraFrame->AddNode(STaRTraCBFrameLog1,5, pMatrix10b);
  aTraFrame->AddNode(STaRTraCBFrameLog1,6, pMatrix12b);
  
  
  // Middle layer:
  
  //STaRTraLog2->SetLineColor(kBlue);
  STaRTraLog2->SetLineColor(40);
  
  aTra->AddNode(STaRTraLog2,7, pMatrix34);
  aTra->AddNode(STaRTraLog2,8, pMatrix36);
  aTra->AddNode(STaRTraLog2,9, pMatrix38);
  aTra->AddNode(STaRTraLog2,10, pMatrix40);
  aTra->AddNode(STaRTraLog2,11, pMatrix42);
  aTra->AddNode(STaRTraLog2,12, pMatrix44);
  aTra->AddNode(STaRTraLog2,13, pMatrix46);
  aTra->AddNode(STaRTraLog2,14, pMatrix48);
  aTra->AddNode(STaRTraLog2,15, pMatrix50);
  aTra->AddNode(STaRTraLog2,16, pMatrix52);
  aTra->AddNode(STaRTraLog2,17, pMatrix54);
  aTra->AddNode(STaRTraLog2,18, pMatrix56);
  
  STaRTraCBFrameLog2->SetLineColor(41);

    
  aTraFrame->AddNode(STaRTraCBFrameLog2,7, pMatrix34b);
  aTraFrame->AddNode(STaRTraCBFrameLog2,8, pMatrix36b);
  aTraFrame->AddNode(STaRTraCBFrameLog2,9, pMatrix38b);
  aTraFrame->AddNode(STaRTraCBFrameLog2,10, pMatrix40b);
  aTraFrame->AddNode(STaRTraCBFrameLog2,11, pMatrix42b);
  aTraFrame->AddNode(STaRTraCBFrameLog2,12, pMatrix44b);
  aTraFrame->AddNode(STaRTraCBFrameLog2,13, pMatrix46b);
  aTraFrame->AddNode(STaRTraCBFrameLog2,14, pMatrix48b);
  aTraFrame->AddNode(STaRTraCBFrameLog2,15, pMatrix50b);
  aTraFrame->AddNode(STaRTraCBFrameLog2,16, pMatrix52b);
  aTraFrame->AddNode(STaRTraCBFrameLog2,17, pMatrix54b);
  aTraFrame->AddNode(STaRTraCBFrameLog2,18, pMatrix56b);
 
  
  
  // Outer layer:
  
  //STaRTraLog3->SetLineColor(kWhite);
  STaRTraLog3->SetLineColor(40);
  
  aTra->AddNode(STaRTraLog3,19, pMatrix66);
  aTra->AddNode(STaRTraLog3,20, pMatrix68);
  aTra->AddNode(STaRTraLog3,21, pMatrix70);
  aTra->AddNode(STaRTraLog3,22, pMatrix72);
  aTra->AddNode(STaRTraLog3,23, pMatrix74);
  aTra->AddNode(STaRTraLog3,24, pMatrix76);
  aTra->AddNode(STaRTraLog3,25, pMatrix78);
  aTra->AddNode(STaRTraLog3,26, pMatrix80);
  aTra->AddNode(STaRTraLog3,27, pMatrix82);
  aTra->AddNode(STaRTraLog3,28, pMatrix84);
  aTra->AddNode(STaRTraLog3,29, pMatrix86);
  aTra->AddNode(STaRTraLog3,30, pMatrix88);

  STaRTraCBFrameLog3->SetLineColor(41);
  
  aTraFrame->AddNode(STaRTraCBFrameLog3,19, pMatrix66b);
  aTraFrame->AddNode(STaRTraCBFrameLog3,20, pMatrix68b);
  aTraFrame->AddNode(STaRTraCBFrameLog3,21, pMatrix70b);
  aTraFrame->AddNode(STaRTraCBFrameLog3,22, pMatrix72b);
  aTraFrame->AddNode(STaRTraCBFrameLog3,23, pMatrix74b);
  aTraFrame->AddNode(STaRTraCBFrameLog3,24, pMatrix76b);
  aTraFrame->AddNode(STaRTraCBFrameLog3,25, pMatrix78b);
  aTraFrame->AddNode(STaRTraCBFrameLog3,26, pMatrix80b);
  aTraFrame->AddNode(STaRTraCBFrameLog3,27, pMatrix82b);
  aTraFrame->AddNode(STaRTraCBFrameLog3,28, pMatrix84b);
  aTraFrame->AddNode(STaRTraCBFrameLog3,29, pMatrix86b);
  aTraFrame->AddNode(STaRTraCBFrameLog3,30, pMatrix88b);
 
  
  pTraWorld->AddNode(aTra, 0, t1);  // when use a mother volume for Tracker
  pTraWorld->AddNode(aTraFrame, 0, t1);  // when use a mother volume for Tracker
  //pAWorld->AddNode(aTraFrame,0, t1);  // when use the world as mother volume of the tracker -> impli vacuum is made in all cave
  //pAWorld->AddNode(aTra,0, t1);  // when use the world as mother volume of the tracker -> impli vacuum is made in all cave
  
  
  
  // ---------------   Finish   -----------------------------------------------
  gGeoMan->CloseGeometry();
  gGeoMan->CheckOverlaps(0.01);
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
    //fRefRot = fRef;
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
      //c4.SetRotation(fRefRot->GetRotation());
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

