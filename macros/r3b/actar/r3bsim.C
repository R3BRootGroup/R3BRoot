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

void r3bsim()
{
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
  detGeo.Add(new TObjString("ACTAR"),         new TObjString("actar_arco2.geo.root"));

  // Number of events
  Int_t nEvents = 10;
  
  // Event display (store trajectories)
  Bool_t fEventDisplay = kTRUE;
  
  // Magnet Field definition
  Bool_t fR3BMagnet = kTRUE;

  // Main Sim function call
  r3ball(nEvents, &detGeo, target4, fEventDisplay, fMC, fGene, fUserPList,
         fR3BMagnet, 1500.,
         OutFile, ParFile);
}

