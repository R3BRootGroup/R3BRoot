

{
  // ----- Load libraries ------------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGenVector");
  gSystem->Load("libGeoBase");
  gSystem->Load("libFairDB");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libR3Bbase");
  gSystem->Load("libR3BGen");
  gSystem->Load("libR3BPassive");
  gSystem->Load("libR3BData");
  gSystem->Load("libR3BLand");
  gSystem->Load("libMbsAPI");
  gSystem->Load("libUnpack");
  // ---------------------------------------------------------------------------


  // ---------------------------------------------------------------------------
  FairRemoteSource* source  = new FairRemoteSource("lxi047");
  //FairLmdSource* source  = new FairLmdSource("/Users/kresan/r323_3607.lmd");

  char *strCalDir = "/Users/kresan/r3b/geo/r3broot/calfiles/land/";
  Int_t type = 94;
  Int_t subType = 9400;

  source->AddUnpacker(new R3BLandUnpack(strCalDir, type, subType));
  // ---------------------------------------------------------------------------


  // ---------------------------------------------------------------------------
  FairRunOnline *run = new FairRunOnline(source);
  run->SetOutputFile("land.root");
  // ---------------------------------------------------------------------------


  // ---------------------------------------------------------------------------
  R3BLandRawAna *ana = new R3BLandRawAna();
  run->AddTask(ana);
  // ---------------------------------------------------------------------------


  // ---------------------------------------------------------------------------
  run->Init();
  run->Run(10000, 0);
  // ---------------------------------------------------------------------------
}


