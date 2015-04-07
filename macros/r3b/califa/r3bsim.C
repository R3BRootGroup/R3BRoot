//--------------------------------------------------------------------
//
// Define the R3B simulation Setup
// Steering macros to main r3ball.C
// Author: Hector Alvarez <hector.alvarez@usc.es>
//
// Last Update: 25/04/2012
// Comments:
//
//
//--------------------------------------------------------------------


Int_t r3bsim(){
  
  // Load the Main Simulation macro
  TString macro_r3ball = getenv("VMCWORKDIR");
  macro_r3ball += "/macros/r3b/califa/r3ball.C";
  gROOT->LoadMacro(macro_r3ball.Data());
  
  // Input files
  //TString EventFile = "p2p_O18_no_exc_state_without_gammas_of_course.dat";
  TString EventFile = "p2p_O18_first_exc_state_with_gammas_no_doppler.dat";
  //TString EventFile = "p2p_O18_second_exc_state_with_gammas.dat";
  //TString EventFile = "p2p_O18_third_exc_state_with_gammas.dat";

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
  detGeo.Add(new TObjString("TARGET"),
  	     new TObjString("target_"+target4+".geo.root"));
  //detGeo.Add(new TObjString("ALADIN"),        
  //     new TObjString("aladin_v13a.geo.root"));
  //detGeo.Add(new TObjString("GLAD"),          
  //	     new TObjString("glad_v13a.geo.root"));
  //detGeo.Add(new TObjString("CRYSTALBALL"),   
  //         new TObjString("cal_v13a.geo.root"));
  detGeo.Add(new TObjString("CALIFA"),        
	     new TObjString("califa_10_v8.11.geo.root"));  //Barrel only
  //new TObjString("califa_v13_811.geo.root"));
             //new TObjString("califa_v5_707717.geo.root"));
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
  //detGeo.Add(new TObjString("GFI"),           
  //     new TObjString("gfi_v13a.geo.root"));
  //detGeo.Add(new TObjString("LAND"),          
  //     new TObjString("land_v12a_10m.geo.root"));
  //detGeo.Add(new TObjString("SCINTNEULAND"),  
  //     new TObjString("neuland_v12a_14m.geo.root"));
  //detGeo.Add(new TObjString("VACVESSELCOOL"), 
  //     new TObjString("vacvessel_v13a.geo.root"));
  //detGeo.Add(new TObjString("MFI"),           
  //     new TObjString("mfi_v13a.geo.root"));
  //detGeo.Add(new TObjString("PSP"),           
  //     new TObjString("psp_v13a.geo.root"));


   //-------------------------------------------------
   //- N# of Sim. Events   |    nEvents     (Int_t)
   //-------------------------------------------------

   Int_t nEvents = 50;

   //-------------------------------------------------
   //- EventDisplay        |    fEventDisplay (Bool_t)
   //-------------------------------------------------
   //   connected:              kTRUE
   //   not connected:          kFALSE
   Bool_t fEventDisplay=kTRUE;

   // Magnet Field definition
   Bool_t fR3BMagnet = kFALSE;
	
   // Including CaloHitFinder task (kFALSE by default)

   Bool_t fCaloHitFinder = kFALSE;  //kTRUE; 

   Double_t fMeasCurrent = 1500.;

   // Main Sim function call
   r3ball(nEvents, detGeo, target2, fEventDisplay, fMC, fGene, fUserPList,
	  fR3BMagnet, fCaloHitFinder, fMeasCurrent, OutFile, ParFile, EventFile);      

}

