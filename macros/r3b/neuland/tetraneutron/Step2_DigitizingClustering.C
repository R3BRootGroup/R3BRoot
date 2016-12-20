inline void ConnectParFileToRuntimeDb(const TString parFile, FairRuntimeDb* rtdb)
{
    FairParRootFileIo* io = new FairParRootFileIo();
    io->open(parFile);
    rtdb->setFirstInput(io);
    rtdb->setOutput(io);
    rtdb->saveOutput();
}

void Step2_DigitizingClustering(const TString simFile, const TString outFile)
{
    const TString parFile = TString(simFile).ReplaceAll(".sim.", ".par.");

    FairRunAna* run = new FairRunAna();
    run->SetInputFile(simFile);
    run->SetOutputFile(outFile);
    ConnectParFileToRuntimeDb(parFile, run->GetRuntimeDb());

    // Primary Data Flow Tasks
    run->AddTask(new R3BNeulandDigitizer());
    run->AddTask(new R3BNeulandClusterFinder());

    // Optional Monitoring Tasks
    run->AddTask(new R3BNeulandMCMon());
    run->AddTask(new R3BNeulandDigiMon());
    run->AddTask(new R3BNeulandClusterMon());

    run->Init();
    run->Run(0, 0);
}
