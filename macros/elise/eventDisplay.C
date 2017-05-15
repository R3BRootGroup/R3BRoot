eventDisplay()
{
   // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libGen");

  //----  Load R3B specific libraries ---------------------------------------
  gSystem->Load("libR3Bbase");
  gSystem->Load("libR3BPassive");
  gSystem->Load("libR3BGen");
  gSystem->Load("libR3BData");
  gSystem->Load("libR3BXBall");
  gSystem->Load("libR3BCalifa");
  gSystem->Load("libR3BDch");
  gSystem->Load("libR3BGfi");
  gSystem->Load("libR3BLand");
  gSystem->Load("libR3BmTof");
  gSystem->Load("libR3BTof");
  gSystem->Load("libR3BTra");
  
  
  //----  Load vis. specific libraries ---------------------------------------
  gSystem->Load("libEve");
  gSystem->Load("libEventDisplay");

                                     
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
 
 
  TFile* file = new TFile("elipar.root");
  file->Get("FairBaseParSet"); 
 
  fRun->SetInputFile("elisim.root");

  fRun->SetOutputFile("test.root");
  fRun->LoadGeometry();

  FairEventManager *fMan= new FairEventManager();
  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
  FairMCPointDraw *LandPoints =   new FairMCPointDraw ("LandPoint",kOrange,  kFullSquare);

                                                               
  fMan->AddTask(Track);
  
  fMan->AddTask(LandPoints);
    
  fMan->Init();                     
  

}
