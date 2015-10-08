#include "Rtypes.h"
#include "TString.h"
#include "TObjString.h"
#include "TMap.h"
#include "TROOT.h"

void R3BNeulandSim(
  const UInt_t num_neutrons,
  const UInt_t num_events,
  const TString input_file,
  const TString base_name,
  const TString base_path = "."
)
{
  // Output files
  const TString out_file = base_path + "/r3bsim." + base_name + ".root";
  const TString par_file = base_path + "/r3bpar." + base_name + ".root";

  // Constant configuration
  const TString  target = "LiH";
  const TString  mc_engine = "TGeant3";
  const TString  generator_engine = "ascii";
  const Bool_t   enable_event_display = kTRUE;
  const Bool_t   enable_user_physics_list = kTRUE;
  const Bool_t   enable_R3B_magnet = kTRUE;
  const Double_t measured_current = 2500.;

  // Geometry
  TMap geometry;
  geometry.Add(new TObjString("TARGET"),        new TObjString("target_" + target + ".geo.root"));
  geometry.Add(new TObjString("ALADIN"),        new TObjString("aladin_v13a.geo.root"));
  geometry.Add(new TObjString("CRYSTALBALL"),   new TObjString("cal_v13a.geo.root"));
  geometry.Add(new TObjString("DCH"),           new TObjString("dch_v13a.geo.root"));
  geometry.Add(new TObjString("SCINTNEULAND"),  new TObjString("neuland_v12a_14m.geo.root"));

  // Load the Main Simulation macro
  gROOT->LoadMacro(TString(getenv("VMCWORKDIR")) + TString("/macros/r3b/r3ball.C"));

  // Main Sim function call
  r3ball(num_events, &geometry, target, enable_event_display, mc_engine, generator_engine, enable_user_physics_list, enable_R3B_magnet, measured_current, out_file, par_file, input_file);
}
