
Int_t lmd_scan(){


  TStopwatch timer;
  timer.Start();

 
  // Create source with unpackers ----------------------------------------------
  R3BDBLmdAnalyzer* source  = new R3BDBLmdAnalyzer();
  // adding the LMD file
  source->AddFile("/Volumes/Data2/land/s406/lmd/r258_2983.lmd");
  source->AddFile("/Volumes/Data2/land/s406/lmd/r258_2984.lmd");
  source->AddFile("/Volumes/Data2/land/s406/lmd/r258_2985.lmd");
  //  source->AddFile("/Volumes/Data2/land/s406/lmd/r258_2986.lmd");
  //  source->AddFile("/Volumes/Data2/land/s406/lmd/r258_2987.lmd");

 
  // Create online run ---------------------------------------------------------
  FairRunOnline* run = new FairRunOnline(source);
 
  // DB Setup 
  FairRunIdGenerator runID;
  UInt_t runId =  runID.generateId();
  Double_t s= (Double_t) runId;
  ValTimeStamp t1(s);
     
  cout << " RUNID generated ---> " << runId << " date:" << t1.AsString("s") <<  endl;
  run->SetRunId(runId); 
  run->SetOutputFile("output.root");
  //run->SetGenerateHtml(kFALSE);
  
  // ---------------------------------------------------------------------------
  
  
  // Create ALADIN field map ---------------------------------------------------
  R3BAladinFieldMap* magField = new R3BAladinFieldMap("AladinMaps");
  Double_t fMeasCurrent = 2500.;// I_current [A]
  magField->SetCurrent(fMeasCurrent);
  magField->SetScale(1.);
  run->SetField(magField);
  
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
   parOut->open("parameter.root");
   rtdb->setSecondInput(parOut);
   rtdb->saveOutput();
   rtdb->print(); 
  // ---------------------------------------------------------------------------
  
 
 // Initialize ----------------------------------------------------------------
  run->Init();
  //((TTree*)gFile->Get("cbmsim"))->SetMaxTreeSize(maxSize);
  FairLogger::GetLogger()->SetLogScreenLevel("INFO");
  // ---------------------------------------------------------------------------

 
  // Run and Analyse LMD Files
  run->Run(-1);
  // ---------------------------------------------------------------------------

 
  // Times report
  Int_t n_starts = source->GetNStartTimes();
  Int_t n_stops = source->GetNStopTimes();
  cout << " SOURCE n_start, n_stop " << n_starts << " : " << n_stops << endl;
 
  for (Int_t i=0; i<n_starts;i++){     
    ValTimeStamp* tst = source->GetStartTimesAt(i);
    cout << " SOURCE START T: " << i << " : " <<  tst->AsString("s") << endl;
  } 

  for (Int_t i=0; i<n_stops;i++){     
    ValTimeStamp* tst = source->GetStopTimesAt(i);
    cout << " SOURCE STOP T: " << i << " : " <<  tst->AsString("s") << endl;
  } 



  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  //cout << "Output file is "    << outputFileName << endl;
  //cout << "Parameter file is " << parFileName << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime<< "s" << endl << endl;

  return 0;
}

