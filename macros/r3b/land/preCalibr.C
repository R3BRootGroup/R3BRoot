void preCalibr(Int_t beamE, Int_t nn, Int_t erel, Int_t d)
{
  // ----- Files ---------------------------------------------------------------
  char strDir[] = "/Users/kresan/neuland";
  char str[100];
  sprintf(str, "%1dAMeV.%1dn.%1dkeV.%1dm", beamE, nn, erel, d);
  TString inFile  = TString(strDir) + "/r3bsim." + TString(str) + ".root";
  TString parFile  = TString(strDir) + "/r3bpar." + TString(str) + ".root";
  TString outFile  = TString(strDir) + "/r3bcalibr." + TString(str) + ".root";
  // ---------------------------------------------------------------------------



  // ----- Load libraries ------------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGenVector");
  gSystem->Load("libGeoBase");
  gSystem->Load("libFairDB");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libR3Bbase");
  gSystem->Load("libR3BGen");
  gSystem->Load("libR3BPassive");
  gSystem->Load("libR3BData");
  gSystem->Load("libR3BLand");
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



  // ----- Number of events to process -----------------------------------------
  Int_t nEvents = 10000;
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
