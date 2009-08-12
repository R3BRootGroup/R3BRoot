
Int_t r3bsim(){

   // Load the Main Simulation macro
   gROOT->LoadMacro("r3ball.C");

   //---------------------------------
   // Define the R3B simulation Setup
   //---------------------------------

   // Number of Simulated Events
   Int_t nEvents = 1;

   // Calorimeter type
   //  0 --> CALIFA
   //  1 --> CrystalBall   
   Int_t fCalVersion = 0;

   // Target type
   TString target1="LeadTarget";
   TString target2="Para";
   TString target3="Para45";
   TString target4="LiH";

   // EventDisplay        fEventDisplay
   //
   //       connected       kTRUE
   //   not connected       kFALSE

   Bool_t fEventDisplay=kTRUE;

   // Monte Carlo engine type
   TString fMC ="TGeant4";
   
   // Event Generator Type
   //        "box " : box generator
   //        "r3b"  : R3B specific generator  
   TString fGene="box";

   // Case Geant4 : loading R3B Special Physic List
   //
   //     VMC Standard  -->  kFALSE
   //     R3B Special   -->  kTRUE;

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

