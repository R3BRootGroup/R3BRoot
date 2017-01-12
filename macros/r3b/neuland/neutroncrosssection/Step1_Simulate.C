void Step1_Simulate(const Double_t energy, // MeV
                    const TString basePath,
                    const TString baseName)
{
    // Output files
    const TString outFile = basePath + "/" + baseName + ".sim.root";
    const TString parFile = basePath + "/" + baseName + ".par.root";

    // Basic simulation setup
    FairRunSim run;
    run.SetName("TGeant4");
    run.SetOutputFile(outFile);
    run.SetMaterials("media_r3b.geo");

    // Geometry
    auto cave = new R3BCave("CAVE");
    cave->SetGeometryFileName("r3b_cave_vacuum.geo");
    run.AddModule(cave);
    run.AddModule(new R3BNeuland("neuland_test.geo.root"));

    // Primary particle generator
    auto primGen = new FairPrimaryGenerator();
    auto boxGen = new FairBoxGenerator(2112, 1);
    boxGen->SetXYZ(0, 0, 0.);
    boxGen->SetThetaRange(0., 0.);
    boxGen->SetPhiRange(0., 360.);
    boxGen->SetEkinRange(energy / 1000., energy / 1000.);
    primGen->AddGenerator(boxGen);
    run.SetGenerator(primGen);

    // Further setup options and initialization
    FairLogger::GetLogger()->SetLogVerbosityLevel("LOW");
    FairLogger::GetLogger()->SetLogScreenLevel("WARNING");
    run.SetStoreTraj(kFALSE);
    run.Init();

    // Connect runtime parameter file
    auto parOut = new FairParRootFileIo(true);
    parOut->open(parFile);
    auto rtdb = run.GetRuntimeDb();
    rtdb->setOutput(parOut);
    rtdb->saveOutput();

    // Save geometry for later reference
    // const TString geoFile = basePath + "/" + baseName + ".geo.root";
    // run->CreateGeometryFile(geoFile);

    // Simulate
    run.Run(1e6);
}
