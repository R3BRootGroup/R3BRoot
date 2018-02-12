void preCalibr(
  const Int_t n_events,
  const TString basename,
  const Double_t beamE,
  const Double_t saturation,
  const Double_t integration,
  const TString output_affix = ""
) {

  // ----- Files ---------------------------------------------------------------
  string basedir = ".";
  TString inFile  = basedir + "/r3bsim." + basename + ".root";
  TString parFile = basedir + "/r3bpar." + basename + ".root";
  TString outFile = basedir + "/r3bcalibr." + basename + output_affix + ".root";
  // ---------------------------------------------------------------------------



  // ----- Timer ---------------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ---------------------------------------------------------------------------



  // ----- Digitization --------------------------------------------------------
  FairRunAna *fRun = new FairRunAna();
  fRun->SetSource(new FairFileSource(inFile));
  fRun->SetOutputFile(outFile);
  // ---------------------------------------------------------------------------



  // ----- Connect the Digitization Task ---------------------------------------
  R3BLandDigitizer *landDigitizer = new R3BLandDigitizer(0);
  landDigitizer->SetSaturationCoefficient(saturation);
  landDigitizer->SetTOFRange(integration);
  fRun->AddTask(landDigitizer);
  // ---------------------------------------------------------------------------



  // ----- Find clusters -------------------------------------------------------
  R3BNeuLandClusterFinder *landClusterFinder = new R3BNeuLandClusterFinder(0);
  fRun->AddTask(landClusterFinder);
  // ---------------------------------------------------------------------------



  // ----- Calibration of 2D cuts ----------------------------------------------
  R3BNeutronCalibr2D *calibr = new R3BNeutronCalibr2D();
  calibr->UseBeam(beamE);
  fRun->AddTask(calibr);
  // ---------------------------------------------------------------------------



  // ----- Runtime DataBase info -----------------------------------------------
  FairRuntimeDb *rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo *parIo1 = new FairParRootFileIo();
  parIo1->open(parFile.Data());
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ---------------------------------------------------------------------------


  // ----- Intialise and run ---------------------------------------------------
  fRun->Init();
  fRun->Run(0, n_events);
  delete fRun;
  // ---------------------------------------------------------------------------



  // ----- Finish --------------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file writen: " << outFile << endl;
  cout << "Parameter file writen " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ---------------------------------------------------------------------------
}
