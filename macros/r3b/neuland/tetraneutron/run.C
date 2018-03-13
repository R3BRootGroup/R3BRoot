#include "../../r3ball.C"
#include "runconfig.C"

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
    const Int_t nEvents = 100000;
    const TString target = "LiH";
    const TString fMC = "TGeant4";
    const TString fGene = "ascii";
    const Bool_t fEventDisplay = kFALSE;
    const Bool_t fUserPList = kFALSE;
    const Bool_t fR3BMagnet = kTRUE;
    const Double_t fMeasCurrent = 2500.;
    const Double_t seed = time(nullptr);

    // Geometry
    TMap geometry;
    // geometry.Add(new TObjString("TARGET"), new TObjString("target_" + target + ".geo.root"));
    // geometry.Add(new TObjString("ALADIN"), new TObjString("aladin_v13a.geo.root"));
    geometry.Add(new TObjString("GLAD"), new TObjString("glad_vacuum.geo.root"));
    // geometry.Add(new TObjString("CRYSTALBALL"), new TObjString("cal_v13a.geo.root"));
    // geometry.Add(new TObjString("DCH"), new TObjString("dch_v13a.geo.root"));
    geometry.Add(new TObjString("NEULAND"), new TObjString(cfg.GetGeom()));

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
    run->AddTask(new R3BNeulandClusterFinder(2. * 7.5, 2. * 7.5, 3. * 7.5, 10.));

    run->AddTask(new R3BNeulandMCMon());
    run->AddTask(new R3BNeulandDigiMon());
    auto cm = new R3BNeulandClusterMon();
    //cm->AddFilter([](const R3BNeulandCluster* c) { return c->GetE() > 5.; });
    run->AddTask(cm);

    run->Init();
    run->Run(0, 0);
    delete run;
}

void Reconstruct(const RunConfig& cfg)
{
    auto theh = new TH2D("theh", "", 100, 0, 1000, 100, 0, 2000);
    theh->SetDirectory(nullptr);
    auto ediff = new TH2D("ediff", "Reconstructed Proton Energy VS Measured Cluster Energy", 100, 0, 300, 100, 0, 300);
    ediff->GetXaxis()->SetTitle("E_{p'}");
    ediff->GetYaxis()->SetTitle("E_{cluster}");
    ediff->SetDirectory(nullptr);
    auto etheta = new TH2D("etheta", "", 100, 0, 1000, 100, -1, 1);

    FairRunAna* run = new FairRunAna();
    run->SetSource(new FairFileSource(cfg.GetDigi()));
    run->SetOutputFile(cfg.GetReco());
    ConnectParFilesToRuntimeDb(cfg.GetPara(), cfg.GetNcut(), run->GetRuntimeDb());

    auto reco2D = new Neuland::Reconstruction2DCalibr();
    reco2D->AddFilter([=](const R3BNeulandCluster* a) {
        return a->GetFirstDigi().GetEToF() > cfg.energy - 50. && a->GetFirstDigi().GetEToF() < cfg.energy + 50.;
    });
    /*reco2D->AddFilter([](const R3BNeulandCluster* cluster) {
        const auto en = Neuland::NeutronEnergyFromElasticProtonScattering(cluster);
        return std::abs(cluster->GetFirstDigi().GetEToF() - en) < 70.;
    });*/

    run->AddTask(new R3BNeulandNeutronReconstruction(reco2D, "NeulandClusters", "NeulandNeutrons2DCalibr"));

    auto recoeng = new Neuland::ReconstructionGate();
    recoeng->AddFilter([=](const R3BNeulandCluster* a) {
        return a->GetFirstDigi().GetEToF() > cfg.energy - 50. && a->GetFirstDigi().GetEToF() < cfg.energy + 50.;
    });
    recoeng->AddFilter([&](const R3BNeulandCluster* a) {
        const auto epe = Neuland::NeutronEnergyFromElasticProtonScattering(a);
        // std::cout << epe;
        //if (Neuland::RecoilProtonEnergy(a) > 10. && epe > 10.)
        {
            const TVector3 pNUnit = a->GetFirstDigi().GetPosition().Unit();
            // const TVector3 pN = std::sqrt( std::pow(a->GetFirstDigi()->GetEToF(), 2) + 2 *
            // a->GetFirstDigi()->GetEToF() * 900);
            const TVector3 pp_Unit = (a->GetEnergyCentroid() - a->GetFirstDigi().GetPosition()).Unit(); //
            // const TVector3 pp_ = std::sqrt( std::pow(Neuland::RecoilProtonEnergy(a), 2) + 2 *
            // Neuland::RecoilProtonEnergy(a) * 900);
            const Double_t cosTheta = pNUnit.Dot(pp_Unit);

            //etheta->Fill(a->GetE(), cosTheta);
             theh->Fill(a->GetFirstDigi().GetEToF(), epe);
            //theh->Fill(Neuland::RecoilProtonEnergy(a), epe);
            //ediff->Fill(Neuland::RecoilProtonEnergy(a), 1.25*a->GetE());
        }
        return true;
    });
    recoeng->AddFilter([](const R3BNeulandCluster* cluster) {
        const auto en = Neuland::NeutronEnergyFromElasticProtonScattering(cluster);
        const bool isMatchingDepositedEnergy = std::abs(Neuland::RecoilProtonEnergy(cluster) - 1.25*cluster->GetE()) < 50.;
        const bool isMatchingToF = std::abs(cluster->GetFirstDigi().GetEToF() - en) < 50.;
        return isMatchingDepositedEnergy && isMatchingToF;
    });
    run->AddTask(new R3BNeulandNeutronReconstruction(recoeng, "NeulandClusters", "NeulandNeutronsEGate"));



    const auto ptrack = GetCutProtonTrack2();
    auto recoHintExtern = new Neuland::ReconstructionHintExtern(cfg.nNeutrons);
    /*recoHintExtern->AddFilter([=](const R3BNeulandCluster* a)
                              {
                                  return a->GetFirstDigi().GetEToF() > cfg.energy - 50. &&
                                         a->GetFirstDigi().GetEToF() < cfg.energy + 50.;
                              });*/
    /*recoHintExtern->AddFilter([](const R3BNeulandCluster* cluster) {
        /*return ptrack->IsInside(
            (cluster->GetForemostDigi().GetPosition() - cluster->GetEnergyCentroid()).Mag(),
            cluster->GetE());*/
        // return cluster->GetEnergyMoment() > 10.;
      //  return cluster->GetE() > 175.;
    //});
    run->AddTask(new R3BNeulandNeutronReconstruction(recoHintExtern, "NeulandClusters", "NeulandNeutronsHintExtern"));

    /*    run->AddTask(new R3BNeulandNeutronReconstruction(
            new Neuland::ReconstructionNeuralNetworkMultiplicity(580., 630., 0., "tcp://gaia:55555"),
            "NeulandClusters",
            "NeulandNeutronsNeuralNetworkMultiplicity"));*/

    auto recoeng2 = new Neuland::ReconstructionGate();
    recoeng2->AddFilter([=](const R3BNeulandCluster* a) {
        return a->GetFirstDigi().GetEToF() > cfg.energy - 50. && a->GetFirstDigi().GetEToF() < cfg.energy + 50.;
    });
    const auto selectProtonTracks = [=](const R3BNeulandCluster* cluster) {
        /*return ptrack->IsInside((cluster->GetForemostDigi().GetPosition() - cluster->GetEnergyCentroid()).Mag(),
                                cluster->GetE());*/
        // return cluster->GetEnergyMoment() > 3.;
        return cluster->GetE() > 50.;
    };
    recoeng2->AddFilter(selectProtonTracks);
    run->AddTask(new R3BNeulandNeutronReconstruction(recoeng2, "NeulandClusters", "NeulandNeutronsPGate"));

    run->Init();
    run->Run(0, 0);
    delete run;
    // theh->DrawClone("colz");
     etheta->DrawClone("colz");
    //ediff->DrawClone("colz");
}

