

{
  // ---------------------------------------------------------------------------
  //FairRemoteSource* source  = new FairRemoteSource("lxi047");
  FairLmdSource* source  = new FairLmdSource();
  source->AddFile("/Users/kresan/neuland/r258_2986.lmd");
  source->AddFile("/Users/kresan/neuland/r323_3607.lmd");

  char *strCalDir = "/Users/kresan/r3b/geo/r3broot/calfiles/land/";
  Int_t type = 94;
  Int_t subType = 9400;

  source->AddUnpacker(new R3BLandUnpack(strCalDir, type, subType));
  // ---------------------------------------------------------------------------


  // ---------------------------------------------------------------------------
  FairRunOnline *run = new FairRunOnline(source);
  run->SetOutputFile("land.root");
  run->SetGenerateHtml(kTRUE);
  // ---------------------------------------------------------------------------


  // ---------------------------------------------------------------------------
  R3BLandRawAna *ana = new R3BLandRawAna();
  run->AddTask(ana);
  // ---------------------------------------------------------------------------


  // ---------------------------------------------------------------------------
  run->Init();
  run->Run(1000, 0);
  // ---------------------------------------------------------------------------
}


