void r3blandreco(Int_t beamE, Int_t nn, Int_t erel, Int_t d)
{
  // ----- Files ---------------------------------------------------------------
  char strDir[] = "/home/kresan/neuland";
  char str[100];
  char str2[100];
  sprintf(str, "%1dAMeV.%1dn.%1dkeV.%1dm", beamE, nn, erel, d);
  sprintf(str2, "%1dAMeV.%1dkeV.%1dm", beamE, erel, d);
  TString inFile  = TString(strDir) + "/r3bsim." + TString(str) + ".root";
  TString digiFile  = TString(strDir) + "/r3bcalibr." + TString(str) + ".root";
  TString parFile  = TString(strDir) + "/r3bpar." + TString(str) + ".root";
  TString calibrFile  = TString(strDir) + "/r3bcalibr." + TString(str2) + ".txt";
  TString outFile  = TString(strDir) + "/r3breco." + TString(str) + ".root";
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
  fRun->AddFriend(digiFile);
  fRun->SetOutputFile(outFile);
  // ---------------------------------------------------------------------------



  // ---------------------------------------------------------------------------
  Double_t beamEnergy;
  Double_t beamBeta;
  if(200 == beamE) {
    beamEnergy=200.;
    beamBeta=0.5676881;
  } else if(600 == beamE) {
    beamEnergy=600.;
    beamBeta=0.7937626;
  } else if(1000 == beamE) {
    beamEnergy=1000.;
    beamBeta=0.8760237;
  }
  // ---------------------------------------------------------------------------
  
  
  
  // ----- Connect the Tracking Task -------------------------------------------
  R3BNeutronTracker2D* tracker  = new R3BNeutronTracker2D();
  tracker->UseBeam(beamEnergy, beamBeta);
  tracker->ReadCalibrFile(calibrFile.Data());
  fRun->AddTask(tracker);
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
