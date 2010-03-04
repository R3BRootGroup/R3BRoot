
void landDigi(){

  // Input files
   TString inFile      = "r3bsim.root";
   TString parFile     = "r3bpar.root";
//  TString inFile      = "/data.local2/mheil/fairroot_results/r3bsim800MeVgeant3hadr5.root"; 
//  TString parFile     = "/data.local2/mheil/fairroot_results/r3bpar800MeVgeant3hadr5.root";
  // Output file         
  TString outFile     = "land_digi.root";
//  TString outFile     = "/data.local2/mheil/fairroot_results/land_digi800MeVgeant3hadr5.root";


  
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  gSystem->Load("libGenVector");

  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");


  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libGen");

  //----  Load R3B specific libraries ---------------------------------------
  gSystem->Load("libR3Bbase");
  gSystem->Load("libR3BGen");
  gSystem->Load("libR3BPassive");
  gSystem->Load("libR3BData");
  gSystem->Load("libR3BLand");




  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Digitization run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);

  // Verbosity Mode level
  // (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  //Connect the Digitization  Task
  R3BLandDigitizer_CFD* land  = new R3BLandDigitizer_CFD(iVerbose);
	double triggerThreshold=0.3;
	double timeShift=2;
	double amplitudeScaling=0.7;
	land->SetCfdParameters(triggerThreshold, timeShift,amplitudeScaling);			//Use CFD
//	land->SetLeParameters(triggerThreshold);																		//Use LE
  fRun->AddTask(land);

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
