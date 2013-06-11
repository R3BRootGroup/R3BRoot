digiAnalyser()
{

  // Input files
	TString inFile      = "r3bsim.root";
	TString digiFile    = "land_digi.root";
	TString parFile     = "r3bpar.root";

	//Output file
  TString outFile     = "landAnalyser.root";


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
