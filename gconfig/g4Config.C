// Configuration macro for Geant4 VirtualMC

void Config()
{
    cout << "[g4Config]: Creating TGeant4 VirtualMC ..." << endl;

    /* Create the run configuration
     *
     * In constructor user has to specify the geometry input
     * and select geometry navigation via the following options:
     * - geomVMCtoGeant4   - geometry defined via VMC, G4 native navigation
     * - geomVMCtoRoot     - geometry defined via VMC, Root navigation
     * - geomRoot          - geometry defined via Root, Root navigation
     * - geomRootToGeant4  - geometry defined via Root, G4 native navigation
     * - geomGeant4        - geometry defined via Geant4, G4 native navigation
     *
     * The second argument in the constructor selects physics list:
     * - emStandard         - standard em physics (default)
     * - emStandard+optical - standard em physics + optical physics
     * - XYZ                - selected hadron physics list ( XYZ = LHEP, QGSP, ...)
     * - XYZ+optical        - selected hadron physics list + optical physics
     *
     * The third argument activates the special processes in the TG4SpecialPhysicsList,
     * which implement VMC features:
     * - stepLimiter       - step limiter (default)
     * - specialCuts       - VMC cuts
     * - specialControls   - VMC controls for activation/inactivation selected processes
     * - stackPopper       - stackPopper process
     *
     * When more than one options are selected, they should be separated with '+'
     * character: e.g. stepLimit+specialCuts.
     */

    // Enable setting the physics list from outside via environment variable
    TString physicsList = gSystem->Getenv("PHYSICSLIST");
    if (physicsList == "")
    {
        physicsList = "QGSP_INCLXX_HP";
    }

    auto runConfiguration =
        new TG4RunConfiguration("geomRoot", physicsList, "stepLimiter+specialCuts" /*+specialControls"*/);
    auto geant4 = new TGeant4("TGeant4", "The Geant4 Monte Carlo", runConfiguration);

    auto stack = new R3BStack(1000);
    stack->SetDebug(kFALSE);
    stack->StoreSecondaries(kTRUE);
    stack->SetMinPoints(0);
    geant4->SetStack(stack);

    geant4->SetMaxNStep(10000);

    const TString geant4macro = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/gconfig/g4config.in");
    cout << "[g4Config]: Processing additional Geant4 macro file " << geant4macro << endl;
    geant4->ProcessGeantMacro(geant4macro);
}
