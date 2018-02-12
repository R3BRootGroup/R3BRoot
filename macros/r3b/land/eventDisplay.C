{
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  
  // -----   Runtime database   ---------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  parIo1->open("par.600AMeV.4n.500keV.14m.root");
  rtdb->setFirstInput(parIo1);
  rtdb->print();
 
  fRun->SetSource(new FairFileSource("sim.600AMeV.4n.500keV.14m.root"));
  fRun->SetOutputFile("test.root");

  R3BEventManager *fMan= new R3BEventManager();
  R3BMCTracks *Track =  new R3BMCTracks ("Monte-Carlo Tracks");
//  FairMCPointDraw *LandPoints =   new FairMCPointDraw ("LandPoint", kOrange,  kFullSquare);
//  FairHitDraw *landRecoHit = new FairHitDraw("LandNeTracks");
//  FairMCPointDraw *landRecoHit =   new FairMCPointDraw ("LandNeTracks",kGreen,  kFullSquare);

  fMan->AddTask(Track);
//  fMan->AddTask(LandPoints);
//  fMan->AddTask(landRecoHit);  
  
  fMan->Init();  
}
