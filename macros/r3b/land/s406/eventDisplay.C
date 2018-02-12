
{
  FairRunAna *fRun= new FairRunAna();
 
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open("/Users/kresan/neuland/r3bpar.s406.255AMeV.root");
  rtdb->setFirstInput(parIo1);
  rtdb->print();

  fRun->SetSource(new FairFileSource("/Users/kresan/neuland/r3bsim.s406.255AMeV.root"));
  fRun->SetOutputFile("test.root");

  R3BEventManager *fMan = new R3BEventManager();
  R3BMCTracks *Track = new R3BMCTracks("Monte-Carlo Tracks");
  FairMCPointDraw *LandPoints = new FairMCPointDraw("LandPoint", kOrange,  kFullSquare);
//  fMan->AddTask(Track);
//  fMan->AddTask(LandPoints);
  
  fMan->Init();  
}
