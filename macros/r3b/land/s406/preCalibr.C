void preCalibr(Int_t beam)
{
  // ----- Files ---------------------------------------------------------------
  char strDir[] = "/Users/kresan/neuland";
  char str[100];
  sprintf(str, "s406.%1dAMeV", beam);
  TString inFile  = TString(strDir) + "/r3bsim." + TString(str) + ".root";
  TString parFile = TString(strDir) + "/r3bpar." + TString(str) + ".root";
  TString outFile = TString(strDir) + "/r3bcalibr." + TString(str) + ".root";
  TString thrFile = "/Users/kresan/r3b/r3broot/land/neuland_thr_ch2.txt";
  // ---------------------------------------------------------------------------


  // ----- Number of events to process -----------------------------------------
  Int_t nEvents = 100000;
  // ---------------------------------------------------------------------------


  // ----- Timer ---------------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ---------------------------------------------------------------------------


  // ----- Digitization --------------------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetSource(new FairFileSource(inFile));
  fRun->SetOutputFile(outFile);
  // ---------------------------------------------------------------------------


  // ----- Connect the Digitization Task ---------------------------------------
  R3BLandDigitizer* landDigitizer = new R3BLandDigitizer(0);
  landDigitizer->UseBeam((Double_t)beam);
  landDigitizer->UseThresholds(thrFile.Data(), 150);
  fRun->AddTask(landDigitizer);

  R3BLandDigitizerQA* landDigitizerQA = new R3BLandDigitizerQA();
  landDigitizerQA->SetBeamEnergy(beam);
  fRun->AddTask(landDigitizerQA);
  // ---------------------------------------------------------------------------


  // ----- Find clusters -------------------------------------------------------
  R3BNeuLandClusterFinder* landClusterFinder  = new R3BNeuLandClusterFinder(0);
  fRun->AddTask(landClusterFinder);
  // ---------------------------------------------------------------------------


  // ----- Calibration of 2D cuts ----------------------------------------------
  R3BNeutronCalibr2D* calibr  = new R3BNeutronCalibr2D();
  calibr->UseBeam(beam);
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
  delete fRun;
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
