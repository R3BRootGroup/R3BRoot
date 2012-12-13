eventDisplay()
{
   // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libGen");

  gSystem->Load("libEve");
  gSystem->Load("libEventDisplay");

  //----  Load R3B specific libraries ---------------------------------------
  gSystem->Load("libR3Bbase");
  gSystem->Load("libR3BGen");
  gSystem->Load("libR3BPassive");
  gSystem->Load("libR3BData");
  gSystem->Load("libR3BCal");
  gSystem->Load("libR3BCalo");
  gSystem->Load("libR3BDch");
  gSystem->Load("libR3BGfi");
  gSystem->Load("libR3BLand");
  gSystem->Load("libR3BmTof");
  gSystem->Load("libR3BTof");
  gSystem->Load("libR3BTra");
  gSystem->Load("libR3BEvtVis");

  //----  Load vis. specific libraries ---------------------------------------


                                     
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
 
//    TFile* file = new TFile("/data.local2/mheil/fairroot_results/comp_RPC/r3bpar_2x2x2.5m_12.5m_128Sn_4n_600AMeV_3000keV.root");
//  TFile* file = new TFile("/data.local2/mheil/fairroot_results/land/r3bpar_10m_land_1n_170MeV.root");
  TFile* file = new TFile("r3bpar.root");
  file->Get("FairBaseParSet"); 

 // -----   Runtime database   ---------------------------------------------

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parIn = new FairParRootFileIo();
//  parIn->open("/data.local2/mheil/fairroot_results/comp_RPC/r3bpar_2x2x2.5m_12.5m_128Sn_4n_600AMeV_3000keV.root");
//  parIn->open("/data.local2/mheil/fairroot_results/land/r3bpar_10m_land_1n_170MeV.root");
  parIn->open("r3bpar.root");
  rtdb->print();


 //-------
 
//  fRun->SetInputFile("/data.local2/mheil/fairroot_results/comp_RPC/r3bsim_2x2x2.5m_12.5m_128Sn_4n_600AMeV_3000keV.root");
//  fRun->SetInputFile("/data.local2/mheil/fairroot_results/land/r3bsim_10m_land_1n_400MeV.root");
 fRun->SetInputFile("r3bsim.root");
//  fRun->AddFriend("land_digi.root"); // 2nd file where LandHits or whatever are stored
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
