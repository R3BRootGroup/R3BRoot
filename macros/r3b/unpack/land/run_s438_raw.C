
void run(TString runNumber)
{
  TStopwatch timer;
  timer.Start();


  const Int_t nev = 100;                         // number of events to read, -1 - untill CTRL+C
  TString dir = "/Volumes/Data/kresan/s438/data/";   // output directory
  TString histDir = "/Users/kresan/webdocs/";        // web-server directory

  const char *node = "lxi047";                       // remote-event server

  TString outputFileName = dir + runNumber + "_raw.root";                     // name of output file
  TString histFileName = histDir + "hist_s438_" + runNumber + "_raw.root";    // name of file with control histograms
  const Int_t refresh = 10000;                                                 // refresh rate for saving control histograms
  TString parFileName = dir + "params_" + runNumber + "_raw.root";            // name of parameter file
  const Long64_t maxSize = 100 * 1024 * 1024;        // 100 MByte             // file split size


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

  source->AddUnpacker(new R3BEventHeaderUnpack());
  // ---------------------------------------------------------------------------


  // Create online run ---------------------------------------------------------
  FairRunOnline* run = new FairRunOnline(source);
  run->SetOutputFile(outputFileName.Data());
  run->SetGenerateHtml(kTRUE, histFileName.Data(), refresh);
  // ---------------------------------------------------------------------------
  
  
  // Create ALADIN field map ---------------------------------------------------
  R3BAladinFieldMap* magField = new R3BAladinFieldMap("AladinMaps");
  Double_t fMeasCurrent = 2500.;// I_current [A]
  magField->SetCurrent(fMeasCurrent);
  magField->SetScale(1.);
  run->SetField(magField);
  // ---------------------------------------------------------------------------
  

  // Add analysis task ---------------------------------------------------------
  R3BLandRawAna* ana = new R3BLandRawAna("LandRawAna", 1);
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
  parOut->open(parFileName.Data());
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();
  // ---------------------------------------------------------------------------
  
  
  // Run -----------------------------------------------------------------------
  run->Run(nev, 0);
  delete run;
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

