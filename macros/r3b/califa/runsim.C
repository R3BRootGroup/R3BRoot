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
  TString fGenerator = "p2p_s444";        // Event generator type: box, gammas, r3b, ion, ascii
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
  TString fStarTrackGeo = "startrack_v13a.geo.root";

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
       run->SetUserCuts("SetCuts.C");
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
    run->AddModule(new R3BTarget(fTargetType, "target_" + fTargetType + ".geo.root"));
  }

  //SiTracker Cooling definition
  if (fVesselcool) {
    run->AddModule(new R3BVacVesselCool(fTargetType, fVesselcoolGeo));
  }

  //Aladin Magnet definition
  if (fAladin && !fGlad) {
    fFieldMap = 0;
    run->AddModule(new R3BAladinMagnet(fAladinGeo));
  }

  //Glad Magnet definition
  if (fGlad && !fAladin) {
    fFieldMap = 1;
    run->AddModule(new R3BGladMagnet(fGladGeo));
  }

  //Crystal Ball
  if (fXBall && !fCalifa) {
    run->AddModule(new R3BXBall(fXBallGeo));
  }

  //CALIFA Calorimeter
  if (fCalifa && !fXBall) {
    R3BCalifa* califa = new R3BCalifa(fCalifaGeo);
    califa->SelectGeometryVersion(fCalifaGeoVer);
    califa->SetNonUniformity(fCalifaNonU);
    run->AddModule(califa);
  }

  // Tracker
  if (fTracker) {
    run->AddModule(new R3BTra(fTrackerGeo));
  }

  // STaRTrack
  if (fStarTrack) {
    run->AddModule(new R3BStartrack(fStarTrackGeo));
  }

  // DCH drift chambers
  if (fDch) {
    run->AddModule(new R3BDch(fDchGeo));
  }

  // Tof
  if (fTof) {
    run->AddModule(new R3BTof(fTofGeo));
  }

  // mTof
  if (fmTof) {
    run->AddModule(new R3BmTof(fmTofGeo));
  }

  // GFI detector
  if (fGfi) {
    run->AddModule(new R3BGfi(fGfiGeo));
  }

  // Land Detector
  if (fLand && !fNeuLand) {
    run->AddModule(new R3BLand(fLandGeo));
  }

  // NeuLand Scintillator Detector
  if(fNeuLand && !fLand) {
    run->AddModule(new R3BLand(fNeuLandGeo));
  }

  // MFI Detector
  if(fMfi) {
    run->AddModule(new R3BMfi(fMfiGeo));
  }

  // PSP Detector
  if(fPsp) {
    run->AddModule(new R3BPsp(fPspGeo));
  }

  // Luminosity detector
  if (fLumon) {
    run->AddModule(new ELILuMon(fLumonGeo));
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

 if (fGenerator.CompareTo("p2p_s444") == 0  ) {
    R3Bp2pevtGenerator* gen = new R3Bp2pevtGenerator();
    int a = 12;
    double unit = 931.494061;
    double energy = 200.;//MeV/u
    double defect_a = 0.;
    double defect_res = 8.66770703125;
    double mass = a*unit+defect_a;
    double momentum = sqrt(energy*energy + 2*energy*mass);
    //calculate beta/gamma
    double beta = momentum/(energy+mass);
    double exe = 4.444;
    double mproton = 938.2720813;
    gen->SetHeavyNucleus(a,defect_a,defect_res);
    gen->SetLightNucleus(mproton,mproton);
    gen->SetExcitation(exe);
    gen->SetInverse(false);
    gen->SetIsotropic(true);
    gen->SetMomDistrib(100.);
    gen->SetBeamEnergy(energy);
    TVector3 bet;
    bet.SetXYZ(0,0,beta);
    primGen->AddGenerator(gen);

#if 1
    // Coincident gammas
    R3BGammaGenerator *gammaGen = new R3BGammaGenerator();
    
    gammaGen->SetEnergyLevel(0, 0.);
    gammaGen->SetEnergyLevel(1, 4.444E-3);
    // gammaGen->SetEnergyLevel(2, 4E-3);

    //gammaGen->SetBranchingRatio(2, 1, 0.);
    //gammaGen->SetBranchingRatio(2, 0, 1.);
    gammaGen->SetBranchingRatio(1, 0, 1.);

    gammaGen->SetInitialLevel(1);

    //gammaGen->SetLorentzBoost(TVector3(0, 0, 0.777792));
    //gammaGen->SetLorentzBoost(TVector3(0, 0, 0.757909));
    gammaGen->SetLorentzBoost(bet);
    primGen->AddGenerator(gammaGen);
#endif
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
  //FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  //parOut->open(ParFile.Data());
  //rtdb->setOutput(parOut);
  //rtdb->saveOutput();
  //rtdb->print();

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


