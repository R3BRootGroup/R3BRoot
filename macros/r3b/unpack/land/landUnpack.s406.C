
{
  TStopwatch timer;
  timer.Start();

 
  const Int_t nev = -1;
  const char *landMappingName = "cfg_neuland_s406.hh";
  const Int_t nBarsPerPlane = 10;
  const Int_t updateRate = 150000;
  const Int_t minStats = 10000;
  const Int_t nModules = 300;
  const char *outputFileName = "rawData.s406.root";
  const char *parFileName = "params.s406.root";
  const Long64_t maxSize = 1 * 1024 * 1024 * 1024; // GBytes


  // Create source with unpackers ----------------------------------------------
  FairLmdSource* source  = new FairLmdSource();
  // adding the LMD file
  source->AddFile("/Users/kresan/lmd/s406/r258_2983.lmd");
  source->AddFile("/Users/kresan/lmd/s406/r258_2984.lmd");
  source->AddFile("/Users/kresan/lmd/s406/r258_2985.lmd");
  source->AddFile("/Users/kresan/lmd/s406/r258_2986.lmd");
  source->AddFile("/Users/kresan/lmd/s406/r258_2987.lmd");

  // MBS parameters ---------------------------------------
  Short_t type = 94;
  Short_t subType = 9400;
  Short_t procId = 10;
  Short_t subCrate = 1;
  Short_t control = 3;
  // ------------------------------------------------------

  source->AddUnpacker(new R3BLandUnpack(type, subType,
                                        procId, subCrate, control));
  // ---------------------------------------------------------------------------


  // Create online run ---------------------------------------------------------
  FairRunOnline* run = new FairRunOnline(source);
  run->SetOutputFile(outputFileName);
  run->SetGenerateHtml(kFALSE, "", 10000);
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


  // Channel mapping -----------------------------------------------------------
  R3BLandMapping *map = new R3BLandMapping();
  map->SetFileName(landMappingName);
  map->SetNofBarsPerPlane(nBarsPerPlane);
  run->AddTask(map);
  // ---------------------------------------------------------------------------


  // TCAL ----------------------------------------------------------------------
  R3BLandTcalFill *tcalFill = new R3BLandTcalFill("TcalFill");
  tcalFill->SetUpdateRate(updateRate);
  tcalFill->SetMinStats(minStats);
  tcalFill->SetNofModules(nModules, 20);
  tcalFill->SetStoreDB(kTRUE); 
  run->AddTask(tcalFill);
  // ---------------------------------------------------------------------------


 
  
  
  // Runtime data base ---------------------------------------------------------
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  R3BFieldPar* fieldPar = (R3BFieldPar*) rtdb->getContainer("R3BFieldPar");
  fieldPar->SetParameters(magField);
  fieldPar->setChanged();

  // Set the SQL based IO as first input
  FairParTSQLIo* input_db = new FairParTSQLIo();  
  input_db->SetShutdown(kTRUE);
  input_db->open();  
  rtdb->setFirstInput(input_db);
  rtdb->setOutput(input_db);
  

   Bool_t kParameterMerged = kTRUE;
   FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
   parOut->open(parFileName);
   rtdb->setSecondInput(parOut);
   rtdb->saveOutput();
   rtdb->print(); 
  // ---------------------------------------------------------------------------
  
 
 // Initialize ----------------------------------------------------------------
  run->Init();
  ((TTree*)gFile->Get("cbmsim"))->SetMaxTreeSize(maxSize);
  FairLogger::GetLogger()->SetLogScreenLevel("INFO");
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

