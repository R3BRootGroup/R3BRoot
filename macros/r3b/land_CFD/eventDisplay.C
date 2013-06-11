eventDisplay()
{
  // -----   Reconstruction run   -------------------------------------------
	cout << "Reconstruction run...\n";
  FairRunAna *fRun= new FairRunAna();
 
  TFile* file = new TFile("r3bpar.root");

  file->Get("FairBaseParSet"); 		//Loads all geometry and information about the run



 // -----   Runtime database   ---------------------------------------------

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parIn = new FairParRootFileIo();
  parIn->open("r3bpar.root");
  rtdb->print();


 //-------
 	cout << "Event Manager...\n";
  fRun->SetInputFile("r3bsim.root");

  fRun->SetOutputFile("test.root");
  fRun->LoadGeometry();

  FairEventManager *fMan= new FairEventManager();
  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
  FairMCPointDraw *LandPoints =   new FairMCPointDraw ("LandPoint",kOrange,  kFullSquare);

                                                               
  fMan->AddTask(Track);
 
  fMan->AddTask(LandPoints);
 
	cout << "Initialize EventManager...\n";
  fMan->Init();                     
  

}
