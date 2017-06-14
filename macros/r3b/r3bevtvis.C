void r3bevtvis()
{
  FairRunAna *fRun = new FairRunAna();
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open("par.root");
  rtdb->setFirstInput(parIo1);
  rtdb->print();
  
  fRun->SetInputFile("sim.root");
  fRun->SetOutputFile("test.root");
  
  R3BEventManager *fMan  = new R3BEventManager();
  R3BMCTracks     *Track = new R3BMCTracks ("Monte-Carlo Tracks");

  fMan->AddTask(Track);
  
  fMan->Init();
}
