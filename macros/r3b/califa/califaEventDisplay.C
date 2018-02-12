void califaEventDisplay()
{
  FairRunAna *fRun = new FairRunAna();
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open("sim_par.root");
  rtdb->setFirstInput(parIo1);
  rtdb->print();
  
  fRun->SetSource(new FairFileSource("sim_out.root"));
  fRun->SetOutputFile("test.root");
  
  R3BEventManager *fMan  = new R3BEventManager();
  R3BMCTracks     *Track = new R3BMCTracks ("Monte-Carlo Tracks");

  R3BCalifaEventDisplay    *CalifaEvtVis    = new R3BCalifaEventDisplay("R3BCalifaEventDisplay");
  R3BCalifaHitEventDisplay *CalifaHitEvtVis = new R3BCalifaHitEventDisplay("R3BCalifaHitEventDisplay");
  CalifaEvtVis->SelectGeometryVersion(10);
  fMan->AddTask(CalifaEvtVis);
  fMan->AddTask(CalifaHitEvtVis);

  fMan->AddTask(Track);
  
  
  fMan->Init();
}
