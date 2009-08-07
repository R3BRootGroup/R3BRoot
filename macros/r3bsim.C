
Int_t r3bsim(){

   // Load the Main Simulation macro
   gROOT->LoadMacro("r3ball.C");

   //---------------------------------
   // Define the R3B simulation Setup
   //---------------------------------

   // Number of Simulated Events
   Int_t nEvents = 1;

   // Calorimeter version
   //  0 --> CALIFA
   //  1 --> CrystalBall   
   Int_t fCalVersion = 0;

   // Target
   TString target1="LeadTarget";
   TString target2="Para";
   TString target3="Para45";
   TString target4="LiH";

   // EventDisplay connected
   Bool_t fEventDisplay=kTRUE;

   // Monte Carlo engine used
   TString fMC ="TGeant3";
   
   // Generator Type
   //        "box " : box generator
   //        "r3b"  : R3B specific generator  
   TString fGene="box";
   
   // Main function call
   r3ball(  nEvents,
            fCalVersion,
            target4,
	    fEventDisplay,
	    fMC,
	    fGene 
          );      

}

