void run_sim()
{
    TString transport = "TGeant3";
    Bool_t userPList = kTRUE; // option for TGeant4

    TString outFile = "sim.root";
    TString parFile = "par.root";

    Bool_t magnet = kTRUE;
    Float_t fieldScale = -0.6;

    TString generator1 = "box";
    TString generator2 = "ascii";
    TString generator3 = "r3b";
    TString generator = generator1;
    TString inputFile = "";

    Int_t nEvents = 1;
    Bool_t storeTrajectories = kTRUE;
    Int_t randomSeed = 335566; // 0 for time-dependent random numbers

    // Target type
    TString target1 = "LeadTarget";
    TString target2 = "Para";
    TString target3 = "Para45";
    TString target4 = "LiH";
    TString targetType = target4;

    // ------------------------------------------------------------------------
    // Stable part ------------------------------------------------------------

    TString dir = getenv("VMCWORKDIR");

    // ----    Debug option   -------------------------------------------------
    gDebug = 0;

    // -----   Timer   --------------------------------------------------------
    TStopwatch timer;
    timer.Start();

    // -----   Create simulation run   ----------------------------------------
    FairRunSim* run = new FairRunSim();
    run->SetName(transport);            // Transport engine
    run->SetOutputFile(outFile.Data()); // Output file
    FairRuntimeDb* rtdb = run->GetRuntimeDb();

    //  R3B Special Physics List in G4 case
    if ((userPList == kTRUE) && (transport.CompareTo("TGeant4") == 0))
    {
        run->SetUserConfig("g4R3bConfig.C");
        run->SetUserCuts("SetR3BCuts.C");
    }

    // -----   Create media   -------------------------------------------------
    run->SetMaterials("media_r3b.geo"); // Materials

    // -----   Create R3B geometry --------------------------------------------
    // R3B Cave definition
    FairModule* cave = new R3BCave("CAVE");
    cave->SetGeometryFileName("r3b_cave.geo");
    run->AddModule(cave);

    // To skip the detector comment out the line with: run->AddModule(...

    // Target
    run->AddModule(new R3BTarget(targetType.Data(), "target_" + targetType + ".geo.root", 0., 0., 0.));

    // GLAD
    run->AddModule(
        new R3BGladMagnet("GladMagnet", "glad_v17_flange.geo.root", "R3BGlad Magnet")); // GLAD should not be moved or rotated

    // PSP
    run->AddModule(new R3BPsp(
        "Psp", "psp_v13a.geo.root", kTRUE, 0., 0., 0., 0., 0., 0., -221., -89., 94.1)); // position, rotation, z1, z2,
                                                                                        // z3

    // R3B SiTracker Cooling definition
    run->AddModule(new R3BVacVesselCool(targetType.Data(), "vacvessel_v14a.geo.root", 0., 0., 0.));

    // STaRTrack
    run->AddModule(new R3BSTaRTra("STaRTrack", "startra_v16-300_2layers.geo.root", kTRUE, 0., 0., 20.));

    // CALIFA
    R3BCalifa* califa = new R3BCalifa("Califa", "califa_10_v8.11.geo.root", kTRUE, 0., 0., 0.);
    califa->SelectGeometryVersion(10);
    // Selecting the Non-uniformity of the crystals (1 means +-1% max deviation)
    califa->SetNonUniformity(1.0);
    run->AddModule(califa);

    // Tof
    run->AddModule(new R3BTof("Tof", "tof_v17a.geo.root", kTRUE, -417.359574, 2.400000, 960.777114, 0., -31., 0.));

    // mTof
    run->AddModule(new R3BmTof("mTof", "mtof_v17a.geo.root", kTRUE, -155.824045, 0.523976, 761.870346, 0., -16.7, 0.));

    // MFI
    run->AddModule(new R3BMfi("Mfi", "mfi_v17a.geo.root", kTRUE, -63.82, 0., 520.25, 0., -13.5, 0.)); // s412

    // NeuLAND
    run->AddModule(new R3BLand("Land", "neuland_v12a_14m.geo.root", kTRUE, 0., 0., 1550.));

    // -----   Create R3B  magnetic field ----------------------------------------
    // NB: <D.B>
    // If the Global Position of the Magnet is changed
    // the Field Map has to be transformed accordingly
    R3BGladFieldMap* magField = new R3BGladFieldMap("R3BGladMap");
    magField->SetScale(fieldScale);

    if (magnet == kTRUE)
    {
        run->SetField(magField);
    }
    else
    {
        run->SetField(NULL);
    }

    // -----   Create PrimaryGenerator   --------------------------------------
    // 1 - Create the Main API class for the Generator
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

    if (generator.CompareTo("box") == 0)
    {
        // 2- Define the BOX generator
        Int_t pdgId = 2212;     // proton beam
        Double32_t theta1 = 0.; // polar angle distribution
        Double32_t theta2 = 2.;
        Double32_t momentum = 1.5;
        FairBoxGenerator* boxGen = new FairBoxGenerator(pdgId, 3);
        boxGen->SetThetaRange(theta1, theta2);
        boxGen->SetPRange(momentum, momentum * 1.2);
        boxGen->SetPhiRange(0, 360);
        boxGen->SetXYZ(0.0, 0.0, -1.5);
        primGen->AddGenerator(boxGen);

        // 128-Sn fragment
        R3BIonGenerator* ionGen = new R3BIonGenerator(50, 128, 50, 10, 0., 0., 1.3);
        ionGen->SetSpotRadius(0.1, -300., 0.);
        primGen->AddGenerator(ionGen);

        // neutrons
        FairBoxGenerator* boxGen_n = new FairBoxGenerator(2112, 3);
        boxGen_n->SetThetaRange(theta1, theta2);
        boxGen_n->SetPRange(momentum, momentum * 1.2);
        boxGen_n->SetPhiRange(0, 360);
        boxGen_n->SetXYZ(0.0, 0.0, -1.5);
        primGen->AddGenerator(boxGen_n);
    }

    if (generator.CompareTo("ascii") == 0)
    {
        R3BAsciiGenerator* gen = new R3BAsciiGenerator((dir + "/input/" + inputFile).Data());
        primGen->AddGenerator(gen);
    }

    if (generator.CompareTo("r3b") == 0)
    {
        Int_t pdg = 2212;
        Float_t beamEnergy = 1.;
        R3BSpecificGenerator* pR3bGen = new R3BSpecificGenerator(pdg, beamEnergy);

        // R3bGen properties
        pR3bGen->SetBeamInteractionFlag("off");
        pR3bGen->SetBeamInteractionFlag("off");
        pR3bGen->SetRndmFlag("off");
        pR3bGen->SetRndmEneFlag("off");
        pR3bGen->SetBoostFlag("off");
        pR3bGen->SetReactionFlag("on");
        pR3bGen->SetGammasFlag("off");
        pR3bGen->SetDecaySchemeFlag("off");
        pR3bGen->SetDissociationFlag("off");
        pR3bGen->SetBackTrackingFlag("off");
        pR3bGen->SetSimEmittanceFlag("off");

        // R3bGen Parameters
        pR3bGen->SetSigmaBeamEnergy(1.e-03); // Sigma(Ebeam) GeV
        pR3bGen->SetEnergyPrim(0.3);         // Particle Energy in MeV
        Int_t fMultiplicity = 50;
        pR3bGen->SetNumberOfParticles(fMultiplicity); // Mult.

        // Reaction type
        //        1: "Elas"
        //        2: "iso"
        //        3: "Trans"
        pR3bGen->SetReactionType("Elas");

        // Target  type
        //        1: "LeadTarget"
        //        2: "Parafin0Deg"
        //        3: "Parafin45Deg"
        //        4: "LiH"

        pR3bGen->SetTargetType(targetType.Data());
        Double_t thickness = (0.11 / 2.) / 10.;         // cm
        pR3bGen->SetTargetHalfThicknessPara(thickness); // cm
        pR3bGen->SetTargetThicknessLiH(3.5);            // cm
        pR3bGen->SetTargetRadius(1.);                   // cm

        pR3bGen->SetSigmaXInEmittance(1.);          // cm
        pR3bGen->SetSigmaXPrimeInEmittance(0.0001); // cm

        // Dump the User settings
        pR3bGen->PrintParameters();
        primGen->AddGenerator(pR3bGen);
    }

    run->SetGenerator(primGen);

    run->SetStoreTraj(storeTrajectories);

    FairLogger::GetLogger()->SetLogVerbosityLevel("LOW");
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");

    // -----   Initialize simulation run   ------------------------------------
    run->Init();
    gMC->SetRandom(new TRandom3(randomSeed));

    // ------  Increase nb of step for CALO
    Int_t nSteps = -15000;
    gMC->SetMaxNStep(nSteps);

    // -----   Runtime database   ---------------------------------------------
    R3BFieldPar* fieldPar = (R3BFieldPar*)rtdb->getContainer("R3BFieldPar");
    if (NULL != magField)
    {
        fieldPar->SetParameters(magField);
        fieldPar->setChanged();
    }
    Bool_t kParameterMerged = kTRUE;
    FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
    parOut->open(parFile.Data());
    rtdb->setOutput(parOut);
    rtdb->saveOutput();
    rtdb->print();

    // -----   Start run   ----------------------------------------------------
    if (nEvents > 0)
    {
        run->Run(nEvents);
    }

    // -----   Finish   -------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Output file is " << outFile << endl;
    cout << "Parameter file is " << parFile << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl;

    cout << " Test passed" << endl;
    cout << " All ok " << endl;
}
