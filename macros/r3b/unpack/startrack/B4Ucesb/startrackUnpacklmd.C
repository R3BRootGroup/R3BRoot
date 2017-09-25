//
//  using Fairlmdsource
//
//

{
  TStopwatch timer;
  timer.Start();


  // Run over all events  (or blocks)
  Int_t nev = -1;
  //nev=100931731456; 

  const char *outputFileName = "startrack_raw_data.root";
  const Long64_t maxSize = 1 * 1024 * 1024 * 1024; // GBytes
  
  // Create source with unpackers ----------------------------------------------
  //FairRemoteSource* source  = new FairRemoteSource("lxi047");
  FairLmdSource* source  = new FairLmdSource();    // using Unpack
  //Fairbin1Source* source  = new Fairbin1Source();  // using DoUnpack


  //source->AddFile("/home/mala/Echanges/R3B_Simulation/r3b_dataread_Marcello/Mixed-alpha-source/2014-03-06-Eth-b0.bin");
  //source->AddFile("/home/mala/Echanges/R3B_data/highThreshold/R2_0");

  // lmd file
  //source->AddFile("/net/nnlxs1/data19/mala/MyE_drive\ docs/R3B-UK/mbs/sitracker001.lmd");  // !! with Type =10, SubType=1

  //source->AddFile("/MIDAS/R3B/GSITest2014/lmd/run222_0818.lmd");  // !! Si+all other detector with Type =104, SubType=10400
  //source->AddFile("/MIDAS/R3B/GSITest2014/lmd/run007_0018.lmd");  // !! Si + internal pulse  with Type =104, SubType=10400
  //source->AddFile("/MIDAS/R3B/GSITest2014/lmd/run006_0016.lmd");  // !! Si only with Type =104, SubType=10400
  //source->AddFile("/MIDAS/R3B/GSITest2014/lmd/sit272_2186.lmd");  // !! Si only empty target with Type =104, SubType=10400
  source->AddFile("/MIDAS/R3B/GSITest2014/lmd/run280_3364.lmd");  // !! Si only with Type =104, SubType=10400  - C target
  //source->AddFile("/scratch/xry76555/R3B-data/GSIdata/run290_3777.lmd");  // !! Si only with Type =104, SubType=10400  - CH2 target
  //source->AddFile("/MIDAS/R3B/GSITest2014/lmd/sitracker_stripped.lmd");  // !! Si only with Type =104, SubType=10400
 
 //  char* strCalDir = "/home/work/example/";

// TODO: set proper values to the variable below when the binary file is from GSI MBS DAQ is used.
// To be checked:
  Short_t type = 104;
  Short_t subType = 10400;
  Short_t procId = 1; 
  Short_t subCrate = -1; // All sub-crates
  //Short_t subCrate = 0; // All sub-crates
  //Short_t subCrate = 20; // All sub-crates
   Short_t control = 37;


  source->AddUnpacker(new R3BStarTrackUnpack("", type, subType,
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
  R3BStarTrackRawAna* ana = new R3BStarTrackRawAna();
  run->AddTask(ana);
  // ---------------------------------------------------------------------------
  
  // Initialize ----------------------------------------------------------------
  run->Init();
  // ---------------------------------------------------------------------------
  // Initialize ----------------------------------------------------------------
  //run->Init();
  //((TTree*)gFile->Get("cbmsim"))->SetMaxTreeSize(maxSize);
  //FairLogger::GetLogger()->SetLogScreenLevel("INFO");
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
  run->Run(nev, 0);
  delete run;
  // ---------------------------------------------------------------------------



  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outputFileName << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime
  << "s" << endl << endl;
}

