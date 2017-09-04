//  -------------------------------------------------------------------------
//
//   ----- General Macro for R3B simulation
//
//         Author: Hector Alvarez <hector.alvarez@usc.es>
//
//         Last Update: 09/09/2013
//
//         Comments:
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
//                         fCalifaHitFinder // Activate CalifaHitFinder task
//                                          // CrystalCalData level to HitData level
//
//  -------------------------------------------------------------------------



void r3ball(Int_t nEvents = 1,
	    TMap& fDetList,
            TString Target = "LeadTarget",
            Bool_t fVis = kFALSE,
            TString fMC = "TGeant3",
            TString fGenerator = "box",
            Bool_t fUserPList = kFALSE,
            Bool_t fR3BMagnet = kTRUE,
            Bool_t fCalifaHitFinder = kFALSE,
            Bool_t fStarTrackHitFinder = kFALSE,
            Double_t fMeasCurrent = 2000.,
            TString OutFile = "r3bsim.root",
            TString ParFile = "r3bpar.root",
            TString InFile = "evt_gen.dat",
            double energy1, double energy2)
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
  
  FairLogger::GetLogger()->SetLogScreenLevel("DEBUG");
  
  //  R3B Special Physics List in G4 case
  if ( (fUserPList  == kTRUE ) &&
       (fMC.CompareTo("TGeant4")   == 0)
      ){
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
  if (fDetList.FindObject("TARGET") ) {
    R3BModule* target= new R3BTarget(Target.Data());
    target->SetGeometryFileName(((TObjString*)fDetList.GetValue("TARGET"))->GetString().Data());
    run->AddModule(target);
  }
  
  //R3B SiTracker Cooling definition
  if (fDetList.FindObject("VACVESSELCOOL") ) {
    R3BModule* vesselcool= new R3BVacVesselCool(Target.Data());
    vesselcool->SetGeometryFileName(((TObjString*)fDetList.GetValue("VACVESSELCOOL"))->GetString().Data());
    run->AddModule(vesselcool);
  }
  //R3B Magnet definition
  if (fDetList.FindObject("ALADIN") ) {
    fFieldMap = 0;
    R3BModule* mag = new R3BMagnet("AladinMagnet");
    mag->SetGeometryFileName(((TObjString*)fDetList.GetValue("ALADIN"))->GetString().Data());
    run->AddModule(mag);
  }
  
  //R3B Magnet definition
  if (fDetList.FindObject("GLAD") ) {
    fFieldMap = 1;
    R3BModule* mag = new R3BGladMagnet("GladMagnet", ((TObjString*)fDetList->GetValue("GLAD"))->GetString(), "GLAD Magnet");
    run->AddModule(mag);
  }
  
  if (fDetList.FindObject("CRYSTALBALL") ) {
    //R3B Crystal Calorimeter
    R3BDetector* xball = new R3BXBall("XBall", kTRUE);
    xball->SetGeometryFileName(((TObjString*)fDetList.GetValue("CRYSTALBALL"))->GetString().Data());
    run->AddModule(xball);
  }
  
  if (fDetList.FindObject("CALIFA") ) {
    // CALIFA Calorimeter
    R3BDetector* califa = new R3BCalifa("Califa", kTRUE);
//    ((R3BCalifa *)califa)->SelectGeometryVersion(0x438b);
    ((R3BCalifa *)califa)->SelectGeometryVersion(17);
    //Selecting the Non-uniformity of the crystals (1 means +-1% max deviation)
    ((R3BCalifa *)califa)->SetNonUniformity(.0);
    califa->SetGeometryFileName(((TObjString*)fDetList.GetValue("CALIFA"))->GetString().Data());
    run->AddModule(califa);
  }

  // Tracker
  if (fDetList.FindObject("TRACKER")  ) {
    R3BDetector* tra = new R3BTra("Tracker", kTRUE);
    tra->SetGeometryFileName(((TObjString*)fDetList.GetValue("TRACKER"))->GetString().Data());
    run->AddModule(tra);
  }
  
  // STaRTrack
  if (fDetList.FindObject("STaRTrack")  ) {
    R3BDetector* tra = new R3BSTaRTra("STaRTrack", kTRUE);
    tra->SetGeometryFileName(((TObjString*)fDetList.GetValue("STaRTrack"))->GetString().Data());
    run->AddModule(tra);
  }
  
  // DCH drift chambers
  if (fDetList.FindObject("DCH") ) {
    R3BDetector* dch = new R3BDch("Dch", kTRUE);
    dch->SetGeometryFileName(((TObjString*)fDetList.GetValue("DCH"))->GetString().Data());
    run->AddModule(dch);
  }
  
  // Tof
  if (fDetList.FindObject("TOF") ) {
    R3BDetector* tof = new R3BTof("Tof", kTRUE);
    tof->SetGeometryFileName(((TObjString*)fDetList.GetValue("TOF"))->GetString().Data());
    run->AddModule(tof);
  }
  
  // mTof
  if (fDetList.FindObject("MTOF") ) {
    R3BDetector* mTof = new R3BmTof("mTof", kTRUE);
    mTof->SetGeometryFileName(((TObjString*)fDetList.GetValue("MTOF"))->GetString().Data());
    run->AddModule(mTof);
  }

  // GFI detector
  if (fDetList.FindObject("GFI") ) {
    R3BDetector* gfi = new R3BGfi("Gfi", kTRUE);
    gfi->SetGeometryFileName(((TObjString*)fDetList.GetValue("GFI"))->GetString().Data());
    run->AddModule(gfi);
  }
  
  // Land Detector
  if (fDetList.FindObject("LAND") ) {
    R3BDetector* land = new R3BLand("Land", kTRUE);
    land->SetVerboseLevel(1);
    land->SetGeometryFileName(((TObjString*)fDetList.GetValue("LAND"))->GetString().Data());
    run->AddModule(land);
  }
  
  // NeuLand Scintillator Detector
  if(fDetList.FindObject("SCINTNEULAND")) {
    R3BDetector* land = new R3BLand("Land", kTRUE);
    land->SetVerboseLevel(1);
    land->SetGeometryFileName(((TObjString*)fDetList.GetValue("SCINTNEULAND"))->GetString().Data());
    run->AddModule(land);
  }
  
  // MFI Detector
  if(fDetList.FindObject("MFI")) {
    R3BDetector* mfi = new R3BMfi("Mfi", kTRUE);
    mfi->SetGeometryFileName(((TObjString*)fDetList.GetValue("MFI"))->GetString().Data());
    run->AddModule(mfi);
  }

  // PSP Detector
  if(fDetList.FindObject("PSP")) {
    R3BDetector* psp = new R3BPsp("Psp", kTRUE);
    psp->SetGeometryFileName(((TObjString*)fDetList.GetValue("PSP"))->GetString().Data());
    run->AddModule(psp);
  }
  
  // Luminosity detector
  if (fDetList.FindObject("LUMON") ) {
    R3BDetector* lumon = new ELILuMon("LuMon", kTRUE);
    lumon->SetGeometryFileName(((TObjString*)fDetList.GetValue("LUMON"))->GetString().Data());
    run->AddModule(lumon);
  }
  
  // -----   Create R3B  magnetic field ----------------------------------------
  Int_t typeOfMagneticField = 0;
  Int_t fieldScale = 1;
  Bool_t fVerbose = kFALSE;
  
  //NB: <D.B>
  // If the Global Position of the Magnet is changed
  // the Field Map has to be transformed accordingly
  if (fFieldMap == 0) {
    R3BAladinFieldMap* magField = new R3BAladinFieldMap("AladinMaps");
    magField->SetCurrent(fMeasCurrent);
    magField->SetScale(fieldScale);
    
    if ( fR3BMagnet == kTRUE ) {
      run->SetField(magField);
    } else {
      run->SetField(NULL);
    }
  } else if(fFieldMap == 1){
    R3BGladFieldMap* magField = new R3BGladFieldMap("R3BGladMap");
    magField->SetScale(fieldScale);
    
    if ( fR3BMagnet == kTRUE ) {
      run->SetField(magField);
    } else {
      run->SetField(NULL);
    }
  }  //! end of field map section
  
  // -----   Create PrimaryGenerator   --------------------------------------

  // 1 - Create the Main API class for the Generator
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  
  if (fGenerator.CompareTo("ion") == 0  ) {
          // R3B Ion Generator
          Int_t z = 30;  // Atomic number
          Int_t a = 65; // Mass number
          Int_t q = 0;   // Charge State 
          Int_t m = 1;   // Multiplicity
          Double_t px = 40./a;  // X-Momentum / per nucleon!!!!!!
          Double_t py = 600./a;  // Y-Momentum / per nucleon!!!!!!
          Double_t pz = 0.01/a;  // Z-Momentum / per nucleon!!!!!!
          R3BIonGenerator* ionGen = new R3BIonGenerator(z,a,q,m,px,py,pz);
          ionGen->SetSpotRadius(1,1,0);
          // add the ion generator
          primGen->AddGenerator(ionGen);
  }

  if (fGenerator.CompareTo("ascii") == 0  ) {
    R3BAsciiGenerator* gen = new R3BAsciiGenerator((dir+"/input/"+InFile).Data());
    primGen->AddGenerator(gen);
  }
  

  if (fGenerator.CompareTo("box") == 0  ) {
	  // 2- Define the BOX generator
	  Double_t pdgId=2212; // proton beam
	  Double_t theta1= 25.;  // polar angle distribution
	  //Double_t theta2= 7.;
	  Double_t theta2= 66.;
//	  Double_t momentum=1.09008; // 500 MeV/c
//	  Double_t momentum=0.4445834; // 100 MeV/c
          Double_t momentum1=TMath::Sqrt(energy1*energy1 + 2*energy1*0.938272046);
          Double_t momentum2=TMath::Sqrt(energy2*energy2 + 2*energy2*0.938272046);
	  FairBoxGenerator* boxGen = new FairBoxGenerator(pdgId, 1);
	  boxGen->SetThetaRange (   theta1,   theta2);
	  boxGen->SetPRange     (momentum1,momentum2);
	  boxGen->SetPhiRange   (0,360.);
	  //boxGen->SetXYZ(0.0,0.0,-1.5);
	  boxGen->SetXYZ(0.0,0.0,0.0);
          boxGen->SetDebug(kFALSE);
	  // add the box generator
	  primGen->AddGenerator(boxGen);

          //primGen->SetTarget(0.25, 0.5);
          //primGen->SmearVertexZ(kTRUE);
  } 
	
  if (fGenerator.CompareTo("gammas") == 0  ) {
        // 2- Define the CALIFA Test gamma generator
        //Double_t pdgId=22; // gamma emission
        Double_t pdgId=2212; // proton emission
        Double_t theta1= 10.;  // polar angle distribution
        Double_t theta2= 40.;
        //Double_t theta2= 90.; 
        //Double_t momentum=0.002; // 0.010 GeV/c = 10 MeV/c 
        Double_t momentumI=0.002; // 0.010 GeV/c = 10 MeV/c 
        Double_t momentumF=0.002; // 0.010 GeV/c = 10 MeV/c 
        //Double_t momentumF=0.808065; // 0.808065 GeV/c (300MeV Kin Energy for protons) 
        //Double_t momentumI=0.31016124; // 0.31016124 GeV/c (50MeV Kin Energy for protons)
        //Double_t momentum=0.4442972; // 0.4442972 GeV/c (100MeV Kin Energy for protons)
        //Double_t momentum=0.5509999; // 0.5509999 GeV/c (150MeV Kin Energy for protons)
        //Double_t momentumI=0.64405; // 0.64405 GeV/c (200MeV Kin Energy for protons) 
        Int_t multiplicity = 1;
        R3BCALIFATestGenerator* gammasGen = new R3BCALIFATestGenerator(pdgId, multiplicity);
        gammasGen->SetThetaRange (theta1,   theta2);
        gammasGen->SetCosTheta();
        gammasGen->SetPRange(momentumI,momentumF);
        gammasGen->SetPhiRange(-180.,180.);
        //gammasGen->SetXYZ(0.0,0.0,-1.5);
        //gammasGen->SetXYZ(0.0,0.0,0);
        gammasGen->SetBoxXYZ(-0.1,0.1,-0.1,0.1,-0.1,0.1);
        //gammasGen->SetLorentzBoost(0.8197505718204776); //beta=0.81975 for 700 A MeV
        // add the gamma generator
        primGen->AddGenerator(gammasGen);
  }


  if (fGenerator.CompareTo("r3b") == 0  ) {
    R3BSpecificGenerator *pR3bGen = new R3BSpecificGenerator();
    
    // R3bGen properties
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
    pR3bGen->SetBeamEnergy(1.); // Beam Energy in GeV
    pR3bGen->SetSigmaBeamEnergy(1.e-03); // Sigma(Ebeam) GeV
    pR3bGen->SetParticleDefinition(2212); // Use Particle Pdg Code
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
  
  if (fGenerator.CompareTo("p2p") == 0  ) {
    R3Bp2pGenerator* gen = new R3Bp2pGenerator(("/lustre/nyx/fairgsi/mwinkel/r3broot/input/p2p/build/" + InFile).Data());
    primGen->AddGenerator(gen);

#if 0
    // Coincident gammas
    R3BGammaGenerator *gammaGen = new R3BGammaGenerator();
    
    gammaGen->SetEnergyLevel(0, 0.);
    gammaGen->SetEnergyLevel(1, 3E-3);
    gammaGen->SetEnergyLevel(2, 4E-3);

    gammaGen->SetBranchingRatio(2, 1, 0.5);
    gammaGen->SetBranchingRatio(2, 0, 0.5);
    gammaGen->SetBranchingRatio(1, 0, 1.);

    gammaGen->SetInitialLevel(2);

    gammaGen->SetLorentzBoost(TVector3(0, 0, 0.777792));

    primGen->AddGenerator(gammaGen);
#endif
  }

  run->SetGenerator(primGen);
  
  
  //-------Set visualisation flag to true------------------------------------
  run->SetStoreTraj(fVis);
  

  FairLogger::GetLogger()->SetLogVerbosityLevel("LOW");

 
  // ----- Initialize CalifaHitFinder task (CrystalCal to Hit) ------------------------------------
  if(fCalifaHitFinder) {
    R3BCalifaCrystalCal2Hit* califaHF = new R3BCalifaCrystalCal2Hit();
    califaHF->SetClusteringAlgorithm(1,0);
    califaHF->SetDetectionThreshold(0.000050);//50 KeV
    califaHF->SetExperimentalResolution(6.);  //percent @ 1 MeV
    //califaHF->SetComponentResolution(.25);    //sigma = 0.5 MeV
    califaHF->SetPhoswichResolution(3.,5.);   //percent @ 1 MeV for LaBr and LaCl 
    califaHF->SelectGeometryVersion(17);
    califaHF->SetAngularWindow(0.25,0.25);      //[0.25 around 14.3 degrees, 3.2 for the complete calorimeter]
    run->AddTask(califaHF);
  }

  // ----- Initialize StarTrackHitfinder task ------------------------------------
  if(fStarTrackHitFinder) {
    R3BSTaRTraHitFinder* trackHF = new R3BSTaRTraHitFinder();
    //trackHF->SetClusteringAlgorithm(1,0);
    trackHF->SetDetectionThreshold(0.000050); //50 KeV
    trackHF->SetExperimentalResolution(0.);
    //trackHF->SetAngularWindow(0.15,0.15);      //[0.25 around 14.3 degrees, 3.2 for the complete calorimeter]
    run->AddTask(trackHF);
  }
  
	
  // -----   Initialize simulation run   ------------------------------------
  run->Init();

  // ------  Increase nb of step for CALO
  Int_t nSteps = 150000;
  TVirtualMC::GetMC()->SetMaxNStep(nSteps);
  
  
  // -----   Runtime database   ---------------------------------------------
  R3BFieldPar* fieldPar = (R3BFieldPar*) rtdb->getContainer("R3BFieldPar");
      fieldPar->SetParameters(magField);
      fieldPar->setChanged();
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


