void N03Example() {
   gSystem->Load("libGeom");
   new TGeoManager("VGM Root geometry", "VGM Root geometry");

   Double_t dx,dy,dz;
   Double_t dx1, dx2, dy1, dy2;
   Double_t vert[20], par[20];
   Double_t theta, phi, h1, bl1, tl1, alpha1, h2, bl2, tl2, alpha2;
   Double_t twist;
   Double_t origin[3];
   Double_t rmin, rmax, rmin1, rmax1, rmin2, rmax2;
   Double_t r, rlo, rhi;
   Double_t phi1, phi2;
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

   // MATERIALS, MIXTURES AND TRACKING MEDIA
// Mixture: Air
   nel     = 2;
   density = 0.001290;
   pMat2 = new TGeoMixture("Air", nel,density);
      a = 14.006740;   z = 7.000000;   w = 0.700000;  // N
   pMat2->DefineElement(0,a,z,w);
      a = 15.999400;   z = 8.000000;   w = 0.300000;  // O
   pMat2->DefineElement(1,a,z,w);
   pMat2->SetIndex(1);
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
   pMed2 = new TGeoMedium("Air", numed,pMat2, par);
// Material: Vacuum
   a       = 1.010000;
   z       = 1.000000;
   density = 0.000000;
   radl    = 579553059467092293666734080.000000;
   absl    = 3511630811855104348084240384.000000;
   pMat1 = new TGeoMaterial("Vacuum", a,z,density,radl,absl);
   pMat1->SetIndex(0);
// Medium: Vacuum
   numed   = 0;  // medium number
   par[0]  = 0.000000; // isvol
   par[1]  = 0.000000; // ifield
   par[2]  = 0.000000; // fieldm
   par[3]  = 0.000000; // tmaxfd
   par[4]  = 0.000000; // stemax
   par[5]  = 0.000000; // deemax
   par[6]  = 0.000000; // epsil
   par[7]  = 0.000000; // stmin
   pMed1 = new TGeoMedium("Vacuum", numed,pMat1, par);
// Material: Silicon
   a       = 28.090000;
   z       = 14.000000;
   density = 2.330000;
   radl    = 9.351106;
   absl    = 456.628489;
   pMat22 = new TGeoMaterial("Silicon", a,z,density,radl,absl);
   pMat22->SetIndex(21);
// Medium: Silicon
   numed   = 21;  // medium number
   par[0]  = 0.000000; // isvol
   par[1]  = 0.000000; // ifield
   par[2]  = 0.000000; // fieldm
   par[3]  = 0.000000; // tmaxfd
   par[4]  = 0.000000; // stemax
   par[5]  = 0.000000; // deemax
   par[6]  = 0.000000; // epsil
   par[7]  = 0.000000; // stmin
   pMed22 = new TGeoMedium("Silicon", numed,pMat22, par);
// Material: Copper
   a       = 63.540000;
   z       = 29.000000;
   density = 8.960000;
   radl    = 1.435029;
   absl    = 155.874854;
   pMat25 = new TGeoMaterial("Copper", a,z,density,radl,absl);
   pMat25->SetIndex(24);
// Medium: Copper
   numed   = 24;  // medium number
   par[0]  = 0.000000; // isvol
   par[1]  = 0.000000; // ifield
   par[2]  = 0.000000; // fieldm
   par[3]  = 0.000000; // tmaxfd
   par[4]  = 0.000000; // stemax
   par[5]  = 0.000000; // deemax
   par[6]  = 0.000000; // epsil
   par[7]  = 0.000000; // stmin
   pMed25 = new TGeoMedium("Copper", numed,pMat25, par);
// Material: Aluminum
   a       = 26.980000;
   z       = 13.000000;
   density = 2.700000;
   radl    = 8.875105;
   absl    = 388.793113;
   pMat21 = new TGeoMaterial("Aluminum", a,z,density,radl,absl);
   pMat21->SetIndex(20);
// Medium: Aluminum
   numed   = 20;  // medium number
   par[0]  = 0.000000; // isvol
   par[1]  = 0.000000; // ifield
   par[2]  = 0.000000; // fieldm
   par[3]  = 0.000000; // tmaxfd
   par[4]  = 0.000000; // stemax
   par[5]  = 0.000000; // deemax
   par[6]  = 0.000000; // epsil
   par[7]  = 0.000000; // stmin
   pMed21 = new TGeoMedium("Aluminum", numed,pMat21, par);

   // TRANSFORMATION MATRICES
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix3 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix2 = new TGeoCombiTrans("", dx,dy,dz,pMatrix3);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 10.820000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix5 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix4 = new TGeoCombiTrans("", dx,dy,dz,pMatrix5);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix33 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix32 = new TGeoCombiTrans("", dx,dy,dz,pMatrix33);
   // Combi transformation: 
   dx = 0.000000;
   dy = -2.100000;
   dz = 4.470000;
   // Rotation: 
   thx = 0.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 90.000000;
   TGeoRotation *pMatrix7 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix6 = new TGeoCombiTrans("", dx,dy,dz,pMatrix7);
   // Combi transformation: 
   dx = 0.000000;
   dy = 2.100000;
   dz = 4.470000;
   // Rotation: 
   thx = 180.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 270.000000;
   TGeoRotation *pMatrix9 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix8 = new TGeoCombiTrans("", dx,dy,dz,pMatrix9);
   // Combi transformation: 
   dx = 2.100000;
   dy = 0.000000;
   dz = 4.470000;
   // Rotation: 
   thx = 180.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 90.000000;    phz = 0.000000;
   TGeoRotation *pMatrix11 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix10 = new TGeoCombiTrans("", dx,dy,dz,pMatrix11);
   // Combi transformation: 
   dx = -2.100000;
   dy = 0.000000;
   dz = 4.470000;
   // Rotation: 
   thx = 0.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 90.000000;    phz = 180.000000;
   TGeoRotation *pMatrix13 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix12 = new TGeoCombiTrans("", dx,dy,dz,pMatrix13);
   // Combi transformation: 
   dx = 0.000000;
   dy = -14.090000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 0.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 270.000000;
   TGeoRotation *pMatrix15 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix14 = new TGeoCombiTrans("", dx,dy,dz,pMatrix15);
   // Combi transformation: 
   dx = 0.000000;
   dy = -17.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 0.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 270.000000;
   TGeoRotation *pMatrix17 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix16 = new TGeoCombiTrans("", dx,dy,dz,pMatrix17);
   // Combi transformation: 
   dx = 0.000000;
   dy = -8.000000;
   dz = 10.820000;
   // Rotation: 
   thx = 90.000000;    phx = 270.000000;
   thy = 90.000000;    phy = 0.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix19 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix18 = new TGeoCombiTrans("", dx,dy,dz,pMatrix19);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix35 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix34 = new TGeoCombiTrans("", dx,dy,dz,pMatrix35);
   // Combi transformation: 
   dx = -2.100000;
   dy = -8.000000;
   dz = 4.470000;
   // Rotation: 
   thx = 90.000000;    phx = 90.000000;
   thy = 0.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 360.000000;
   TGeoRotation *pMatrix21 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix20 = new TGeoCombiTrans("", dx,dy,dz,pMatrix21);
   // Combi transformation: 
   dx = 2.100000;
   dy = 8.000000;
   dz = 4.470000;
   // Rotation: 
   thx = 90.000000;    phx = 90.000000;
   thy = 180.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 180.000000;
   TGeoRotation *pMatrix23 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix22 = new TGeoCombiTrans("", dx,dy,dz,pMatrix23);
   // Combi transformation: 
   dx = 8.000000;
   dy = -2.100000;
   dz = 4.470000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 180.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 90.000000;
   TGeoRotation *pMatrix25 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix24 = new TGeoCombiTrans("", dx,dy,dz,pMatrix25);
   // Combi transformation: 
   dx = -8.000000;
   dy = 2.100000;
   dz = 4.470000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 180.000000;    phy = 0.000000;
   thz = 90.000000;    phz = 90.000000;
   TGeoRotation *pMatrix27 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix26 = new TGeoCombiTrans("", dx,dy,dz,pMatrix27);
   // Combi transformation: 
   dx = -6.240000;
   dy = -3.900000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix29 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix28 = new TGeoCombiTrans("", dx,dy,dz,pMatrix29);
   // Combi transformation: 
   dx = -6.240000;
   dy = -3.900000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix31 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix30 = new TGeoCombiTrans("", dx,dy,dz,pMatrix31);
   // Shape: World type: TGeoBBox
   dx = 1500.000000;
   dy = 1500.000000;
   dz = 1500.000000;
   TGeoShape *pWorld_1 = new TGeoBBox("World", dx,dy,dz);
   // Volume: World
   pWorld_82ab3c0 = new TGeoVolume("World",pWorld_1, pMed2);
   pWorld_82ab3c0->SetVisLeaves(kTRUE);

   // SET TOP VOLUME OF GEOMETRY
   gGeoManager->SetTopVolume(pWorld_82ab3c0);

   // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
   // Shape: SiVacuumSphereWorld type: TGeoSphere
   rmin   = 0.000000;
   rmax   = 24.250000;
   theta1 = 0.000000;
   theta2 = 180.000000;
   phi1   = 0.000000;
   phi2   = 360.000000;
   TGeoShape *pSiVacuumSphereWorld_2 = new TGeoSphere("SiVacuumSphereWorld",rmin,rmax,theta1, theta2,phi1,phi2);
   // Volume: SiVacuumLogWorld
   pSiVacuumLogWorld_82abe78 = new TGeoVolume("SiVacuumLogWorld",pSiVacuumSphereWorld_2, pMed1);
   pSiVacuumLogWorld_82abe78->SetVisLeaves(kTRUE);
   pWorld_82ab3c0->AddNode(pSiVacuumLogWorld_82abe78, 0, pMatrix2);
   // Shape: TraBoxWorld type: TGeoBBox
   dx = 3.650000;
   dy = 2.120000;
   dz = 0.020000;
   TGeoShape *pTraBoxWorld_3 = new TGeoBBox("TraBoxWorld", dx,dy,dz);
   // Volume: TraLogWorld
   pTraLogWorld_82ac060 = new TGeoVolume("TraLogWorld",pTraBoxWorld_3, pMed1);
   pTraLogWorld_82ac060->SetVisLeaves(kTRUE);
   pSiVacuumLogWorld_82abe78->AddNode(pTraLogWorld_82ac060, 0, pMatrix4);
   pSiVacuumLogWorld_82abe78->AddNode(pTraLogWorld_82ac060, 1, pMatrix6);
   pSiVacuumLogWorld_82abe78->AddNode(pTraLogWorld_82ac060, 2, pMatrix8);
   pSiVacuumLogWorld_82abe78->AddNode(pTraLogWorld_82ac060, 3, pMatrix10);
   pSiVacuumLogWorld_82abe78->AddNode(pTraLogWorld_82ac060, 4, pMatrix12);
   // Shape: MontagePlatform type: TGeoTubeSeg
   rmin = 2.750000;
   rmax = 18.000000;
   dz   = 0.150000;
   phi1 = 0.000000;
   phi2 = 360.000000;
   TGeoShape *pMontagePlatform_5 = new TGeoTubeSeg("MontagePlatform",rmin,rmax,dz,phi1,phi2);
   // Volume: MontagePlatformLog
   pMontagePlatformLog_82ac3a0 = new TGeoVolume("MontagePlatformLog",pMontagePlatform_5, pMed25);
   pMontagePlatformLog_82ac3a0->SetVisLeaves(kTRUE);
   pSiVacuumLogWorld_82abe78->AddNode(pMontagePlatformLog_82ac3a0, 0, pMatrix14);
   // Shape: MontageRing type: TGeoTubeSeg
   rmin = 12.000000;
   rmax = 15.000000;
   dz   = 0.250000;
   phi1 = 0.000000;
   phi2 = 360.000000;
   TGeoShape *pMontageRing_6 = new TGeoTubeSeg("MontageRing",rmin,rmax,dz,phi1,phi2);
   // Volume: MontageRingLog
   pMontageRingLog_82ac548 = new TGeoVolume("MontageRingLog",pMontageRing_6, pMed25);
   pMontageRingLog_82ac548->SetVisLeaves(kTRUE);
   pSiVacuumLogWorld_82abe78->AddNode(pMontageRingLog_82ac548, 0, pMatrix16);
   // Shape: electronicBox type: TGeoBBox
   dx = 4.000000;
   dy = 3.500000;
   dz = 1.000000;
   TGeoShape *pelectronicBox_7 = new TGeoBBox("electronicBox", dx,dy,dz);
   // Volume: electronicsLog
   pelectronicsLog_82ac6f0 = new TGeoVolume("electronicsLog",pelectronicBox_7, pMed21);
   pelectronicsLog_82ac6f0->SetVisLeaves(kTRUE);
   pSiVacuumLogWorld_82abe78->AddNode(pelectronicsLog_82ac6f0, 0, pMatrix18);
   pSiVacuumLogWorld_82abe78->AddNode(pelectronicsLog_82ac6f0, 1, pMatrix20);
   pSiVacuumLogWorld_82abe78->AddNode(pelectronicsLog_82ac6f0, 2, pMatrix22);
   pSiVacuumLogWorld_82abe78->AddNode(pelectronicsLog_82ac6f0, 3, pMatrix24);
   pSiVacuumLogWorld_82abe78->AddNode(pelectronicsLog_82ac6f0, 4, pMatrix26);
   // Shape: targetWheel type: TGeoTubeSeg
   rmin = 0.410000;
   rmax = 5.300000;
   dz   = 0.100000;
   phi1 = 0.000000;
   phi2 = 360.000000;
   TGeoShape *ptargetWheel_9 = new TGeoTubeSeg("targetWheel",rmin,rmax,dz,phi1,phi2);
   // Volume: targetWheelLog
   ptargetWheelLog_82aca70 = new TGeoVolume("targetWheelLog",ptargetWheel_9, pMed21);
   ptargetWheelLog_82aca70->SetVisLeaves(kTRUE);
   pSiVacuumLogWorld_82abe78->AddNode(ptargetWheelLog_82aca70, 0, pMatrix28);
   // Shape: targetWheel2 type: TGeoTubeSeg
   rmin = 9.500000;
   rmax = 10.000000;
   dz   = 0.100000;
   phi1 = 0.000000;
   phi2 = 360.000000;
   TGeoShape *ptargetWheel2_10 = new TGeoTubeSeg("targetWheel2",rmin,rmax,dz,phi1,phi2);
   // Volume: targetWheel2Log
   ptargetWheel2Log_82acc40 = new TGeoVolume("targetWheel2Log",ptargetWheel2_10, pMed21);
   ptargetWheel2Log_82acc40->SetVisLeaves(kTRUE);
   pSiVacuumLogWorld_82abe78->AddNode(ptargetWheel2Log_82acc40, 0, pMatrix30);
   // Shape: TraBox type: TGeoBBox
   dx = 3.600000;
   dy = 2.070000;
   dz = 0.015000;
   TGeoShape *pTraBox_4 = new TGeoBBox("TraBox", dx,dy,dz);
   // Volume: TraLog
   pTraLog_82ac1d0 = new TGeoVolume("TraLog",pTraBox_4, pMed22);
   pTraLog_82ac1d0->SetVisLeaves(kTRUE);
   pTraLogWorld_82ac060->AddNode(pTraLog_82ac1d0, 0, pMatrix32);
   // Shape: innerElectronicBox type: TGeoBBox
   dx = 3.800000;
   dy = 3.300000;
   dz = 0.800000;
   TGeoShape *pinnerElectronicBox_8 = new TGeoBBox("innerElectronicBox", dx,dy,dz);
   // Volume: innerElectronicsLog
   pinnerElectronicsLog_82ac898 = new TGeoVolume("innerElectronicsLog",pinnerElectronicBox_8, pMed1);
   pinnerElectronicsLog_82ac898->SetVisLeaves(kTRUE);
   pelectronicsLog_82ac6f0->AddNode(pinnerElectronicsLog_82ac898, 0, pMatrix34);

   // CLOSE GEOMETRY
   gGeoManager->CloseGeometry();
}
