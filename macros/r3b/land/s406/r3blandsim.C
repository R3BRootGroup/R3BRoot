
void r3blandsim(Int_t beam)
{
  // Load the Main Simulation macro
  TString macro_r3ball = getenv("VMCWORKDIR");
  macro_r3ball += "/macros/r3b/r3ball.C";
  gROOT->LoadMacro(macro_r3ball.Data());

  Int_t nEvents = 100000;

  // Files
  char str[100];
  sprintf(str, "%1dAMeV", beam);
  TString InFile  = "/Users/kresan/Dropbox/data/input/signal.p2p." + TString(str) + ".dat";
  TString OutFile = "/Users/kresan/neuland/r3bsim.s406." + TString(str) + ".root";
  TString ParFile = "/Users/kresan/neuland/r3bpar.s406." + TString(str) + ".root";

  TString fMC = "TGeant3";
  TString fGene = "ascii";
  Bool_t fUserPList = kTRUE;

  // Target type
  TString target = "LiH";

  TMap detGeo;
  detGeo.Add(new TObjString("TARGET"),        new TObjString("target_"+target+".geo.root"));
  detGeo.Add(new TObjString("ALADIN"),        new TObjString("aladin_v13a.geo.root"));
  detGeo.Add(new TObjString("CRYSTALBALL"),   new TObjString("cal_v13a.geo.root"));
  detGeo.Add(new TObjString("DCH"),           new TObjString("dch_v13a.geo.root"));
  if(beam < 1200) {
    detGeo.Add(new TObjString("SCINTNEULAND"),  new TObjString("neuland_proto_v13a_5m.geo.root"));
  } else if(beam > 1200) {
    detGeo.Add(new TObjString("SCINTNEULAND"),  new TObjString("neuland_proto_v13a_11.4m.geo.root"));
  }

  Bool_t fEventDisplay = kFALSE;

  // Magnet Field definition
  Bool_t fR3BMagnet = kTRUE;

  // Main Sim function call
  r3ball(nEvents, detGeo, target, fEventDisplay, fMC, fGene, fUserPList,
         fR3BMagnet, 2500., OutFile, ParFile, InFile);
}
