void runsim(Int_t nEvents = 0)
{

  // =========== Configuration area =============================

  TString OutFile = "sim_out.root";  // Output file for data
  TString ParFile = "sim_par.root";  // Output file for params

  Bool_t fVis = true;                // Store tracks for visualization
  Bool_t fUserPList= false;          // Use of R3B special physics list
  Bool_t fR3BMagnet = true;          // Magnetic field definition
  Bool_t fCalifaHitFinder = true;    // Apply hit finder task

  TString fMC = "TGeant4";           // MonteCarlo engine: TGeant3, TGeant4, TFluka
  TString fGenerator = "box";        // Event generator type: box, gammas, r3b, ion, ascii
  TString fEventFile = "";           // Input event file in the case of ascii generator

  Int_t    fFieldMap = -1;           // Magentic field map selector
  Double_t fMeasCurrent = 2000.;     // Magnetic field current
  Int_t    fFieldScale = 1;          // Magnetic field scale factor

  // ---------------  Detector selection: true - false -------------------------------

  Bool_t  fTarget = true;            // Target
  TString fTargetType = "LiH";       // Target selection: LeadTarget, Para, Para45, LiH

  Bool_t  fVesselcool = false;       // SiTracker Cooling  
  TString fVesselcoolGeo = "vacvessel_v13a.geo.root";

  Bool_t  fAladin = false;           // Aladin Magnet
  TString fAladinGeo = "aladin_v13a.geo.root";         

  Bool_t  fGlad = false;             // Glad Magnet
  TString fGladGeo = "glad_v17_flange.geo.root";

  Bool_t  fXBall = false;            // Crystal Ball
  TString fXBallGeo = "cal_v13a.geo.root";

  Bool_t  fCalifa = true;           // Califa Calorimeter
  TString fCalifaGeo = "califa_10_v8.11.geo.root";
  Int_t   fCalifaGeoVer = 10;
  Double_t fCalifaNonU = 1.0; //Non-uniformity: 1 means +-1% max deviation   

  Bool_t  fTracker = false;          // Tracker
  TString fTrackerGeo = "tra_v13vac.geo.root";

  Bool_t  fStarTrack = false;        // STaR Tracker
  TString fStarTrackGeo = "startra_v13a.geo.root";

  Bool_t fDch = false;               // Drift Chambers
  TString fDchGeo = "dch_v13a.geo.root";
  
  Bool_t fTof = false;               // ToF Detector
  TString fTofGeo = "tof_v13a.geo.root";
  
  Bool_t fmTof = false;              // mTof Detector
  TString fmTofGeo = "mtof_v13a.geo.root";
  
  Bool_t fGfi = false;               // Fiber Detector
  TString fGfiGeo = "gfi_v13a.geo.root";
  
  Bool_t fLand = false;              // Land Detector
  TString fLandGeo = "land_v12a_10m.geo.root";
  
  Bool_t fNeuLand = false;           // NeuLand Detector 
  TString fNeuLandGeo = "neuland_v12a_14m.geo.root";
  
  Bool_t fMfi = false;              // MFI Detector 
  TString fMfiGeo = "mfi_v13a.geo.root";
  
  Bool_t fPsp = false;              // PSP Detector
  TString fPspGeo = "psp_v13a.geo.root";
  
  Bool_t fLumon = false;            // Luminosity Detector
  TString fLumonGeo = "lumon_v13a.geo.root";

  // ========= End of Configuration area =======================


  TString dir = gSystem->Getenv("VMCWORKDIR");
  TString r3bdir = dir + "/macros/";
  r3bdir.ReplaceAll("//","/");

  TString r3b_geomdir = dir + "/geometry/";
  gSystem->Setenv("GEOMPATH",r3b_geomdir.Data());
  r3b_geomdir.ReplaceAll("//","/");

  TString r3b_confdir = dir + "/gconfig/";
  gSystem->Setenv("CONFIG_DIR",r3b_confdir.Data());
  r3b_confdir.ReplaceAll("//","/");


  // ----    Debug option   -------------------------------------------------
  gDebug = 0;
  // ------------------------------------------------------------------------

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

 
  // -----   Create simulation run   ----------------------------------------
  FairRunSim* run = new FairRunSim();
  run->SetName(fMC);                  // Transport engine
  run->SetOutputFile(OutFile);        // Output file

  //  R3B Special Physics List in G4 case
  if ( (fUserPList) && (fMC.CompareTo("TGeant4") == 0) ) {
       run->SetUserConfig("g4R3bConfig.C");
       run->SetUserCuts("SetR3BCuts.C");
   }

  // -----   Create media   -------------------------------------------------
  run->SetMaterials("media_r3b.geo");       // Materials

  // -----   Create R3B geometry --------------------------------------------

  //Cave definition
  FairModule* cave= new R3BCave("CAVE");
  cave->SetGeometryFileName("r3b_cave.geo");
  run->AddModule(cave);

  //Target definition
  if (fTarget) {
    R3BModule* target= new R3BTarget(fTargetType);
    target->SetGeometryFileName("target_"+fTargetType+".geo.root");
    run->AddModule(target);
  }

  //SiTracker Cooling definition
  if (fVesselcool) {
    R3BModule* vesselcool= new R3BVacVesselCool(fTargetType);
    vesselcool->SetGeometryFileName(fVesselcoolGeo);
    run->AddModule(vesselcool);
  }

  //Aladin Magnet definition
  if (fAladin && !fGlad) {
    fFieldMap = 0;
    R3BModule* mag = new R3BMagnet("AladinMagnet");
    mag->SetGeometryFileName(fAladinGeo);
    run->AddModule(mag);
  }

  //Glad Magnet definition
  if (fGlad && !fAladin) {
    fFieldMap = 1;
    R3BModule* mag = new R3BGladMagnet("GladMagnet", fGladGeo, "GLAD Magnet");
    run->AddModule(mag);
  }

  //Crystal Ball
  if (fXBall && !fCalifa) {
    R3BDetector* xball = new R3BXBall("XBall", kTRUE);
    xball->SetGeometryFileName(fXBallGeo);
    run->AddModule(xball);
  }
  
  //CALIFA Calorimeter
  if (fCalifa && !fXBall) {
    R3BDetector* califa = new R3BCalifa("Califa", kTRUE);
    ((R3BCalifa *)califa)->SelectGeometryVersion(fCalifaGeoVer);
    ((R3BCalifa *)califa)->SetNonUniformity(fCalifaNonU);
    califa->SetGeometryFileName(fCalifaGeo);
    run->AddModule(califa);
  }
  
  // Tracker
  if (fTracker) {
    R3BDetector* tra = new R3BTra("Tracker", kTRUE);
    tra->SetGeometryFileName(fTrackerGeo);
    run->AddModule(tra);
  }
  
  // STaRTrack
  if (fStarTrack) {
    R3BDetector* tra = new R3BSTaRTra("STaRTrack", kTRUE);
    tra->SetGeometryFileName(fStarTrackGeo);
    run->AddModule(tra);
  }

  // DCH drift chambers
  if (fDch) {
    R3BDetector* dch = new R3BDch("Dch", kTRUE);
    dch->SetGeometryFileName(fDchGeo);
    run->AddModule(dch);
  }
  
  // Tof
  if (fTof) {
    R3BDetector* tof = new R3BTof("Tof", kTRUE);
    tof->SetGeometryFileName(fTofGeo);
    run->AddModule(tof);
  }
  
  // mTof
  if (fmTof) {
    R3BDetector* mTof = new R3BmTof("mTof", kTRUE);
    mTof->SetGeometryFileName(fmTofGeo);
    run->AddModule(mTof);
  }
  
  // GFI detector
  if (fGfi) {
    R3BDetector* gfi = new R3BGfi("Gfi", kTRUE);
    gfi->SetGeometryFileName(fGfiGeo);
    run->AddModule(gfi);
  }
  
  // Land Detector
  if (fLand && !fNeuLand) {
    R3BDetector* land = new R3BLand("Land", kTRUE);
    land->SetVerboseLevel(1);
    land->SetGeometryFileName(fLandGeo);
    run->AddModule(land);
  }
  
  // NeuLand Scintillator Detector
  if(fNeuLand && !fLand) {
    R3BDetector* land = new R3BLand("Land", kTRUE);
    land->SetVerboseLevel(1);
    land->SetGeometryFileName(fNeuLandGeo);
    run->AddModule(land);
  }
  
  // MFI Detector
  if(fMfi) {
    R3BDetector* mfi = new R3BMfi("Mfi", kTRUE);
    mfi->SetGeometryFileName(fMfiGeo);
    run->AddModule(mfi);
  }

  // PSP Detector
  if(fPsp) {
    R3BDetector* psp = new R3BPsp("Psp", kTRUE);
    psp->SetGeometryFileName(fPspGeo);
    run->AddModule(psp);
  }
  
  // Luminosity detector
  if (fLumon) {
    R3BDetector* lumon = new ELILuMon("LuMon", kTRUE);
    lumon->SetGeometryFileName(fLumonGeo);
    run->AddModule(lumon);
  }

  // -----   Create R3B  magnetic field ----------------------------------------
  //NB: <D.B>
  // If the Global Position of the Magnet is changed
  // the Field Map has to be transformed accordingly
  if (fFieldMap == 0) {
    R3BAladinFieldMap* magField = new R3BAladinFieldMap("AladinMaps");
    magField->SetCurrent(fMeasCurrent);
    magField->SetScale(fFieldScale);

    if ( fR3BMagnet == kTRUE ) {
      run->SetField(magField);
    } else {
      run->SetField(NULL);
    }
  } else if(fFieldMap == 1){
    R3BGladFieldMap* magField = new R3BGladFieldMap("R3BGladMap");
    magField->SetScale(fFieldScale);

    if ( fR3BMagnet == kTRUE ) {
      run->SetField(magField);
    } else {
      run->SetField(NULL);
    }
  }  //! end of field map section

  // -----   Create PrimaryGenerator   --------------------------------------

  // 1 - Create the Main API class for the Generator
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

  if (fGenerator.CompareTo("box") == 0  ) {
	  // 2- Define the BOX generator
	  Double_t pdgId=211; // pion beam
	  Double_t theta1= 30.;  // polar angle distribution
	  Double_t theta2= 160.;
	  Double_t momentum=.8; // 10 GeV/c
	  FairBoxGenerator* boxGen = new FairBoxGenerator(pdgId, 10);
	  boxGen->SetThetaRange (   theta1,   theta2);
	  boxGen->SetPRange     (momentum,momentum*2.);
	  boxGen->SetPhiRange   (0.,360.);
	  boxGen->SetXYZ(0.0,0.0,0.0);
	  // add the box generator
	  primGen->AddGenerator(boxGen);
  } 

  run->SetGenerator(primGen);


  //-------Set visualisation flag to true------------------------------------
  run->SetStoreTraj(fVis);

  FairLogger::GetLogger()->SetLogVerbosityLevel("LOW");
 
  // ----- Initialize Califa HitFinder task (from CrystalCal Level to Hit Level)
  if(fCalifaHitFinder) {
    R3BCalifaCrystalCal2Hit* califaHF = new R3BCalifaCrystalCal2Hit();
    califaHF->SetDetectionThreshold(0.000050);//50 KeV
    califaHF->SetExperimentalResolution(5.);  //5% at 1 MeV
    califaHF->SetAngularWindow(3.2,3.2);      //[0.25 around 14.3 degrees, 3.2 for the complete calorimeter]
    run->AddTask(califaHF);
  }
	
  // -----   Initialize simulation run   ------------------------------------
  run->Init();

  // -----   Runtime database   ---------------------------------------------
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open(ParFile.Data());
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();
  
  // -----   Start run   ----------------------------------------------------
  if (nEvents>0) run->Run(nEvents);

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << OutFile << endl;
  cout << "Parameter file is " << ParFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime 
       << "s" << endl << endl;
  // ------------------------------------------------------------------------

  cout << " Test passed" << endl;
  cout << " All ok " << endl;

}


