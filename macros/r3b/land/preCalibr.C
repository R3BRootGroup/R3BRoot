void preCalibr(Int_t nNeutrons = 4, Int_t nEvents = 100, Int_t beamE = 600, Int_t Erel = 500)
{
  Int_t d;
  if(Erel == 100) {
    d = 35; 
  } else {
    d = 14;
  }
 
  // ----- Files ---------------------------------------------------------------
  char strDir[] = ".";
  

  char str[100];
  sprintf(str, "%1dAMeV.%1dn.%1dkeV.%1dm.root", beamE,nNeutrons, Erel, d);
  TString inFile  = TString(strDir) + "/sim." + TString(str);
  TString parFile  = TString(strDir) + "/par." + TString(str);
  TString outFile  = TString(strDir) + "/r3bcalibr." + TString(str);
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
  R3BLandDigitizer* landDigitizer  = new R3BLandDigitizer(0);
  landDigitizer->UseBeam(beamE);
  landDigitizer->SetTOFRange(400.);
  fRun->AddTask(landDigitizer);

  R3BLandDigitizerQA* landDigitizerQA = new R3BLandDigitizerQA();
  landDigitizerQA->SetBeamEnergy(beamE);
  fRun->AddTask(landDigitizerQA);
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
  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  // ---------------------------------------------------------------------------
}
