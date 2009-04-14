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
// Mixture: plasticForGFI
   nel     = 2;
   density = 1.032000;
   pMat35 = new TGeoMixture("plasticForGFI", nel,density);
      a = 12.010700;   z = 6.000000;   w = 0.914708;  // C
   pMat35->DefineElement(0,a,z,w);
      a = 1.007940;   z = 1.000000;   w = 0.085292;  // H
   pMat35->DefineElement(1,a,z,w);
   pMat35->SetIndex(34);
// Medium: plasticForGFI
   numed   = 34;  // medium number
   par[0]  = 0.000000; // isvol
   par[1]  = 0.000000; // ifield
   par[2]  = 0.000000; // fieldm
   par[3]  = 0.000000; // tmaxfd
   par[4]  = 0.000000; // stemax
   par[5]  = 0.000000; // deemax
   par[6]  = 0.000000; // epsil
   par[7]  = 0.000000; // stmin
   pMed35 = new TGeoMedium("plasticForGFI", numed,pMat35, par);
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
   dx = 73.700000;
   dy = 0.000000;
   dz = 525.400000;
   // Rotation: 
   thx = 106.700000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 16.700000;    phz = 0.000000;
   TGeoRotation *pMatrix3 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix2 = new TGeoCombiTrans("", dx,dy,dz,pMatrix3);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix7 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix6 = new TGeoCombiTrans("", dx,dy,dz,pMatrix7);
   // Combi transformation: 
   dx = 0.000000;
   dy = 27.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix9 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix8 = new TGeoCombiTrans("", dx,dy,dz,pMatrix9);
   // Combi transformation: 
   dx = 0.000000;
   dy = -27.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix11 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix10 = new TGeoCombiTrans("", dx,dy,dz,pMatrix11);
   // Combi transformation: 
   dx = 27.000000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix13 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix12 = new TGeoCombiTrans("", dx,dy,dz,pMatrix13);
   // Combi transformation: 
   dx = -27.000000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix15 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix14 = new TGeoCombiTrans("", dx,dy,dz,pMatrix15);
   // Combi transformation: 
   dx = 141.800000;
   dy = 0.000000;
   dz = 727.300000;
   // Rotation: 
   thx = 106.700000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 16.700000;    phz = 0.000000;
   TGeoRotation *pMatrix5 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix4 = new TGeoCombiTrans("", dx,dy,dz,pMatrix5);
   // Shape: World type: TGeoBBox
   dx = 1500.000000;
   dy = 1500.000000;
   dz = 1500.000000;
   TGeoShape *pWorld_1 = new TGeoBBox("World", dx,dy,dz);
   // Volume: World
   pWorld_82aa9a8 = new TGeoVolume("World",pWorld_1, pMed2);
   pWorld_82aa9a8->SetVisLeaves(kTRUE);

   // SET TOP VOLUME OF GEOMETRY
   gGeoManager->SetTopVolume(pWorld_82aa9a8);

   // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
   // Shape: GFIBoxWorld type: TGeoBBox
   dx = 29.000000;
   dy = 29.000000;
   dz = 0.050000;
   TGeoShape *pGFIBoxWorld_2 = new TGeoBBox("GFIBoxWorld", dx,dy,dz);
   // Volume: GFILogWorld
   pGFILogWorld_82ab420 = new TGeoVolume("GFILogWorld",pGFIBoxWorld_2, pMed1);
   pGFILogWorld_82ab420->SetVisLeaves(kTRUE);
   pWorld_82aa9a8->AddNode(pGFILogWorld_82ab420, 0, pMatrix2);
   pWorld_82aa9a8->AddNode(pGFILogWorld_82ab420, 1, pMatrix4);
   // Shape: GFIBox type: TGeoBBox
   dx = 25.000000;
   dy = 25.000000;
   dz = 0.050000;
   TGeoShape *pGFIBox_3 = new TGeoBBox("GFIBox", dx,dy,dz);
   // Volume: GFILog
   pGFILog_82ab5c8 = new TGeoVolume("GFILog",pGFIBox_3, pMed35);
   pGFILog_82ab5c8->SetVisLeaves(kTRUE);
   pGFILogWorld_82ab420->AddNode(pGFILog_82ab5c8, 0, pMatrix6);
   // Shape: UpFrame type: TGeoBBox
   dx = 29.000000;
   dy = 2.000000;
   dz = 0.050000;
   TGeoShape *pUpFrame_4 = new TGeoBBox("UpFrame", dx,dy,dz);
   // Volume: logicUpFrame
   plogicUpFrame_82ab760 = new TGeoVolume("logicUpFrame",pUpFrame_4, pMed21);
   plogicUpFrame_82ab760->SetVisLeaves(kTRUE);
   pGFILogWorld_82ab420->AddNode(plogicUpFrame_82ab760, 0, pMatrix8);
   // Shape: DownFrame type: TGeoBBox
   dx = 29.000000;
   dy = 2.000000;
   dz = 0.050000;
   TGeoShape *pDownFrame_5 = new TGeoBBox("DownFrame", dx,dy,dz);
   // Volume: logicDownFrame
   plogicDownFrame_82ab928 = new TGeoVolume("logicDownFrame",pDownFrame_5, pMed21);
   plogicDownFrame_82ab928->SetVisLeaves(kTRUE);
   pGFILogWorld_82ab420->AddNode(plogicDownFrame_82ab928, 0, pMatrix10);
   // Shape: RightFrame type: TGeoBBox
   dx = 2.000000;
   dy = 25.000000;
   dz = 0.050000;
   TGeoShape *pRightFrame_6 = new TGeoBBox("RightFrame", dx,dy,dz);
   // Volume: logicRightFrame
   plogicRightFrame_82abac8 = new TGeoVolume("logicRightFrame",pRightFrame_6, pMed21);
   plogicRightFrame_82abac8->SetVisLeaves(kTRUE);
   pGFILogWorld_82ab420->AddNode(plogicRightFrame_82abac8, 0, pMatrix12);
   // Shape: LeftFrame type: TGeoBBox
   dx = 2.000000;
   dy = 25.000000;
   dz = 0.050000;
   TGeoShape *pLeftFrame_7 = new TGeoBBox("LeftFrame", dx,dy,dz);
   // Volume: logicLeftFrame
   plogicLeftFrame_82abc68 = new TGeoVolume("logicLeftFrame",pLeftFrame_7, pMed21);
   plogicLeftFrame_82abc68->SetVisLeaves(kTRUE);
   pGFILogWorld_82ab420->AddNode(plogicLeftFrame_82abc68, 0, pMatrix14);

   // CLOSE GEOMETRY
   gGeoManager->CloseGeometry();
}
