inline void ConnectParFileToRuntimeDb(const TString parFile, FairRuntimeDb* rtdb)
{
    auto io = new FairParRootFileIo();
    io->open(parFile);
    rtdb->setFirstInput(io);
    rtdb->setOutput(io);
    rtdb->saveOutput();
}

void PhysListPlots(Int_t const TotalNumberOfThreads = 1)
{
    // This computes all the plots explaining the physics list.
    
    // Connect to InputsFile:
    R3BInputClass* Inputs = new R3BInputClass();
    Inputs->DisableErrorPrinting();
    Inputs->LinkFile("../InputFiles/InputsFile.txt");
    Inputs->ReadFile();
    
    // Retrieve General Inputs:
    TString TheOutputPath = Inputs->GetInputString("TheOutputPath");
    TString MCFile = TheOutputPath + Inputs->GetInputString("R3BRoot_MonteCarlo_OutputFile");
    TString ParFile = TheOutputPath + Inputs->GetInputString("R3BRoot_MonteCarlo_ParFile");
    TString VisFile = TheOutputPath + Inputs->GetInputString("R3BRoot_MonteCarlo_PlotFile");
    TString DigiFile = TheOutputPath + Inputs->GetInputString("NeuLAND_Digitizer_OutputFile");
    TString JanFile = TheOutputPath + Inputs->GetInputString("NeuLAND_Digitizer_JanMayer_Processing");
    TString SignalFile = TheOutputPath + Inputs->GetInputString("NeuLAND_SignalFile");
    TString ClusterFile = TheOutputPath + Inputs->GetInputString("NeuLAND_ClusterFile");
    TString FilteredFile = TheOutputPath + Inputs->GetInputString("NeuLAND_FilteredClusterFile");
    TString TradMedFile = TheOutputPath + Inputs->GetInputString("NeuLAND_TraditionalMethod_ReconstructionFile");
    TString ScoreFile = TheOutputPath + Inputs->GetInputString("NeuLAND_ScoringOutputFile");
    TString DNNScoreFile = TheOutputPath + Inputs->GetInputString("NeuLAND_DNNScoringOutputFile");
    TString DistFile = TheOutputPath + Inputs->GetInputString("NeuLAND_Distributions");
    TString DNNFile = TheOutputPath + Inputs->GetInputString("NeuLAND_DNN_OutputFile");
    TString FinalFile = TheOutputPath + Inputs->GetInputString("NeuLAND_Reconstruction_FinalFile");
    TString BetaFile = TheOutputPath + Inputs->GetInputString("BetaReconstruction_OutputFile");
    TString SingleFile = TheOutputPath + Inputs->GetInputString("SingleReconstruction_OutputFile");
    Int_t nEvents = Inputs->GetInputInteger("R3BRoot_nEvents");
    
    // Corrent the number of events for MT effects:
    if (TotalNumberOfThreads>1)
    {
        nEvents = nEvents/TotalNumberOfThreads;
        nEvents = nEvents*TotalNumberOfThreads;
    }
    
    // Retrieve other inputs:
    Bool_t SimulationData_IsAvailable = Inputs->GetInputBoolian("NeuLAND_TranslateToSignals_Mark_TruePrimarySignals");
    Bool_t MergeMCFile = Inputs->GetInputBoolian("MT_Merge_MCFile");
    if (TotalNumberOfThreads==1) {MergeMCFile = kTRUE;}
    
    // Then, execute the macro if there are no errors:
    if ((Inputs->ContainsNoErrors()==kTRUE)&&(MergeMCFile==kTRUE)&&(SimulationData_IsAvailable==kTRUE))
    {
        // Activate the timer:
        TStopwatch timer;
        timer.Start();

        // Create the Master FairTask:
        FairRunAna* run = new FairRunAna();
        run->SetInputFile(MCFile);
        run->AddFriend(DigiFile);
        ConnectParFileToRuntimeDb(ParFile,run->GetRuntimeDb());
  
        // Create the Physics List class:
        R3BPhysList* plist = new R3BPhysList();
        
        // Pass parameters:
        plist->LinkInputClass(Inputs);
        plist->SetNevents(nEvents);
       
        // Add it to the Mother of FairTasks:
        run->AddTask(plist);

        // Run the FairTasks:
        run->Init();
        run->Run(0,nEvents);

        // Finish the timer:
        timer.Stop();

        // Give final output:
        cout << endl;
        cout << "Macro finished succesfully!" << endl;
        cout << "Output file written: " << DistFile << endl;
        cout << "Parameter file written: " << ParFile << endl;
        cout << "Real time: " << timer.RealTime() << "s, CPU time: " << timer.CpuTime() << "s" << endl;
        cout << endl;
    }
    else
    {
        // Then, print what has gone wrong:
        Inputs->PrintAllErrors();
        if (MergeMCFile!=kTRUE) {cout << "\n\nATTENTION: This macro can only be executed if the MC files have been merged!\n\n";}
        if (SimulationData_IsAvailable!=kTRUE) {cout << "NOTE: This macro can only be executed for simulated data!\n\n";}
    }

    // Done.
}
