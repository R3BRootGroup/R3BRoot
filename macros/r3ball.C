//  -------------------------------------------------------------------------
//
//   ----- General Macro for R3B simulation
//
//         Author: Denis Bertini <D.Bertini@gsi.de>
//
//         Last Update: 06/08/09
//
//
//
//
//  -------------------------------------------------------------------------
//
//   Usage inside ROOT interpreter: 
//        1) root -l
//        2)[root] .L r3ball.C 
//                         
//        3)[root] r3ball( nevt,
//                         CALVersion,    // (0 -> CALIFA, 1-> Crystal BALL )
//                         TargetType,    // "LeadTarget" 
//                         Visualization, // kFalse or kTRUE   
//                         fMC ,        // "TGeant3" or "TGeant4"   
//                         fGenerator   // Generator type
//
//  -------------------------------------------------------------------------



void r3ball(Int_t nEvents = 1,
	    Int_t fCaloVersion = 0,
	    TString Target = "LeadTarget",
            Bool_t fVis=kFALSE,
            TString fMC="TGeant3",
            TString fGenerator="box")
{
  
  TString dir = getenv("VMCWORKDIR");
  TString r3bdir = dir + "/macros";

  TString r3b_geomdir = dir + "/geometry";
  gSystem->Setenv("GEOMPATH",r3b_geomdir.Data());

  TString r3b_confdir = dir + "gconfig";
  gSystem->Setenv("CONFIG_DIR",r3b_confdir.Data());

// Output files
  TString OutFile = "r3bsim.root";
  TString ParFile = "r3bpar.root";


  // In general, the following parts need not be touched
  // ========================================================================

  // ----    Debug option   -------------------------------------------------
  gDebug = 0;
  // ------------------------------------------------------------------------

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  gSystem->Load("libGenVector");

  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libGen");

  //----  Load R3B specific libraries ---------------------------------------
  gSystem->Load("libR3Bbase");
  gSystem->Load("libR3BGen");
  gSystem->Load("libR3BPassive");
  gSystem->Load("libR3BData");
  gSystem->Load("libR3BCal");
  gSystem->Load("libR3BCalo");
  gSystem->Load("libR3BDch");
  gSystem->Load("libR3BGfi");
  gSystem->Load("libR3BLand");
  gSystem->Load("libR3BmTof");
  gSystem->Load("libR3BTof");
  gSystem->Load("libR3BTra");

 
  // -----   Create simulation run   ----------------------------------------
  FairRunSim* run = new FairRunSim();
  run->SetName(fMC.Data());              // Transport engine
  run->SetOutputFile(OutFile.Data());          // Output file
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  
  // -----   Create media   -------------------------------------------------
  run->SetMaterials("media_r3b.geo");       // Materials
  

  // -----   Create R3B geometry --------------------------------------------
  //R3B Cave definition
  FairModule* cave= new R3BCave("CAVE");
  cave->SetGeometryFileName("r3b_cave.geo");
  run->AddModule(cave);
  
  //R3B Target definition
  FairModule* target= new R3BTarget(Target.Data());
  run->AddModule(target);
  
  //R3B Magnet definition
  FairModule* mag = new R3BMagnet("AladinMagnet");
  run->AddModule(mag);
  
  if (fCaloVersion == 1) {
  //R3B Crystal Calorimeter
  FairDetector* cal = new R3BCal("CrystalCal", kTRUE);
  run->AddModule(cal);
  } else {
  // CALIFA Calorimeter
  FairDetector* calo = new R3BCalo("Califa", kTRUE);
  run->AddModule(calo);
  }
  
  // Tracker
  FairDetector* tra = new R3BTra("Tracker", kTRUE);
  Double_t fCutOffSi = 1.0e-06;  // Cut-Off -> 10KeV only in Si
  ((R3BTra*) tra)->SetEnergyCutOff(fCutOffSi);
  run->AddModule(tra);
  
  
  // DCH drift chambers
  FairDetector* dch = new R3BDch("Dch", kTRUE);
  run->AddModule(dch);
  
  // Tof
  FairDetector* tof = new R3BTof("Tof", kTRUE);
  Double_t fCutOffSci = 1.0e-05;  // Cut-Off -> 10.KeV only in Sci.
  ((R3BTof*) tof)->SetEnergyCutOff(fCutOffSci);
  run->AddModule(tof);
  
  // mTof
  FairDetector* mTof = new R3BmTof("mTof", kTRUE);
  ((R3BmTof*) mTof)->SetEnergyCutOff(fCutOffSci);
  run->AddModule(mTof);

  // GFI detector
  FairDetector* gfi = new R3BGfi("Gfi", kTRUE);
  ((R3BGfi*) gfi)->SetEnergyCutOff(fCutOffSci);
  run->AddModule(gfi);

  // Land Detector
  FairDetector* land = new R3BLand("Land", kTRUE);
  run->AddModule(land);
 
  // -----   Create R3B  magnetic field ----------------------------------------
  Int_t typeOfMagneticField = 0;
  Int_t fieldScale = 1;
  Bool_t fVerbose = kFALSE;

  R3BFieldMap* magField = new R3BFieldMap(typeOfMagneticField,fVerbose);
  magField->SetPosition(0., 0., 0.);
  magField->SetScale(fieldScale);
  run->SetField(magField);



  // -----   Create PrimaryGenerator   --------------------------------------

  // 1 - Create the Main API class for the Generator
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();


  if (fGenerator.CompareTo("box") == 0  ) {
  // 2- Define the BOX generator
  Double_t pdgId=211; // pion beam
  Double_t theta1= 0.;  // polar angle distribution
  Double_t theta2= 7.;
  Double_t momentum=.8; // 10 GeV/c
  FairBoxGenerator* boxGen = new FairBoxGenerator(pdgId, 50);
  boxGen->SetThetaRange (   theta1,   theta2);
  boxGen->SetPRange     (momentum,momentum*2.);
  boxGen->SetPhiRange   (0.,360.);
  boxGen->SetXYZ(0.0,0.0,-1.5);
  // add the box generator
  primGen->AddGenerator(boxGen);
  } 
  
 if (fGenerator.CompareTo("r3b") == 0  ) {
  R3BPrimaryGenerator *pR3bGen = new R3BPrimaryGenerator();

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
  if (fVis==kTRUE){
     run->SetStoreTraj(kTRUE);
  }else{
     run->SetStoreTraj(kFALSE);
  }   

  // -----   Initialize simulation run   ------------------------------------
  run->Init();

  // ------  Increase nb of step for CALO
  Int_t nSteps = -15000;
  gMC->SetMaxNStep(nSteps);

  // -----   Runtime database   ---------------------------------------------
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


