#include "../../r3ball.C"

void r3blandsim(
  const Int_t n_neutrons,
  const Int_t n_events,
  const TString neuland_geo_file,
  const TString input_file,
  const TString basename
){

  // Output files
  const TString out_file = "r3bsim." + basename + ".root";
  const TString par_file = "r3bpar." + basename + ".root";

  // Constant configuration
  const Int_t    nEvents = 10000;
  const TString  target = "LiH";
  const TString  fMC   = "TGeant3";
  const TString  fGene = "ascii";
  const Bool_t   fEventDisplay = kTRUE;
  const Bool_t   fUserPList = kTRUE;
  const Bool_t   fR3BMagnet = kTRUE;
  const Double_t fMeasCurrent = 2500.; 

  // Geometry
  TMap geometry;
  geometry.Add(new TObjString("TARGET"),        new TObjString("target_" + target + ".geo.root"));
  geometry.Add(new TObjString("ALADIN"),        new TObjString("aladin_v13a.geo.root"));
  geometry.Add(new TObjString("CRYSTALBALL"),   new TObjString("cal_v13a.geo.root"));
  geometry.Add(new TObjString("DCH"),           new TObjString("dch_v13a.geo.root"));
  geometry.Add(new TObjString("SCINTNEULAND"),  new TObjString(TString(neuland_geo_file)));


  // Main Sim function call
  r3ball(nEvents, &geometry, target, fEventDisplay, fMC, fGene, fUserPList, fR3BMagnet, fMeasCurrent, out_file, par_file, input_file);
}
