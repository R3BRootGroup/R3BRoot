{
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  
  // -----   Runtime database   ---------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo*  parIo1 = new FairParRootFileIo();
  //parIo1->open("r3blandpar.root");
  parIo1->open("r3bpar.600AMeV.1n.500keV.15m.root");
  rtdb->setFirstInput(parIo1);
  rtdb->print();
 
  //fRun->SetInputFile("r3blandsim.root");
  fRun->SetInputFile("r3bsim.600AMeV.1n.500keV.15m.root");
//  fRun->AddFriend("land_digi.root"); // 2nd file where LandHits or whatever are stored
  fRun->SetOutputFile("test.root");

  R3BEventManager *fMan= new R3BEventManager();
  R3BMCTracks *Track =  new R3BMCTracks ("Monte-Carlo Tracks");
  FairMCPointDraw *LandPoints =   new FairMCPointDraw ("LandPoint", kOrange,  kFullSquare);
//  FairHitDraw *landRecoHit = new FairHitDraw("LandNeTracks");
//  FairMCPointDraw *landRecoHit =   new FairMCPointDraw ("LandNeTracks",kGreen,  kFullSquare);

  fMan->AddTask(Track);
  fMan->AddTask(LandPoints);
//  fMan->AddTask(landRecoHit);  
  
  fMan->Init();  
}
