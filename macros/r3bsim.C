
Int_t r3bsim(){

   // Load the Main Simulation macro
   gROOT->LoadMacro("r3ball.C");

   //---------------------------------
   // Define the R3B simulation Setup
   //---------------------------------

   // Number of Simulated Events
   Int_t nEvents =2;
   // Calorimeter version
   //  0 --> CALIFA
   //  1 --> CrystalBall   
   Int_t fCalVersion = 0;
   // Target 
   TString target="LeadTarget";
   // EventDisplay connected 
   Bool_t fEventDisplay=kTRUE;
   // Monte Carlo engine used
   TString fMC ="TGeant3";

   // Main function call
   r3ball(  nEvents,
            fCalVersion,
            target, 
	    fEventDisplay,
	    fMC
          );      

}

