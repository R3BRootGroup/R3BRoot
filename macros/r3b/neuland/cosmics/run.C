class RunConfig
{
  public:
    RunConfig(const TString path_)
        : path(path_)
        , prefix("cosmics")
        , nEvents(1e6)
    {
    }

    TString GetGeom() const { return "neuland_v2_1400cm_30dp.geo.root"; }

    TString GetSimu() const { return TString::Format("%s/%s.simu.root", path.Data(), prefix.Data()); }

    TString GetPara() const { return TString::Format("%s/%s.para.root", path.Data(), prefix.Data()); }

    TString GetDigi() const { return TString::Format("%s/%s.digi.root", path.Data(), prefix.Data()); }

    FairGenerator* GetGen() const
    {
        Double_t pdg = 13;       // muons -
        Double_t EThresh = .001; // in GeV
        auto cosmicGen = new R3BCosmicGenerator(pdg, 1, EThresh);
        cosmicGen->SetThetaRange(0., 180.);
        cosmicGen->SetPhiRange(0., 360.);
        cosmicGen->SetBoxXYZ(-150., 1400., 150., 1400. + 300., 150.); // (x1,z1,x2,z2,y)
        cosmicGen->Init();
        return cosmicGen;
    }

  public:
    TString path;
    TString prefix;
    Int_t nEvents;
};

void ConnectParFileToRuntimeDb(const TString parFile, FairRuntimeDb* rtdb)
{
    FairParRootFileIo* io = new FairParRootFileIo();
    io->open(parFile);
    rtdb->setFirstInput(io);
    rtdb->setOutput(io);
    rtdb->saveOutput();
}

void Simulate(const RunConfig& cfg)
{
    // System paths
    const TString workDirectory = getenv("VMCWORKDIR");
    gSystem->Setenv("GEOMPATH", workDirectory + "/geometry");
    gSystem->Setenv("CONFIG_DIR", workDirectory + "/gconfig");

    // Simulation Base
    FairRunSim run;
    run.SetName("TGeant4");
    run.SetOutputFile(cfg.GetSimu());
    run.SetMaterials("media_r3b.geo");

    // World
    auto cave = new R3BCave("Cave");
    cave->SetGeometryFileName("r3b_cave_vacuum.geo");
    run.AddModule(cave);

    // NeuLAND
    run.AddModule(new R3BNeuland(cfg.GetGeom()));

    // Primaries
    auto primGen = new FairPrimaryGenerator();
    primGen->AddGenerator(cfg.GetGen());
    run.SetGenerator(primGen);

    // Logging
    run.SetStoreTraj(false);
    FairLogger::GetLogger()->SetLogVerbosityLevel("LOW");
    FairLogger::GetLogger()->SetLogScreenLevel("FATAL");

    // Init & Special MC Settings
    run.Init();
    TVirtualMC::GetMC()->SetRandom(new TRandom3(time(nullptr)));
    TVirtualMC::GetMC()->SetMaxNStep(100000);

    // Database
    auto rtdb = run.GetRuntimeDb();
    auto parOut = new FairParRootFileIo(true);
    parOut->open(cfg.GetPara());
    rtdb->setOutput(parOut);
    rtdb->saveOutput();
    rtdb->print();

    // Go
    run.Run(cfg.nEvents);
}

void Digitize(const RunConfig& cfg)
{
    FairRunAna run;
    run.SetSource(new FairFileSource(cfg.GetSimu()));
    run.SetOutputFile(cfg.GetDigi());
    ConnectParFileToRuntimeDb(cfg.GetPara(), run.GetRuntimeDb());

    // Analyze Raw MC Data
    run.AddTask(new R3BNeulandMCMon(/* "3DTRACK" */));

    // Digitize
    run.AddTask(new R3BNeulandDigitizer());
    run.AddTask(new R3BNeulandDigiMon("NeulandDigis", "NeulandDigiMon" /*, "3DTRACK" */));
    run.AddTask(new R3BNeulandDigiEval());

    // Cluster building
    run.AddTask(new R3BNeulandClusterFinder(1. * 7.5, 1. * 7.5, 2. * 7.5, 1., "NeulandDigis", "NeulandClusters"));
    run.AddTask(new R3BNeulandClusterMon("NeulandClusters", "NeulandClusterMon" /*, "3DTRACK" */));

    run.Init();
    run.Run(0, 0);
}

// Macro entry point, dispatches tasks
void run(const TString what, const TString path)
{
    const RunConfig cfg(path);

    std::map<TString, std::function<void(RunConfig)>> functions;
    functions["simu"] = Simulate;
    functions["digi"] = Digitize;

    functions.at(what)(cfg);
}
