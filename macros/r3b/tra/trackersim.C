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


Int_t trackersim(){

   // Load the Main Simulation macro
   gROOT->LoadMacro("r3ball.C");

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

   TString fGene="box";

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

    TObjString det1("TARGET");
    TObjString det2("ALADIN");
    TObjString det3("CALIFA");
    TObjString det4("CRYSTALBALL");
    TObjString det5("TOF");
    TObjString det6("MTOF");
    TObjString det7("DCH");
    TObjString det8("TRACKER");
    TObjString det9("GFI");
    TObjString det10("LAND");
    TObjString det11("RPCMLAND");
    TObjString det12("RPCFLAND");

    TObjArray fDetList;
    fDetList.Add(&det1);
    fDetList.Add(&det2);
    fDetList.Add(&det7);
//    fDetList.Add(&det10);
//    fDetList.Add(&det9);
//    fDetList.Add(&det6);
//    fDetList.Add(&det7);
//    fDetList.Add(&det8);
//    fDetList.Add(&det9);
//    fDetList.Add(&det10);
//    fDetList.Add(&det11);


   //-------------------------------------------------
   //- N# of Sim. Events   |    nEvents     (Int_t)
   //-------------------------------------------------

   Int_t nEvents = 1;

   //-------------------------------------------------
   //- EventDisplay        |    fEventDisplay (Bool_t)
   //-------------------------------------------------
   //   connected:              kTRUE
   //   not connected:          kFALSE

   Bool_t fEventDisplay=kTRUE;

    // Magnet Field definition
   Bool_t fR3BMagnet = kTRUE;

   // Main Sim function call
   r3ball(  nEvents,
            fDetList,
            target1,
	    fEventDisplay,
	    fMC,
	    fGene,
	    fUserPList,
            fR3BMagnet
          );      

}

