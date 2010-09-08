
void veto() {
  gSystem->Load("libGeom");
   TGeoManager* geom =
   new TGeoManager("VGM Root geometry", "VGM Root geometry");
  // out-of-file geometry definition
   Double_t dx,dy,dz;
   Double_t w1,w2,w3;
   Double_t dxw,dyw,dzw;
   Double_t dxp,dyp,dzp;
   Double_t dxpad,dypad,dzpad;
   Double_t a;
   Double_t thx, phx, thy, phy, thz, phz;
   Double_t thx1, phx1, thy1, phy1, thz1, phz1;
   Double_t thx2, phx2, thy2, phy2, thz2, phz2;
   Double_t z, density, w;
   Int_t nel, numed;


/****************************************************************************/
// Material definition


  // Mixture: Air
  TGeoMedium * pMed2=NULL;
   if (gGeoManager->GetMedium("Air") ){
       pMed2=gGeoManager->GetMedium("Air");
   }else{
     nel     = 2;
     density = 0.001290;
     TGeoMixture*
	 pMat2 = new TGeoMixture("Air", nel,density);
     a = 14.006740;   z = 7.000000;   w = 0.700000;  // N
     pMat2->DefineElement(0,a,z,w);
     a = 15.999400;   z = 8.000000;   w = 0.300000;  // O
     pMat2->DefineElement(1,a,z,w);
     pMat2->SetIndex(1);
     // Medium: Air
     numed   = 1;  // medium number
     Double_t par[8];
     par[0]  = 0.000000; // isvol
     par[1]  = 0.000000; // ifield
     par[2]  = 0.000000; // fieldm
     par[3]  = 0.000000; // tmaxfd
     par[4]  = 0.000000; // stemax
     par[5]  = 0.000000; // deemax
     par[6]  = 0.000100; // epsil
     par[7]  = 0.000000; // stmin
     pMed2 = new TGeoMedium("Air", numed,pMat2, par);
   }


  // Mixture: plasticForVETO
  TGeoMedium * pMed34=NULL;
   if (gGeoManager->GetMedium("plasticForVETO") ){
       pMed34=gGeoManager->GetMedium("plasticForVETO");
   }else{
     nel     = 2;
     density = 1.032000;
     TGeoMixture*
	 pMat34 = new TGeoMixture("plasticForVETO", nel,density);
     a = 12.010700;   z = 6.000000;   w = 0.914708;  // C
     pMat34->DefineElement(0,a,z,w);
     a = 1.007940;   z = 1.000000;   w = 0.085292;  // H
     pMat34->DefineElement(1,a,z,w);
     pMat34->SetIndex(33);
     // Medium: plasticForVETO
     numed   = 33;  // medium number
     Double_t par[8];
     par[0]  = 0.000000; // isvol
     par[1]  = 0.000000; // ifield
     par[2]  = 0.000000; // fieldm
     par[3]  = 0.000000; // tmaxfd
     par[4]  = 0.000000; // stemax
     par[5]  = 0.000000; // deemax
     par[6]  = 0.000100; // epsil
     par[7]  = 0.000000; // stmin
     pMed34 = new TGeoMedium("plasticForVETO", numed,pMat34,par);
   }
   //global position
   w1=400.0;
   w2=0.00;
   w3=900.0;

  // TRANSFORMATION MATRICES
   // Combi transformation: 
    dx = 419.700000;
    dy = 0.000000;
    dz = 952.400000;
   // dz = 0.;
   // Rotation for whole veto:
   thx = 121.000000;   phx = 0.000000;
   thy = 90.000000;    phy = 90.000000;
   thz = 31.000000;    phz = 0.000000;

   TGeoRotation *pMatrix3 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
   TGeoCombiTrans*
   pMatrix2 = new TGeoCombiTrans("", w1,w2,w3,pMatrix3);


     // Shape: World type: TGeoBBox
   dxw = 1500.000000;
   dyw = 1500.000000;
   dzw = 1500.000000;
   TGeoShape *pWorld = new TGeoBBox("World", dxw,dyw,dzw);
   //Top Volume
   //TGeoVolume* pWorld = gGeoManager->GetTopVolume();
   vWorld = new TGeoVolume("World",pWorld, pMed2);
   vWorld->SetVisLeaves(kTRUE);
   gGeoManager->SetTopVolume(vWorld);


   // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
   // Shape: VETOBox type: TGeoBBox
   dx = 50.000000;
   dy = 50.000000;
   dz = 10.000000;

   dxp = 50.00;
   dyp = 50.00;
   dzp = 5.00;
   

   dxpad = 5.00;
   dypad = 50.00;
   dzpad = 5.00;

    TGeoRotation *pMatrix51 = new TGeoRotation();
   pMatrix51->RotateX(0.);
   pMatrix51->RotateY(0.);
   pMatrix51->RotateZ(0.);
   TGeoCombiTrans* pMatrix41=new TGeoCombiTrans("",0.,0.,-dzp,pMatrix51);

   TGeoRotation *pMatrix52 = new TGeoRotation();
   pMatrix52->RotateX(0.);
   pMatrix52->RotateY(0.);
   pMatrix52->RotateZ(90.);
   
   TGeoCombiTrans* pMatrix42=new TGeoCombiTrans("",0.,0.,dzp,pMatrix52);

   TGeoVolume* pVETOLog= gGeoManager->MakeBox("VETOLog",pMed2,dx,dy,dz);

   
   // Volume: VETOLog
   TGeoVolume*pVETOLogp = gGeoManager->MakeBox("VETOLogp",pMed2,dxp,dyp,dzp);
   pVETOLog->SetVisLeaves(kTRUE);
  
   TGeoVolume* pVETOLogpad= gGeoManager->MakeBox("VETOLogpad",pMed34,dxpad,dypad,dzpad);

   
  
  //  Double_t x=-45.0;
//    for(Int_t i=1;i<11;i++){
//      pVETOLog->AddNode(pVETOLogpad,i,new TGeoCombiTrans(x,0.,-dzp,pMatrix51));
//       x=x+2*5.0;
//    }
   
//    Double_t x=-45.0;
//   for(Int_t i=11;i<21;i++){
//      pVETOLog->AddNode(pVETOLogpad,i,new TGeoCombiTrans(0.,x,dzp,pMatrix52));
//       x=x+2*5.0;
//    }
  
   Double_t x=-45.0;
   for(Int_t i=1;i<11;i++){
     pVETOLogp->AddNode(pVETOLogpad,i,new TGeoCombiTrans(x,0.,0.,pMatrix51));
     x=x+2*5.0;
   } 
   
  

  pVETOLog->AddNode(pVETOLogp,1,pMatrix41);
  pVETOLog->AddNode(pVETOLogp,1,pMatrix42);
   vWorld->AddNode(pVETOLog, 1);
   gGeoManager->CloseGeometry();
   TFile *f = new TFile("veto.root","RECREATE");
   gGeoManager->Write();
   f->Close();

   // AddSensitiveVolume(pVETOLogp);
//    fNbOfSensitiveVol+=2;

//    AddSensitiveVolume(pVETOLog);
//    fNbOfSensitiveVol+=1;
   

}




