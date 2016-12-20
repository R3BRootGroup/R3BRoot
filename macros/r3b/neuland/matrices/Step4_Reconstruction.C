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
    TStopwatch timer;
    timer.Start();

    const TString parFile = (TString(clusterFile).ReplaceAll(".digi.", ".par."));
    const TString outFile = (TString(clusterFile).ReplaceAll(".digi.", ".reco."));

    FairRunAna* run = new FairRunAna();
    // FairLogger::GetLogger()->SetLogScreenLevel("DEBUG");
    run->SetInputFile(clusterFile);
    run->SetOutputFile(outFile);
    ConnectParFileToRuntimeDb(parFile, calibrFile, run->GetRuntimeDb());

    run->AddTask(new R3BNeulandNeutronReconstruction());

    run->Init();
    
    run->Run(0, 0);

    timer.Stop();

    cout << "Macro finished succesfully!" << endl;
    cout << "Output file writen: " << outFile << endl;
    cout << "Parameter file writen: " << parFile << endl;
    cout << "Real time: " << timer.RealTime() << "s, CPU time: " << timer.CpuTime() << "s" << endl;
}
