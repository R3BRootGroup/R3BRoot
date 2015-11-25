#include <iomanip>
#include <iostream>
#include "TGeoManager.h"
#include "TMath.h"

// Create Matrix Unity
TGeoRotation *fGlobalRot = new TGeoRotation();

// Create a null translation
TGeoTranslation *fGlobalTrans = new TGeoTranslation();
TGeoRotation *fRefRot = NULL;

TGeoManager*  gGeoMan = NULL;

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

void create_dch_geo(const char* geoTag)
{

  fGlobalTrans->SetTranslation(0.0,0.0,0.0);

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
  TString geoFileName = geoPath + "/geometry/dch_";
  geoFileName = geoFileName + geoTag + ".geo.root";
  // --------------------------------------------------------------------------



  // -----------------   Get and create the required media    -----------------
  FairGeoMedia*   geoMedia = geoFace->getMedia();
  FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

  FairGeoMedium* mAir      = geoMedia->getMedium("Air");
  if ( ! mAir ) Fatal("Main", "FairMedium Air not found");
  geoBuild->createMedium(mAir);
  TGeoMedium* pMed2 = gGeoMan->GetMedium("Air");
  if ( ! pMed2 ) Fatal("Main", "Medium Air not found");

  FairGeoMedium* mAl      = geoMedia->getMedium("aluminium");
  if ( ! mAl ) Fatal("Main", "FairMedium aluminium not found");
  geoBuild->createMedium(mAl);
  TGeoMedium* pMed21 = gGeoMan->GetMedium("aluminium");
  if ( ! pMed21 ) Fatal("Main", "Medium aluminium not found");

  FairGeoMedium* mMylar      = geoMedia->getMedium("mylar");
  if ( ! mMylar ) Fatal("Main", "FairMedium mylar not found");
  geoBuild->createMedium(mMylar);
  TGeoMedium* pMed15 = gGeoMan->GetMedium("mylar");
  if ( ! pMed15 ) Fatal("Main", "Medium mylar not found");

  FairGeoMedium* mHe      = geoMedia->getMedium("helium");
  if ( ! mHe ) Fatal("Main", "FairMedium helium not found");
  geoBuild->createMedium(mHe);
  TGeoMedium* pMed4 = gGeoMan->GetMedium("helium");
  if ( ! pMed4 ) Fatal("Main", "Medium helium not found");

  FairGeoMedium* mDCH      = geoMedia->getMedium("DCHgas");
  if ( ! mDCH ) Fatal("Main", "FairMedium DCHgas not found");
  geoBuild->createMedium(mDCH);
  TGeoMedium* pMed33 = gGeoMan->GetMedium("DCHgas");
  if ( ! pMed33 ) Fatal("Main", "Medium DCHgas not found");
  // --------------------------------------------------------------------------



  // --------------   Create geometry and top volume  -------------------------
  gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoMan->SetName("DCHgeom");
  TGeoVolume* top = new TGeoVolumeAssembly("TOP");
  gGeoMan->SetTopVolume(top);
  // --------------------------------------------------------------------------



  Double_t dx,dy,dz;
  Double_t theta, phi;
  Double_t thx, phx, thy, phy, thz, phz;
  Double_t alpha;



  // Define DCH Geometry
  TGeoVolume* topDCH = new TGeoVolumeAssembly("DCH");


  //Active volume	(envelope box enclosing the sense wires)	
  Double_t actGasDx = 99.07/2.; // [cm]	//99.07326
  Double_t actGasDy = 77.60/2.;  // [cm]	//77.59584
  Double_t actGasDz = 5.20/2.;     // [cm]
  //distance of x and y planes centres is 4 cm. Distance between the first x halfplane and the second y halfplane is 4cm + 2*0.69*cos30 cm.

  TGeoShape* pActGasBox = new TGeoBBox("ActGasBox",
				       actGasDx,
				       actGasDy,
				       actGasDz);
  TGeoVolume*
    pActGasDchLog = new TGeoVolume("ActGASBoxLog",pActGasBox, pMed33);
  pActGasDchLog->SetVisLeaves(kTRUE);
  pActGasDchLog->SetVisContainers(kTRUE);


  // Gas box
  Double_t gasDx = 106.4/2.; // [cm]
  Double_t gasDy = 83.4/2.;  // [cm]
  Double_t gasDz = 7.50;     // [cm]	

  TGeoShape* pGasBox = new TGeoBBox("GasBox",
				    gasDx,
				    gasDy,
				    gasDz);
  TGeoVolume*
    pGasDchLog = new TGeoVolume("GASBoxLog",pGasBox, pMed33);
  pGasDchLog->SetVisLeaves(kTRUE);
  pGasDchLog->SetVisContainers(kTRUE);


  // Al Frame
  Double_t alDx = 125.8/2.; // [cm]
  Double_t alDy = 103.4/2.; // [cm]
  //Double_t alDz = 4.06;     // [cm]
  Double_t alDz = 7.50;     // [cm]	//modified

  TGeoShape* pAlBox = new TGeoBBox("AlBox",
				   alDx,
				   alDy,
				   alDz);

  TGeoVolume*
    pAlDchLog = new TGeoVolume("ALBoxLog",pAlBox, pMed21);
  pAlDchLog->SetVisLeaves(kTRUE);



  // Mylar Entrance exit windows
  Double_t mylDx= gasDx; //[cm]
  Double_t mylDy= gasDy; //[cm]
  Double_t mylDz= 0.0006; //[cm]
  TGeoShape* pMylarBox = new TGeoBBox("MylarBox",
				      mylDx,
				      mylDy,
				      mylDz);

  TGeoVolume*
    pMylDchLog = new TGeoVolume("MYLBoxLog",pMylarBox, pMed15);
  pMylDchLog->SetVisLeaves(kTRUE);



  // First assembly
  TGeoVolume *dch1 = new TGeoVolumeAssembly("DCH1");

  TGeoRotation *rot = new TGeoRotation();
  rot->RotateX(0.);
  rot->RotateY(0.);
  rot->RotateZ(0.);
  //Double_t tx = -3.5;	//correct values? active-area vs Al frame offsets?
  //Double_t ty = -5.;
  Double_t tx = +3.5;	//corrected values, active-area vs Al frame offsets. F.Wamers.
  Double_t ty = -3.5;
  Double_t tz = 0.;

  TGeoCombiTrans*
    pTransfo1 = new TGeoCombiTrans("", 0.,0.,0.,rot);
  TGeoCombiTrans*
    pTransfo2 = new TGeoCombiTrans("", tx,ty,tz,rot);
  TGeoCombiTrans*
    pTransfo3 = new TGeoCombiTrans("", -tx,-ty,-tz,rot); //new, in order to compensate Al vs Gas

  dch1->AddNode(pAlDchLog,0,pTransfo3);	//1->3
  pAlDchLog->AddNode(pGasDchLog,0,pTransfo2);	//place gas in aluminum, shifted
  pGasDchLog->AddNode(pActGasDchLog, 0,pTransfo1);	//place active in gas, centrally
  // Mylar Windows front+back
  //dch1->AddNode(pMylDchLog,0,new TGeoCombiTrans("", tx,ty,-alDz-mylDz,rot));
  //dch1->AddNode(pMylDchLog,1,new TGeoCombiTrans("", tx,ty, alDz+mylDz,rot));
  dch1->AddNode(pMylDchLog,0,new TGeoCombiTrans("", 0.,0.,-alDz-mylDz,rot));
  dch1->AddNode(pMylDchLog,1,new TGeoCombiTrans("", 0.,0., alDz+mylDz,rot));



  // Global Positioning
  //in agreement with the s318 tracker, those are supposed to be the centres of the active volumes!!!
  //Double_t pDch1x = -123.22 ; //Justyna 
  //Double_t pDch1y = 3.6 ;     //Justyna
  //Double_t pDch1z = 444.13 ;  //Justyna

  //Double_t pDch2x = -167.0 ;  //Justyna
  //Double_t pDch2y = 1.02 ;    //Justyna
  //Double_t pDch2z = 535.1 ;   //Justyna

  //use identical values as for the tracker config and the digitizer. F. Wamers. 
  Double_t pDch1x = -123.219446 ; //Felix 
  Double_t pDch1y = 3.597104 ;     //Felix
  Double_t pDch1z = 444.126271 ;  //Felix

  Double_t pDch2x = -167.015888 ;  //Felix
  Double_t pDch2y = 1.016917 ;    //Felix
  Double_t pDch2z = 535.093884 ;   //Felix
   
  //   Double_t pDch1x = -132.233355 ; //Christoph  
  //   Double_t pDch1y = 1.037475 ;     //Christoph 
  //   Double_t pDch1z = 438.710168 ;  //Christoph

  //   Double_t pDch2x = -170.8653 ;  //Christoph
  //   Double_t pDch2y = 2.075139 ;    //Christoph
  //   Double_t pDch2z = 538.614091 ;   //Christoph
   

  //The order of rotation matters!!! Rotate first z, and then y! Felix

  Double_t aDch1 = -31.0 ; 
  Double_t aDch2 = -31.0 ; 

  TGeoRotation *gRot1 = new TGeoRotation();
  gRot1->RotateX(0.);
  gRot1->RotateZ(-8.880000); //Justyna
  //gRot1->RotateZ(+8.880000); //Felix
  //   gRot1->RotateZ(-2.5); //Christoph
  gRot1->RotateY(aDch1);
   
  TGeoRotation *gRot2 = new TGeoRotation();
  gRot2->RotateX(0.);
  gRot2->RotateZ(9.350000); //Justyna
  //gRot2->RotateZ(-9.350000); //Felix
  //   gRot2->RotateZ(8.4); //Christoph
  gRot2->RotateY(aDch2);


  // Helium Bag definition
  Double_t heDx= alDx ; //[cm]
  Double_t heDy= alDy ; //[cm]
  Double_t heDz=(pDch2z-pDch1z)*0.953874/2.; //[cm]
  alpha=0.;     //[degre]
  Double_t beta =0.;     //[degre]
  phi  =15.20; //[degre]

  TGeoShape* pHePara = new TGeoPara("HePara", heDx, heDy, heDz,
				    alpha,beta,phi);

  TGeoVolume*
    pHeDchLog = new TGeoVolume("HeParaLog",pHePara, pMed4);
  pHeDchLog->SetVisLeaves(kTRUE);




  topDCH->AddNode(dch1,0,new TGeoCombiTrans("",pDch1x,pDch1y,pDch1z,gRot1) );
  topDCH->AddNode(dch1,1,new TGeoCombiTrans("",pDch2x,pDch2y,pDch2z,gRot2) );
  topDCH->AddNode(pHeDchLog,0,new TGeoCombiTrans("",(pDch1x+pDch2x)/2.,
						 pDch2y,
						 (pDch1z+pDch2z)/2.,
						 gRot1) );


  TGeoCombiTrans *temp1 = new TGeoCombiTrans();
  top->AddNode(topDCH, 0, GetGlobalPosition(temp1));
  
   
   
  // ---------------   Finish   -----------------------------------------------
  gGeoMan->CloseGeometry();
  gGeoMan->CheckOverlaps(0.001);
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

