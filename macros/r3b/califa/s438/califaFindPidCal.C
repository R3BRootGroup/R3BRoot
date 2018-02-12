// Find average ratio (Nf + Ns) / Energy for each crystal
void califaFindPidCal(TString inputFile, TString outputFile, 
	       TString parRootFile, Int_t nEvents = 0) {
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  
  // -----   Create analysis run   ----------------------------------------
  FairRunAna* fRun = new FairRunAna();
  fRun->SetSource(new FairFileSource(inputFile));
  fRun->SetOutputFile(outputFile);
  
  FairRootManager::Instance()->SetCompressData(true);

  // -----   Runtime database   ---------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parRootIn; 
  FairParAsciiFileIo* parAsciiIn; 
  
  // if(parAsciiFile) {
  //   parAsciiIn = new FairParAsciiFileIo();
  //   parAsciiIn->open(parAsciiFile);
  //   rtdb->setFirstInput(parAsciiIn);
  //   if(parRootFile) {
  //     parRootIn = new FairParRootFileIo();
  //     parRootIn->open(parRootFile);
  //     rtdb->setSecondInput(parRootIn);
  //   }
  // }
  //else if(parRootFile) {
  parRootIn = new FairParRootFileIo();
  parRootIn->open(parRootFile);
  rtdb->setFirstInput(parRootIn);
    //}
  //rtdb->setOutput(parIn);
  //rtdb->saveOutput();
  rtdb->print();
  
  /*TList *lPars = rtdb->getListOfContainers();
  for (Int_t i=0;i<lPars->GetEntries();i++) {
    FairParSet* apar = (FairParSet*) lPars->At(i);
    apar->Print();
  }    
  R3BCaloCalPar* par = (R3BCaloCalPar*)rtdb->getContainer("CaloCalPar");  
  if(par) par->Print();
  else cout << "NO PAR!!!!" << endl << endl;

  R3BCaloDUCalPar* du =  par->GetDUCalParAt(0);
  if(du) du->Print();
  else cout << "NO DU!!!!" << endl << endl;
  */

  //Crystal calibration
  R3BCaloCalibParFinder* cal = new R3BCaloCalibParFinder();
  cal->SetOutputFile("pidPars.txt");
  fRun->AddTask(cal);
  

  fRun->Init();       
  FairLogger::GetLogger()->SetLogScreenLevel("INFO");
             
  fRun->Run(0,nEvents);
  
  delete fRun;

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------
     	
}
