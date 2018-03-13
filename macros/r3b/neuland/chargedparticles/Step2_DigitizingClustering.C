inline void ConnectParFileToRuntimeDb(const TString parFile, FairRuntimeDb* rtdb)
{
    FairParRootFileIo* io = new FairParRootFileIo();
    io->open(parFile);
    rtdb->setFirstInput(io);
    rtdb->setOutput(io);
    rtdb->saveOutput();
}

void Step2_DigitizingClustering(const TString simFile)
{
    const TString parFile = (TString(simFile).ReplaceAll(".sim.", ".par."));
    const TString outFile = (TString(simFile).ReplaceAll(".sim.", ".digi."));

    FairRunAna* run = new FairRunAna();
    // FairLogger::GetLogger()->SetLogScreenLevel("DEBUG");
    run->SetSource(new FairFileSource(simFile);
    run->SetOutputFile(outFile);
    ConnectParFileToRuntimeDb(parFile, run->GetRuntimeDb());

    run->AddTask(new R3BNeulandDigitizer());
    run->AddTask(new R3BNeulandClusterFinder());

    // Optional
    run->AddTask(new R3BNeulandMCMon("FULLSIMANA"));
    run->AddTask(new R3BNeulandDigiMon());
    run->AddTask(new R3BNeulandClusterMon());

    run->Init();
    run->Run(0, 0);
    delete run;
}
