//--------------------------------------------------------------------
//
// Define the R3B simulation Setup
// Steering macros to main r3ball_batch.C
// Author: Hector Alvarez <hector.alvarez@usc.es>
//
// Last Update: 03/09/2015
// Comments: Changes for root6, pablo.cabanelas@usc.es
//
//
//--------------------------------------------------------------------

#include "r3ball_batch.C"

using namespace std;
using std::cout;

void r3bsim_batch(Double_t fEnergyP=0.005, Int_t fMult=1, Int_t nEvents=5, Int_t fGeoVer=11, Double_t fNonUni=1.)
{

  cout << "Running r3bsim_batch with arguments:" <<endl;
  cout << "Primary energy: " << fEnergyP << " MeV" <<endl;
  cout << "Multiplicity: " << fMult <<endl;
  cout << "Number of events: " << nEvents <<endl;
  cout << "CALIFA geo version: " << fGeoVer <<endl;
  cout << "Non uniformity: " << fNonUni <<endl<<endl;

  // Output files
  TString OutFile = "r3bsim.root";
  TString ParFile = "r3bpar.root";
  
  //-------------------------------------------------
  // Monte Carlo type     |    fMC        (TString)
  //-------------------------------------------------
  //   Geant3:                 "TGeant3"
  //   Geant4:                 "TGeant4"
  //   Fluka :                 "TFluka"
  TString fMC = "TGeant4";
  
  //-------------------------------------------------
  // Primaries generation
  // Event Generator Type |   fGene       (TString)
  //-------------------------------------------------
  // Box generator:             "box"
  // ASCII generator            "ascii"
  // R3B spec. generator:       "r3b"
  TString fGene = "gammas";
  
  //-------------------------------------------------
  // Secondaries  generation (G4 only)
  // R3B Spec. PhysicList |     fUserPList (Bool_t)
  // ----------------------------------------------
  //     VMC Standard           kFALSE
  //     R3B Special            kTRUE;
  Bool_t fUserPList = kTRUE;

  // Target type
  TString target1 = "LeadTarget";
  TString target2 = "Para";
  TString target3 = "Para45";
  TString target4 = "LiH";
  
  //-------------------------------------------------
  //- Geometrical Setup Definition
  //-  Non Sensitiv        |    fDetName (String)
  //-------------------------------------------------
  //   Target:                  TARGET
  //   Magnet:                  ALADIN
  //                            GLAD
  //-------------------------------------------------
  //-  Sensitiv            |    fDetName
  //-------------------------------------------------
  //   Calorimeter:             CRYSTALBALL
  //                            CALIFA
  //
  //   Tof:                     TOF
  //                            MTOF
  //
  //   Tracker:                 DCH
  //                            TRACKER
  //                            STaRTrack
  //                            GFI
  //
  //   Neutron:                 LAND
  //                            SCINTNEULAND
  TMap detGeo;
  detGeo.Add(new TObjString("TARGET"),        new TObjString("target_"+target4+".geo.root"));
  //detGeo.Add(new TObjString("ALADIN"),        new TObjString("aladin_v13a.geo.root"));
  //detGeo.Add(new TObjString("GLAD"),          new TObjString("glad_v17_flange.geo.root"));
  //detGeo.Add(new TObjString("CRYSTALBALL"),   new TObjString("cal_v13a.geo.root"));
  //detGeo.Add(new TObjString("CALIFA"),        new TObjString("califa_v14a.geo.root"));
  detGeo.Add(new TObjString("CALIFA"),        new TObjString("califa_v13_811.geo.root"));
  //detGeo.Add(new TObjString("TOF"),           new TObjString("tof_v13a.geo.root"));
  //detGeo.Add(new TObjString("MTOF"),          new TObjString("mtof_v13a.geo.root"));
  //detGeo.Add(new TObjString("DTOF"),          new TObjString("dtof_v15a.geo.root"));
  //detGeo.Add(new TObjString("DCH"),           new TObjString("dch_v13a.geo.root"));
  detGeo.Add(new TObjString("TRACKER"),       new TObjString("tra_v13vac.geo.root"));
  //detGeo.Add(new TObjString("STaRTrack"),     new TObjString("startra_v14a.geo.root"));
  //detGeo.Add(new TObjString("STaRTrack"),     new TObjString("startra_v13a.geo.root"));
  //detGeo.Add(new TObjString("GFI"),           new TObjString("gfi_v13a.geo.root"));
  //detGeo.Add(new TObjString("LAND"),          new TObjString("land_v12a_10m.geo.root"));
  //detGeo.Add(new TObjString("SCINTNEULAND"),  new TObjString("neuland_v12a_14m.geo.root"));
  //detGeo.Add(new TObjString("VACVESSELCOOL"), new TObjString("vacvessel_v14a.geo.root"));   // to be used only with the R3B tracker: "STaRTrack", not "TRACKER"
  //detGeo.Add(new TObjString("VACVESSELCOOL"), new TObjString("vacvessel_v13a.geo.root"));
  //detGeo.Add(new TObjString("MFI"),           new TObjString("mfi_v13a.geo.root"));
  //detGeo.Add(new TObjString("PSP"),           new TObjString("psp_v13a.geo.root"));

  // Event display (store trajectories)
  Bool_t fEventDisplay = kFALSE;
  
  // Magnet Field definition
  Bool_t fR3BMagnet = kTRUE;

  // Main Sim function call
  r3ball_batch(nEvents, &detGeo, target4, fEventDisplay, fMC, fGene, fUserPList,
         fR3BMagnet, 1500.,OutFile, ParFile,"",
	 fEnergyP,fMult,fGeoVer,fNonUni);
}
