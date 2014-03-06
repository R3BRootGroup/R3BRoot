

{
  TStopwatch timer;
  timer.Start();
  Int_t nev=100000;
  // Create source with unpackers ----------------------------------------------
//  FairRemoteSource* source  = new FairRemoteSource("lxi047");
  FairLmdSource* source  = new FairLmdSource();
// adding the LMD file
  source->AddFile("r258_2986.lmd");
//  source->AddFile("r323_3607.lmd");

  Short_t type = 94;
  Short_t subType = 9400;
  Short_t procId = 10;
  Short_t subCrate = -1; // All sub-crates
  Short_t control = 3;

  source->AddUnpacker(new R3BLandUnpack("", type, subType,
                                        procId, subCrate, control));
  // ---------------------------------------------------------------------------


  // Create online run ---------------------------------------------------------
  FairRunOnline* run = new FairRunOnline(source);
  run->SetOutputFile("land_raw_data.root");
  run->SetGenerateHtml(kFALSE);
  // ---------------------------------------------------------------------------
  
  
  // Create ALADIN field map ---------------------------------------------------
  R3BAladinFieldMap* magField = new R3BAladinFieldMap("AladinMaps");
  Double_t fMeasCurrent = 2500.;// I_current [A]
  magField->SetCurrent(fMeasCurrent);
  magField->SetScale(1.);
  run->SetField(magField);
  // ---------------------------------------------------------------------------
  
  
  // Add analysis task ---------------------------------------------------------
  R3BLandRawAna* ana = new R3BLandRawAna();
  run->AddTask(ana);
  // ---------------------------------------------------------------------------


  R3BLandMapping *map = new R3BLandMapping();
  map->SetFileName("cfg_neuland_s406.hh");
  run->AddTask(map);


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
  parOut->open("r3bpar.root");
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();
  // ---------------------------------------------------------------------------
  
  
  // Run -----------------------------------------------------------------------
  // test with 100 events
  run->Run(nev, 0);
  // ---------------------------------------------------------------------------


  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
//  cout << "Output file is "    << OutFile << endl;
//  cout << "Parameter file is " << ParFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime<< "s" << endl << endl;
}


