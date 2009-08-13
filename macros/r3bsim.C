//--------------------------------------------------------------------
//
// Define the R3B simulation Setup
// Steering macros to main r3ball.C
// Author: <D.Bertini@gsi.de>
//
// Last Update: 13/08/09
// Comments:
//         - 13/08/09 : adding flag for User Physics List
//
//
//--------------------------------------------------------------------


Int_t r3bsim(){

   // Load the Main Simulation macro
   gROOT->LoadMacro("r3ball.C");


   //- N# of Sim. Events   |    nEvents     (Int_t)
   //-------------------------------------------------
   Int_t nEvents = 1;

   //- Calorimeter type    |    fCalVersion (Int_t)
   //-------------------------------------------------
   //   CALIFA:                 0
   //   CrystalBall:            1
   Int_t fCalVersion = 0;

   // Target type
   TString target1="LeadTarget";
   TString target2="Para";
   TString target3="Para45";
   TString target4="LiH";

   //- EventDisplay        |    fEventDisplay (Bool_t)
   //-------------------------------------------------
   //   connected:              kTRUE
   //   not connected:          kFALSE

   Bool_t fEventDisplay=kTRUE;

   // Monte Carlo type     |    fMC        (TString)
   //-------------------------------------------------
   //   Geant3:                 "TGeant3"
   //   Geant4:                 "TGeant4"
   //   Fluka :                 "TFluka"

   TString fMC ="TGeant4";
   
   // Event Generator Type |   fGene       (TString)
   //-------------------------------------------------
   // Box generator:             "box"
   // R3B spec. generator:       "r3b"
   TString fGene="box";

   // R3B Spec. PhysicList |     fUserPList (Bool_t)
   // ----------------------------------------------
   //     VMC Standard           kFALSE
   //     R3B Special            kTRUE;

   Bool_t fUserPList= kTRUE;

   // Main Sim function call
   r3ball(  nEvents,
            fCalVersion,
            target4,
	    fEventDisplay,
	    fMC,
	    fGene,
            fUserPList
          );      

}

