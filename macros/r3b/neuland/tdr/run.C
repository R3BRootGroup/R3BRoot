// The runconfig holds the filename and some common code
#include "runconfig.C"

// The simulation task that performes the actual simulation
// Creates simu-files with points from the Sn Input files
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

    // Magnet
    run.AddModule(new R3BGladMagnet("glad_vacuum.geo.root"));
    auto magField = new R3BGladFieldMap("R3BGladMap");
    magField->SetScale(-0.6);
    run.SetField(magField);

    // NeuLAND
    // TODO: Separate Detector geometry and positioning.
    run.AddModule(new R3BNeuland(cfg.GetGeom()));

    // Primaries
    auto primGen = new FairPrimaryGenerator();
    primGen->AddGenerator(new R3BAsciiGenerator(workDirectory + "/input/" + cfg.GetNeut()));
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
    auto fieldPar = (R3BFieldPar*)rtdb->getContainer("R3BFieldPar");
    fieldPar->SetParameters(magField);
    fieldPar->setChanged();
    auto parOut = new FairParRootFileIo(true);
    parOut->open(cfg.GetPara());
    rtdb->setOutput(parOut);
    rtdb->saveOutput();
    rtdb->print();

    // Go
    run.Run(1e4);
}

// Digitizing and Cluster building
// Reads simu-files and creates digis and clusters
void Digitize(const RunConfig& cfg)
{
    FairRunAna run;
    run.SetSource(new FairFileSource(cfg.GetSimu()));
    run.SetOutputFile(cfg.GetDigi());
    ConnectParFileToRuntimeDb(cfg.GetPara(), run.GetRuntimeDb());

    // Analyze Raw MC Data
    run.AddTask(new R3BNeulandMCMon());

    // Digitize
    run.AddTask(new R3BNeulandDigitizer());
    run.AddTask(new R3BNeulandDigiMon());

    // Cluster building
    run.AddTask(new R3BNeulandClusterFinder(1. * 7.5, 1. * 7.5, 2. * 7.5, 1., "NeulandDigis", "NeulandClusters"));
    run.AddTask(new R3BNeulandClusterMon("NeulandClusters", "NeulandClusterMon"));

    // Cheating to classify clusters as primary or secondary
    run.AddTask(
        new R3BNeulandNeutronClusters("NeulandClusters", "NeulandPrimaryClusters", "NeulandSecondaryClusters", 25.));
    run.AddTask(new R3BNeulandClusterMon("NeulandPrimaryClusters", "NeulandPrimaryClustersMon"));
    run.AddTask(new R3BNeulandClusterMon("NeulandSecondaryClusters", "NeulandSecondaryClustersMon"));

    run.Init();
    run.Run(0, 0);
}

// For the 2D Calibr Method from TDR, create the file with the cuts
void Calibr(const RunConfig& cfg)
{
    // Only needs to run once with all available neutron multiplicities
    if (cfg.nNeutrons != cfg.nMax)
    {
        return;
    }

    // TODO: Try to eliminate this by autoloading
    gSystem->Load("libR3BNeuland");

    Neuland::Neutron2DCalibr cal = Neuland::Neutron2DCalibr(cfg.nNeutrons);
    for (Int_t n = 1; n <= cfg.nNeutrons; n++)
    {
        RunConfig tmp(cfg);
        tmp.nNeutrons = n;
        cal.AddClusterFile(tmp.GetDigi());
    }

    // Starting parameters
    std::vector<Double_t> vslope = { 0.04, 0.001, 0.001, 10 };
    std::vector<Double_t> vdistance = {
        Double_t(cfg.energy) / 60., 0.5, Double_t(cfg.energy) / 100., Double_t(cfg.energy) / 10.
    };
    std::vector<Double_t> vdist_off = { 3, 0.5, 3, 6 };

    // Create Cuts
    cal.Optimize(vslope, vdistance, vdist_off);

    // Write the output files (root, dat, pdf)
    cal.WriteParameterFile(cfg.GetNcut());
    std::ofstream o(TString(cfg.GetNcut()).ReplaceAll(".root", ".dat"));
    cal.Print(o);
    cal.Draw(TString(cfg.GetNcut()).ReplaceAll(".root", ".pdf"));
}

