void readTCal_Convert(){
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna* fRun= new FairRunAna();
  fRun->SetInputFile("rawData.s406.root");
  fRun->SetOutputFile("dummyana.root");

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open("params.s406.root","READ");
  rtdb->setFirstInput(parInput1);

  // Set the SQL IO as second input
  FairParTSQLIo* inp = new FairParTSQLIo();
  inp->SetVerbosity(1);
  inp->open();
  rtdb->setSecondInput(inp);

  // -----   Tasks to run   -------------------------------------------------
  fRun->AddTask(new R3BLandParamRead());

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

   

  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* parOutput = new FairParRootFileIo(kParameterMerged);
  parOutput->open("par_merged.root","RECREATE");//RECREATE etc. like in TFile from root
  rtdb->setOutput(parOutput);
  rtdb->setVersionsChanged(kTRUE);
  // Activate 
  TList *lPars = rtdb->getListOfContainers();
  for (Int_t i=0;i<lPars->GetEntries();i++) {
	FairParSet* apar = (FairParSet*) lPars->At(i);
    apar->setChanged();
  }    

  rtdb->saveOutput();
  
  rtdb->print();

  // fRun->Run();

}
