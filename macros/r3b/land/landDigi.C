void landDigi(){

  // Input files
   TString inFile1="/data.local2/mheil/fairroot_results/comp_RPC/r3bsim_2x2x2.5m_35m_131Sn_1n_1000AMeV_100keV.root";
//   TString inFile1      = "r3bsim.root";
   TString parFile="/data.local2/mheil/fairroot_results/comp_RPC/r3bpar_2x2x2.5m_35m_131Sn_1n_1000AMeV_100keV.root";
//   TString parFile     = "/data.local2/mheil/fairroot_results/land/r3bpar_10m_land_1n_170MeV.root";
//   TString parFile     = "r3bpar.root";
  // Output file         
  TString outFile ="/data.local2/mheil/fairroot_results/comp_RPC/land_digi_2x2x2.5m_35m_131Sn_1n_1000AMeV_100keV.root";
//  TString outFile     = "land_digi.root";

 
 
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Digitization run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetSource(new FairFileSource(inFile1));
//  fRun->AddFile(inFile2);
//  fRun->AddFile(inFile3);
//  fRun->AddFile(inFile4);
  fRun->SetOutputFile(outFile);



  // Verbosity Mode level
  // (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;
  //Connect the Digitization  Task
  R3BLandDigitizer* land  = new R3BLandDigitizer();
  //Connect the Tracking Task
  R3BNeutronTracker* tracker  = new R3BNeutronTracker();

  // -----   beam energy per nucleon  --------------------------------------

//  Double_t beamEnergy=200.;
//  Double_t beamBeta=0.5676881;

//  Double_t beamEnergy=600.;
//  Double_t beamBeta=0.7937626;

  Double_t beamEnergy=1000.;
  Double_t beamBeta=0.8760237;



  ((R3BNeutronTracker*) tracker)->UseBeam(beamEnergy,beamBeta);

  fRun->AddTask(land);
  fRun->AddTask(tracker);

  // Runtime DataBase info
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile.Data());
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();

  // Load the Root Geometry
  fRun->LoadGeometry();
  
  // Number of events to process
  Int_t nEvents = 10000;
  
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);

  delete fRun;

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file writen:  "    << outFile << endl;
  cout << "Parameter file writen " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}
