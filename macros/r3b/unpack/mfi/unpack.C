
{
  TStopwatch timer;
  timer.Start();


  const Int_t nev = 10000;       // number of events to read, -1 - untill CTRL+C
  const char *outputFileName = "rawData.root";                   // name of output file
  const char *parFileName = "params.root";                       // name of parameter file


  // Create source with unpackers ----------------------------------------------
  FairLmdSource* source  = new FairLmdSource();
  source->AddFile("/Users/kresan/lmd/run130_local109_0001.lmd");

  // MFI MBS parameters -------------------------------
  Short_t type = 97;
  Short_t subType = 9700;
  Short_t procId = 1;
  Short_t subCrate = 0;
  Short_t control = 9;
  source->AddUnpacker(new R3BMfiUnpack(type, subType,
                                       procId, subCrate, control));
  // ------------------------------------------------------
  // ---------------------------------------------------------------------------


  // Create online run ---------------------------------------------------------
  FairRunOnline* run = new FairRunOnline(source);
  run->SetOutputFile(outputFileName);
  // ---------------------------------------------------------------------------
  
  
  // Create ALADIN field map ---------------------------------------------------
  R3BAladinFieldMap* magField = new R3BAladinFieldMap("AladinMaps");
  Double_t fMeasCurrent = 2500.;// I_current [A]
  magField->SetCurrent(fMeasCurrent);
  magField->SetScale(1.);
  run->SetField(magField);
  // ---------------------------------------------------------------------------
  

  // Initialize ----------------------------------------------------------------
  run->Init();
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

