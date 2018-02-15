void run_sim()
{
    TString transport = "TGeant4";
    Bool_t userPList = kFALSE; // option for TGeant4

    TString outFile = "sim.root";
    TString parFile = "par.root";

    Bool_t magnet = kTRUE;
    Float_t fieldScale = -0.68;

    TString generator1 = "box";
    TString generator2 = "ascii";
    TString generator3 = "r3b";
    TString generator = generator2;
    TString inputFile = "fragment_tracker.dat";

    Int_t nEvents = 3000;
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
        run->SetUserCuts("SetCuts.C");
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
    run->AddModule(new R3BTarget(targetType, "target_" + targetType + ".geo.root"));

    // GLAD
    //run->AddModule(new R3BGladMagnet("glad_v17_flange.geo.root")); // GLAD should not be moved or rotated

    // PSP
    run->AddModule(new R3BPsp("psp_v13a.geo.root", {}, -221., -89., 94.1));
    
    // Fi4 detector
    run->AddModule(new R3BFi4("fi4_v17a.geo.root", {-73.274339-TMath::Tan(TMath::DegToRad()*16.7)*100, 0.069976, 513.649524+100.}, {"" ,-90.,16.7,90.}));

    // Fi5 detector
    run->AddModule(new R3BFi5("fi5_v17a.geo.root", {-73.274339-TMath::Tan(TMath::DegToRad()*16.7)*200, 0.069976, 513.649524+200.}, {"" ,-90.,16.7,90.}));

    // R3B SiTracker Cooling definition
    //run->AddModule(new R3BVacVesselCool(targetType, "vacvessel_v14a.geo.root"));

    // STaRTrack
    //run->AddModule(new R3BSTaRTra("startra_v16-300_2layers.geo.root", { 0., 0., 20. }));

    // CALIFA
    R3BCalifa* califa = new R3BCalifa("califa_10_v8.11.geo.root");
    califa->SelectGeometryVersion(10);
    // Selecting the Non-uniformity of the crystals (1 means +-1% max deviation)
    califa->SetNonUniformity(1.0);
    //run->AddModule(califa);

    // Tof
    //run->AddModule(new R3BTof("tof_v17a.geo.root", { -417.359574, 2.400000, 960.777114 }, { "", -90., +31., 90. }));

    // dTof
    run->AddModule(new R3BdTof("dtof_v17a.geo.root", { -155.824045+(2.7*5)*TMath::Cos(16.7*TMath::DegToRad()), 0.523976, 761.870346 }, { "", -90., +16.7, 90. }));

    // MFI
    //run->AddModule(new R3BMfi("mfi_v17a.geo.root", { -63.82, 0., 520.25 }, { "", 90., +13.5, 90. })); // s412

    // NeuLAND
    // run->AddModule(new R3BNeuland("neuland_test.geo.root", { 0., 0., 1400. + 12 * 5. }));

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
        FairIonGenerator* boxGen = new FairIonGenerator(50, 128, 50, 1, 0., 0., 1.4, 0., 0., 0.);
        primGen->AddGenerator(boxGen);
    }

    if (generator.CompareTo("ascii") == 0)
    {
        R3BAsciiGenerator* gen = new R3BAsciiGenerator((dir + "/input/" + inputFile).Data());
        primGen->AddGenerator(gen);
    }

    run->SetGenerator(primGen);

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

    cout << " Test passed" << endl;
    cout << " All ok " << endl;

    // Snap a picture of the geometry
    // If this crashes, set "OpenGL.SavePicturesViaFBO: no" in your .rootrc
    /*gStyle->SetCanvasPreferGL(kTRUE);
    gGeoManager->GetTopVolume()->Draw("ogl");
    TGLViewer* v = (TGLViewer*)gPad->GetViewer3D();
    v->SetStyle(TGLRnrCtx::kOutline);
    v->RequestDraw();
    v->SavePicture("run_sim-side.png");
    v->SetPerspectiveCamera(TGLViewer::kCameraPerspXOZ, 25., 0, 0, -90. * TMath::DegToRad(), 0. * TMath::DegToRad());
    v->SavePicture("run_sim-top.png");*/
}
