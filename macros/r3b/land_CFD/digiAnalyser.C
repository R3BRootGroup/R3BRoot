digiAnalyser()
{

  // Input files
	TString inFile      = "r3bsim.root";
	TString digiFile    = "land_digi.root";
	TString parFile     = "r3bpar.root";

	//Output file
  TString outFile     = "landAnalyser.root";


   // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libGen");

  //----  Load R3B specific libraries ---------------------------------------
  gSystem->Load("libR3Bbase");
  gSystem->Load("libR3BPassive");
  gSystem->Load("libR3BGen");
  gSystem->Load("libR3BData");
  gSystem->Load("libR3BCal");
  gSystem->Load("libR3BCalo");
  gSystem->Load("libR3BDch");
  gSystem->Load("libR3BGfi");
  gSystem->Load("libR3BLand");
  gSystem->Load("libR3BmTof");
  gSystem->Load("libR3BTof");
  gSystem->Load("libR3BTra");
  
                                     
  // -----   Reconstruction run   -------------------------------------------
	cout << "Reconstruction run...\n";
  FairRunAna *fRun= new FairRunAna();
 // fRun->SetInputFile(inFile);
//	fRun->AddFriend(digiFile);
	fRun->SetInputFile(digiFile);
	fRun->AddFriend(inFile);
  fRun->SetOutputFile(outFile);

  // Verbosity Mode level
  // (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 3;

	R3BLandDigiAnalyser* analyser = new R3BLandDigiAnalyser(iVerbose);
  analyser->acceptEmptyEvents(false);
	fRun->AddTask(analyser);

  // Runtime DataBase info
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open(parFile.Data());
  rtdb->setFirstInput(parIo1);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();

  // Load the Root Geometry
  fRun->LoadGeometry();
  
 Int_t firstEvent= 0; 
 Int_t lastEvent = 100;
 

  // -----   Intialise and run   --------------------------------------------
	cout << "init...\n";
  fRun->Init();

  fRun->Run(firstEvent, lastEvent);

	// ----- Finnish ----------------------------------------------------------


}
