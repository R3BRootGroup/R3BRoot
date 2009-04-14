void ne() {
   gSystem->Load("libGeom");
   TGeoManager* geom =
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

   TGeoMaterial *vacuum = new TGeoMaterial("vacuum",0,0,0);
   TGeoMaterial *Fe = new TGeoMaterial("Fe",55.84,26.7,7.87);
   TGeoMaterial *Cu = new TGeoMaterial("Cu",63.549,29,8.92);

//------------------Creat media----------------------------------
   TGeoMedium *Iron = new TGeoMedium("Iron",1,Fe);

 // BC408 plastic medium
 // Mixture: BC408
   nel     = 2;
   density = 1.032000;
   pMat37 = new TGeoMixture("BC408", nel,density);
      a = 1.007940;   z = 1.000000;   w = 0.520000;  // H
   pMat37->DefineElement(0,a,z,w);
      a = 12.010700;   z = 6.000000;   w = 0.480000;  // C
   pMat37->DefineElement(1,a,z,w);
   pMat37->SetIndex(36);
// Medium: BC408
   numed   = 36;  // medium number
   par[0]  = 0.000000; // isvol
   par[1]  = 0.000000; // ifield
   par[2]  = 0.000000; // fieldm
   par[3]  = 0.000000; // tmaxfd
   par[4]  = 0.000000; // stemax
   par[5]  = 0.000000; // deemax
   par[6]  = 0.000000; // epsil
   par[7]  = 0.000000; // stmin
   pMed37 = new TGeoMedium("BC408", numed,pMat37, par);



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

   // TRANSFORMATION MATRICES
   // Shape: World type: TGeoBBox
   dx = 1500.000000;
   dy = 1500.000000;
   dz = 1500.000000;
   TGeoShape *pWorld = new TGeoBBox("World", dx,dy,dz);
   // Volume: World
   vWorld = new TGeoVolume("World",pWorld, pMed2);
   vWorld->SetVisLeaves(kTRUE);

   // SET TOP VOLUME OF GEOMETRY
   gGeoManager->SetTopVolume(vWorld);

   // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
  Double_t padle_h_dim1x = 100.1;
  Double_t padle_h_dim1y = 5.1;
  Double_t padle_h_dim1z = 5.1;
  TGeoShape* padle_h_box1 = new TGeoBBox("padle_h_box1",
					 padle_h_dim1x,
					 padle_h_dim1y,
					 padle_h_dim1z);
  Double_t padle_h_dim2x = 100.0;
  Double_t padle_h_dim2y = 5.0;
  Double_t padle_h_dim2z = 5.0;
  TGeoShape* padle_h_box2 = new TGeoBBox("padle_h_box2",
					 padle_h_dim2x,
					 padle_h_dim2y,
					 padle_h_dim2z);

  // Create a composite shape
  TGeoCompositeShape *sheetbox
      = new TGeoCompositeShape("diffbox", "padle_h_box1 - padle_h_box2");
  // Corresponding boolean volume
  TGeoVolume *bvol = new TGeoVolume("sheetbox",sheetbox,Iron);

  //end-cap list
  Double_t dx1 = 5.100000;
  Double_t dx2 = 2.510000;
  Double_t dy1 = 5.100000;
  Double_t dy2 = 2.510000;
  Double_t dz  = 5.000000;

  TGeoVolume *trap = geom->MakeTrd2("atrap",pMed37,dx1,dx2,dy1,dy2,dz);

  // sheet Iron list
  //------------------ Iron sheets -----------------------------------------
  Double_t padle_h_dim3x = 100.0;
  Double_t padle_h_dim3y = 5.0;
  Double_t padle_h_dim3z = 0.125;

  TGeoVolume *padle_h_box3
      =geom->MakeBox("padle_h_box3",Iron,
                                    padle_h_dim3x,
				    padle_h_dim3y,
				    padle_h_dim3z);

  Double_t padle_h_dim4x = 100.0;
  Double_t padle_h_dim4y = 5.0;
  Double_t padle_h_dim4z = 0.25;

  TGeoVolume *padle_h_box4
      =geom->MakeBox("padle_h_box4",Iron,
                                    padle_h_dim4x,
				    padle_h_dim4y,
				    padle_h_dim4z);

   //------------------ BC408 sheets -----------------------------------------
  Double_t padle_h_dim5x = 100.0;
  Double_t padle_h_dim5y = 5.0;
  Double_t padle_h_dim5z = 0.25;

  TGeoVolume *padle_h_box5
      =geom->MakeBox("padle_h_box5",Iron,
                                    padle_h_dim5x,
				    padle_h_dim5y,
				    padle_h_dim5z);




 // Make the elementary assembly of the whole structure
   TGeoVolume *aLand = new TGeoVolumeAssembly("ALAND");

   TGeoRotation *rott = new TGeoRotation();
   rott->RotateX(0.);
   rott->RotateY(0.);
   rott->RotateZ(0.);
   Double_t xx = 0.;
   Double_t yy = -95.;
   Double_t zz = -45.;

   aLand->AddNode(bvol,1,new TGeoCombiTrans(xx,yy,zz,rott));

   TGeoRotation *rot1 = new TGeoRotation();
   rot1->RotateX(0.);
   rot1->RotateY(90.);
   rot1->RotateZ(90.);
   xx = 105.1;
   yy = -95.;
   zz = -45.;

   aLand->AddNode(trap,2,new TGeoCombiTrans(xx,yy,zz,rot1));

   TGeoRotation *rot2 = new TGeoRotation();
   rot2->RotateX(0.);
   rot2->RotateY(90.);
   rot2->RotateZ(270.);
   xx = -105.1;
   yy = -95.;
   zz = -45.;
   aLand->AddNode(trap,3,new TGeoCombiTrans(xx,yy,zz,rot2));



   //paddles
   TGeoRotation *rot3 = new TGeoRotation();
   rot3->RotateX(0.);
   rot3->RotateY(0.);
   rot3->RotateZ(0.);
   xx = 0.;
   yy = -95.;
   zz = -49.875;

   aLand->AddNode(padle_h_box3,4,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -49.;
   aLand->AddNode(padle_h_box3,5,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -48.;
   aLand->AddNode(padle_h_box3,6,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -47.;
   aLand->AddNode(padle_h_box3,7,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -46.;
   aLand->AddNode(padle_h_box3,8,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -45.;
   aLand->AddNode(padle_h_box3,9,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -44.;
   aLand->AddNode(padle_h_box3,10,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -43.;
   aLand->AddNode(padle_h_box3,11,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -42.;
   aLand->AddNode(padle_h_box3,12,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -41.;
   aLand->AddNode(padle_h_box3,13,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -40.125;
   aLand->AddNode(padle_h_box3,14,new TGeoCombiTrans(xx,yy,zz,rot3));


   //bc408 paddles

   xx = 0.;
   yy = -95.;
   zz = -49.5;
   aLand->AddNode(padle_h_box5,15,new TGeoCombiTrans(xx,yy,zz,rot3));
//   zz = -49.5;
//   aLand->AddNode(padle_h_box5,16,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -48.5;
   aLand->AddNode(padle_h_box5,16,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -47.5;
   aLand->AddNode(padle_h_box5,17,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -46.5;
   aLand->AddNode(padle_h_box5,18,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -45.5;
   aLand->AddNode(padle_h_box5,19,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -44.5;
   aLand->AddNode(padle_h_box5,20,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -43.5;
   aLand->AddNode(padle_h_box5,21,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -42.5;
   aLand->AddNode(padle_h_box5,22,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -42.5;
   aLand->AddNode(padle_h_box5,23,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -41.5;
   aLand->AddNode(padle_h_box5,24,new TGeoCombiTrans(xx,yy,zz,rot3));
   zz = -40.5;
   aLand->AddNode(padle_h_box5,25,new TGeoCombiTrans(xx,yy,zz,rot3));

 //------------------------- Horizontal Assembly Multiplication & Rotation -----------------------------------------------------------------------  


  TGeoVolume *cell = new TGeoVolumeAssembly("CELL");

   TGeoRotation *rot4 = new TGeoRotation();
   rot4->RotateX(0.);
   rot4->RotateY(0.);
   rot4->RotateZ(0.);

   Double_t tx=0.;
   Double_t ty=0.;
   Double_t tz=0.;

 for ( Int_t i = 0; i < 5; i++ )
 {
  for( Int_t j = 0; j < 20; j++ )
  {
     tx=0.;
     ty=j*10.2;
     tz=i*20.;
     Int_t nindex = (j+20*i)+1; //1-100
     cell->AddNode(aLand,nindex,new TGeoCombiTrans(tx,ty,tz,rot4));
  }
  }
//------------------------- Vertical Assembly Multiplication & Rotation -----------------------------------------------------------------------  

   TGeoRotation *rot5 = new TGeoRotation();
   rot5->RotateX(0.);
   rot5->RotateY(0.);
   rot5->RotateZ(90.);

  for ( Int_t i = 0; i < 5; i++ )
 {
  for( Int_t j = 0; j < 20; j++ )
  {
     tx=j*10.2-192.;
     ty=1.9;
     tz=(2*i+1)*10.;
     Int_t nindex2 = (j+20*i)+101; //101 200
     cell->AddNode(aLand,nindex2,new TGeoCombiTrans(tx,ty,tz,rot5));
  }
 }


//
  vWorld->AddNode(cell,1);

  gGeoManager->CloseGeometry();
// save in ROOT file
  TFile *f = new TFile("ne.root","RECREATE");
  gGeoManager->Write();
  f->Close();

}
