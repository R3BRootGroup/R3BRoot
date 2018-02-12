eventDisplay()
{
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
 

//  TFile* file = new TFile("r3bpar.root");
  //TFile* file = new TFile("/lustre/land/jmargan/simulation/Ox_1p/r3bpar_bug.root");
  TFile* file = new TFile("/u/fwamers/Promotion/L_Analysis/s318/qfs_analysis/ne17/sim_r3broot/output/r3bpar.root");
  file->Get("FairBaseParSet"); 

 // -----   Runtime database   ---------------------------------------------

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parIn = new FairParRootFileIo();
//  parIn->open("r3bpar.root");
  //parIn->open("/lustre/land/jmargan/simulation/Ox_1p/r3bpar_bug.root");
  parIn->open("/u/fwamers/Promotion/L_Analysis/s318/qfs_analysis/ne17/sim_r3broot/output/r3bpar.root");
  rtdb->print();


 //-------

  //FairFileSource* source = new FairFileSource("r3bsim.root");
  //source->AddFriend("land_digi.root");
  //FairFileSource* source = new FairFileSource("/lustre/land/jmargan/simulation/Ox_1p/r3bsim_bug.root");
  FairFileSource* source = new FairFileSource("/u/fwamers/Promotion/L_Analysis/s318/qfs_analysis/ne17/sim_r3broot/output/r3bsim.root");
  //FairFileSource* source = new FairFileSource("r3bsim_1000ev_01-30en.root");

  fRun->SetSource(source);
 
//  fRun->SetOutputFile("test.root");
  //fRun->SetOutputFile("/lustre/land/jmargan/simulation/Ox_1p/test.root");
  fRun->SetOutputFile("/u/fwamers/Promotion/L_Analysis/s318/qfs_analysis/ne17/sim_r3broot/output/test.root");
  //fRun->LoadGeometry();

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
