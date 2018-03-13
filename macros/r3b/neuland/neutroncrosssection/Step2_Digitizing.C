inline void ConnectParFileToRuntimeDb(const TString parFile, FairRuntimeDb* rtdb)
{
    auto io = new FairParRootFileIo();
    io->open(parFile);
    rtdb->setFirstInput(io);
    rtdb->setOutput(io);
    rtdb->saveOutput();
}

void Step2_Digitizing(const TString simFile)
{
    const TString parFile = (TString(simFile).ReplaceAll(".sim.", ".par."));
    const TString outFile = (TString(simFile).ReplaceAll(".sim.", ".mon."));

    FairRunAna run;
    run.SetSource(new FairFileSource(simFile));
    run.SetOutputFile(outFile);
    ConnectParFileToRuntimeDb(parFile, run.GetRuntimeDb());

    run.AddTask(new R3BNeulandDigitizer());

    run.AddTask(new R3BNeulandMCMon());
    run.AddTask(new R3BNeulandDigiMon());

    run.Init();
    run.Run(0, 0);
}
