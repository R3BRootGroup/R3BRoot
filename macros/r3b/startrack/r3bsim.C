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

Int_t r3bsim(){

   // Load the Main Simulation macro
   //gROOT->LoadMacro("r3ball.C");
  TString macro_r3ball = getenv("VMCWORKDIR");
  macro_r3ball += "/macros/r3b/startrack/r3ball.C";
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
   //TString fMC ="TGeant3";

   //-------------------------------------------------
   // Primaries generation
   // Event Generator Type |   fGene       (TString)
   //-------------------------------------------------
   // Box generator:             "box"
   // R3B spec. generator:       "r3b"

   //TString fGene="box";
   TString fGene="gammas"; // particle source  
   // TString fGene="ion_gun";
   //TString fGene="p2p";
   //TString fGene="cosmic";
   //TString fGene="ascii";

   TString InFile = "../../../R3B-Input/Input/KINR_12C_a_a_4.8GeV.event";  // only used when ascii evt generator


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
   TString target5="CTarget";   // C for s318
   TString target6="CH2Target"; // CH2 for s318
   TString target7="ETTarget";  // empty target for s318
 
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
   //   Neutron:                 LAND
   //                            SCINTNEULAND

  TMap detGeo;

  // Target:
  detGeo.Add(new TObjString("TARGET"),        new TObjString("target_"+target4+".geo.root"));
  //detGeo.Add(new TObjString("TARGET"),        new TObjString("target_"+target2+".geo.root"));  // ~10mg/cm2 (thin) 
  //detGeo.Add(new TObjString("TARGET"),        new TObjString("target_"+target2+".geo.S438.root")); // =0.5g/cm2 (thick)
  //detGeo.Add(new TObjString("TARGET"),        new TObjString("target_"+target2+"Empty.geo.S438.root"));

  // Setup
//  detGeo.Add(new TObjString("ALADIN"),        new TObjString("aladin_v13a.geo.root"));
//  detGeo.Add(new TObjString("GLAD"),          new TObjString("glad_v17_flange.geo.root"));
//  detGeo.Add(new TObjString("CRYSTALBALL"),   new TObjString("cal_v13a.geo.root"));
  //detGeo.Add(new TObjString("CALIFA"),        new TObjString("califa_Marc.geo.root"));
  detGeo.Add(new TObjString("CALIFA"),        new TObjString("califa_v14a.geo.root"));
//  detGeo.Add(new TObjString("TOF"),           new TObjString("tof_v13a.geo.root"));
//  detGeo.Add(new TObjString("MTOF"),          new TObjString("mtof_v13a.geo.root"));
//  detGeo.Add(new TObjString("DCH"),           new TObjString("dch_v13a.geo.root"));
//  detGeo.Add(new TObjString("TRACKER"),       new TObjString("tra_v13vac.geo.root"));
//  detGeo.Add(new TObjString("STaRTrack"),     new TObjString("startra_v14a.geo.root"));
  //detGeo.Add(new TObjString("STaRTrack"),     new TObjString("startra_v15.geo.root"));   // for p2p
  //detGeo.Add(new TObjString("STaRTrack"),     new TObjString("startra_v15300.geo.root")); // for p2p
  //detGeo.Add(new TObjString("STaRTrack"),     new TObjString("startra_v16.geo.root"));  
  //detGeo.Add(new TObjString("STaRTrack"),     new TObjString("startra_v16-300.geo.root"));
  //detGeo.Add(new TObjString("STaRTrack"),     new TObjString("startra_v16_2layers.geo.root")); // for elastic
  detGeo.Add(new TObjString("STaRTrack"),     new TObjString("startra_v16-300_2layers.geo.root")); // for elastic
//  detGeo.Add(new TObjString("STaRTrack"),     new TObjString("startra_S438.geo.root"));
//  detGeo.Add(new TObjString("GFI"),           new TObjString("gfi_v13a.geo.root"));
//  detGeo.Add(new TObjString("LAND"),          new TObjString("land_v12a_10m.geo.root"));
//  detGeo.Add(new TObjString("SCINTNEULAND"),  new TObjString("neuland_v12a_14m.geo.root"));
//  detGeo.Add(new TObjString("VACVESSELCOOL"), new TObjString("vacvessel_v14a.geo.root"));
  //  detGeo.Add(new TObjString("VACVESSELCOOL"), new TObjString("vacvessel_v13a.geo.root"));
///  detGeo.Add(new TObjString("MFI"),           new TObjString("mfi_v13a.geo.root"));
//  detGeo.Add(new TObjString("PSP"),           new TObjString("psp_v13a.geo.root"));




   //-------------------------------------------------
   //- N# of Sim. Events   |    nEvents     (Int_t)
   //-------------------------------------------------

    //Int_t nEvents = 100;
  Int_t nEvents = 1000;
       //Int_t nEvents = 20000;

   //-------------------------------------------------
   //- EventDisplay        |    fEventDisplay (Bool_t)
   //-------------------------------------------------
   //   connected:              kTRUE
   //   not connected:          kFALSE

   //Bool_t fEventDisplay=kFALSE;
   Bool_t fEventDisplay=kTRUE;

    // Magnet Field definition
   //Bool_t fR3BMagnet = kTRUE;
   Bool_t fR3BMagnet = kFALSE;

   // Main Sim function call
   r3ball(  nEvents,
            detGeo,
	    target4, //           target2,
	    fEventDisplay,
	    fMC,
	    fGene,
	    fUserPList,
            fR3BMagnet,
       	    1500.,
	    OutFile,
	    ParFile,
	    InFile   // for ascii evt generator
          );      

}

