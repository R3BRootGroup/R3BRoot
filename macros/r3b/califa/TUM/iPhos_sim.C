//--------------------------------------------------------------------
//
// Define the R3B simulation Setup
// Steering macros to main r3ball.C
// Author: Michael Bendel <Michael.Bendel@tum.de>
//
// Last Update: 06/03/2014
// Comments:
//
//
//--------------------------------------------------------------------

#include "r3ball.C"

void iPhos_sim(){
  
  // Input files
  TString EventFile = "doesn't matter";

  // Output files
  TString OutFile = "testsim.root";
  TString ParFile = "testpar.root";
  
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
  // Events from file:          "ascii"
  // p2p events from file:      "p2p"
  
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
  //detGeo.Add(new TObjString("TARGET"),
  //	     new TObjString("target_"+target4+".geo.root"));
  //detGeo.Add(new TObjString("ALADIN"),        
  //     new TObjString("aladin_v13a.geo.root"));
  //detGeo.Add(new TObjString("GLAD"),          
  //	     new TObjString("glad_v17_flange.geo.root"));
  //detGeo.Add(new TObjString("CRYSTALBALL"),   
  //         new TObjString("cal_v13a.geo.root"));
  detGeo.Add(new TObjString("CALIFA"),        
	     new TObjString("califa_16_v8.11_cc0.2.geo.root"));
  //new TObjString("califa_10_v8.11.geo.root"));
  //detGeo.Add(new TObjString("TOF"),           
  //     new TObjString("tof_v13a.geo.root"));
  //detGeo.Add(new TObjString("MTOF"),          
  //     new TObjString("mtof_v13a.geo.root"));
  //detGeo.Add(new TObjString("DCH"),           
  //     new TObjString("dch_v13a.geo.root"));
  //detGeo.Add(new TObjString("TRACKER"),       
  //     new TObjString("tra_v13vac.geo.root"));
  //detGeo.Add(new TObjString("STaRTrack"),     
  //         new TObjString("startra_v13a.geo.root"));
  //detGeo.Add(new TObjString("STaRTrack"),
  // 	     new TObjString("startra_v14b.geo.root"));
  //detGeo.Add(new TObjString("GFI"),           
  //     new TObjString("gfi_v13a.geo.root"));
  //detGeo.Add(new TObjString("LAND"),          
  //     new TObjString("land_v12a_10m.geo.root"));
  //detGeo.Add(new TObjString("SCINTNEULAND"),  
  //     new TObjString("neuland_v12a_14m.geo.root"));
  //detGeo.Add(new TObjString("VACVESSELCOOL"), 
  // 	     new TObjString("vacvessel_v13b.geo.root"));
//detGeo.Add(new TObjString("MFI"),           
  //     new TObjString("mfi_v13a.geo.root"));
  //detGeo.Add(new TObjString("PSP"),           
  //     new TObjString("psp_v13a.geo.root"));


   //-------------------------------------------------
   //- N# of Sim. Events   |    nEvents     (Int_t)
   //-------------------------------------------------

   Int_t nEvents = 1000;

   //-------------------------------------------------
   //- EventDisplay        |    fEventDisplay (Bool_t)
   //-------------------------------------------------
   //   connected:              kTRUE
   //   not connected:          kFALSE
   Bool_t fEventDisplay=kTRUE;

   // Magnet Field definition
   Bool_t fR3BMagnet = kFALSE;
	
   // Including CalifaHitFinder task (kFALSE by default)
   Bool_t fCalifaHitFinder = kTRUE; 

   // Including StarTrackHitFinder task (kFALSE by default)
   Bool_t fStarTrackHitFinder = kFALSE;

   Double_t fMeasCurrent = 1500.;

   // Main Sim function call
   r3ball(nEvents,
	  detGeo,
	  target4,
	  fEventDisplay,
	  fMC,
	  fGene,
	  fUserPList,
	  fR3BMagnet,
	  fCalifaHitFinder,
	  fStarTrackHitFinder,
	  fMeasCurrent,
	  OutFile,
	  ParFile,
	  EventFile);      

}
