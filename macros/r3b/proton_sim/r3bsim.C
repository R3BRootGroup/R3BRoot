//--------------------------------------------------------------------
//
// Define the R3B simulation Setup
// Steering macros to main r3ball.C
// Author: <D.Bertini@gsi.de>
//
// Last Update: 17/08/09
// Comments:
//         - 17/08/09 : Adding detector list
//         - 13/08/09 : adding flag for User Physics List
//
//
//--------------------------------------------------------------------

Int_t r3bsim()
{
  // Load the Main Simulation macro
  TString macro_r3ball = getenv("VMCWORKDIR");
  macro_r3ball += "/macros/r3b/r3ball.C";
  gROOT->LoadMacro(macro_r3ball.Data());
  
  // Ascii input file
  TString evtFile = "evt_gen.dat";
  TString iFile = "/lustre/land/jmargan/simulation/Ox_1p/" + evtFile;
  
  // Output files
  TString OutFile = "/lustre/land/jmargan/simulation/Ox_1p/r3bsim_bug.root";
  TString ParFile = "/lustre/land/jmargan/simulation/Ox_1p/r3bpar_bug.root";
  
  // ALADIN current
  Double_t fMeasCurrent = 1797.;// I_current [A]  number from s318_172.txt file
  
  //-------------------------------------------------
  // Monte Carlo type     |    fMC        (TString)
  //-------------------------------------------------
  //   Geant3:                 "TGeant3"
  //   Geant4:                 "TGeant4"
  //   Fluka :                 "TFluka"
  TString fMC ="TGeant3";
  
  //-------------------------------------------------
  // Primaries generation
  // Event Generator Type |   fGene       (TString)
  //-------------------------------------------------
  // Box generator:             "box"
  // Ascii generator:           "ascii"
  // R3B spec. generator:       "r3b"
  TString fGene="ascii";
  
  //-------------------------------------------------
  // Secondaries  generation (G4 only)
  // R3B Spec. PhysicList |     fUserPList (Bool_t)
  // ----------------------------------------------
  //     VMC Standard           kFALSE
  //     R3B Special            kTRUE;
  Bool_t fUserPList= kTRUE;
  
  // Target type
  TString target1="LeadTarget";
  TString target2="Para";
  TString target3="Para45";
  TString target4="LiH";
  
  //-------------------------------------------------
  //- Geometrical Setup Definition
  //-  Non Sensitiv        |    fDetName (String)
  //-------------------------------------------------
  //   Target:                  TARGET
  //   Magnet:                  ALADIN
  //
  //-------------------------------------------------
  //-  Sensitiv            |    fDetName
  //-------------------------------------------------
  //   Calorimeter:             CALIFA
  //                            CRYSTALBALL
  //
  //   Tof:                     TOF
  //                            MTOF
  //
  //   Tracker:                 DCH
  //                            TRACKER
  //                            GFI
  //
  //   Neutron Detector
  //   Plastic                  LAND
  //   RPC
  //                            RPCFLAND
  //                            RPCMLAND
  TMap detGeo;
  detGeo.Add(new TObjString("TARGET"),        new TObjString("target_"+target1+".geo.root"));
  detGeo.Add(new TObjString("ALADIN"),        new TObjString("aladin_v13a.geo.root"));
  detGeo.Add(new TObjString("CRYSTALBALL"),   new TObjString("cal_v13a.geo.root"));
  detGeo.Add(new TObjString("TOF"),           new TObjString("tof_v13a.geo.root"));
  detGeo.Add(new TObjString("MTOF"),          new TObjString("mtof_v13a.geo.root"));
  detGeo.Add(new TObjString("DCH"),           new TObjString("dch_v13a.geo.root"));
  detGeo.Add(new TObjString("TRACKER"),       new TObjString("tra_v13vac.geo.root"));
  detGeo.Add(new TObjString("GFI"),           new TObjString("gfi_v13a.geo.root"));
  
  //-------------------------------------------------
  //- N# of Sim. Events   |    nEvents     (Int_t)
  //-------------------------------------------------
  Int_t nEvents = 100; //23347;
  
  //-------------------------------------------------
  //- EventDisplay        |    fEventDisplay (Bool_t)
  //-------------------------------------------------
  //   connected:              kTRUE
  //   not connected:          kFALSE
  Bool_t fEventDisplay=kTRUE;
  
  // Magnet Field definition
  Bool_t fR3BMagnet = kTRUE;
  
  // Main Sim function call
  r3ball(nEvents, detGeo, target1, fEventDisplay, fMC, fGene, fUserPList,
         fR3BMagnet, fMeasCurrent,
         OutFile, ParFile, iFile);
}

