//--------------------------------------------------------------------
//
// Define the R3B simulation Setup
// Steering macros to main r3ball.C
// Author: <D.Bertini@gsi.de>
//
// Last Update: 30/06/12 (marc labiche)
// Comments:
//         - 30/06/12 : Adding option to use the vaccum vessel and
//           cooling for new Si Tracker
//         - 17/08/09 : Adding detector list
//         - 13/08/09 : adding flag for User Physics List
//
//
//--------------------------------------------------------------------

#include "r3ball.C"

void r3bsim_new()
{
  // Output files
  TString OutFile = "r3bsim_new.root";
  TString ParFile = "r3bpar_new.root";
  
  //-------------------------------------------------
  // Monte Carlo type     |    fMC        (TString)
  //-------------------------------------------------
  //   Geant3:                 "TGeant3"
  //   Geant4:                 "TGeant4"
  //   Fluka :                 "TFluka"
  TString fMC = "TGeant3";
  
  //-------------------------------------------------
  // Primaries generation
  // Event Generator Type |   fGene       (TString)
  //-------------------------------------------------
  // Box generator:             "box"
  // ASCII generator            "ascii"
  // R3B spec. generator:       "r3b"
  TString fGene = "box";
  
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
//  detGeo.Add(new TObjString("ALADIN"),        new TObjString("aladin_v13a.geo.root"));
  detGeo.Add(new TObjString("GLAD"),          new TObjString("glad_v17_flange.geo.root"));
//  detGeo.Add(new TObjString("CRYSTALBALL"),   new TObjString("cal_v13a.geo.root"));
//  detGeo.Add(new TObjString("CALIFA"),        new TObjString("califa_v14a.geo.root"));
  detGeo.Add(new TObjString("CALIFA"),        new TObjString("califa_10_v8.11.geo.root"));
//  detGeo.Add(new TObjString("TOF"),           new TObjString("tof_v13a.geo.root"));
  detGeo.Add(new TObjString("MTOF"),          new TObjString("mtof_v13a.geo.root"));
//  detGeo.Add(new TObjString("DTOF"),          new TObjString("dtof_v15a.geo.root"));
//  detGeo.Add(new TObjString("DCH"),           new TObjString("dch_v13a.geo.root"));
//  detGeo.Add(new TObjString("TRACKER"),       new TObjString("tra_v13vac.geo.root"));
  detGeo.Add(new TObjString("STaRTrack"),     new TObjString("startra_v14a.geo.root"));
//  detGeo.Add(new TObjString("STaRTrack"),     new TObjString("startra_v13a.geo.root"));
//  detGeo.Add(new TObjString("GFI"),           new TObjString("gfi_v13a.geo.root"));
//  detGeo.Add(new TObjString("LAND"),          new TObjString("land_v12a_10m.geo.root"));
  detGeo.Add(new TObjString("SCINTNEULAND"),  new TObjString("neuland_v12a_14m.geo.root"));
//  detGeo.Add(new TObjString("VACVESSELCOOL"), new TObjString("vacvessel_v14a.geo.root"));   // to be used only with the R3B tracker: "STaRTrack", not "TRACKER"
//  detGeo.Add(new TObjString("VACVESSELCOOL"), new TObjString("vacvessel_v13a.geo.root"));
  detGeo.Add(new TObjString("MFI"),           new TObjString("mfi_v13a.geo.root"));
  detGeo.Add(new TObjString("PSP"),           new TObjString("psp_v13a.geo.root"));

  // Number of events
  Int_t nEvents = 1;
  
  // Event display (store trajectories)
  Bool_t fEventDisplay = kTRUE;
  
  // Magnet Field definition
  Bool_t fR3BMagnet = kTRUE;

  // Main Sim function call
  r3ball(nEvents, &detGeo, target4, fEventDisplay, fMC, fGene, fUserPList,
         fR3BMagnet, 2100.,
         OutFile, ParFile, "", 335566);
}
