void Step1_Simulate(const Double_t momentum, const UInt_t nEvents, const TString basePath, const TString baseName)
{
    // Output files
    const TString outFile = basePath + "/" + baseName + ".sim.root";
    const TString parFile = basePath + "/" + baseName + ".par.root";
    const TString geoFile = basePath + "/" + baseName + ".geo.root";

    // Basic simulation setup
    FairRunSim* run = new FairRunSim();
    run->SetName("TGeant4");
    run->SetOutputFile(outFile);
    run->SetMaterials("media_r3b.geo");

    // Geometry: Cave
    FairModule* cave = new R3BCave("CAVE");
    cave->SetGeometryFileName("r3b_cave_vacuum.geo");
    run->AddModule(cave);

    R3BNeuland* neuland = new R3BNeuland();
    neuland->SetGeometryFileName("neuland_test.geo.root");
    run->AddModule(neuland);

    // Primary particle generator
    const Int_t particleID = 2112;
    const Int_t nParticles = 1;
    FairBoxGenerator* boxGen = new FairBoxGenerator(particleID, nParticles);
    boxGen->SetXYZ(0, 0, 0.);
    boxGen->SetThetaRange(0., 0.);
    boxGen->SetPhiRange(0., 0.);
    boxGen->SetPRange(momentum, momentum);
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
    primGen->AddGenerator(boxGen);
    run->SetGenerator(primGen);

    // Further setup options and initialization
    FairLogger::GetLogger()->SetLogVerbosityLevel("LOW");
    FairLogger::GetLogger()->SetLogScreenLevel("WARNING");
    run->SetStoreTraj(kFALSE);
    run->Init();

    // Connect runtime parameter file
    FairParRootFileIo* parFileIO = new FairParRootFileIo(kTRUE);
    parFileIO->open(parFile);
    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    rtdb->setOutput(parFileIO);
    rtdb->saveOutput();

    // Save geometry for later reference
    // run->CreateGeometryFile(geoFile);

    // Simulate
    run->Run(nEvents);
}
