//--------------------------------------------------------------------
//
// Define the R3B simulation Setup
// Steering macros to main r3ball.C
// Author: Hector Alvarez <hector.alvarez@usc.es>
//
// Last Update: 13/09/2013
// Comments:
//
//
//--------------------------------------------------------------------


Int_t califasim()
{
  // Load the Main Simulation macro
  TString macro_r3ball= getenv("VMCWORKDIR");
  macro_r3ball+="/macros/r3b/califa/r3ball.C";
  gROOT->LoadMacro(macro_r3ball.Data());
  
  
  // Output files
  TString OutFile = "r3bsim.root";
  TString ParFile = "r3bpar.root";
  
  
  //-------------------------------------------------
  // Monte Carlo type     |    fMC        (TString)
  //-------------------------------------------------
  //   Geant3:                 "TGeant3"
  //   Geant4:                 "TGeant4"
  //   Fluka :                 "TFluka"
  TString fMC ="TGeant4";
  
  
  //-------------------------------------------------
  // Primaries generation
  // Event Generator Type |   fGene       (TString)
  //-------------------------------------------------
  // Box generator:             "box"
  // CALIFA generator:          "gammas"
  // R3B spec. generator:       "r3b"
  TString fGene="gammas";
  
  
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
  //                            GLAD
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
  //   Neutron:                 LAND
  TMap detGeo;
  detGeo.Add(new TObjString("CALIFA"), new TObjString("califa_v13_811.geo.root"));

  
  //-------------------------------------------------
  //- N# of Sim. Events   |    nEvents     (Int_t)
  //-------------------------------------------------
  Int_t nEvents = 10;
  
  
  //-------------------------------------------------
  //- EventDisplay        |    fEventDisplay (Bool_t)
  //-------------------------------------------------
  //   connected:              kTRUE
  //   not connected:          kFALSE
  Bool_t fEventDisplay=kTRUE;
  
  
  // Magnet Field definition
	//Bool_t fR3BMagnet = kTRUE;
	Bool_t fR3BMagnet = kFALSE;

	
  // Main Sim function call
  r3ball(nEvents, detGeo, target2, fEventDisplay, fMC, fGene, fUserPList,
         fR3BMagnet, kFALSE, 2000., OutFile, ParFile);
}
