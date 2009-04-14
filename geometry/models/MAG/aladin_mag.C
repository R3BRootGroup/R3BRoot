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
// Material: Iron
   a       = 55.850000;
   z       = 26.000000;
   density = 7.870000;
   radl    = 1.757717;
   absl    = 169.994418;
   pMat24 = new TGeoMaterial("Iron", a,z,density,radl,absl);
   pMat24->SetIndex(23);
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
   pMatrix8 = new TGeoCombiTrans("", dx,dy,dz,pMatrix9);
   // Shape: World type: TGeoBBox
   dx = 1500.000000;
   dy = 1500.000000;
   dz = 1500.000000;
   TGeoShape *pWorld_1 = new TGeoBBox("World", dx,dy,dz);
   // Volume: World
   pWorld_82aa730 = new TGeoVolume("World",pWorld_1, pMed2);
   pWorld_82aa730->SetVisLeaves(kTRUE);

   // SET TOP VOLUME OF GEOMETRY
   gGeoManager->SetTopVolume(pWorld_82aa730);

   // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
   // Shape: FeYoke_up type: TGeoBBox
   dx = 78.000000;
   dy = 25.000000;
   dz = 88.000000;
   TGeoShape *pFeYoke_up_2 = new TGeoBBox("FeYoke_up", dx,dy,dz);
   // Volume: Feyoke_up
   pFeyoke_up_82ab1a8 = new TGeoVolume("Feyoke_up",pFeYoke_up_2, pMed24);
   pFeyoke_up_82ab1a8->SetVisLeaves(kTRUE);
   pWorld_82aa730->AddNode(pFeyoke_up_82ab1a8, 10, pMatrix2);
   // Shape: FeYoke_down type: TGeoBBox
   dx = 78.000000;
   dy = 25.000000;
   dz = 88.000000;
   TGeoShape *pFeYoke_down_3 = new TGeoBBox("FeYoke_down", dx,dy,dz);
   // Volume: Feyoke_down
   pFeyoke_down_82ab348 = new TGeoVolume("Feyoke_down",pFeYoke_down_3, pMed24);
   pFeyoke_down_82ab348->SetVisLeaves(kTRUE);
   pWorld_82aa730->AddNode(pFeyoke_down_82ab348, 11, pMatrix4);
   // Shape: FeYoke_left type: TGeoBBox
   dx = 25.000000;
   dy = 75.000000;
   dz = 88.000000;
   TGeoShape *pFeYoke_left_4 = new TGeoBBox("FeYoke_left", dx,dy,dz);
   // Volume: Feyoke_left
   pFeyoke_left_82ab4e8 = new TGeoVolume("Feyoke_left",pFeYoke_left_4, pMed24);
   pFeyoke_left_82ab4e8->SetVisLeaves(kTRUE);
   pWorld_82aa730->AddNode(pFeyoke_left_82ab4e8, 12, pMatrix6);
   // Shape: FeYoke_right type: TGeoBBox
   dx = 25.000000;
   dy = 75.000000;
   dz = 88.000000;
   TGeoShape *pFeYoke_right_5 = new TGeoBBox("FeYoke_right", dx,dy,dz);
   // Volume: Feyoke_right
   pFeyoke_right_82ab690 = new TGeoVolume("Feyoke_right",pFeYoke_right_5, pMed24);
   pFeyoke_right_82ab690->SetVisLeaves(kTRUE);
   pWorld_82aa730->AddNode(pFeyoke_right_82ab690, 15, pMatrix8);

   // CLOSE GEOMETRY
   gGeoManager->CloseGeometry();
}
