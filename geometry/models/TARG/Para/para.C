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
// Mixture: CH2
   nel     = 2;
   density = 0.930000;
   pMat16 = new TGeoMixture("CH2", nel,density);
      a = 12.010700;   z = 6.000000;   w = 0.856281;  // C
   pMat16->DefineElement(0,a,z,w);
      a = 1.007940;   z = 1.000000;   w = 0.143719;  // H
   pMat16->DefineElement(1,a,z,w);
   pMat16->SetIndex(15);
// Medium: CH2
   numed   = 15;  // medium number
   par[0]  = 0.000000; // isvol
   par[1]  = 0.000000; // ifield
   par[2]  = 0.000000; // fieldm
   par[3]  = 0.000000; // tmaxfd
   par[4]  = 0.000000; // stemax
   par[5]  = 0.000000; // deemax
   par[6]  = 0.000000; // epsil
   par[7]  = 0.000000; // stmin
   pMed16 = new TGeoMedium("CH2", numed,pMat16, par);

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
   // Shape: World type: TGeoBBox
   dx = 1500.000000;
   dy = 1500.000000;
   dz = 1500.000000;
   TGeoShape *pWorld_1 = new TGeoBBox("World", dx,dy,dz);
   // Volume: World
   pWorld_82aa360 = new TGeoVolume("World",pWorld_1, pMed2);
   pWorld_82aa360->SetVisLeaves(kTRUE);

   // SET TOP VOLUME OF GEOMETRY
   gGeoManager->SetTopVolume(pWorld_82aa360);

   // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
   // Shape: Parafin0deg type: TGeoTubeSeg
   rmin = 0.000000;
   rmax = 1.000000;
   dz   = 0.005500;
   phi1 = 0.000000;
   phi2 = 360.000000;
   TGeoShape *pParafin0deg_2 = new TGeoTubeSeg("Parafin0deg",rmin,rmax,dz,phi1,phi2);
   // Volume: Parafin0deg
   pParafin0deg_82aadf8 = new TGeoVolume("Parafin0deg",pParafin0deg_2, pMed16);
   pParafin0deg_82aadf8->SetVisLeaves(kTRUE);
   pWorld_82aa360->AddNode(pParafin0deg_82aadf8, 0, pMatrix2);

   // CLOSE GEOMETRY
   gGeoManager->CloseGeometry();
}
