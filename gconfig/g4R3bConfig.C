// Configuration macro for Geant4 VirtualMC
void Config()
{
   // Load the User Defined Physics List
   cout << "-I- R3B Config(): Loading G4 User Defined Physics List " << endl;

    gSystem->Load("libR3BPlist.so");

    cout<<endl;

    ///    Create the run configuration
   /// In constructor user has to specify the geometry input
   /// and select geometry navigation via the following options:
   /// - geomVMCtoGeant4   - geometry defined via VMC, G4 native navigation
   /// - geomVMCtoRoot     - geometry defined via VMC, Root navigation
   /// - geomRoot          - geometry defined via Root, Root navigation
   /// - geomRootToGeant4  - geometry defined via Root, G4 native navigation
   /// - geomGeant4        - geometry defined via Geant4, G4 native navigation

   /// The second argument in the constructor selects physics list:
   /// - emStandard         - standard em physics (default)
   /// - emStandard+optical - standard em physics + optical physics
   /// - XYZ                - selected hadron physics list ( XYZ = LHEP, QGSP, ...)
   /// - XYZ+optical        - selected hadron physics list + optical physics


   // Create the Run Configuration
   //R3BRunConfiguration* runConfiguration
   //  = new R3BRunConfiguration("geomRootToGeant4", "stepLimiter+specialCuts+specialControls");

/// Create the G4 VMC 
   //TGeant4* geant4 = new TGeant4("TGeant4", "The Geant4 Monte Carlo",
				// runConfiguration);
   //cout << "-I- R3B Config(): Geant4 with R3B dedicated Physics list " << endl;
   //cout << "-I- R3B Config(): has been created..." << endl;


  // Create Geant4 VMC
  //  
  TGeant4 *geant4 = 0;
  if ( ! gMC ) {
    TG4RunConfiguration* runConfiguration 
      = new TG4RunConfiguration("geomRoot", 
                                "QGSP_BERT", 
                                "specialCuts+stackPopper+stepLimiter+specialControls",
                                 true);

    geant4 = new TGeant4("TGeant4", "The Geant4 Monte Carlo", runConfiguration);
    cout << "-I- R3B Config(): Geant4 with R3B dedicated Physics list " << endl;
    cout << "-I- R3B Config(): has been created..." << endl;
  } 
  else {
    cout << "Monte Carlo has been already created." << endl;
  }  



/// create the Specific stack
   R3BStack *stack = new R3BStack(1000); 
   stack->SetDebug(kFALSE);
   stack->StoreSecondaries(kTRUE);
   stack->SetMinPoints(0);
   geant4->SetStack(stack);

   if(FairRunSim::Instance()->IsExtDecayer()){
      TVirtualMCDecayer* decayer = TPythia6Decayer::Instance();
      geant4->SetExternalDecayer(decayer);
   }
  
/// Customise Geant4 setting
/// (verbose level, global range cut, ..)

   TString configm(gSystem->Getenv("VMCWORKDIR"));
   configm1 = configm + "/gconfig/g4r3bconfig.in";
   cout << " -I g4Config() using g4conf  macro: " << configm1 << endl;

   //set geant4 specific stuff
  geant4->SetMaxNStep(10000);  // default is 30000
  geant4->ProcessGeantMacro(configm1.Data());

}
