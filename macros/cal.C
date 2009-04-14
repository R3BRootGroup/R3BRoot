Int_t cal(){
// conversion of CALO to Root TGeo
  Double_t pi=3.141516;
  TGeoManager *gGeo = new TGeoManager("CALO","Calorimeter");

// create some materials

   TGeoMaterial *vacuum=new TGeoMaterial("vacuum",0,0,0);
   TGeoMaterial *Fe=new TGeoMaterial("Fe",55.845,26,7.87); 

   TGeoMedium *Air=new TGeoMedium("Vacuum",0,vacuum);
   TGeoMedium *Iron=new TGeoMedium("Iron",1,Fe);
// crystal material CSI <DB>
// to implement properly
   TGeoMedium *CrystalMaterial=new TGeoMedium("CSI",1,Fe);

// create a top volume
   TGeoVolume *top=gGeo->MakeBox("top",Air,1000,1000,1000);
   gGeo->SetTopVolume(top);
// see boundaries
   gGeo->SetTopVisible(0);

// Make a Sphere as a enveloppe

//   TGeoVolume *gSphe=gGeo->MakeSphere("gSphe",Air,242.5,700.,-45.,180.,-45.,360.);
   TGeoVolume *gSphe=gGeo->MakeSphere("gSphe",Air,0.,700.,0,180,0,360.);
   gSphe->SetLineColor(2);
   gSphe->SetFillColor(2);

   // Sphere placement in top volume
   top->AddNode(gSphe,1,
   new TGeoCombiTrans(0.,0.,0.,new TGeoRotation("R1",90,0.,0.))
   );

//---------------------------------------------------------------------------------------
// add here the famous .list file

   Double_t zPlane[2]={-100.,100.};
   Double_t rInner[2]={0.,0.};
   Double_t rOuter[2]={36.88,66.38};

   TGeoVolume *vl = gGeo->MakePgon("PGON",CrystalMaterial, -45.0,360.0,5,2);
   TGeoPgon *pPgon = (TGeoPgon*)(vl->GetShape());
   pPgon->DefineSection(0,zPlane[0],rInner[0],rOuter[0]);
   pPgon->DefineSection(1,zPlane[1],rInner[1],rOuter[1]);


 // Parameters
   Double_t NAJA_X[12] = {-184.0195,184.0195,0,0,-297.7205,-297.7205,297.7205,297.7205,
                         0,0,-184.0195,184.0195};
   Double_t NAJA_Y[12] = {297.7205,297.7205,184.0195,184.0195,0,0,0,0,-184.0195,-184.0195,
                         -297.7205,-297.7205};
   Double_t NAJA_Z[12] = {0,0,297.7205,-297.7205,-184.0195,184.0195,-184.0195,184.0195,
                          297.7205,-297.7205,0,0};

   // X component
   Double_t NAJA_Rot_col_X[12][3];

   NAJA_Rot_col_X[0][0] = TMath::Sin(18.000000*pi/180)*TMath::Cos(211.720000*pi/180);
   NAJA_Rot_col_X[0][1] = TMath::Sin(72.000000*pi/180)*TMath::Cos(31.720000*pi/180);
   NAJA_Rot_col_X[0][2] = TMath::Sin(90.000000*pi/180)*TMath::Cos(121.720000*pi/180);

   NAJA_Rot_col_X[1][0] = TMath::Sin(18.000000*pi/180)*TMath::Cos(328.280000*pi/180);
   NAJA_Rot_col_X[1][1] = TMath::Sin(108.000000*pi/180)*TMath::Cos(328.280000*pi/180);
   NAJA_Rot_col_X[1][2] = TMath::Sin(90.000000*pi/180)*TMath::Cos(58.280000*pi/180);

   NAJA_Rot_col_X[2][0] = TMath::Sin(99.350000*pi/180)*TMath::Cos(15.450000*pi/180);
   NAJA_Rot_col_X[2][1] = TMath::Sin(120.000000*pi/180)*TMath::Cos(110.905000*pi/180);
   NAJA_Rot_col_X[2][2] = TMath::Sin(31.718000*pi/180)*TMath::Cos(90.000000*pi/180);

   NAJA_Rot_col_X[3][0] = TMath::Sin(80.650000*pi/180)*TMath::Cos(15.450000*pi/180);
   NAJA_Rot_col_X[3][1] = TMath::Sin(120.000000*pi/180)*TMath::Cos(290.905000*pi/180);
   NAJA_Rot_col_X[3][2] = TMath::Sin(148.282000*pi/180)*TMath::Cos(90.000000*pi/180);

   NAJA_Rot_col_X[4][0] = TMath::Sin(105.24000*pi/180)*TMath::Cos(80.310000*pi/180);
   NAJA_Rot_col_X[4][1] = TMath::Sin(144.00000*pi/180)*TMath::Cos(328.28620*pi/180);
   NAJA_Rot_col_X[4][2] = TMath::Sin(121.717186*pi/180)*TMath::Cos(180.0035095*pi/180);

   NAJA_Rot_col_X[5][0] = TMath::Sin(74.760000*pi/180)*TMath::Cos(80.310000*pi/180);
   NAJA_Rot_col_X[5][1] = TMath::Sin(144.000000*pi/180)*TMath::Cos(148.286600*pi/180);
   NAJA_Rot_col_X[5][2] = TMath::Sin(58.2822647*pi/180)*TMath::Cos(180.0037079*pi/180);

   NAJA_Rot_col_X[6][0] = TMath::Sin(105.240000*pi/180)*TMath::Cos(99.690000*pi/180);
   NAJA_Rot_col_X[6][1] = TMath::Sin(36.000000*pi/180)*TMath::Cos(31.713800*pi/180);
   NAJA_Rot_col_X[6][2] = TMath::Sin(121.717000*pi/180)*TMath::Cos(359.9964905*pi/180);

   NAJA_Rot_col_X[7][0] = TMath::Sin(74.760000*pi/180)*TMath::Cos(99.690000*pi/180);
   NAJA_Rot_col_X[7][1] = TMath::Sin(36.000000*pi/180)*TMath::Cos(211.713560*pi/180);
   NAJA_Rot_col_X[7][2] = TMath::Sin(58.282450*pi/180)*TMath::Cos(359.996300*pi/180);

   NAJA_Rot_col_X[8][0] = TMath::Sin(99.350000*pi/180)*TMath::Cos(344.550000*pi/180);
   NAJA_Rot_col_X[8][1] = TMath::Sin(60.000000*pi/180)*TMath::Cos(69.095000*pi/180);
   NAJA_Rot_col_X[8][2] = TMath::Sin(31.717600*pi/180)*TMath::Cos(270.000000*pi/180);

   NAJA_Rot_col_X[9][0] = TMath::Sin(80.650000*pi/180)*TMath::Cos(344.550000*pi/180);
   NAJA_Rot_col_X[9][1] = TMath::Sin(60.000000*pi/180)*TMath::Cos(249.095000*pi/180);
   NAJA_Rot_col_X[9][2] = TMath::Sin(148.282400*pi/180)*TMath::Cos(270.000000*pi/180);

   NAJA_Rot_col_X[10][0] = TMath::Sin(18.000000*pi/180)*TMath::Cos(148.280000*pi/180);
   NAJA_Rot_col_X[10][1] = TMath::Sin(108.000000*pi/180)*TMath::Cos(328.280000*pi/180);
   NAJA_Rot_col_X[10][2] = TMath::Sin(90.000000*pi/180)*TMath::Cos(238.280000*pi/180);

   NAJA_Rot_col_X[11][0] = TMath::Sin(18.000000*pi/180)*TMath::Cos(31.720000*pi/180);
   NAJA_Rot_col_X[11][1] = TMath::Sin(72.000000*pi/180)*TMath::Cos(211.720000*pi/180);
   NAJA_Rot_col_X[11][2] = TMath::Sin(90.000000*pi/180)*TMath::Cos(301.720000*pi/180);

   // Y component
   Double_t NAJA_Rot_col_Y[12][3];

   NAJA_Rot_col_Y[0][0] =  TMath::Sin(18.000000*pi/180)*TMath::Sin(211.720000*pi/180);
   NAJA_Rot_col_Y[0][1] =  TMath::Sin(72.000000*pi/180)*TMath::Sin(31.720000*pi/180);
   NAJA_Rot_col_Y[0][2] =  TMath::Sin(90.000000*pi/180)*TMath::Sin(121.720000*pi/180);

   NAJA_Rot_col_Y[1][0] =  TMath::Sin(18.000000*pi/180)*TMath::Sin(328.280000*pi/180);
   NAJA_Rot_col_Y[1][1] =  TMath::Sin(108.000000*pi/180)*TMath::Sin(328.280000*pi/180);
   NAJA_Rot_col_Y[1][2] =  TMath::Sin(90.000000*pi/180)*TMath::Sin(58.280000*pi/180);

   NAJA_Rot_col_Y[2][0] =  TMath::Sin(99.350000*pi/180)*TMath::Sin(15.450000*pi/180);
   NAJA_Rot_col_Y[2][1] =  TMath::Sin(120.000000*pi/180)*TMath::Sin(110.905000*pi/180);
   NAJA_Rot_col_Y[2][2] =  TMath::Sin(31.718000*pi/180)*TMath::Sin(90.000000*pi/180);

   NAJA_Rot_col_Y[3][0] =  TMath::Sin(80.650000*pi/180)*TMath::Sin(15.450000*pi/180);
   NAJA_Rot_col_Y[3][1] =  TMath::Sin(120.000000*pi/180)*TMath::Sin(290.905000*pi/180);
   NAJA_Rot_col_Y[3][2] =  TMath::Sin(148.282000*pi/180)*TMath::Sin(90.000000*pi/180);

   NAJA_Rot_col_Y[4][0] =  TMath::Sin(105.24000*pi/180)*TMath::Sin(80.310000*pi/180);
   NAJA_Rot_col_Y[4][1] =  TMath::Sin(144.00000*pi/180)*TMath::Sin(328.28620*pi/180);
   NAJA_Rot_col_Y[4][2] =  TMath::Sin(121.717186*pi/180)*TMath::Sin(180.0035095*pi/180);

   NAJA_Rot_col_Y[5][0] = TMath::Sin(74.760000*pi/180)*TMath::Sin(80.310000*pi/180);
   NAJA_Rot_col_Y[5][1] = TMath::Sin(144.000000*pi/180)*TMath::Sin(148.286600*pi/180);
   NAJA_Rot_col_Y[5][2] = TMath::Sin(58.2822647*pi/180)*TMath::Sin(180.0037079*pi/180);

   NAJA_Rot_col_Y[6][0] = TMath::Sin(105.240000*pi/180)*TMath::Sin(99.690000*pi/180);
   NAJA_Rot_col_Y[6][1] = TMath::Sin(36.000000*pi/180)*TMath::Sin(31.713800*pi/180);
   NAJA_Rot_col_Y[6][2] = TMath::Sin(121.717000*pi/180)*TMath::Sin(359.9964905*pi/180);

   NAJA_Rot_col_Y[7][0] = TMath::Sin(74.760000*pi/180)*TMath::Sin(99.690000*pi/180);
   NAJA_Rot_col_Y[7][1] = TMath::Sin(36.000000*pi/180)*TMath::Sin(211.713560*pi/180);
   NAJA_Rot_col_Y[7][2] = TMath::Sin(58.282450*pi/180)*TMath::Sin(359.996300*pi/180);

   NAJA_Rot_col_Y[8][0] = TMath::Sin(99.350000*pi/180)*TMath::Sin(344.550000*pi/180);
   NAJA_Rot_col_Y[8][1] = TMath::Sin(60.000000*pi/180)*TMath::Sin(69.095000*pi/180);
   NAJA_Rot_col_Y[8][2] = TMath::Sin(31.717600*pi/180)*TMath::Sin(270.000000*pi/180);

   NAJA_Rot_col_Y[9][0] = TMath::Sin(80.650000*pi/180)*TMath::Sin(344.550000*pi/180);
   NAJA_Rot_col_Y[9][1] = TMath::Sin(60.000000*pi/180)*TMath::Sin(249.095000*pi/180);
   NAJA_Rot_col_Y[9][2] = TMath::Sin(148.282400*pi/180)*TMath::Sin(270.000000*pi/180);

   NAJA_Rot_col_Y[10][0] = TMath::Sin(18.000000*pi/180)*TMath::Sin(148.280000*pi/180);
   NAJA_Rot_col_Y[10][1] = TMath::Sin(108.000000*pi/180)*TMath::Sin(328.280000*pi/180);
   NAJA_Rot_col_Y[10][2] = TMath::Sin(90.000000*pi/180)*TMath::Sin(238.280000*pi/180);

   NAJA_Rot_col_Y[11][0] = TMath::Sin(18.000000*pi/180)*TMath::Sin(31.720000*pi/180);
   NAJA_Rot_col_Y[11][1] = TMath::Sin(72.000000*pi/180)*TMath::Sin(211.720000*pi/180);
   NAJA_Rot_col_Y[11][2] = TMath::Sin(90.000000*pi/180)*TMath::Sin(301.720000*pi/180);


   // Z component
   Double_t NAJA_Rot_col_Z[12][3];

   NAJA_Rot_col_Z[0][0] = TMath::Cos(18.000000*pi/180);
   NAJA_Rot_col_Z[0][1] = TMath::Cos(72.000000*pi/180);
   NAJA_Rot_col_Z[0][2] = TMath::Cos(90.000000*pi/180);

   NAJA_Rot_col_Z[1][0] = TMath::Cos(18.000000*pi/180);
   NAJA_Rot_col_Z[1][1] = TMath::Cos(108.000000*pi/180);
   NAJA_Rot_col_Z[1][2] = TMath::Cos(90.000000*pi/180);

   NAJA_Rot_col_Z[2][0] = TMath::Cos(99.350000*pi/180);
   NAJA_Rot_col_Z[2][1] = TMath::Cos(120.000000*pi/180);
   NAJA_Rot_col_Z[2][2] = TMath::Cos(31.718000*pi/180);

   NAJA_Rot_col_Z[3][0] = TMath::Cos(80.650000*pi/180);
   NAJA_Rot_col_Z[3][1] = TMath::Cos(120.000000*pi/180);
   NAJA_Rot_col_Z[3][2] = TMath::Cos(148.282000*pi/180);

   NAJA_Rot_col_Z[4][0] = TMath::Cos(105.24000*pi/180);
   NAJA_Rot_col_Z[4][1] = TMath::Cos(144.00000*pi/180);
   NAJA_Rot_col_Z[4][2] = TMath::Cos(121.717186*pi/180);

   NAJA_Rot_col_Z[5][0] = TMath::Cos(74.760000*pi/180);
   NAJA_Rot_col_Z[5][1] = TMath::Cos(144.000000*pi/180);
   NAJA_Rot_col_Z[5][2] = TMath::Cos(58.2822647*pi/180);

   NAJA_Rot_col_Z[6][0] = TMath::Cos(105.240000*pi/180);
   NAJA_Rot_col_Z[6][1] = TMath::Cos(36.000000*pi/180);
   NAJA_Rot_col_Z[6][2] = TMath::Cos(121.717000*pi/180);

   NAJA_Rot_col_Z[7][0] = TMath::Cos(74.760000*pi/180);
   NAJA_Rot_col_Z[7][1] = TMath::Cos(36.000000*pi/180);
   NAJA_Rot_col_Z[7][2] = TMath::Cos(58.282450*pi/180);

   NAJA_Rot_col_Z[8][0] = TMath::Cos(99.350000*pi/180);
   NAJA_Rot_col_Z[8][1] = TMath::Cos(60.000000*pi/180);
   NAJA_Rot_col_Z[8][2] = TMath::Cos(31.717600*pi/180);

   NAJA_Rot_col_Z[9][0] = TMath::Cos(80.650000*pi/180);
   NAJA_Rot_col_Z[9][1] = TMath::Cos(60.000000*pi/180);
   NAJA_Rot_col_Z[9][2] = TMath::Cos(148.282400*pi/180);

   NAJA_Rot_col_Z[10][0] = TMath::Cos(18.000000*pi/180);
   NAJA_Rot_col_Z[10][1] = TMath::Cos(108.000000*pi/180);
   NAJA_Rot_col_Z[10][2] = TMath::Cos(90.000000*pi/180);

   NAJA_Rot_col_Z[11][0] = TMath::Cos(18.000000*pi/180);
   NAJA_Rot_col_Z[11][1] = TMath::Cos(72.000000*pi/180);
   NAJA_Rot_col_Z[11][2] = TMath::Cos(90.000000*pi/180);

   /*
for(G4int iter1=0 ; iter1<12 ; iter1++) {
    pIndividualRingRot = 
        new G4RotationMatrix(NAJA_Rot_col_X[iter1],NAJA_Rot_col_Y[iter1],NAJA_Rot_col_Z[iter1]);
    pRingRot =  
        new G4RotationMatrix(((*pIndividualRingRot)*(*pGlobalRot)));

    crystalPhysNAJA = 
        new G4PVPlacement(pRingRot,G4ThreeVector(NAJA_X[iter1],NAJA_Y[iter1],NAJA_Z[iter1]),
                          crystalLogNAJA,"crystalPhysNAJA",
                          worldLog,false,iter1);
}



    G4RotationMatrix* pGlobalRot =
      new G4RotationMatrix(pi/2,pi/2,0.);
*/

   //Global rotation
   TGeoRotation*
       pGlobalRot = new TGeoRotation("R2",0.,0.,0.);
//       pGlobalRot->setAngles(90.,90.,0.)

 for(Int_t  i=6; i<12; i++) {
     char buffer[15];
     sprintf(buffer,"rot%i",i);
     TGeoRotation*
	 pIndividualRingRot = new TGeoRotation(buffer,0.,0.,0.);

     Double_t rot[9];
     rot[0] = NAJA_Rot_col_X[i][0];rot[1] = NAJA_Rot_col_Y[i][0];rot[2] = NAJA_Rot_col_Z[i][0];
     rot[3] = NAJA_Rot_col_X[i][1];rot[4] = NAJA_Rot_col_Y[i][1];rot[5] = NAJA_Rot_col_Z[i][1];
     rot[6] = NAJA_Rot_col_X[i][2];rot[7] = NAJA_Rot_col_Y[i][2];rot[8] = NAJA_Rot_col_Z[i][2];


     pIndividualRingRot->SetMatrix(rot);

     pIndividualRingRot->Print();

    // pIndividualRingRot->MultiplyBy(pGlobalRot);

 // Add in new world
     gSphe->AddNode(vl,i,
		  new TGeoCombiTrans(NAJA_X[i],NAJA_Y[i],NAJA_Z[i],pIndividualRingRot)
		 );
 }

//---------------------------------------------------------------------------------------



//   top->SetVisibility(0);
//   gGeo->CloseGeometry();

   // in GL viewer
//   top->Draw("ogl");

 return 0;
}
