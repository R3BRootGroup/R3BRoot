#include "../../r3ball.C"
#include "runconfig.C"

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
    // geometry.Add(new TObjString("GLAD"), new TObjString("glad_v13a.geo.root"));
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
    run->SetInputFile(cfg.GetSimu());
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
        run->AddTask(new R3BNeulandDigiMon(TString("NeulandDigisT") + TString::Itoa(thresh, 10), TString("NeulandDigiMonT") + TString::Itoa(thresh, 10)));
    }*/

    run->AddTask(new R3BNeulandClusterFinder(1. * 7.5, 1. * 7.5, 1. * 7.5, 5.));

    run->AddTask(new R3BNeulandMCMon());
    run->AddTask(new R3BNeulandClusterMon());

    run->Init();
    run->Run(0, 0);
    delete run;
}

void Reconstruct(const RunConfig& cfg)
{
    FairRunAna* run = new FairRunAna();
    run->SetInputFile(cfg.GetDigi());
    run->SetOutputFile(cfg.GetReco());
    ConnectParFilesToRuntimeDb(cfg.GetPara(), cfg.GetNcut(), run->GetRuntimeDb());

    run->AddTask(
        new R3BNeulandNeutronReconstruction(new Neuland::Reconstruction2DCalibr(cfg.energy - 50, cfg.energy + 50., 0.),
                                            "NeulandClusters",
                                            "NeulandNeutrons2DCalibr"));
    run->AddTask(
        new R3BNeulandNeutronReconstruction(new Neuland::ReconstructionEGate(cfg.energy - 50, cfg.energy + 50., 0.),
                                            "NeulandClusters",
                                            "NeulandNeutronsEGate"));
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
    run->SetInputFile(cfg.GetReco());
    run->AddFriend(cfg.GetSimu());
    run->SetOutputFile(cfg.GetEval());
    ConnectParFileToRuntimeDb(cfg.GetPara(), run->GetRuntimeDb());

    run->AddTask(new R3BNeulandNeutronReconstructionMon("NeulandNeutrons2DCalibr", "NeulandRecoMon2DCalibr"));
    run->AddTask(new R3BNeulandNeutronReconstructionMon("NeulandNeutronsEGate", "NeulandRecoMonEGate"));
    /*run->AddTask(new R3BNeulandNeutronReconstructionMon("NeulandNeutronsNeuralNetworkMultiplicity",
                                                        "NeulandRecoMonNeuralNetworkMultiplicity"));*/

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
