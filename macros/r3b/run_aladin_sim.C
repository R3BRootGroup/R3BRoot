void run_aladin_sim()
{
    TString transport = "TGeant4";

    TString outFile = "aladin_sim.root";
    TString parFile = "aladin_par.root";

    Bool_t magnet = kTRUE;
    Float_t aladinCurrent = 2500.;

    TString generator1 = "box";
    TString generator2 = "ascii";
    TString generator3 = "r3b";
    TString generator = generator1;
    TString inputFile = "";

    Int_t nEvents = 1;
    Bool_t storeTrajectories = kTRUE;
    Int_t randomSeed = 335566;

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
    // ------------------------------------------------------------------------

    // -----   Timer   --------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    // ------------------------------------------------------------------------

    // -----   Create simulation run   ----------------------------------------
    FairRunSim* run = new FairRunSim();
    run->SetName(transport);            // Transport engine
    run->SetOutputFile(outFile.Data()); // Output file
    FairRuntimeDb* rtdb = run->GetRuntimeDb();

    // -----   Create media   -------------------------------------------------
    run->SetMaterials("media_r3b.geo"); // Materials

    // Magnetic field map type
    Int_t fieldMap = 0;

    // -----   Create R3B geometry --------------------------------------------
    // R3B Cave definition
    FairModule* cave = new R3BCave("CAVE");
    cave->SetGeometryFileName("r3b_cave.geo");
    run->AddModule(cave);

    // To skip the detector comment out the line with: run->AddModule(...

    // Target
    run->AddModule(new R3BTarget(targetType, "target_" + targetType + ".geo.root"));

    // ALADIN
    run->AddModule(new R3BAladinMagnet("aladin_v13a.geo.root")); // ALADIN should not be moved or rotated

    // Crystal-Ball
    R3BXBall* xball = new R3BXBall("cal_v13a.geo.root");
    xball->SelectCollectionOption(2);
    run->AddModule(xball);

    // Tracker
    R3BTra* tra = new R3BTra("tra_v13vac.geo.root");
    tra->SetEnergyCut(1e-4);
    run->AddModule(tra);

    // DCH
    run->AddModule(new R3BDch("dch_v13a.geo.root",
                              -123.219446,
                              3.597104,
                              444.126271,
                              -31.,
                              -8.88,
                              -167.015888,
                              1.016917,
                              535.093884,
                              -31.,
                              9.35));

    // Tof
    run->AddModule(new R3BTof("tof_v17a.geo.root", { -377.359574, 2.400000, 960.777114 }, { "", -90., +31., 90. }));

    // mTof
    run->AddModule(new R3BmTof("mtof_v17a.geo.root", { -115.824045, 0.523976, 761.870346 }, { "", -90., +16.7, 90. }));

    // MFI
    run->AddModule(new R3BMfi("mfi_v17a.geo.root", { -63.82, 0., 520.25 }, { "", -90., +13.5, 90. })); // s412

    // GFI detector
    run->AddModule(new R3BGfi(
        "gfi_v13a.geo.root", -73.274339, 0.069976, 513.649524, -16.7, -147.135037, 0.069976, 729.680342, -16.7));

    // LAND
    run->AddModule(new R3BLand("land_v12a_10m.geo.root", { 0., 0., 1100. }));

    // -----   Create R3B  magnetic field -------------------------------------
    // NB: <D.B>
    // If the Global Position of the Magnet is changed
    // the Field Map has to be transformed accordingly
    R3BAladinFieldMap* magField = new R3BAladinFieldMap("AladinMaps");
    magField->SetCurrent(aladinCurrent);

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

    //-------Set visualisation flag to true------------------------------------
    run->SetStoreTraj(storeTrajectories);

    FairLogger::GetLogger()->SetLogVerbosityLevel("LOW");
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");

    // -----   Initialize simulation run   ------------------------------------
    run->Init();
    TVirtualMC::GetMC()->SetRandom(new TRandom3(randomSeed));

    // ------  Increase nb of step for CALO
    Int_t nSteps = -15000;
    TVirtualMC::GetMC()->SetMaxNStep(nSteps);

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
    // ------------------------------------------------------------------------

    cout << " Test passed" << endl;
    cout << " All ok " << endl;

    // Snap a picture of the geometry
    // If this crashes, set "OpenGL.SavePicturesViaFBO: no" in your .rootrc
    /*gStyle->SetCanvasPreferGL(kTRUE);
    gGeoManager->GetTopVolume()->Draw("ogl");
    TGLViewer* v = (TGLViewer*)gPad->GetViewer3D();
    v->SetStyle(TGLRnrCtx::kOutline);
    v->RequestDraw();
    v->SavePicture("run_aladin_sim-side.png");
    v->SetPerspectiveCamera(TGLViewer::kCameraPerspXOZ, 25., 0, 0, -90. * TMath::DegToRad(), 0. * TMath::DegToRad());
    v->SavePicture("run_aladin_sim-top.png");*/
}
