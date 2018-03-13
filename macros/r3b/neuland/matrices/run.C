#include "../../r3ball.C"
#include "runconfig.C"

TCutG* GetCutProtonTrack()
{
    TCutG* cutg = new TCutG("cutg", 13);
    cutg->SetPoint(0, 0.678913, 81.5722);
    cutg->SetPoint(1, 8.46645, 162.758);
    cutg->SetPoint(2, 37.6198, 333.827);
    cutg->SetPoint(3, 64.976, 459.472);
    cutg->SetPoint(4, 83.0471, 543.557);
    cutg->SetPoint(5, 86.3419, 495.232);
    cutg->SetPoint(6, 51.897, 325.129);
    cutg->SetPoint(7, 26.5375, 175.322);
    cutg->SetPoint(8, 14.9561, 98.0026);
    cutg->SetPoint(9, 7.16853, 42.9124);
    cutg->SetPoint(10, 1.67732, 42.9124);
    cutg->SetPoint(11, 0.279552, 77.7062);
    cutg->SetPoint(12, 0.678913, 81.5722);
    return cutg;
}

TCutG* GetCutProtonTrack2()
{
    TCutG* cutg = new TCutG("cutg", 5); // 8);
    cutg->SetPoint(0, 11.4569, 194.501);
    cutg->SetPoint(1, 98.5325, 597.315);
    cutg->SetPoint(2, 99.8586, 508.12);
    cutg->SetPoint(3, 14.7277, 112.98);
    cutg->SetPoint(4, 10.3076, 184.91);
    cutg->SetPoint(5, 11.4569, 194.501);
    // cutg->SetPoint(0, 2.24663, 103.802);
    // cutg->SetPoint(1, 9.66944, 184.021);
    // cutg->SetPoint(2, 83.6006, 539.691);
    // cutg->SetPoint(3, 88.1532, 446.907);
    // cutg->SetPoint(4, 13.6283, 117.332);
    // cutg->SetPoint(5, 4.52296, 41.9459);
    // cutg->SetPoint(6, 1.3559, 94.1366);
    // cutg->SetPoint(7, 2.24663, 103.802);
    return cutg;
}

void ConnectParFileToRuntimeDb(const TString parFile, FairRuntimeDb* rtdb)
{
    FairParRootFileIo* io = new FairParRootFileIo();
    io->open(parFile);
    rtdb->setFirstInput(io);
    rtdb->setOutput(io);
    rtdb->saveOutput();
}

void ConnectParFilesToRuntimeDb(const TString parFile, const TString secondInput, FairRuntimeDb* rtdb)
{
    FairParRootFileIo* io = new FairParRootFileIo();
    io->open(parFile);

    FairParRootFileIo* io2 = new FairParRootFileIo();
    io2->open(secondInput);

    rtdb->setFirstInput(io);
    rtdb->setSecondInput(io2);

    rtdb->setOutput(io);
    rtdb->saveOutput();
}

void Simulate(const RunConfig& cfg)
{
    // System paths
    const TString workDirectory = getenv("VMCWORKDIR");
    gSystem->Setenv("GEOMPATH", workDirectory + "/geometry");
    gSystem->Setenv("CONFIG_DIR", workDirectory + "/gconfig");

    // Constant configuration
    const Int_t nEvents = 10000;
    const TString target = "LiH";
    const TString fMC = "TGeant4";
    const TString fGene = "ascii";
    const Bool_t fEventDisplay = kFALSE;
    const Bool_t fUserPList = kFALSE;
    const Bool_t fR3BMagnet = kTRUE;
    const Double_t fMeasCurrent = 2500.;

    // Geometry
    TMap geometry;
    // geometry.Add(new TObjString("TARGET"), new TObjString("target_" + target + ".geo.root"));
    // geometry.Add(new TObjString("ALADIN"), new TObjString("aladin_v13a.geo.root"));
    // geometry.Add(new TObjString("GLAD"), new TObjString("glad_v17_flange.geo.root"));
    geometry.Add(new TObjString("GLAD"), new TObjString("glad_vacuum.geo.root"));
    // geometry.Add(new TObjString("CRYSTALBALL"), new TObjString("cal_v13a.geo.root"));
    // geometry.Add(new TObjString("DCH"), new TObjString("dch_v13a.geo.root"));
    geometry.Add(new TObjString("NEULAND"), new TObjString(cfg.GetGeom()));
    // geometry.Add(new TObjString("CHAMBER"), new TObjString("Chamber.geo.root"));

    // Main Sim function call
    r3ball(nEvents,
           &geometry,
           target,
           fEventDisplay,
           fMC,
           fGene,
           fUserPList,
           fR3BMagnet,
           fMeasCurrent,
           cfg.GetSimu(),
           cfg.GetPara(),
           cfg.GetNeut());
}

