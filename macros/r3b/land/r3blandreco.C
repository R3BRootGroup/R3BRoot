void r3blandreco(Int_t nNeutrons = 4, Int_t nEvents = 100, Int_t beamE = 600, Int_t Erel = 500)
{
  Int_t d;
  if(Erel == 100){
    d = 35;
  }
  else{
    d = 14;
  }
  
  // ----- Files ---------------------------------------------------------------
  char strDir[] = ".";
  char str[100];
  char str2[100];
  sprintf(str, "%1dAMeV.%1dn.%1dkeV.%1dm.root", beamE,nNeutrons, Erel, d);
  sprintf(str2, "%1dAMeV.%1dkeV.%1dm", beamE, Erel, d);
  TString inFile  = TString(strDir) + "/sim." + TString(str);
  TString digiFile  = TString(strDir) + "/r3bcalibr." + TString(str);
  TString parFile  = TString(strDir) + "/par." + TString(str);
  TString calibrFile  = TString(strDir) + "/r3bcalibr." + TString(str2) + ".txt";
  TString outFile  = TString(strDir) + "/r3breco." + TString(str);
  // ---------------------------------------------------------------------------



  // ----- Timer ---------------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ---------------------------------------------------------------------------



  // ----- Digitization --------------------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  FairFileSource* source = new FairFileSource(inFile);
  source->AddFriend(digiFile);
  fRun->SetSource(source);
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

  // Use only one of the following options:

  // Option 1 - use 2D cuts from file, produced out of set of 4 event classes.
  tracker->ReadCalibrFile((char*)calibrFile.Data());

  // Option 2 - disable cuts for determination of number of neutrons.
  // Manually set number of incident neutrons (for test purpose).
  //tracker->Disable2DEventCut(4);

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
  if(tracker->GetNTracks4() > (0.5*nEvents))
  {
    cout << " Reconstruction successful" << endl;
    cout << " All ok " << endl;
  }
  // ---------------------------------------------------------------------------

  delete fRun;
}