void Evaluate(const RunConfig& cfg)
{
    auto theh = new TH2D("theh", "", 100, 0, 1000, 100, 0, 2000);
    theh->SetDirectory(nullptr);
    auto ediff =
        new TH2D("ediff", "Reconstructed Proton Energy VS Measured Cluster Energy", 100, 0, 1000, 100, 0, 1000);
    ediff->GetXaxis()->SetTitle("E_{p'}");
    ediff->GetYaxis()->SetTitle("E_{cluster}");
    ediff->SetDirectory(nullptr);
    auto etheta = new TH2D("etheta", "", 100, 0, 1000, 100, -1, 1);

    FairRunAna* run = new FairRunAna();
    run->SetSource(new FairFileSource(cfg.GetReco()));
    run->AddFriend(cfg.GetSimu());
    run->AddFriend(cfg.GetDigi());
    run->SetOutputFile(cfg.GetEval());
    ConnectParFileToRuntimeDb(cfg.GetPara(), run->GetRuntimeDb());

    run->AddTask(new R3BNeulandNeutronReconstructionMon("NeulandNeutrons2DCalibr", "NeulandRecoMon2DCalibr"));
    run->AddTask(new R3BNeulandNeutronReconstructionMon("NeulandNeutronsEGate", "NeulandRecoMonEGate"));
    run->AddTask(new R3BNeulandNeutronReconstructionMon("NeulandNeutronsPGate", "NeulandRecoMonPGate"));
    run->AddTask(new R3BNeulandNeutronReconstructionMon("NeulandNeutronsHintExtern", "NeulandRecoMonHintExtern"));
    /*run->AddTask(new R3BNeulandNeutronReconstructionMon("NeulandNeutronsNeuralNetworkMultiplicity",
                                                        "NeulandRecoMonNeuralNetworkMultiplicity"));*/

    run->AddTask(new R3BNeulandNeutronClusters("NeulandClusters", "NeulandNeutronClusters", 10.));
    run->AddTask(new R3BNeulandClusterMon("NeulandNeutronClusters", "NeulandNeutronClusterMon"));

    auto recoeng = new Neuland::ReconstructionGate();
    run->AddTask(
        new R3BNeulandNeutronReconstruction(recoeng, "NeulandNeutronClusters", "NeulandNeutronsEGateNeutronClusters"));
    run->AddTask(new R3BNeulandNeutronReconstructionMon("NeulandNeutronsEGateNeutronClusters",
                                                        "NeulandRecoMonEGateNeutronClusters"));

    run->Init();
    run->Run(0, 0);
    delete run;
    // theh->DrawClone("colz");
    // ediff->DrawClone("colz");
    etheta->DrawClone("colz");
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

    // FairLogger::GetLogger()->SetLogScreenLevel("DEBUG");
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
