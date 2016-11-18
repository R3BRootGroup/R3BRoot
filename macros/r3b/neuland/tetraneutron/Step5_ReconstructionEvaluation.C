inline void ConnectParFileToRuntimeDb(const TString parFile, FairRuntimeDb* rtdb)
{
    FairParRootFileIo* io = new FairParRootFileIo();
    io->open(parFile);
    rtdb->setFirstInput(io);
    rtdb->setOutput(io);
    rtdb->saveOutput();
}

void Step5_ReconstructionEvaluation(const TString file)
{
    const TString parFile = (TString(file).ReplaceAll(".reco.", ".par."));
    const TString mcpFile = (TString(file).ReplaceAll(".reco.", ".sim."));
    const TString outFile = (TString(file).ReplaceAll(".reco.", ".eval."));

    FairRunAna* run = new FairRunAna();
    run->SetInputFile(file);
    run->AddFriend(mcpFile);
    run->SetOutputFile(outFile);
    ConnectParFileToRuntimeDb(parFile, run->GetRuntimeDb());

    run->AddTask(new R3BNeulandNeutronReconstructionMon("NeulandNeutrons2DCalibr", "NeulandRecoMon2DCalibr"));
    run->AddTask(new R3BNeulandNeutronReconstructionMon("NeulandNeutronsEGate", "NeulandRecoMonEGate"));
    run->AddTask(new R3BNeulandNeutronReconstructionMon("NeulandNeutronsNeuralNetworkMultiplicity",
                                                        "NeulandRecoMonNeuralNetworkMultiplicity"));

    run->Init();
    run->Run(0, 0);
}
