void create_sfi_geo(const char* geoTag)
{

  //fGlobalTrans->SetTranslation(0.0,0.0,0.0);

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
  TString geoFileName = geoPath + "/geometry/sfi_";
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

  FairGeoMedium* mVac      = geoMedia->getMedium("vacuum");
  if ( ! mVac ) Fatal("Main", "FairMedium vacuum not found");
  geoBuild->createMedium(mVac);
  TGeoMedium* pMed1 = gGeoMan->GetMedium("vacuum");
  if ( ! pMed1 ) Fatal("Main", "Medium vacuum not found");
  
  FairGeoMedium* mGfi      = geoMedia->getMedium("plasticForGFI");
  if ( ! mGfi ) Fatal("Main", "FairMedium plasticForGFI not found");
  geoBuild->createMedium(mGfi);
  TGeoMedium* pMed35 = gGeoMan->GetMedium("plasticForGFI");
  if ( ! pMed35 ) Fatal("Main", "Medium plasticForGFI not found");

  FairGeoMedium* mAl      = geoMedia->getMedium("aluminium");
  if ( ! mAl ) Fatal("Main", "FairMedium aluminium not found");
  geoBuild->createMedium(mAl);
  TGeoMedium* pMed21 = gGeoMan->GetMedium("aluminium");
  if ( ! pMed21 ) Fatal("Main", "Medium aluminium not found");
  // --------------------------------------------------------------------------



  // --------------   Create geometry and top volume  -------------------------
  gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoMan->SetName("GFIgeom");
  TGeoVolume* top = new TGeoVolumeAssembly("TOP");
  gGeoMan->SetTopVolume(top);
  // --------------------------------------------------------------------------
  
  //LABPOS(GFI1,-73.274339,0.069976,513.649524)
  Float_t dx = -73.274339; //dE tracker, correction due to wrong angle
  Float_t dy = 0.069976;
  Float_t dz = 513.649524;
  
  TGeoRotation *pMatrix3 = new TGeoRotation();
  //pMatrix3->RotateY(-16.7);
  TGeoCombiTrans*
  pMatrix2 = new TGeoCombiTrans("", dx,dy,dz,pMatrix3);

  //LABPOS(GFI2,-147.135037,0.069976,729.680342)
  dx = -147.135037; //dE tracker, correction due to wrong angle
  dy = 0.069976;
  dz = 729.680342;

  TGeoRotation *pMatrix5 = new TGeoRotation();
  //pMatrix5->RotateY(-16.7);
  TGeoCombiTrans*
  pMatrix4 = new TGeoCombiTrans("", dx,dy,dz,pMatrix5);
  
  
  // World definition
  TGeoVolume* pWorld = gGeoManager->GetTopVolume();
  pWorld->SetVisLeaves(kTRUE);
  
  // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY 
  
  // Volume: GFILogWorld
  
  TGeoVolume*   pGFILogWorld = new TGeoVolumeAssembly("GFILogWorld");
  pGFILogWorld->SetVisLeaves(kTRUE);
  
  // Global positioning
  pWorld->AddNode( pGFILogWorld, 0, pMatrix2 );
  
  Float_t detector_size = 5.120000;
  Float_t fiber_thickness = 0.020000; 
  
  TGeoShape *pGFITube = new TGeoBBox("GFITube", fiber_thickness/2,detector_size/2,fiber_thickness/2);
  
  TGeoVolume* pGFILog = new TGeoVolume("SFILog",pGFITube, pMed35);

  TGeoShape *pGFITubeActive = new TGeoBBox("GFITubeActive", (fiber_thickness * .98)/2, detector_size/2-0.0001, (fiber_thickness * .98)/2);

  TGeoVolume* pGFILogActive = new TGeoVolume("SFI1Log",pGFITubeActive,pMed35);
  
  pGFILog->SetLineColor((Color_t) 1);
  pGFILog->SetVisLeaves(kTRUE);
  
  TGeoRotation *pMatrixTube = new TGeoRotation();
  pMatrixTube->RotateZ(90);
  
  pGFILog -> AddNode(pGFILogActive, 0, new TGeoCombiTrans());
  
  for(int fiber_id = 0; fiber_id < detector_size / fiber_thickness; fiber_id++)
  {
     pGFILogWorld->AddNode(pGFILog, fiber_id, 
		new TGeoCombiTrans("", 
			-detector_size / 2 + (fiber_id + .5) * fiber_thickness, 
			0,
			0,
			new TGeoRotation()
		)
	);
      pGFILogWorld->AddNode(pGFILog, fiber_id + detector_size / fiber_thickness, 
		new TGeoCombiTrans("", 
			0,
			-detector_size / 2 + (fiber_id + .5) * fiber_thickness,
			fiber_thickness,
			pMatrixTube
		)
	);
  }
  
  // Add the sensitive part
//  AddSensitiveVolume(pGFILog);
//  fNbOfSensitiveVol+=1;
  
  
  
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
