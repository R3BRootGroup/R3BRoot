void preCalibr(Int_t beamE,Int_t nEvents=10000,  Int_t nn, Int_t erel)
{
  // ----- Files ---------------------------------------------------------------
  char strDir[] = ".";
  
  Int_t d;
  if(100 == erel) {
    d = 35; 
  } else {
    d = 14;
  }
  
  char str[100];
  sprintf(str, "%1dAMeV.%1dn.%1dkeV.%1dm.root", beamE,nn, erel, d);
  TString inFile  = TString(strDir) + "/r3bsim." + TString(str);
  TString parFile  = TString(strDir) + "/r3bpar." + TString(str);
  TString outFile  = TString(strDir) + "/r3bcalibr." + TString(str);
  // ---------------------------------------------------------------------------



  // ----- Timer ---------------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ---------------------------------------------------------------------------



  // ----- Digitization --------------------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  // ---------------------------------------------------------------------------



  // ----- Connect the Digitization Task ---------------------------------------
  R3BLandDigitizer* landDigitizer  = new R3BLandDigitizer(0);
  landDigitizer->UseBeam(beamE);
  fRun->AddTask(landDigitizer);
  // ---------------------------------------------------------------------------



  // ----- Find clusters -------------------------------------------------------
  R3BNeuLandClusterFinder* landClusterFinder  = new R3BNeuLandClusterFinder(0);
  fRun->AddTask(landClusterFinder);
  // ---------------------------------------------------------------------------



  // ----- Calibration of 2D cuts ----------------------------------------------
  R3BNeutronCalibr2D* calibr  = new R3BNeutronCalibr2D();
  calibr->UseBeam(beamE);
  fRun->AddTask(calibr);
  // ---------------------------------------------------------------------------



  // ----- Runtime DataBase info -----------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile.Data());
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ---------------------------------------------------------------------------

  
  // ----- Intialise and run ---------------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
  // ---------------------------------------------------------------------------



  // ----- Finish --------------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file writen:  "    << outFile << endl;
  cout << "Parameter file writen " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ---------------------------------------------------------------------------
}
