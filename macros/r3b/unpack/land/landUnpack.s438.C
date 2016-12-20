
{
  TStopwatch timer;
  timer.Start();


  const Int_t nev = 100000;       // number of events to read, -1 - untill CTRL+C
  const UInt_t runId = 1;         // run ID

  const char *node = "lxi047";    // remote-event server

  const char *landMappingName = "cfg_neuland_s438.hh";   // mapping file
  const Int_t nBarsPerPlane = 50;                        // number of bars per plane
  const Int_t nModules = 100;                            // total number of bars

  const char *outputFileName = "rawData.s438.root";                   // name of output file
  const char *histFileName = "/Users/kresan/webdocs/hist.s438.root";  // name of file with control histograms
  const Int_t refresh = 10000;                                        // refresh rate for saving control histograms
  const char *parFileName = "params.s438.root";                       // name of parameter file
  const Long64_t maxSize = 500 * 1024 * 1024;                         // file split size

  const Int_t updateRate = 1000000;     // rate of TCAL parameters update
  const Int_t minStats = 1000;          // minimum entries per PMT for TCAL calibration


  // Create source with unpackers ----------------------------------------------
  FairRemoteSource* source  = new FairRemoteSource(node);

  // NeuLAND MBS parameters -------------------------------
  Short_t type = 94;
  Short_t subType = 9400;
  Short_t procId = 10;
  Short_t subCrate = 1;
  Short_t control = 3;
  source->AddUnpacker(new R3BLandUnpack(type, subType,
                                        procId, subCrate, control));
  // ------------------------------------------------------

  // LOS MBS parameters -----------------------------------
  type = 88;
  subType = 8800;
  procId = 10;
  subCrate = 7;
  control = 5;
  source->AddUnpacker(new R3BLosUnpack(type, subType,
                                       procId, subCrate, control));
  // ------------------------------------------------------
  // ---------------------------------------------------------------------------


  // Create online run ---------------------------------------------------------
  FairRunOnline* run = new FairRunOnline(source);
  run->SetRunId(runId);
  run->SetOutputFile(outputFileName);
  run->SetGenerateHtml(kTRUE, histFileName, refresh);
  // ---------------------------------------------------------------------------
  
  
  // Create ALADIN field map ---------------------------------------------------
  R3BAladinFieldMap* magField = new R3BAladinFieldMap("AladinMaps");
  Double_t fMeasCurrent = 2500.;// I_current [A]
  magField->SetCurrent(fMeasCurrent);
  magField->SetScale(1.);
  run->SetField(magField);
  // ---------------------------------------------------------------------------
  
  
  // Channel mapping -----------------------------------------------------------
  R3BLandMapping *map = new R3BLandMapping();
  map->SetFileName(landMappingName);
  map->SetNofBarsPerPlane(nBarsPerPlane);
  run->AddTask(map);
  // ---------------------------------------------------------------------------


  // TCAL ----------------------------------------------------------------------
  R3BNeulandMapped2CalPar *tcalFill = new R3BNeulandMapped2CalPar("NeulandMapped2Cal");
  tcalFill->SetUpdateRate(updateRate);
  tcalFill->SetMinStats(minStats);
  tcalFill->SetNofModules(nModules, 20);
  run->AddTask(tcalFill);
  // ---------------------------------------------------------------------------


  // Add analysis task ---------------------------------------------------------
  R3BLandRawAna* ana = new R3BLandRawAna();
  run->AddTask(ana);
  // ---------------------------------------------------------------------------


  // Initialize ----------------------------------------------------------------
  run->Init();
  ((TTree*)gFile->Get("cbmsim"))->SetMaxTreeSize(maxSize);
  FairLogger::GetLogger()->SetLogScreenLevel("INFO");
  // ---------------------------------------------------------------------------
  
  
  // Runtime data base ---------------------------------------------------------
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  R3BFieldPar* fieldPar = (R3BFieldPar*) rtdb->getContainer("R3BFieldPar");
  fieldPar->SetParameters(magField);
  fieldPar->setChanged();
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open(parFileName);
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();
  // ---------------------------------------------------------------------------
  
  
  // Run -----------------------------------------------------------------------
  run->Run(nev, 0);
  // ---------------------------------------------------------------------------


  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outputFileName << endl;
  cout << "Parameter file is " << parFileName << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime<< "s" << endl << endl;
}

