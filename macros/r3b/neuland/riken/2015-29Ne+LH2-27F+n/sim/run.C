class RunConfig
{
  public:
    RunConfig(const TString path_)
        : path(path_)
    {
    }

    TString GetSimu() const { return TString::Format("%s/simu.root", path.Data()); }

    TString GetPara() const { return TString::Format("%s/para.root", path.Data()); }

    TString GetDigi() const { return TString::Format("%s/digi.root", path.Data()); }

    void ConnectParFileToRuntimeDb(FairRuntimeDb* rtdb) const
    {
        FairParRootFileIo* io = new FairParRootFileIo();
        io->open(GetPara());
        rtdb->setFirstInput(io);
        rtdb->setOutput(io);
        rtdb->saveOutput();
    }

  public:
    TString path;
};

void Simulate(const RunConfig& cfg)
{
    // System paths
    const TString workDirectory = getenv("VMCWORKDIR");
    gSystem->Setenv("GEOMPATH", workDirectory + "/geometry");
    gSystem->Setenv("CONFIG_DIR", workDirectory + "/gconfig");

    // Simulation base
    FairRunSim run;
    run.SetName("TGeant4");
    run.SetOutputFile(cfg.GetSimu());
    run.SetMaterials("media_r3b.geo");

    // World
    auto cave = new R3BCave("Cave");
    cave->SetGeometryFileName("r3b_cave_vacuum.geo");
    run.AddModule(cave);

    // SAMURAI experimental setup from 2015 without NeuLAND
    // GDML file exported from smsimulator, modified to use assembly volume
    // Center of magnet is coordinate origin. -> Move center of target to (0,0,0) in R3BRoot
    run.AddModule(new R3BGeoGDML("samurai-2015.gdml", TGeoTranslation(0., 0., 436.7)));

    // NeuLAND Demonstrator (4 double planes)
    // Distance = distance Target-Magnet + Magnet-NeuLAND + 1/2*NeuLAND depth
    run.AddModule(new R3BNeuland(4, TGeoTranslation(0., 0., 436.7 + 703.16 + 20.)));

    // Some random magnetic field -> Charged Fragments irrelevant
    // TODO: Maybe import SAMURAI Magnetic field. Not really needed for NeuLAND simulations
    auto magField = new R3BGladFieldMap("R3BGladMap");
    magField->SetScale(-0.6);
    run.SetField(magField);

    // Primaries
    auto primGen = new FairPrimaryGenerator();
    auto gen = new R3BPhaseSpaceGenerator();
    gen->SetBeamEnergyAMeV(215);
    gen->SetErelkeV(1000);
    gen->AddHeavyIon({ "F-27", 9, 27, 5, 0., 25.17527 });
    gen->AddParticle(2112);
    primGen->AddGenerator(gen);
    // primGen->AddGenerator(new R3BAsciiGenerator(workDirectory + "/input/" + "siminput_29Ne_LH2_1n.dat"));
    run.SetGenerator(primGen);

    // Init & Special MC Settings
    run.SetStoreTraj(false);
    run.Init();
    TVirtualMC::GetMC()->SetRandom(new TRandom3(time(nullptr)));
    TVirtualMC::GetMC()->SetMaxNStep(100000);

    // Database
    auto rtdb = run.GetRuntimeDb();
    auto parOut = new FairParRootFileIo(true);
    parOut->open(cfg.GetPara());
    rtdb->setOutput(parOut);
    rtdb->saveOutput();

    // Go
    run.Run(1e6);

    // Draw geometry
    if (!gROOT->IsBatch())
    {
        gGeoManager->GetTopVolume()->Draw("ogl");
    }
}

void Digitize(const RunConfig& cfg)
{
    FairRunAna run;
    run.SetSource(new FairFileSource(cfg.GetSimu()));
    run.SetOutputFile(cfg.GetDigi());
    cfg.ConnectParFileToRuntimeDb(run.GetRuntimeDb());

    // Analyze Raw MC Data
    run.AddTask(new R3BNeulandMCMon());

    // Digitize
    run.AddTask(new R3BNeulandDigitizer());
    run.AddTask(new R3BNeulandDigiMon("NeulandDigis", "NeulandDigiMon"));

    // Cluster building
    run.AddTask(new R3BNeulandClusterFinder(1. * 7.5, 1. * 7.5, 2. * 7.5, 1., "NeulandDigis", "NeulandClusters"));
    run.AddTask(new R3BNeulandClusterMon("NeulandClusters", "NeulandClusterMon"));

    run.Init();
    run.Run(0, 0);
}

// Macro entry point, dispatches tasks
void run(const TString what, const TString path)
{
    FairLogger::GetLogger()->SetLogVerbosityLevel("LOW");
    FairLogger::GetLogger()->SetLogScreenLevel("FATAL");

    std::map<TString, std::function<void(RunConfig)>> functions;
    functions["simu"] = Simulate;
    functions["digi"] = Digitize;

    const RunConfig cfg(path);
    functions.at(what)(cfg);
}
