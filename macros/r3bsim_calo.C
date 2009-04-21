void r3bsim_calo(Int_t nEvents = 1)
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
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
//  gSystem->Load("libField");
  gSystem->Load("libGen");

  //----  Load R3B specific libraries ---------------------------------------
  gSystem->Load("libR3Bbase");
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
  run->SetName("TGeant3");              // Transport engine
  run->SetOutputFile(OutFile.Data());          // Output file
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  

  // -----   Create media   -------------------------------------------------
  run->SetMaterials("media_pnd.geo");       // Materials
  
  // -----   Create R3B geometry --------------------------------------------


  //R3B Cave definition
  FairModule* cave= new R3BCave("CAVE");
  cave->SetGeometryFileName("r3b_cave.geo");
  run->AddModule(cave);

  //R3B Target definition
  FairModule* target= new R3BTarget("LeadTarget");
  run->AddModule(target);

  //R3B Magnet definition
  FairModule* mag = new R3BMagnet("AladinMagnet");
  run->AddModule(mag);

  // Crystal Calorimeter
//  FairDetector* calB = new R3BCal("CalB", kTRUE);
//  run->AddModule(calB);

  // DCH drift chambers
//  FairDetector* dch = new R3BDch("Dch", kTRUE);
//  run->AddModule(dch);

  // GFI detector
//  FairDetector* gfi = new R3BGfi("Gfi", kTRUE);
//  run->AddModule(gfi);

  // Land Detector
//  FairDetector* land = new R3BLand("Land", kTRUE);
//  run->AddModule(land);

  // Tof
//  FairDetector* tof = new R3BTof("Tof", kTRUE);
//  run->AddModule(tof);

  // mTof
//  FairDetector* mTof = new R3BmTof("mTof", kTRUE);
//  run->AddModule(mTof);

  // Tracker
//  FairDetector* tra = new R3BTra("Tracker", kTRUE);
//  run->AddModule(tra);

  // R3b  Calorimeter
  FairDetector* calo = new R3BCalo("Calo", kTRUE);
  run->AddModule(calo);


  // -----   Create PrimaryGenerator   --------------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

  Double_t pdgId=11; // pion beam
  Double_t theta1= 0.;  // polar angle distribution
  Double_t theta2= 0.1;
  Double_t momentum=1.; // 10 GeV/c
  FairBoxGenerator* boxGen = new FairBoxGenerator(pdgId, 1);
  boxGen->SetThetaRange (   theta1,   theta2);
  boxGen->SetPRange     (momentum,momentum);
  boxGen->SetPhiRange   (0.,360.);

  primGen->AddGenerator(boxGen);

  run->SetGenerator(primGen);

  // -----   Initialize simulation run   ------------------------------------
  run->Init();

  // -----   Runtime database   ---------------------------------------------
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open(ParFile.Data());
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();
   


  // -----   Start run   ----------------------------------------------------
//  run->Run(nEvents);
  
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
}


