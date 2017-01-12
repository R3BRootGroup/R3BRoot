void Step1_Simulate(const Int_t nEvents,
                    const TString basePath,
                    const TString baseName,
                    const UInt_t particleID,
                    const Double_t kinenergy,
                    const TString geoTag)
{
    // System paths
    const TString workDirectory = getenv("VMCWORKDIR");
    gSystem->Setenv("GEOMPATH", workDirectory + "/geometry");
    gSystem->Setenv("CONFIG_DIR", workDirectory + "/gconfig");

    // Output files
    const TString outFile = basePath + "/" + baseName + ".sim.root";
    const TString parFile = basePath + "/" + baseName + ".par.root";

    // Basic simulation setup
    FairRunSim* run = new FairRunSim();
    run->SetName("TGeant4");
    run->SetOutputFile(outFile);
    run->SetMaterials("media_r3b.geo");

    // run->SetUserConfig("g4R3bConfig.C");
    // run->SetUserCuts("SetCuts.C");

    // Geometry: Cave
    FairModule* cave = new R3BCave("CAVE300MeV");
    // cave->SetGeometryFileName("r3b_cave.geo");
    cave->SetGeometryFileName("r3b_cave_vacuum.geo");
    run->AddModule(cave);

    // Geometry: Neuland
    R3BDetector* neuland = new R3BNeuland();
    neuland->SetGeometryFileName("neuland_v2_1400cm_50dp.geo.root");
    run->AddModule(neuland);

    // Primary particle generator
    FairBoxGenerator* boxGen = new FairBoxGenerator(particleID);
    boxGen->SetThetaRange(0., 1.);
    boxGen->SetPhiRange(0., 360.);
    boxGen->SetEkinRange(kinenergy / 1000., kinenergy / 1000.);
    boxGen->SetXYZ(0., 0., 0.);
    boxGen->SetDebug(1);
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
    primGen->AddGenerator(boxGen);
    run->SetGenerator(primGen);

    // Further setup options and initialization
    FairLogger::GetLogger()->SetLogVerbosityLevel("LOW");
    run->SetStoreTraj(kTRUE);
    run->Init();

    // Connect runtime parameter file
    FairParRootFileIo* parFileIO = new FairParRootFileIo(kTRUE);
    parFileIO->open(parFile);
    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    rtdb->setOutput(parFileIO);
    rtdb->saveOutput();

    // Simulate
    run->Run(nEvents);
}
