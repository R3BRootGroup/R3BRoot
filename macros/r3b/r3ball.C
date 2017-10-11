//  -------------------------------------------------------------------------
//
//   ----- General Macro for R3B simulation
//
//         Author: Denis Bertini <D.Bertini@gsi.de>
//
//         Last Update: 30/06/12 (marc Labiche)
//
//         Comments:
//               - 30/06/12 Adding Vaccum Vessel and cooling for new Si tracker
//               - 17/08/09 Adding R3BModule R3BDetector
//                          for global positionning
//               - 12/08/09 Adding R3B Special Physics List
//               - 06/08/09 Adding R3B specific Event Generator
//
//
//  -------------------------------------------------------------------------
//
//   Usage inside ROOT interpreter:
//        1) root -l
//        2)[root] .L r3ball.C
//
//        3)[root] r3ball( nevt,
//                         fDetList,     // List of Detectors
//                         TargetType,    // "LeadTarget"
//                         Visualization, // kFalse or kTRUE
//                         fMC ,        // "TGeant3" or "TGeant4"
//                         fGenerator   // Generator type
//
//  -------------------------------------------------------------------------



void r3ball(Int_t nEvents = 1,
            TMap* fDetList = NULL,
            TString Target = "LeadTarget",
            Bool_t fVis = kFALSE,
            TString fMC = "TGeant3",
            TString fGenerator = "box",
            Bool_t fUserPList = kFALSE,
            Bool_t fR3BMagnet = kTRUE,
            Double_t fMeasCurrent = 2000.,
            TString OutFile = "r3bsim.root",
            TString ParFile = "r3bpar.root",
            TString InFile = "evt_gen.dat",
            Int_t randomSeed = 0)
{
  TString dir = getenv("VMCWORKDIR");
  TString r3bdir = dir + "/macros";

  TString r3b_geomdir = dir + "/geometry";
  gSystem->Setenv("GEOMPATH",r3b_geomdir.Data());

  TString r3b_confdir = dir + "gconfig";
  gSystem->Setenv("CONFIG_DIR",r3b_confdir.Data());

  // In general, the following parts need not be touched
  // ========================================================================

  // ----    Debug option   -------------------------------------------------
  gDebug = 0;
  // ------------------------------------------------------------------------

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------


  // -----   Create simulation run   ----------------------------------------
  FairRunSim* run = new FairRunSim();
  run->SetName(fMC.Data());              // Transport engine
  run->SetOutputFile(OutFile.Data());          // Output file
  FairRuntimeDb* rtdb = run->GetRuntimeDb();

  //  R3B Special Physics List in G4 case
  if ( (fUserPList  == kTRUE ) &&
      (fMC.CompareTo("TGeant4")   == 0)) {
    run->SetUserConfig("g4R3bConfig.C");
    run->SetUserCuts("SetCuts.C");
  }


  // -----   Create media   -------------------------------------------------
  run->SetMaterials("media_r3b.geo");       // Materials


  // Magnetic field map type
  Int_t fFieldMap = 0;


  // Global Transformations
  //- Two ways for a Volume Rotation are supported
  //-- 1) Global Rotation (Euler Angles definition)
  //-- This represent the composition of : first a rotation about Z axis with
  //-- angle phi, then a rotation with theta about the rotated X axis, and
  //-- finally a rotation with psi about the new Z axis.
  Double_t phi,theta,psi;

  //-- 2) Rotation in Ref. Frame of the Volume
  //-- Rotation is Using Local Ref. Frame axis angles
  Double_t thetaX,thetaY,thetaZ;

  //- Global Translation  Lab. frame.
  Double_t tx,ty,tz;


  // -----   Create R3B geometry --------------------------------------------
  //R3B Cave definition
  FairModule* cave= new R3BCave("CAVE");
  cave->SetGeometryFileName("r3b_cave.geo");
  run->AddModule(cave);

  //R3B Target definition
  if (fDetList->FindObject("TARGET") ) {
    run->AddModule(new R3BTarget(Target, ((TObjString*)fDetList->GetValue("TARGET"))->GetString()));
  }

  //R3B SiTracker Cooling definition
  if (fDetList->FindObject("VACVESSELCOOL") ) {
    run->AddModule(new R3BVacVesselCool(Target, ((TObjString*)fDetList->GetValue("VACVESSELCOOL"))->GetString().Data()));
  }

  //R3B Magnet definition
  if (fDetList->FindObject("ALADIN") ) {
    fFieldMap = 0;
    run->AddModule(new R3BAladinMagnet(((TObjString*)fDetList->GetValue("ALADIN"))->GetString()));
  }

  //R3B Magnet definition
  if (fDetList->FindObject("GLAD") ) {
    fFieldMap = 1;
    run->AddModule(new R3BGladMagnet(((TObjString*)fDetList->GetValue("GLAD"))->GetString()));
  }

  //R3B Crystal Calorimeter
  if (fDetList->FindObject("CRYSTALBALL") ) {
    R3BXBall* xball = new R3BXBall(((TObjString*)fDetList->GetValue("CRYSTALBALL"))->GetString());
    xball->SelectCollectionOption(2);
    run->AddModule(xball);
  }

  // CALIFA Calorimeter
  if (fDetList->FindObject("CALIFA") ) {
    R3BCalifa* califa = new R3BCalifa(((TObjString*)fDetList->GetValue("CALIFA"))->GetString());
    califa->SelectGeometryVersion(10);
    //Selecting the Non-uniformity of the crystals (1 means +-1% max deviation)
    califa->SetNonUniformity(1.0);
    run->AddModule(califa);
  }

  // Tracker
  if (fDetList->FindObject("TRACKER")  ) {
    R3BDetector* tra = new R3BTra(((TObjString*)fDetList->GetValue("TRACKER"))->GetString());
    tra->SetEnergyCut(1e-4);
    run->AddModule(tra);
  }

  // STaRTrack
//  if (fDetList->FindObject("STaRTrack")  ) {
//    run->AddModule(new R3BSTaRTra(((TObjString*)fDetList->GetValue("STaRTrack"))->GetString()));
  if (fDetList->FindObject("STaRTrack")  ) {
    run->AddModule(new R3BStartrack(((TObjString*)fDetList->GetValue("STaRTrack"))->GetString()));
  }

  // DCH drift chambers
  if (fDetList->FindObject("DCH") ) {
    run->AddModule(new R3BDch(((TObjString*)fDetList->GetValue("DCH"))->GetString()));
  }

  // Tof
  if (fDetList->FindObject("TOF") ) {
    run->AddModule(new R3BTof(((TObjString*)fDetList->GetValue("TOF"))->GetString()));
  }

  // mTof
  if (fDetList->FindObject("MTOF") ) {
    run->AddModule(new R3BmTof(((TObjString*)fDetList->GetValue("MTOF"))->GetString()));
  }

  // dTof
  if (fDetList->FindObject("DTOF") ) {
    run->AddModule(new R3BdTof(((TObjString*)fDetList->GetValue("DTOF"))->GetString()));
  }

  // GFI detector
  if (fDetList->FindObject("GFI") ) {
    run->AddModule(new R3BGfi(((TObjString*)fDetList->GetValue("GFI"))->GetString()));
  }

  // Land Detector
  if (fDetList->FindObject("LAND") ) {
    run->AddModule(new R3BLand(((TObjString*)fDetList->GetValue("LAND"))->GetString()));
  }

  // Neuland Detector
  if(fDetList->FindObject("NEULAND")) {
    run->AddModule(new R3BNeuland(((TObjString*)fDetList->GetValue("NEULAND"))->GetString()));
  }

  // MFI Detector
  if(fDetList->FindObject("MFI")) {
    run->AddModule(new R3BMfi(((TObjString*)fDetList->GetValue("MFI"))->GetString()));
  }

  // PSP Detector
  if(fDetList->FindObject("PSP")) {
    run->AddModule(new R3BPsp(((TObjString*)fDetList->GetValue("PSP"))->GetString()));
  }

  // Luminosity detector
  if (fDetList->FindObject("LUMON") ) {
    run->AddModule(new ELILuMon(((TObjString*)fDetList->GetValue("LUMON"))->GetString()));
  }

  // DEPRECATED: NeuLand Scintillator Detector
  if(fDetList->FindObject("SCINTNEULAND")) {
    run->AddModule(new R3BLand(((TObjString*)fDetList->GetValue("SCINTNEULAND"))->GetString()));
  }

  // -----   Create R3B  magnetic field ----------------------------------------
  Int_t typeOfMagneticField = 0;
  Float_t fieldScale = -0.5;
  Bool_t fVerbose = kFALSE;

  //NB: <D.B>
  // If the Global Position of the Magnet is changed
  // the Field Map has to be transformed accordingly
  FairField *magField = NULL;
  if (fFieldMap == 0) {
    magField = new R3BAladinFieldMap("AladinMaps");
    ((R3BAladinFieldMap*)magField)->SetCurrent(fMeasCurrent);
    ((R3BAladinFieldMap*)magField)->SetScale(fieldScale);

    if ( fR3BMagnet == kTRUE ) {
      run->SetField(magField);
    } else {
      run->SetField(NULL);
    }
  } else if(fFieldMap == 1){
    magField = new R3BGladFieldMap("R3BGladMap");
    ((R3BGladFieldMap*)magField)->SetScale(fieldScale);

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
    Int_t pdgId = 2212; // proton beam
    Double32_t theta1 = 0.;  // polar angle distribution
    Double32_t theta2 = 2.;
    Double32_t momentum = 1.3;
    FairBoxGenerator* boxGen = new FairBoxGenerator(pdgId, 3);
    boxGen->SetThetaRange(theta1, theta2);
    boxGen->SetPRange(momentum, momentum*1.2);
    boxGen->SetPhiRange(0, 360);
    boxGen->SetXYZ(0.0, 0.0, -1.5);
//    boxGen->SetXYZ(0.0, 0.0, -300.);
    // add the box generator
    primGen->AddGenerator(boxGen);

    // 128-Sn fragment
    R3BIonGenerator* ionGen = new R3BIonGenerator(50, 128, 50, 10, 0., 0., 1.3);
    ionGen->SetSpotRadius(0.1, -100., 0.);
    primGen->AddGenerator(ionGen);

    // neutrons
    FairBoxGenerator* boxGen_n = new FairBoxGenerator(2112, 3);
    boxGen_n->SetThetaRange(theta1, theta2);
    boxGen_n->SetPRange(momentum, momentum*1.2);
    boxGen_n->SetPhiRange(0, 360);
    boxGen_n->SetXYZ(0.0, 0.0, -1.5);
    primGen->AddGenerator(boxGen_n);
  }

  if (fGenerator.CompareTo("ascii") == 0  ) {
    R3BAsciiGenerator* gen = new R3BAsciiGenerator((dir+"/input/"+InFile).Data());
    primGen->AddGenerator(gen);
  }

  if (fGenerator.CompareTo("r3b") == 0  ) {
    Int_t pdg = 2212;
    Float_t beamEnergy = 1.;
    R3BSpecificGenerator *pR3bGen = new R3BSpecificGenerator(pdg, beamEnergy);

    // R3bGen properties
    pR3bGen->SetBeamInteractionFlag("off");
    pR3bGen->SetBeamInteractionFlag("off");
    pR3bGen->SetRndmFlag("off");
    pR3bGen->SetRndmEneFlag("off");
    pR3bGen->SetBoostFlag("off");
    pR3bGen->SetReactionFlag("on");
    pR3bGen->SetGammasFlag("off");
    pR3bGen->SetDecaySchemeFlag("off");
    pR3bGen->SetDissociationFlag("off");
    pR3bGen->SetBackTrackingFlag("off");
    pR3bGen->SetSimEmittanceFlag("off");

    // R3bGen Parameters
    pR3bGen->SetSigmaBeamEnergy(1.e-03); // Sigma(Ebeam) GeV
    pR3bGen->SetEnergyPrim(0.3); // Particle Energy in MeV
    Int_t fMultiplicity = 50;
    pR3bGen->SetNumberOfParticles(fMultiplicity); // Mult.

    // Reaction type
    //        1: "Elas"
    //        2: "iso"
    //        3: "Trans"
    pR3bGen->SetReactionType("Elas");

    // Target  type
    //        1: "LeadTarget"
    //        2: "Parafin0Deg"
    //        3: "Parafin45Deg"
    //        4: "LiH"

    pR3bGen->SetTargetType(Target.Data());
    Double_t thickness = (0.11/2.)/10.;  // cm
    pR3bGen->SetTargetHalfThicknessPara(thickness); // cm
    pR3bGen->SetTargetThicknessLiH(3.5); // cm
    pR3bGen->SetTargetRadius(1.); // cm

    pR3bGen->SetSigmaXInEmittance(1.); //cm
    pR3bGen->SetSigmaXPrimeInEmittance(0.0001); //cm

    // Dump the User settings
    pR3bGen->PrintParameters();
    primGen->AddGenerator(pR3bGen);
  }

  run->SetGenerator(primGen);


  //-------Set visualisation flag to true------------------------------------
  run->SetStoreTraj(fVis);


  FairLogger::GetLogger()->SetLogVerbosityLevel("LOW");
  FairLogger::GetLogger()->SetLogScreenLevel("INFO");


  // -----   Initialize simulation run   ------------------------------------
  run->Init();
  TVirtualMC::GetMC()->SetRandom(new TRandom3(randomSeed));

  // ------  Increase nb of step for CALO
  Int_t nSteps = -15000;
  TVirtualMC::GetMC()->SetMaxNStep(nSteps);


  // -----   Runtime database   ---------------------------------------------
  R3BFieldPar* fieldPar = (R3BFieldPar*) rtdb->getContainer("R3BFieldPar");
  if(NULL != magField)
  {
      fieldPar->SetParameters(magField);
      fieldPar->setChanged();
  }
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open(ParFile.Data());
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();


  // -----   Start run   ----------------------------------------------------
  if(nEvents > 0) {
    run->Run(nEvents);
  }


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
