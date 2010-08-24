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


Int_t chimer(){

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
   // R3B spec. generator:       "r3b"

   TString fGene="iqmd";

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
   //   Neutron:                 LAND

    TObjString det1("TARGET");
    TObjString det2("ALADIN");
    TObjString det3("CALIFA");
    TObjString det4("ATOF");
    TObjString det5("CHIMERA");
    TObjString det6("LAND");

    TObjArray fDetList;
    //fDetList.Add(&det1);
    fDetList.Add(&det4);
    fDetList.Add(&det5);
    fDetList.Add(&det6);
 

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

   // Main Sim function call
   r3ball(  nEvents,
            fDetList,
            target4,
	    fEventDisplay,
	    fMC,
	    fGene,
            fUserPList
          );      

}

