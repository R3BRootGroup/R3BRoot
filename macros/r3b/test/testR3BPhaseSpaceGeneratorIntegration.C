void testR3BPhaseSpaceGeneratorIntegration()
{
    // System paths
    const TString workDirectory = getenv("VMCWORKDIR");
    gSystem->Setenv("GEOMPATH", workDirectory + "/geometry");
    gSystem->Setenv("CONFIG_DIR", workDirectory + "/gconfig");

    // Simulation Base
    FairRunSim run;
    run.SetName("TGeant4");
    run.SetOutputFile("testR3BPhaseSpaceGeneratorIntegration.root");
    run.SetMaterials("media_r3b.geo");

    // World
    auto cave = new R3BCave("Cave");
    cave->SetGeometryFileName("r3b_cave_vacuum.geo");
    run.AddModule(cave);

    // Magnet
    run.AddModule(new R3BGladMagnet("glad_v17_flange.geo.root"));
    auto magField = new R3BGladFieldMap("R3BGladMap");
    magField->SetScale(-0.6);
    run.SetField(magField);

    // Primaries
    auto primGen = new FairPrimaryGenerator();
    auto gen = new R3BPhaseSpaceGenerator();
    gen->SetBeamEnergyAMeV(600);
    gen->SetErelkeV(500);
    gen->AddHeavyIon({ "B-17", 5, 17, 5 });
    gen->AddParticle(2112);
    gen->AddParticle(2112);
    primGen->AddGenerator(gen);
    run.SetGenerator(primGen);

    // Logging
    run.SetStoreTraj(false);
    FairLogger::GetLogger()->SetLogVerbosityLevel("LOW");
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");

    // Init & Special MC Settings
    run.Init();

    // Database
    auto rtdb = run.GetRuntimeDb();
    auto fieldPar = (R3BFieldPar*)rtdb->getContainer("R3BFieldPar");
    fieldPar->SetParameters(magField);
    fieldPar->setChanged();
    auto parOut = new FairParRootFileIo(true);
    parOut->open("testR3BPhaseSpaceGeneratorIntegration.para.root");
    rtdb->setOutput(parOut);
    rtdb->saveOutput();
    rtdb->print();

    // Go
    run.Run(1);

    // Test Output
    auto file = TFile::Open("testR3BPhaseSpaceGeneratorIntegration.root");
    auto tree = (TTree*)file->Get("evt");
    auto mctc = new TClonesArray("R3BMCTrack");
    tree->SetBranchAddress("MCTrack", &mctc);

    tree->GetEvent(0);
    if (mctc->GetEntries() < 3)
    {
        cout << "Not enough particles produced" << endl;
        return;
    }

    auto track = (R3BMCTrack*)mctc->At(1);
    if (track->GetPdgCode() != 2112 || track->GetMotherId() != -1)
    {
        cout << "Not the correct primary particle" << endl;
        return;
    }

    const Double_t ekin = track->GetEnergy() - track->GetMass();
    cout << "Ekin of primary neutron 1" << ekin << endl;
    if ((ekin - 0.6) > 0.02)
    {
        cout << "Primary neutron doesn't have the correct energy!" << endl;
        return;
    }

    cout << " Test passed" << endl;
    cout << " All ok " << endl;
}
