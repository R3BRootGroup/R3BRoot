inline void ConnectParFileToRuntimeDb(const TString parFile, const TString secondInput, FairRuntimeDb* rtdb)
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

void Step4_Reconstruction(const TString clusterFile, const TString calibrFile)
{
    const TString parFile = (TString(clusterFile).ReplaceAll(".digi.", ".par."));
    const TString outFile = (TString(clusterFile).ReplaceAll(".digi.", ".reco."));

    FairRunAna* run = new FairRunAna();
    run->SetInputFile(clusterFile);
    run->SetOutputFile(outFile);
    ConnectParFileToRuntimeDb(parFile, calibrFile, run->GetRuntimeDb());

    run->AddTask(new R3BNeulandNeutronReconstruction(
        new Neuland::Reconstruction2DCalibr(580., 630., 0.), "NeulandClusters", "NeulandNeutrons2DCalibr"));
    run->AddTask(new R3BNeulandNeutronReconstruction(
        new Neuland::ReconstructionEGate(580., 630., 0.), "NeulandClusters", "NeulandNeutronsEGate"));
    run->AddTask(new R3BNeulandNeutronReconstruction(
        new Neuland::ReconstructionNeuralNetworkMultiplicity(580., 630., 0., "tcp://gaia:55555"),
        "NeulandClusters",
        "NeulandNeutronsNeuralNetworkMultiplicity"));

    run->Init();
    run->Run(0, 0);
}
