class RunConfig
{
  public:
    RunConfig(const TString path_, const TString plist_, const Int_t energy_, const Double_t thresh_)
        : path(path_)
        , plist(plist_)
        , energy(energy_)
        , thresh(thresh_)
    {
        if (energy == 110)
        {
            velocityAcceptanceMin = 0.42;
            velocityAcceptanceMax = 0.46;
        }
        if (energy == 250)
        {
            velocityAcceptanceMin = 0.60;
            velocityAcceptanceMax = 0.64;
        }
    }

    TString GetSimu() const { return TString::Format("%s/%s-%dMeV.simu.root", path.Data(), plist.Data(), energy); }

    TString GetPara() const { return TString::Format("%s/%s-%dMeV.para.root", path.Data(), plist.Data(), energy); }

    TString GetDigi() const
    {
        return TString::Format("%s/%s-%dMeV-thresh%.1f.digi.root", path.Data(), plist.Data(), energy, thresh);
    }

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
    TString plist;
    Int_t energy;
    Double_t thresh;
    Double_t velocityAcceptanceMin;
    Double_t velocityAcceptanceMax;
};

void Simulate(const RunConfig& cfg)
{
    // System paths
    const TString workDirectory = getenv("VMCWORKDIR");
    gSystem->Setenv("GEOMPATH", workDirectory + "/geometry");
    gSystem->Setenv("CONFIG_DIR", workDirectory + "/gconfig");

    // Physics list
    gSystem->Setenv("PHYSICSLIST", cfg.plist);

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

    // Primaries
    auto primGen = new FairPrimaryGenerator();
    auto gen = new FairBoxGenerator(2112, 1);
    gen->SetXYZ(0, 0, 0.);
    gen->SetThetaRange(0., 2.);
    gen->SetPhiRange(0., 360.);
    gen->SetEkinRange(cfg.energy / 1000., cfg.energy / 1000.);
    primGen->AddGenerator(gen);
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
    run.Run(1e5);

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

    auto tacquila = new Neuland::DigitizingTacQuila();
    tacquila->SetPMTThreshold(2.);

    auto digitizer = new R3BNeulandDigitizer(tacquila);

    // Hit energy threshold
    digitizer->AddFilter([=](const R3BNeulandDigi* digi) { return digi->GetE() > cfg.thresh; });

    // ±40mrad radial acceptance
    const Double_t radialAcceptance = 1080. * TMath::Tan(0.040);
    digitizer->AddFilter([=](const R3BNeulandDigi* digi) { return digi->GetPosition().Perp() < radialAcceptance; });

    // velocity cut
    digitizer->AddFilter([=](const R3BNeulandDigi* digi) {
        return digi->GetBeta() < cfg.velocityAcceptanceMax && digi->GetBeta() > cfg.velocityAcceptanceMin;
    });

    run.AddTask(digitizer);

    run.AddTask(new R3BNeulandMCMon());
    run.AddTask(new R3BNeulandDigiMon());

    run.Init();
    run.Run(0, 0);
}

// Macro entry point, dispatches tasks
void run(const TString what, const TString path, const TString plist, const Int_t energy, const Double_t thresh = 0.)
{
    FairLogger::GetLogger()->SetLogVerbosityLevel("LOW");
    FairLogger::GetLogger()->SetLogScreenLevel("FATAL");

    std::map<TString, std::function<void(RunConfig)>> functions;
    functions["simu"] = Simulate;
    functions["digi"] = Digitize;

    const RunConfig cfg(path, plist, energy, thresh / 10.);
    functions.at(what)(cfg);
}
