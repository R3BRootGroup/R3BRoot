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
// Mixture: mixtureForDCH
   nel     = 3;
   density = 0.001017;
   pMat33 = new TGeoMixture("mixtureForDCH", nel,density);
      a = 39.948000;   z = 18.000000;   w = 0.800000;  // AR
   pMat33->DefineElement(0,a,z,w);
      a = 12.010700;   z = 6.000000;   w = 0.054582;  // C
   pMat33->DefineElement(1,a,z,w);
      a = 15.999400;   z = 8.000000;   w = 0.145418;  // O
   pMat33->DefineElement(2,a,z,w);
   pMat33->SetIndex(32);
// Medium: mixtureForDCH
   numed   = 32;  // medium number
   par[0]  = 0.000000; // isvol
   par[1]  = 0.000000; // ifield
   par[2]  = 0.000000; // fieldm
   par[3]  = 0.000000; // tmaxfd
   par[4]  = 0.000000; // stemax
   par[5]  = 0.000000; // deemax
   par[6]  = 0.000000; // epsil
   par[7]  = 0.000000; // stmin
   pMed33 = new TGeoMedium("mixtureForDCH", numed,pMat33, par);
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
// Mixture: Mylar
   nel     = 3;
   density = 1.397000;
   pMat15 = new TGeoMixture("Mylar", nel,density);
      a = 12.010700;   z = 6.000000;   w = 0.625010;  // C
   pMat15->DefineElement(0,a,z,w);
      a = 1.007940;   z = 1.000000;   w = 0.041961;  // H
   pMat15->DefineElement(1,a,z,w);
      a = 15.999400;   z = 8.000000;   w = 0.333029;  // O
   pMat15->DefineElement(2,a,z,w);
   pMat15->SetIndex(14);
// Medium: Mylar
   numed   = 14;  // medium number
   par[0]  = 0.000000; // isvol
   par[1]  = 0.000000; // ifield
   par[2]  = 0.000000; // fieldm
   par[3]  = 0.000000; // tmaxfd
   par[4]  = 0.000000; // stemax
   par[5]  = 0.000000; // deemax
   par[6]  = 0.000000; // epsil
   par[7]  = 0.000000; // stmin
   pMed15 = new TGeoMedium("Mylar", numed,pMat15, par);
// Material: HeliumGas
   a       = 4.000000;
   z       = 2.000000;
   density = 0.000125;
   radl    = 683475.828563;
   absl    = 4444726.310227;
   pMat4 = new TGeoMaterial("HeliumGas", a,z,density,radl,absl);
   pMat4->SetIndex(3);
