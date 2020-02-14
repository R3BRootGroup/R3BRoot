inline void ConnectParFileToRuntimeDb(const TString parFile, FairRuntimeDb* rtdb)
{
    auto io = new FairParRootFileIo();
    io->open(parFile);
    rtdb->setFirstInput(io);
    rtdb->setOutput(io);
    rtdb->saveOutput();
}

void Get4nInvMass()
{
    // Converts the reconstruction results back into the original files.
    Bool_t LimitMultToMC = kTRUE;
    
    // Connect to InputsFile:
    R3BInputClass* Inputs = new R3BInputClass();
    Inputs->DisableErrorPrinting();
    Inputs->LinkFile("../../InputFiles/InputsFile.txt");
    Inputs->ReadFile();
        
    // Retrieve required inputs:
    TString TheOutputPath = Inputs->GetInputString("TheOutputPath");
    TString SignalFile = TheOutputPath + Inputs->GetInputString("NeuLAND_SignalFile");
    TString ClusterFile = TheOutputPath + Inputs->GetInputString("NeuLAND_ClusterFile");
    TString FinalFile = TheOutputPath + Inputs->GetInputString("NeuLAND_Reconstruction_FinalFile");
    TString ParFile = TheOutputPath + Inputs->GetInputString("R3BRoot_MonteCarlo_ParFile");
    TString MassFile = TheOutputPath + "/TetraNeutron_InvMass.root";
    Int_t nEvents = Inputs->GetInputInteger("R3BRoot_nEvents");
    
    // Then, execute the first macro for retrieving the signals:
    if (Inputs->ContainsNoErrors()==kTRUE)
    {
        // Activate the timer:
        TStopwatch timer;
        timer.Start();

        // Create the Master FairTask:
        FairRunAna* run = new FairRunAna();
        run->SetInputFile(FinalFile);
        run->AddFriend(SignalFile);
        run->AddFriend(ClusterFile);
        run->SetOutputFile(MassFile);
        ConnectParFileToRuntimeDb(ParFile,run->GetRuntimeDb());
  
        // Create the tasks:
        InvMass_4n* Mass4n_DNN = new InvMass_4n();
        Mass4n_DNN->LinkInputClass(Inputs);
        Mass4n_DNN->SetNevents(nEvents);
        Mass4n_DNN->SetBranchTitle("NeutronTracks_DNNScoringPlus");
        
        InvMass_4n* Mass4n_ScoringPlus = new InvMass_4n();
        Mass4n_ScoringPlus->LinkInputClass(Inputs);
        Mass4n_ScoringPlus->SetNevents(nEvents);
        Mass4n_ScoringPlus->SetBranchTitle("NeutronTracks_ScoringPlus_Clusters_DNNMult");
        
        InvMass_4n* Mass4n_TDR = new InvMass_4n();
        Mass4n_TDR->LinkInputClass(Inputs);
        Mass4n_TDR->SetNevents(nEvents);
        Mass4n_TDR->SetBranchTitle("NeutronTracks_TradMed_Clusters_CutsMult");
        
        InvMass_4n* Mass4n_TDR_DNNMult = new InvMass_4n();
        Mass4n_TDR_DNNMult->LinkInputClass(Inputs);
        Mass4n_TDR_DNNMult->SetNevents(nEvents);
        Mass4n_TDR_DNNMult->SetBranchTitle("NeutronTracks_TradMed_Clusters_DNNMult");
        
        InvMass_4n* Mass4n_Perfect = new InvMass_4n();
        Mass4n_Perfect->LinkInputClass(Inputs);
        Mass4n_Perfect->SetNevents(nEvents);
        Mass4n_Perfect->SetBranchTitle("NeutronTracks_PerfectMethod_Signals");
        
        if (LimitMultToMC==kTRUE)
        {
            Mass4n_DNN->LimitToMCMultiplicity();
            Mass4n_ScoringPlus->LimitToMCMultiplicity();
            Mass4n_TDR->LimitToMCMultiplicity();
            Mass4n_TDR_DNNMult->LimitToMCMultiplicity();
            Mass4n_Perfect->LimitToMCMultiplicity();
        }
        else
        {
            Mass4n_DNN->ReleaseMCMultiplicity();
            Mass4n_ScoringPlus->ReleaseMCMultiplicity();
            Mass4n_TDR->ReleaseMCMultiplicity();
            Mass4n_TDR_DNNMult->ReleaseMCMultiplicity();
            Mass4n_Perfect->ReleaseMCMultiplicity();
        }
        
        // Add them to the Mother FairTask:
        run->AddTask(Mass4n_DNN);
        run->AddTask(Mass4n_ScoringPlus);
        run->AddTask(Mass4n_TDR);
        run->AddTask(Mass4n_TDR_DNNMult);
        run->AddTask(Mass4n_Perfect);

        // Run the FairTask:
        run->Init();
        run->Run(0,nEvents);

        // Finish the timer:
        timer.Stop();

        // Give final output:
        cout << endl;
        cout << "Macro finished succesfully!" << endl;
        cout << "Output file written: " << SignalFile << endl;
        cout << "Parameter file written: " << ParFile << endl;
        cout << "Real time: " << timer.RealTime() << "s, CPU time: " << timer.CpuTime() << "s" << endl;
        cout << endl;
    }
    else
    {
        // Then, print what has gone wrong:
        Inputs->PrintAllErrors();
    }

    // Done.
}
