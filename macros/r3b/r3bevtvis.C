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
  rtdb->print();
  
  fRun->SetInputFile("r3bsim.root");
  fRun->SetOutputFile("test.root");
  //fRun->LoadGeometry();
  
  R3BEventManager *fMan= new R3BEventManager();
  R3BMCTracks *Track =  new R3BMCTracks ("Monte-Carlo Tracks");
  
  fMan->AddTask(Track);
  fMan->Init();
}
