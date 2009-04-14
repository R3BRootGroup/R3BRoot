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
// Material: H2
   a       = 1.007940;
   z       = 1.000000;
   density = 0.070800;
   radl    = 816.908193;
   absl    = 4956.556132;
   pMat3 = new TGeoMaterial("H2", a,z,density,radl,absl);
   pMat3->SetIndex(2);
// Medium: H2
   numed   = 2;  // medium number
   par[0]  = 0.000000; // isvol
   par[1]  = 0.000000; // ifield
   par[2]  = 0.000000; // fieldm
   par[3]  = 0.000000; // tmaxfd
   par[4]  = 0.000000; // stemax
   par[5]  = 0.000000; // deemax
   par[6]  = 0.000000; // epsil
   par[7]  = 0.000000; // stmin
   pMed3 = new TGeoMedium("H2", numed,pMat3, par);

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
   dz = 1.765000;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix5 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix4 = new TGeoCombiTrans("", dx,dy,dz,pMatrix5);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 0.007500;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix7 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix6 = new TGeoCombiTrans("", dx,dy,dz,pMatrix7);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 0.007500;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix9 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix8 = new TGeoCombiTrans("", dx,dy,dz,pMatrix9);
   // Combi transformation: 
   dx = 0.000000;
   dy = 0.000000;
   dz = 3.522500;
   // Rotation: 
   thx = 90.000000;    phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 0.000000;    phz = 0.000000;
   TGeoRotation *pMatrix11 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   pMatrix10 = new TGeoCombiTrans("", dx,dy,dz,pMatrix11);
   // Shape: World type: TGeoBBox
   dx = 1500.000000;
   dy = 1500.000000;
   dz = 1500.000000;
   TGeoShape *pWorld_1 = new TGeoBBox("World", dx,dy,dz);
   // Volume: World
   pWorld_82aa760 = new TGeoVolume("World",pWorld_1, pMed2);
   pWorld_82aa760->SetVisLeaves(kTRUE);

   // SET TOP VOLUME OF GEOMETRY
   gGeoManager->SetTopVolume(pWorld_82aa760);

   // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
   // Shape: TargetEnveloppe type: TGeoTubeSeg
   rmin = 0.000000;
   rmax = 1.015000;
   dz   = 7.045000;
   phi1 = 0.000000;
   phi2 = 360.000000;
   TGeoShape *pTargetEnveloppe_2 = new TGeoTubeSeg("TargetEnveloppe",rmin,rmax,dz,phi1,phi2);
   // Volume: TargetEnveloppe
   pTargetEnveloppe_82ab208 = new TGeoVolume("TargetEnveloppe",pTargetEnveloppe_2, pMed1);
   pTargetEnveloppe_82ab208->SetVisLeaves(kTRUE);
   pWorld_82aa760->AddNode(pTargetEnveloppe_82ab208, 0, pMatrix2);
   // Shape: Target1 type: TGeoTubeSeg
   rmin = 1.000000;
   rmax = 1.015000;
   dz   = 1.750000;
   phi1 = 0.000000;
   phi2 = 360.000000;
   TGeoShape *pTarget1_3 = new TGeoTubeSeg("Target1",rmin,rmax,dz,phi1,phi2);
   // Volume: Target1
   pTarget1_82ab3f8 = new TGeoVolume("Target1",pTarget1_3, pMed15);
   pTarget1_82ab3f8->SetVisLeaves(kTRUE);
   pTargetEnveloppe_82ab208->AddNode(pTarget1_82ab3f8, 0, pMatrix4);
   // Shape: Target2 type: TGeoTubeSeg
   rmin = 0.000000;
   rmax = 1.000000;
   dz   = 1.750000;
   phi1 = 0.000000;
   phi2 = 360.000000;
   TGeoShape *pTarget2_4 = new TGeoTubeSeg("Target2",rmin,rmax,dz,phi1,phi2);
   // Volume: Target2
   pTarget2_82ab590 = new TGeoVolume("Target2",pTarget2_4, pMed3);
   pTarget2_82ab590->SetVisLeaves(kTRUE);
   pTargetEnveloppe_82ab208->AddNode(pTarget2_82ab590, 0, pMatrix6);
   // Shape: Target3 type: TGeoTubeSeg
   rmin = 0.000000;
   rmax = 1.015000;
   dz   = 0.007500;
   phi1 = 0.000000;
   phi2 = 360.000000;
   TGeoShape *pTarget3_5 = new TGeoTubeSeg("Target3",rmin,rmax,dz,phi1,phi2);
   // Volume: Target3
   pTarget3_82ab760 = new TGeoVolume("Target3",pTarget3_5, pMed15);
   pTarget3_82ab760->SetVisLeaves(kTRUE);
   pTargetEnveloppe_82ab208->AddNode(pTarget3_82ab760, 0, pMatrix8);
   pTargetEnveloppe_82ab208->AddNode(pTarget3_82ab760, 0, pMatrix10);

   // CLOSE GEOMETRY
   gGeoManager->CloseGeometry();
}
