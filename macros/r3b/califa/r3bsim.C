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
   gROOT->LoadMacro("r3ball.C");

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

    TObjString det0("TARGET");
    TObjString det1("ALADIN");
    TObjString det2("GLAD");
    TObjString det3("CALIFA");
    TObjString det4("CRYSTALBALL");
    TObjString det5("TOF");
    TObjString det6("MTOF");
    TObjString det7("DCH");
    TObjString det8("TRACKER");
    TObjString det9("GFI");
    TObjString det10("LAND");

    TObjArray fDetList;
    //fDetList.Add(&det0);
    //fDetList.Add(&det2);
    fDetList.Add(&det3);
    //fDetList.Add(&det5);
    //fDetList.Add(&det6);
    //fDetList.Add(&det7);
    //fDetList.Add(&det8);
    //fDetList.Add(&det9);
    //fDetList.Add(&det10);


   //-------------------------------------------------
   //- N# of Sim. Events   |    nEvents     (Int_t)
   //-------------------------------------------------

   Int_t nEvents = 100;

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
   r3ball(  nEvents,
            fDetList,
            target2,
		    fEventDisplay,
	        fMC,
	        fGene,
	        fUserPList,
            fR3BMagnet
          );      

}

