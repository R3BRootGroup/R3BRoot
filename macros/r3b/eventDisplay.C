
eventDisplay()
{
  TString  InputFile = "r3bsim.root";
  TString  ParFile = "r3bpar.root";
	
  FairRunAna *run = new FairRunAna();
	
  run->SetInputFile(InputFile.Data());
  run->SetOutputFile("test.root");
	
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(ParFile.Data());
  rtdb->setFirstInput(parInput1);
	
  FairEventManager *man= new FairEventManager();
  FairMCTracks *Track =  new FairMCTracks("Monte-Carlo Tracks");
  FairMCPointDraw *LandPoint = new FairMCPointDraw("LandPoint", kOrange, kFullSquare);
  
  man->AddTask(Track);
  man->AddTask(LandPoint);
  
	man->Init(1,4,10000);
}
