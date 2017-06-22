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
            TMap& fDetList,
            TString Target = "LeadTarget",
            Bool_t fVis = kFALSE,
            TString fMC = "TGeant3",
            TString fGenerator = "box",
            Bool_t fUserPList = kFALSE,
            Bool_t fR3BMagnet = kTRUE,
            Double_t fMeasCurrent = 2000.,
            TString OutFile = "r3bsim.root",
            TString ParFile = "r3bpar.root",
            TString InFile = "evt_gen.dat")
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
  //cave->SetGeometryFileName("r3b_cave.geo");
  cave->SetGeometryFileName("tracker_cave.geo");
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
    ((R3BCalifa *)califa)->SelectGeometryVersion(10);
    //Selecting the Non-uniformity of the crystals (1 means +-1% max deviation)
    ((R3BCalifa *)califa)->SetNonUniformity(1.0);
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
  
  if (fGenerator.CompareTo("box") == 0  ) {
    // 2- Define the BOX generator
    //Int_t pdgId = 211; // pion beam
    Int_t pdgId = 2212; // proton beam
    Double32_t theta1 = 7.;  // polar angle distribution
    Double32_t theta2 = 130.;
    Double32_t momentum = 0.8;
    FairBoxGenerator* boxGen = new FairBoxGenerator(pdgId, 1);
    boxGen->SetThetaRange(theta1, theta2);
    boxGen->SetPRange(momentum, momentum);
    //boxGen->SetPRange(momentum, momentum*2.);
    boxGen->SetPhiRange(0, 360);
    boxGen->SetXYZ(0.0, 0.0, 0.0);  //  
    //    boxGen->SetXYZ(0.0, 0.0, -1.5);
    //    boxGen->SetXYZ(0.0, 0.0, -300.);
    // add the box generator
    primGen->AddGenerator(boxGen);
  }
 
  if (fGenerator.CompareTo("gammas") == 0  ) {
	// 2- Define the CALIFA Test gamma generator
	Double_t pdgId=22; // gamma emission
	//Double_t pdgId=2212; // proton emission
	//Double_t pdgId=1000020040; // alpha emission
	Double_t theta1= 0.;  
	Double_t theta2= 180.;	
	//Double_t theta1= 5.;  // polar angle distribution
	//Double_t theta2= 105.;	
	//Double_t theta1= 5.3;  // polar angle distribution
	//Double_t theta2= 5.39;	
	//Double_t theta1= 100.;  // polar angle distribution
	//Double_t theta2= 105.;	
	//Double_t theta1= 7.;  // polar angle distribution	
	//Double_t theta2= 100.;	
	//Double_t theta2= 90.;	
	//Double_t momentum=0.729136; // 0.729 GeV/c (250 MeV Kin Energy for protons) 
	//Double_t momentum=0.612; // 0.612 GeV/c (50 MeV Kin Energy for alphas) 
	//Double_t momentum=0.001; // 0.001 GeV/c = 1 MeV/c 
	//Double_t momentum=0.55; // 0.55 GeV/c proton <=> 149 MeV kinetic energy proton
	//Double_t momentum=0.002; // 0.010 GeV/c = 10 MeV/c 
	//Double_t momentumI=0.5; // 0.0001 GeV/c = 0.1 MeV/c 
	//Double_t momentumF=0.5; // 
	Double_t momentum=0.001; // 1 MeV/c
	//Double_t momentum=0.808065; // 0.8 GeV/c
	//Double_t momentumF=0.8; //  
	//Double_t momentumI=0.808065; // 0.808065 GeV/c (30MeV Kin Energy for protons) 
        //Double_t momentumF=0.808065; // 0.808065 GeV/c (300MeV Kin Energy for protons) 
	//Double_t momentumI=0.31016124; // 0.31016124 GeV/c (50MeV Kin Energy for protons)
	//Double_t momentumF=0.31016124; // 0.31016124 GeV/c (50MeV Kin Energy for protons)
	//Double_t momentum=0.4442972; // 0.4442972 GeV/c (100MeV Kin Energy for protons)
	//Double_t momentum=0.5509999; // 0.5509999 GeV/c (150MeV Kin Energy for protons)
	//Double_t momentumI=0.64405; // 0.64405 GeV/c (200MeV Kin Energy for protons) 
	//Int_t multiplicity = 12;
	Int_t multiplicity = 1;
	R3BCALIFATestGenerator* gammasGen = new R3BCALIFATestGenerator(pdgId, multiplicity);
	gammasGen->SetThetaRange (theta1,  theta2);
	//gammasGen->SetThetaRange (10,  10);
	gammasGen->SetCosTheta();
	gammasGen->SetPRange(momentum,momentum);
	//gammasGen->SetPRange(momentumI,momentumF);
	gammasGen->SetPhiRange(0.,360.);
	//gammasGen->SetPhiRange(269.,301.);
	//gammasGen->SetPhiRange(269.,275.);
	//gammasGen->SetXYZ(0.0,0.0,-1.5);

	//gammasGen->SetXYZ(0.0,0.0,0.0);
	//
	//gammasGen->SetBoxXYZ(Xmin,Ymin,Zmin,Xmax,Ymax,Zmax)
	//gammasGen->SetBoxXYZ(-1.5,-1.5,0.,1.5,1.5,0.1); // thick CH2
	//gammasGen->SetBoxXYZ(0.,0.,0.,0.,0.,6.);  // LiqH target + pencil beam
	//gammasGen->SetBoxXYZ(-1.,-1.,0,1.,1.,6.);  // LiqH target
	gammasGen->SetBoxXYZ(-1.5,-1.5,0,1.5,1.5,6.);  // LiqH target

	//gammasGen->SetBoxXYZ(-1.,1.,-1.,1.,-0.0005,0.0005);  // 10um CH2 target thick
	//gammasGen->SetBoxXYZ(-1.,-1.,-0.29,1.,1.,0.29);  // 5.8 mm CH2 target thick

	//gammasGen->SetBoxXYZ(-0.1,-0.1,0.,0.1,0.1,0.);  // infinitively thin CH2 target thick + 2mm beam spot

	//gammasGen->SetBoxXYZ(-1.,-1.,-0.29,1.,1.,0.29);  // 5.8 mm CH2 target thick
	//gammasGen->SetLorentzBoost(0.8197505718204776); //beta=0.81975 for 700 A MeV
	gammasGen->SetLorentzBoost(0.5);  // beta=50%
	// add the gamma generator
	primGen->AddGenerator(gammasGen);
  } 

  if (fGenerator.CompareTo("ion_gun") == 0  ) {
  // 2- Define the BOX generator
    Int_t Z= 50;  // Sn
    Int_t A= 132; // 132Sn
    //Int_t Z= 6;  // C
    //Int_t A= 12; // 12C
    Int_t Charge=Z; // 
    Int_t Multiplicity=1;//10300911

    Double_t Px= 0.0;  // GeV/A
    Double_t Py= 0.0;  // GeV/A
    Double_t Pz= 1.;   // GeV/A   


    Double_t R=1.5; //2.;  //  spot radius [cm]
    Double_t Vz=-6.;  // source position on z axis [cm]
    Double_t Offset=0;  // offset along y axis [cm]

    //FairIonGenerator *fIongen= new FairIonGenerator(Z,A,Charge,Multiplicity,Px,Py,Pz,Vx,Vy,Vz);
    R3BIonGenerator *fIongen= new R3BIonGenerator(Z,A,Charge,Multiplicity,Px,Py,Pz);
    fIongen->SetSpotRadius(R,Vz,Offset);

  primGen->AddGenerator(fIongen);
  }

  if (fGenerator.CompareTo("cosmic") == 0  ) {
  // 2- Define the Cosmic generator
  Double_t pdgId=13; // muons -
  Double_t theta1= 30.;  // polar angle distribution
  Double_t theta2= 150.;
  Double_t E_Threshold=.1; // in GeV 
  R3BCosmicGenerator* CosmGen = new R3BCosmicGenerator(pdgId, 1, E_Threshold);
  CosmGen->SetThetaRange (   theta1,   theta2);
  CosmGen->SetPhiRange   (180.,360.);
  CosmGen->SetBoxXYZ(-50.0, -50.0, 50.0, 50.0, 50.0);  // (x1,z1,x2,z2,y) above Crystalball
  CosmGen->Init();
  // add the cosmic generator
  primGen->AddGenerator(CosmGen);
  } 
 
  if (fGenerator.CompareTo("ascii") == 0  ) {
    R3BAsciiGenerator* gen = new R3BAsciiGenerator((dir+"/input/"+InFile).Data());
    primGen->AddGenerator(gen);
  }

  if (fGenerator.CompareTo("p2p") == 0  ) {
    //R3Bp2pGenerator* P2PGen = new R3Bp2pGenerator("../../../../../Theory_p2p/Leonid1/MarcRoot/quasi"); 
    //R3Bp2pGenerator* P2PGen = new R3Bp2pGenerator("../../../../Theory_p2p/Leonid1/MarcRoot/QFS_400MeVA_12C_p_2p_sigma165.out"); 

    //R3Bp2pGenerator* P2PGen = new R3Bp2pGenerator("../../../../../R3B-Input/Theory_p2p/Leonid1/MarcRoot/quasi_elastic_400MeVA_12C_p_2p_sigma165.out");
    R3BpApGenerator* P2PGen = new R3BpApGenerator("../../../../../Echanges/Theory_p2p/Leonid1/MarcRoot/quasi_elastic_400MeVA_12C_p_ap.out");
    //R3Bp2pGenerator* P2PGen = new R3Bp2pGenerator("../../../../Theory_p2p/Leonid1/quasi_elastic_400MeVA_12C_p_2p_100keVSIGMA.out");

  primGen->AddGenerator(P2PGen); 
  } 
   
  if (fGenerator.CompareTo("r3b") == 0  ) {
    R3BSpecificGenerator *pR3bGen = new R3BSpecificGenerator();
    
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
  
  run->SetGenerator(primGen);
  
  
  //-------Set visualisation flag to true------------------------------------
  run->SetStoreTraj(fVis);
  

  FairLogger::GetLogger()->SetLogVerbosityLevel("LOW");

  
  // -----   Initialize simulation run   ------------------------------------
  run->Init();

  
  // ------  Increase nb of step for CALO
  Int_t nSteps = -15000;
  gMC->SetMaxNStep(nSteps);
  
  
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