// Medium: HeliumGas
   numed   = 3;  // medium number
   par[0]  = 0.000000; // isvol
   par[1]  = 0.000000; // ifield
   par[2]  = 0.000000; // fieldm
   par[3]  = 0.000000; // tmaxfd
   par[4]  = 0.000000; // stemax
   par[5]  = 0.000000; // deemax
   par[6]  = 0.000000; // epsil
   par[7]  = 0.000000; // stmin
   pMed4 = new TGeoMedium("HeliumGas", numed,pMat4, par);

   // TRANSFORMATION MATRICES
   // Combi transformation: 
   dx = 128.700000;
   dy = 0.000000;
   dz = 443.900000;
   // Rotation: 
   thx = 121.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 31.000000;    phz = 0.000000;
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
   TGeoRotation *pMatrix9 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix8 = new TGeoCombiTrans("", dx,dy,dz,pMatrix9);
   // Combi transformation: 
   dx = 0.000000;
   dy = 42.200000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix11 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix10 = new TGeoCombiTrans("", dx,dy,dz,pMatrix11);
   // Combi transformation: 
   dx = 0.000000;
   dy = -42.200000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix13 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix12 = new TGeoCombiTrans("", dx,dy,dz,pMatrix13);
   // Combi transformation: 
   dx = 53.400000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix15 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix14 = new TGeoCombiTrans("", dx,dy,dz,pMatrix15);
   // Combi transformation: 
   dx = -53.400000;
   dy = 0.000000;
   dz = 0.000000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix17 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix16 = new TGeoCombiTrans("", dx,dy,dz,pMatrix17);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 4.060600;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix19 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix18 = new TGeoCombiTrans("", dx,dy,dz,pMatrix19);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = -4.060600;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix21 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix20 = new TGeoCombiTrans("", dx,dy,dz,pMatrix21);
   // Combi transformation: 
   dx = 169.100000;
   dy = 0.000000;
   dz = 535.800000;
   // Rotation: 
   thx = 121.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 31.000000;    phz = 0.000000;
   TGeoRotation *pMatrix5 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix4 = new TGeoCombiTrans("", dx,dy,dz,pMatrix5);
   // Combi transformation: 
   dx = 148.900000;
   dy = 0.000000;
   dz = 489.850000;
   // Rotation: 
   thx = 121.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 31.000000;    phz = 0.000000;
   TGeoRotation *pMatrix7 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix6 = new TGeoCombiTrans("", dx,dy,dz,pMatrix7);
   // Shape: World type: TGeoBBox
   dx = 1500.000000;
   dy = 1500.000000;
   dz = 1500.000000;
   TGeoShape *pWorld_1 = new TGeoBBox("World", dx,dy,dz);
   // Volume: World
   pWorld_82aadb8 = new TGeoVolume("World",pWorld_1, pMed2);
   pWorld_82aadb8->SetVisLeaves(kTRUE);

   // SET TOP VOLUME OF GEOMETRY
   gGeoManager->SetTopVolume(pWorld_82aadb8);

   // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
   // Shape: DCHBoxWorld type: TGeoBBox
   dx = 55.400000;
   dy = 44.200000;
   dz = 4.061200;
   TGeoShape *pDCHBoxWorld_2 = new TGeoBBox("DCHBoxWorld", dx,dy,dz);
   // Volume: DCHLogWorld
   pDCHLogWorld_82ab830 = new TGeoVolume("DCHLogWorld",pDCHBoxWorld_2, pMed1);
   pDCHLogWorld_82ab830->SetVisLeaves(kTRUE);
   pWorld_82aadb8->AddNode(pDCHLogWorld_82ab830, 0, pMatrix2);
   pWorld_82aadb8->AddNode(pDCHLogWorld_82ab830, 1, pMatrix4);
   // Shape: heliumBag type: TGeoPara
   dx    = 55.400000;
   dy    = 44.200000;
   dz    = 43.830510;
   alpha = 0.000000;
   theta = 0.000000;
   phi   = 0.000000;
   TGeoShape *pheliumBag_10 = new TGeoPara("heliumBag",dx,dy,dz,alpha,theta,phi);
   // Volume: heliumBag
   pheliumBag_82ac5a0 = new TGeoVolume("heliumBag",pheliumBag_10, pMed4);
   pheliumBag_82ac5a0->SetVisLeaves(kTRUE);
   pWorld_82aadb8->AddNode(pheliumBag_82ac5a0, 0, pMatrix6);
   // Shape: DCHBox type: TGeoBBox
   dx = 51.400000;
   dy = 40.200000;
   dz = 4.060000;
   TGeoShape *pDCHBox_3 = new TGeoBBox("DCHBox", dx,dy,dz);
   // Volume: DCHLog
   pDCHLog_82ab9d8 = new TGeoVolume("DCHLog",pDCHBox_3, pMed33);
   pDCHLog_82ab9d8->SetVisLeaves(kTRUE);
   pDCHLogWorld_82ab830->AddNode(pDCHLog_82ab9d8, 0, pMatrix8);
   // Shape: UpFrame type: TGeoBBox
   dx = 55.400000;
   dy = 2.000000;
   dz = 4.061200;
   TGeoShape *pUpFrame_4 = new TGeoBBox("UpFrame", dx,dy,dz);
   // Volume: logicUpFrame
   plogicUpFrame_82abb70 = new TGeoVolume("logicUpFrame",pUpFrame_4, pMed21);
   plogicUpFrame_82abb70->SetVisLeaves(kTRUE);
   pDCHLogWorld_82ab830->AddNode(plogicUpFrame_82abb70, 0, pMatrix10);
   // Shape: DownFrame type: TGeoBBox
   dx = 55.400000;
   dy = 2.000000;
   dz = 4.061200;
   TGeoShape *pDownFrame_5 = new TGeoBBox("DownFrame", dx,dy,dz);
   // Volume: logicDownFrame
   plogicDownFrame_82abd38 = new TGeoVolume("logicDownFrame",pDownFrame_5, pMed21);
   plogicDownFrame_82abd38->SetVisLeaves(kTRUE);
   pDCHLogWorld_82ab830->AddNode(plogicDownFrame_82abd38, 0, pMatrix12);
   // Shape: RightFrame type: TGeoBBox
   dx = 2.000000;
   dy = 40.200000;
   dz = 4.060000;
   TGeoShape *pRightFrame_6 = new TGeoBBox("RightFrame", dx,dy,dz);
   // Volume: logicRightFrame
   plogicRightFrame_82abed8 = new TGeoVolume("logicRightFrame",pRightFrame_6, pMed21);
   plogicRightFrame_82abed8->SetVisLeaves(kTRUE);
   pDCHLogWorld_82ab830->AddNode(plogicRightFrame_82abed8, 0, pMatrix14);
   // Shape: LeftFrame type: TGeoBBox
   dx = 2.000000;
   dy = 40.200000;
   dz = 4.060000;
   TGeoShape *pLeftFrame_7 = new TGeoBBox("LeftFrame", dx,dy,dz);
   // Volume: logicLeftFrame
   plogicLeftFrame_82ac078 = new TGeoVolume("logicLeftFrame",pLeftFrame_7, pMed21);
   plogicLeftFrame_82ac078->SetVisLeaves(kTRUE);
   pDCHLogWorld_82ab830->AddNode(plogicLeftFrame_82ac078, 0, pMatrix16);
   // Shape: FrontFrame type: TGeoBBox
   dx = 51.400000;
   dy = 40.200000;
   dz = 0.000600;
   TGeoShape *pFrontFrame_8 = new TGeoBBox("FrontFrame", dx,dy,dz);
   // Volume: logicFrontFrame
   plogicFrontFrame_82ac218 = new TGeoVolume("logicFrontFrame",pFrontFrame_8, pMed15);
   plogicFrontFrame_82ac218->SetVisLeaves(kTRUE);
   pDCHLogWorld_82ab830->AddNode(plogicFrontFrame_82ac218, 0, pMatrix18);
   // Shape: BackFrame type: TGeoBBox
   dx = 51.400000;
   dy = 40.200000;
   dz = 0.000600;
   TGeoShape *pBackFrame_9 = new TGeoBBox("BackFrame", dx,dy,dz);
   // Volume: logicBackFrame
   plogicBackFrame_82ac3b8 = new TGeoVolume("logicBackFrame",pBackFrame_9, pMed15);
   plogicBackFrame_82ac3b8->SetVisLeaves(kTRUE);
   pDCHLogWorld_82ab830->AddNode(plogicBackFrame_82ac3b8, 0, pMatrix20);

   // CLOSE GEOMETRY
   gGeoManager->CloseGeometry();
}