void Digitize(const RunConfig& cfg)
{
    FairRunAna* run = new FairRunAna();
    run->SetSource(new FairFileSource(cfg.GetSimu()));
    run->SetOutputFile(cfg.GetDigi());
    ConnectParFileToRuntimeDb(cfg.GetPara(), run->GetRuntimeDb());

    run->AddTask(new R3BNeulandDigitizer());
    run->AddTask(new R3BNeulandDigiMon());

    /*for (Int_t thresh = 0; thresh <= 30; thresh += 1)
    {
        Neuland::DigitizingTacQuila* de = new Neuland::DigitizingTacQuila();
        de->SetPMTThreshold(thresh);
        run->AddTask(
            new R3BNeulandDigitizer(de, "NeulandPoints", TString("NeulandDigisT") + TString::Itoa(thresh, 10)));
        run->AddTask(new R3BNeulandDigiMon(TString("NeulandDigisT") + TString::Itoa(thresh, 10),
    TString("NeulandDigiMonT") + TString::Itoa(thresh, 10)));
    }*/

    run->AddTask(new R3BNeulandClusterFinder(1. * 7.5, 1. * 7.5, 2. * 7.5, 1.));

    run->AddTask(new R3BNeulandMCMon());
    run->AddTask(new R3BNeulandClusterMon("NeulandClusters", "NeulandClusterMon"));

    auto* mon = new R3BNeulandClusterMon("NeulandClusters", "NeulandClusterMonFiltered");
    mon->AddFilter([=](const R3BNeulandCluster* a)
                   {
                       return a->GetFirstDigi().GetEToF() > cfg.energy - 50. &&
                              a->GetFirstDigi().GetEToF() < cfg.energy + 50.;
                   });

    mon->AddFilter([](const R3BNeulandCluster* cluster)
                   {
                       return cluster->GetE() > 10.;
                   });

    /*const auto ptrack = GetCutProtonTrack2();
    const auto selectProtonTracks = [=](const R3BNeulandCluster* cluster)
    {
        return ptrack->IsInside((cluster->GetForemostDigi().GetPosition() - cluster->GetEnergyCentroid()).Mag(),
                                cluster->GetE());
    };
    mon->AddFilter(selectProtonTracks);*/

    run->AddTask(mon);

    run->Init();
    run->Run(0, 0);
    delete run;
}

void Reconstruct(const RunConfig& cfg)
{
    FairRunAna* run = new FairRunAna();
    run->SetSource(new FairFileSource(cfg.GetDigi()));
    run->SetOutputFile(cfg.GetReco());
    ConnectParFilesToRuntimeDb(cfg.GetPara(), cfg.GetNcut(), run->GetRuntimeDb());

    run->AddTask(
        new R3BNeulandNeutronReconstruction(new Neuland::Reconstruction2DCalibr(cfg.energy - 50, cfg.energy + 50., 2.),
                                            "NeulandClusters",
                                            "NeulandNeutrons2DCalibr"));

    auto recoeng = new Neuland::ReconstructionGate();
    recoeng->AddFilter([=](const R3BNeulandCluster* a)
                       {
                           return a->GetFirstDigi().GetEToF() > cfg.energy - 50. &&
                                  a->GetFirstDigi().GetEToF() < cfg.energy + 50.;
                       });
    recoeng->AddFilter([](const R3BNeulandCluster* cluster)
                       {
                           return cluster->GetE() > 10.;
                       });
    /*recoeng->AddFilter([](const R3BNeulandCluster* cluster)
                       {
                           return cluster->GetEnergyMoment() > 2.;
                       });*/



    /*const auto ptrack = GetCutProtonTrack();
    const auto selectProtonTracks = [=](const R3BNeulandCluster* cluster)
    {
        return ptrack->IsInside((cluster->GetForemostDigi().GetPosition() - cluster->GetEnergyCentroid()).Mag(),
                                cluster->GetE());
    };
    recoeng->AddFilter(selectProtonTracks);*/

    run->AddTask(new R3BNeulandNeutronReconstruction(recoeng, "NeulandClusters", "NeulandNeutronsEGate"));

    /*run->AddTask(new R3BNeulandNeutronReconstruction(
        new Neuland::ReconstructionNeuralNetworkMultiplicity(580., 630., 0., "tcp://gaia:55555"),
        "NeulandClusters",
        "NeulandNeutronsNeuralNetworkMultiplicity"));*/

    run->Init();
    run->Run(0, 0);
    delete run;
}

void Evaluate(const RunConfig& cfg)
{
    FairRunAna* run = new FairRunAna();
    run->SetSource(new FairFileSource(cfg.GetReco()));
    run->AddFriend(cfg.GetSimu());
    run->AddFriend(cfg.GetDigi());
    run->SetOutputFile(cfg.GetEval());
    ConnectParFileToRuntimeDb(cfg.GetPara(), run->GetRuntimeDb());

    run->AddTask(new R3BNeulandNeutronReconstructionMon("NeulandNeutrons2DCalibr", "NeulandRecoMon2DCalibr"));
    run->AddTask(new R3BNeulandNeutronReconstructionMon("NeulandNeutronsEGate", "NeulandRecoMonEGate"));
    /*run->AddTask(new R3BNeulandNeutronReconstructionMon("NeulandNeutronsNeuralNetworkMultiplicity",
                                                        "NeulandRecoMonNeuralNetworkMultiplicity"));*/

    run->AddTask(new R3BNeulandNeutronClusters("NeulandClusters", "NeulandNeutronClusters", 30.));
    run->AddTask(new R3BNeulandClusterMon("NeulandNeutronClusters", "NeulandNeutronClusterMon"));
    run->AddTask(new R3BNeulandNeutronReconstruction(new Neuland::ReconstructionEGate(), "NeulandNeutronClusters", "NeulandNeutronsGateNeutronClusters"));
    run->AddTask(new R3BNeulandNeutronReconstructionMon("NeulandNeutronsEGateNeutronClusters", "NeulandRecoMonEGateNeutronClusters"));

    run->Init();
    run->Run(0, 0);
    delete run;
}

void run(const TString what,
         const TString path,
         const Int_t distance,
         const Int_t energy,
         const Int_t erel,
         const Int_t nDoublePlanes,
         const Int_t nNeutrons)
{
    const RunConfig cfg(path, distance, energy, erel, nDoublePlanes, nNeutrons);

    if (what == "simu")
    {
        Simulate(cfg);
    }
    else if (what == "digi")
    {
        Digitize(cfg);
    }
    else if (what == "reco")
    {
        Reconstruct(cfg);
    }
    else if (what == "eval")
    {
        Evaluate(cfg);
    }
    else
    {
        std::cerr << "Unknown Operation" << std::endl;
    }
}
