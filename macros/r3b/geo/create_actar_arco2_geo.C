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
Bool_t fLabTrans = kFALSE;

TGeoCombiTrans* GetGlobalPosition(TGeoCombiTrans *fRef);


void create_actar_geo(const char* geoTag)
{

  fGlobalTrans->SetTranslation(0.0,0.0,0.0);

  //Design values for the R3B ACTAR detector (from E. Maev info, maev@pnpi.spb.ru)
  Double_t worldLength = 60.;  //60cm^3 world box side
  Double_t chamberInnerRadius = 20.0; //cylindrical inner radius (cm)
  Double_t chamberThickness = 0.6;  //chamber aluminum thickness (cm)
  Double_t chamberPlateRadius = 23.5; //back stainless steel part (cm)
  Double_t chamberPlateThickness = 2.0;  //back part thickness (cm)
  Double_t chamberHalfLength = 23.5; //half-length APPROX. (cm)
  Double_t backwardsBeamWindowRadius = 2.5; // inlet radius(cm)
  Double_t forwardsBeamWindowRadius = 3.5; //outlet radius (cm)
  Double_t inletWindowRadius = 2.45; // inlet berylium window radius (cm)
  Double_t inletWindowThickness = 0.05; //inlet berylium window thickness (cm)
  Double_t inletWindowCenter = -chamberHalfLength-chamberPlateThickness-1.0-inletWindowRadius-inletWindowThickness; // Z coordinate of inlet window center (cm)
  Double_t outletWindowRadius = 3.45; // inlet berylium window radius (cm)
  Double_t outletWindowThickness = 0.05; //inlet berylium window thickness (cm)
  Double_t outletWindowCenter = chamberHalfLength+outletWindowRadius+outletWindowThickness; // Z coordinate of inlet window center (cm)

  Double_t anodeThickness = 0.10;  //(cm)
  Double_t cathodeThickness = 0.10;  //(cm)
  Double_t anodeRadius = 32.4/2; //back stainless steel part (cm)
  Double_t positionAnode = -10.0; //anode center Z position (cm)
  Double_t positionCathode = positionAnode + 21.6; //anode center Z position (cm)
  Double_t positionGrid = positionAnode + 0.3; //grid center Z position (cm)
  Double_t gridThickness = 0.001; //55 microm, 1mm step, RECALCULATE!!! (cm)
  Double_t ringRadius = 34.0/2; //field mapping rings radius (cm)
  Double_t ringGauge = 0.1; //field mapping rings gauge (diameter) (cm)
  Double_t ringDistance = 1.8; //distance between rings (cm)


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
  TString geoFileName = geoPath + "/geometry/actar_";
  geoFileName = geoFileName + geoTag + ".geo.root";
  // --------------------------------------------------------------------------



  // -----------------   Get and create the required media    -----------------
  FairGeoMedia*   geoMedia = geoFace->getMedia();
  FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

  FairGeoMedium* mAir      = geoMedia->getMedium("Air");
  if ( ! mAir ) Fatal("Main", "FairMedium Air not found");
  geoBuild->createMedium(mAir);
  TGeoMedium* pAirMedium = gGeoMan->GetMedium("Air");
  if ( ! pAirMedium ) Fatal("Main", "Medium Air not found");

  FairGeoMedium* mCar      = geoMedia->getMedium("CarbonFibre");
  if ( ! mCar ) Fatal("Main", "FairMedium CarbonFibre not found");
  geoBuild->createMedium(mCar);
  TGeoMedium* pCarbonFibreMedium = gGeoMan->GetMedium("CarbonFibre");
  if ( ! pCarbonFibreMedium ) Fatal("Main", "Medium CarbonFibre not found");

  FairGeoMedium* mTfl      = geoMedia->getMedium("Tefflon");
  if ( ! mTfl ) Fatal("Main", "FairMedium Tefflon not found");
  geoBuild->createMedium(mTfl);
  TGeoMedium* pWrappingMedium = gGeoMan->GetMedium("Tefflon");
  if ( ! pWrappingMedium ) Fatal("Main", "Medium Tefflon not found");

  FairGeoMedium* mAl      = geoMedia->getMedium("aluminium");
  if ( ! mAl ) Fatal("Main", "FairMedium aluminium not found");
  geoBuild->createMedium(mAl);
  TGeoMedium* pAlHousing = gGeoMan->GetMedium("aluminium");
  if ( ! pAlHousing ) Fatal("Main", "Medium aluminium not found");

  FairGeoMedium* mSteel      = geoMedia->getMedium("Steel");
  if ( ! mSteel ) Fatal("Main", "FairMedium Steel not found");
  geoBuild->createMedium(mSteel);
  TGeoMedium* pMedSteel = gGeoMan->GetMedium("Steel");
  if ( ! pMedSteel ) Fatal("Main", "Medium Steel not found");

  FairGeoMedium* mHe      = geoMedia->getMedium("ArCO2");
  if ( ! mHe ) Fatal("Main", "FairMedium ArCO2 not found");
  geoBuild->createMedium(mHe);
  TGeoMedium* pMed4 = gGeoMan->GetMedium("ArCO2");
  if ( ! pMed4 ) Fatal("Main", "Medium ArCO2 not found");

  FairGeoMedium* mBeryllium = geoMedia->getMedium("beryllium");
  if ( ! mBeryllium ) Fatal("Main", "FairMedium beryllium not found");
  geoBuild->createMedium(mBeryllium);
  TGeoMedium* pMedBe = gGeoMan->GetMedium("beryllium");
  if ( ! pMedBe ) Fatal("Main", "Medium beryllium not found");

  FairGeoMedium* mCopper = geoMedia->getMedium("copper");
  if ( ! mCopper ) Fatal("Main", "FairMedium copper not found");
  geoBuild->createMedium(mCopper);
  TGeoMedium* pMedCu = gGeoMan->GetMedium("copper");
  if ( ! pMedCu ) Fatal("Main", "Medium copper not found");
  // --------------------------------------------------------------------------



  // --------------   Create geometry and top volume  -------------------------
  gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoMan->SetName("ACTARgeom");
  TGeoVolume* top = new TGeoVolumeAssembly("TOP");
  gGeoMan->SetTopVolume(top);
  // --------------------------------------------------------------------------


  // Defintion of the Mother Volume Actar
  TGeoShape *pActarWorldOut = new TGeoBBox("Actar_boxOut",
					worldLength/2.0,
					worldLength/2.0,
					worldLength/2.0);

  TGeoVolume* pWorld = new TGeoVolume("ActarWorld", pActarWorldOut, pAirMedium);

  TGeoCombiTrans *t0 = new TGeoCombiTrans();
  TGeoCombiTrans *pGlobalc = GetGlobalPosition(t0);
  t0->RegisterYourself();

  //defining top volume
  top->AddNode(pWorld, 0, pGlobalc);

  //Colors in visualization
  Color_t col_chamber = kBlue-6;
  Color_t col_anode = kGray+3;
  Color_t col_cathode = kGray+5;
  Color_t col_grid = kGray;
  Color_t col_rings = kOrange;
  Color_t col_windows = kGray-1;

  /*** Some definitions ***/
  TGeoRotation *rotUni = new TGeoRotation();          //unitary rotation

  //CHAMBER, BACKWARD FLANGE
  TGeoVolume *Actar_chamber_1 = gGeoManager->MakeTube("Actar_chamber_1",pAlHousing,chamberInnerRadius, chamberInnerRadius+chamberThickness, chamberHalfLength);
  Actar_chamber_1->SetLineColor(col_chamber);

  TGeoVolume *Actar_chamber_2 = gGeoManager->MakeTube("Actar_chamber_2",pMedSteel,backwardsBeamWindowRadius, chamberPlateRadius, chamberPlateThickness/2);
  Actar_chamber_2->SetLineColor(col_chamber);
  TGeoCombiTrans *t_chamber_2 = new TGeoCombiTrans("t_chamber_2",0.,0.,-chamberHalfLength-chamberPlateThickness/2,rotUni);
  t_chamber_2->RegisterYourself();

  TGeoVolume *Actar_chamber_3 = gGeoManager->MakeTube("Actar_chamber_3",pAlHousing,forwardsBeamWindowRadius, chamberInnerRadius+chamberThickness, chamberThickness/2);
  Actar_chamber_3->SetLineColor(col_chamber);
  TGeoCombiTrans *t_chamber_3 = new TGeoCombiTrans("t_chamber_3",0.,0.,chamberHalfLength+chamberThickness/2,rotUni);
  t_chamber_3->RegisterYourself();

  TGeoVolume *Actar_chamber_4 = gGeoManager->MakeTube("Actar_chamber_4",pMedSteel,backwardsBeamWindowRadius, 5.0,3.5/2);
  Actar_chamber_3->SetLineColor(col_chamber);
  TGeoCombiTrans *t_chamber_4 = new TGeoCombiTrans("t_chamber_4",0.,0.,inletWindowCenter+3.5/2,rotUni);
  t_chamber_4->RegisterYourself();

  TGeoVolume *Actar_chamber_5 = gGeoManager->MakeTube("Actar_chamber_5",pMedSteel,forwardsBeamWindowRadius, 6.0,3.5/2);
  Actar_chamber_3->SetLineColor(col_chamber);
  TGeoCombiTrans *t_chamber_5 = new TGeoCombiTrans("t_chamber_5",0.,0.,outletWindowCenter-3.5/2+chamberThickness,rotUni);
  t_chamber_5->RegisterYourself();

  //BERYLIUM WINDOWS
  TGeoVolume *Actar_BeInletWindow = gGeoManager->MakeSphere("Actar_BeInletWindow",pMedBe,inletWindowRadius,inletWindowRadius+inletWindowThickness, 0.,90.,0.,360.);
  Actar_BeInletWindow->SetLineColor(col_windows);

  TGeoVolume *Actar_BeOutletWindow = gGeoManager->MakeSphere("Actar_BeOutletWindow",pMedBe,outletWindowRadius,outletWindowRadius+outletWindowThickness, 90.,180.,0.,360.);
  Actar_BeOutletWindow->SetLineColor(col_windows);

  TGeoVolume *Actar_chamber_inner = gGeoManager->MakeTube("Actar_chamber_inner",pMed4,0, chamberInnerRadius, chamberHalfLength);

  //ANODE, CATHODE, GRID, RINGS
  TGeoVolume *Actar_anode = gGeoManager->MakeTube("Actar_anode",pMedSteel, 0., anodeRadius, anodeThickness/2);
  Actar_anode->SetLineColor(col_anode);

  TGeoVolume *Actar_cathode = gGeoManager->MakeTube("Actar_cathode",pMedSteel, 0., anodeRadius, cathodeThickness/2);
  Actar_cathode->SetLineColor(col_cathode);

  TGeoVolume *Actar_grid = gGeoManager->MakeTube("Actar_grid",pMedSteel, 0., anodeRadius, gridThickness/2);
  Actar_grid->SetLineColor(col_grid);

  TGeoVolume *Actar_rings = gGeoManager->MakeTorus("Actar_rings",pMedCu, ringRadius, 0., ringGauge/2, 0, 360);
  Actar_rings->SetLineColor(col_rings);

  for(Int_t ringNumber=0;ringNumber<11;ringNumber++)
    Actar_chamber_inner->AddNode(Actar_rings,ringNumber,new TGeoCombiTrans(0.,0., positionAnode+ ringDistance*(ringNumber+1),rotUni));

  Actar_chamber_inner->AddNode(Actar_anode,0,new TGeoCombiTrans(0.,0.,positionAnode,rotUni));
  Actar_chamber_inner->AddNode(Actar_grid,0,new TGeoCombiTrans(0.,0.,positionGrid,rotUni));
  Actar_chamber_inner->AddNode(Actar_cathode,0,new TGeoCombiTrans(0.,0.,positionCathode,rotUni));

  pWorld->AddNode(Actar_chamber_1,0,t0);
  pWorld->AddNode(Actar_chamber_2,0,t_chamber_2);
  pWorld->AddNode(Actar_chamber_3,0,t_chamber_3);
  pWorld->AddNode(Actar_chamber_4,0,t_chamber_4);
  pWorld->AddNode(Actar_chamber_5,0,t_chamber_5);
  pWorld->AddNode(Actar_BeInletWindow,0,new TGeoCombiTrans(0.,0.,inletWindowCenter,rotUni));
  pWorld->AddNode(Actar_BeOutletWindow,0,new TGeoCombiTrans(0.,0.,outletWindowCenter,rotUni));
  pWorld->AddNode(Actar_chamber_inner,0,t0);


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
