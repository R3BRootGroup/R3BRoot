r3bevtvis()
{
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  TFile* file = new TFile("r3bpar.root");
  file->Get("FairBaseParSet"); 
  
  // -----   Runtime database   ---------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parIn = new FairParRootFileIo();
  parIn->open("r3bpar.root");
  rtdb->setFirstInput(parIn);
  rtdb->print();
  
  fRun->SetInputFile("r3bsim.root");
  fRun->SetOutputFile("test.root");
  
  R3BEventManager *fMan= new R3BEventManager();
  R3BMCTracks *Track =  new R3BMCTracks ("Monte-Carlo Tracks");
  R3BCaloEventDisplay *CaloEvtVis = new R3BCaloEventDisplay("R3BCaloEventDisplay");
  R3BCaloHitEventDisplay *CaloHitEvtVis = new R3BCaloHitEventDisplay("R3BCaloHitEventDisplay");
  
  fMan->AddTask(CaloEvtVis);
  fMan->AddTask(CaloHitEvtVis);
  fMan->AddTask(Track);
  fMan->Init();
}
