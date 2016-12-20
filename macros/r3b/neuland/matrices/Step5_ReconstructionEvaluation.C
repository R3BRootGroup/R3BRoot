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
    TStopwatch timer;
    timer.Start();

    const TString parFile = (TString(file).ReplaceAll(".reco.", ".par."));
    const TString mcpFile = (TString(file).ReplaceAll(".reco.", ".sim."));
    const TString outFile = (TString(file).ReplaceAll(".reco.", ".eval."));

    FairRunAna* run = new FairRunAna();
    // FairLogger::GetLogger()->SetLogScreenLevel("DEBUG");
    run->SetInputFile(file);
    run->AddFriend(mcpFile);
    run->SetOutputFile(outFile);
    ConnectParFileToRuntimeDb(parFile, run->GetRuntimeDb());

    run->AddTask(new R3BNeulandNeutronReconstructionEvaluation());

    run->Init();

    run->Run(0, 0);

    timer.Stop();

    cout << "Macro finished succesfully!" << endl;
    cout << "Output file writen: " << outFile << endl;
    cout << "Parameter file writen: " << parFile << endl;
    cout << "Real time: " << timer.RealTime() << "s, CPU time: " << timer.CpuTime() << "s" << endl;
}