// Reconstruct neutrons using the TDR and Scoring Methods.
// Also create neutrons from the cheated clusters for comparison
void Reconstruct(const RunConfig& cfg)
{
    FairRunAna run;
    run.SetSource(new FairFileSource(cfg.GetDigi()));
    run.SetOutputFile(cfg.GetReco());
    ConnectParFilesToRuntimeDb(cfg.GetPara(), cfg.GetNcut(), run.GetRuntimeDb());

    // TDR Reco
    auto recotdr = new Neuland::RecoTDR();
    recotdr->SetBeam(605, 0.793);
    run.AddTask(new R3BNeulandNeutronReconstruction(recotdr, "NeulandClusters", "NeulandNeutronsTDR"));

    // Scoring Reco
    TH1D hScore("scoreScoring", "scoreScoring", 100, -50., 50.);

    auto reco = new Neuland::ClusterScoring();
    reco->SetMinScore(0);
    reco->SetMaxN(cfg.nNeutrons);

    // Custom EToF Cut
    auto scoreEToF = [](Neuland::ClusterScoring::ScoredClusters& scs) {
        for (auto& sc : scs)
        {
            if (sc.cluster->GetEToF() > 620 || sc.cluster->GetEToF() < 580)
            {
                sc.score -= 100.;
            }
        }
    };
    reco->AddScorer(scoreEToF);

    reco->AddScorer(Neuland::ClusterScoring::scoreEnergy);
    reco->AddScorer(Neuland::ClusterScoring::scoreClusterSize);
    reco->AddScorer(Neuland::ClusterScoring::scoreHitSequenceWithDescecdingZPosition);
    reco->AddScorer(Neuland::ClusterScoring::scoreElasticSecondaryInterations); // This seems to hurt eff
    reco->AddScorer(Neuland::ClusterScoring::scoreProtonTracksMatchingToF);
    reco->AddScorer(Neuland::ClusterScoring::handleMultiHitClusters);

    // Create Scores with the same reconstruction procedure, but different score limits
    auto reco1 = new Neuland::ClusterScoring(*reco);
    reco1->SetMinScore(1);

    auto reco3 = new Neuland::ClusterScoring(*reco);
    reco3->SetMinScore(3);

    // Report on the score
    auto scoreReport = [&](Neuland::ClusterScoring::ScoredClusters& scs) {
        for (const auto& sc : scs)
        {
            hScore.Fill(sc.score);
        }
    };
    reco->AddScorer(scoreReport);

    run.AddTask(new R3BNeulandNeutronReconstruction(reco, "NeulandClusters", "NeulandNeutronsScoring"));
    run.AddTask(new R3BNeulandNeutronReconstruction(reco1, "NeulandClusters", "NeulandNeutronsScoring1"));
    run.AddTask(new R3BNeulandNeutronReconstruction(reco3, "NeulandClusters", "NeulandNeutronsScoring3"));

    // Cheating
    auto recocheat = new Neuland::ClusterScoring();
    recocheat->SetMinScore(-1);
    run.AddTask(new R3BNeulandNeutronReconstruction(recocheat, "NeulandPrimaryClusters", "NeulandNeutronsCheat"));

    run.Init();
    run.Run(0, 0);

    hScore.Write();
}

void Evaluate(const RunConfig& cfg)
{
    FairRunAna run;
    run.SetSource(new FairFileSource(cfg.GetReco()));
    run.AddFriend(cfg.GetSimu());
    run.AddFriend(cfg.GetDigi());
    run.SetOutputFile(cfg.GetEval());
    ConnectParFileToRuntimeDb(cfg.GetPara(), run.GetRuntimeDb());

    run.AddTask(new R3BNeulandNeutronReconstructionMon("NeulandNeutronsCheat", "NeulandRecoMonCheat"));
    run.AddTask(new R3BNeulandNeutronReconstructionStatistics(
        "NeulandPrimaryClusters", "NeulandSecondaryClusters", "NeulandNeutronsCheat"));

    run.AddTask(new R3BNeulandNeutronReconstructionMon("NeulandNeutronsScoring", "NeulandRecoMonScoring"));
    run.AddTask(new R3BNeulandNeutronReconstructionStatistics(
        "NeulandPrimaryClusters", "NeulandSecondaryClusters", "NeulandNeutronsScoring"));
    run.AddTask(new R3BNeulandNeutronReconstructionMon("NeulandNeutronsScoring1", "NeulandRecoMonScoring1"));
    run.AddTask(new R3BNeulandNeutronReconstructionStatistics(
        "NeulandPrimaryClusters", "NeulandSecondaryClusters", "NeulandNeutronsScoring1"));
    run.AddTask(new R3BNeulandNeutronReconstructionMon("NeulandNeutronsScoring3", "NeulandRecoMonScoring3"));
    run.AddTask(new R3BNeulandNeutronReconstructionStatistics(
        "NeulandPrimaryClusters", "NeulandSecondaryClusters", "NeulandNeutronsScoring3"));

    run.AddTask(new R3BNeulandNeutronReconstructionMon("NeulandNeutronsTDR", "NeulandRecoMonTDR"));
    run.AddTask(new R3BNeulandNeutronReconstructionStatistics(
        "NeulandPrimaryClusters", "NeulandSecondaryClusters", "NeulandNeutronsTDR"));

    run.Init();
    run.Run(0, 0);
}

// Macro entry point, dispatches tasks
void run(const TString what,
         const TString path,
         const Int_t distance,
         const Int_t energy,
         const Int_t erel,
         const Int_t nDoublePlanes,
         const Int_t nNeutrons)
{
    const RunConfig cfg(path, distance, energy, erel, nDoublePlanes, nNeutrons);

    std::map<TString, std::function<void(RunConfig)>> functions;
    functions["simu"] = Simulate;
    functions["digi"] = Digitize;
    functions["calibr"] = Calibr;
    functions["reco"] = Reconstruct;
    functions["eval"] = Evaluate;

    functions.at(what)(cfg);
}
