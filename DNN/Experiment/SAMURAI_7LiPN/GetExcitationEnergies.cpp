inline void ConnectParFileToRuntimeDb(const TString parFile, FairRuntimeDb* rtdb)
{
    auto io = new FairParRootFileIo();
    io->open(parFile);
    rtdb->setFirstInput(io);
    rtdb->setOutput(io);
    rtdb->saveOutput();
}

void GetExcitationEnergies()
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
    TString ExFile = TheOutputPath + "/ExcitationEnergies.root";
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
        run->SetOutputFile(ExFile);
        ConnectParFileToRuntimeDb(ParFile,run->GetRuntimeDb());
  
        // Create the tasks:
        Excitation_Energy_7LiPN* GetExDNN = new Excitation_Energy_7LiPN();
        GetExDNN->LinkInputClass(Inputs);
        GetExDNN->SetNevents(nEvents);
        GetExDNN->SetBranchTitle("NeutronTracks_DNNScoringPlus");
        
        Excitation_Energy_7LiPN* GetExDNN_STOF = new Excitation_Energy_7LiPN();
        GetExDNN_STOF->LinkInputClass(Inputs);
        GetExDNN_STOF->SetNevents(nEvents);
        GetExDNN_STOF->SetBranchTitle("NeutronTracks_DNNScoringPlus_SingleTOF");
        
        Excitation_Energy_7LiPN* GetExScoringPlus = new Excitation_Energy_7LiPN();
        GetExScoringPlus->LinkInputClass(Inputs);
        GetExScoringPlus->SetNevents(nEvents);
        GetExScoringPlus->SetBranchTitle("NeutronTracks_ScoringPlus_Clusters_DNNMult");
        
        Excitation_Energy_7LiPN* GetExTDR = new Excitation_Energy_7LiPN();
        GetExTDR->LinkInputClass(Inputs);
        GetExTDR->SetNevents(nEvents);
        GetExTDR->SetBranchTitle("NeutronTracks_TradMed_Clusters_CutsMult");
        
        Excitation_Energy_7LiPN* GetExTDR_DNNMult = new Excitation_Energy_7LiPN();
        GetExTDR_DNNMult->LinkInputClass(Inputs);
        GetExTDR_DNNMult->SetNevents(nEvents);
        GetExTDR_DNNMult->SetBranchTitle("NeutronTracks_TradMed_Clusters_DNNMult");
        
        Excitation_Energy_7LiPN* GetExBeta = new Excitation_Energy_7LiPN();
        GetExBeta->LinkInputClass(Inputs);
        GetExBeta->SetNevents(nEvents);
        GetExBeta->SetBranchTitle("NeutronTracks_BetaReconstruction_NeuLAND");
        
        Excitation_Energy_7LiPN* GetExSingle = new Excitation_Energy_7LiPN();
        GetExSingle->LinkInputClass(Inputs);
        GetExSingle->SetNevents(nEvents);
        GetExSingle->SetBranchTitle("NeutronTracks_SingleReconstruction_NeuLAND");
        
        Excitation_Energy_7LiPN* GetExPerfect = new Excitation_Energy_7LiPN();
        GetExPerfect->LinkInputClass(Inputs);
        GetExPerfect->SetNevents(nEvents);
        GetExPerfect->SetBranchTitle("NeutronTracks_PerfectMethod_Signals");
        
        if (LimitMultToMC==kTRUE)
        {
            GetExDNN->LimitToMCMultiplicity();
            GetExDNN_STOF->LimitToMCMultiplicity();
            GetExScoringPlus->LimitToMCMultiplicity();
            GetExTDR->LimitToMCMultiplicity();
            GetExTDR_DNNMult->LimitToMCMultiplicity();
            GetExBeta->LimitToMCMultiplicity();
            GetExSingle->LimitToMCMultiplicity();
            GetExPerfect->LimitToMCMultiplicity();
        }
        else
        {
            GetExDNN->ReleaseMCMultiplicity();
            GetExDNN_STOF->ReleaseMCMultiplicity();
            GetExScoringPlus->ReleaseMCMultiplicity();
            GetExTDR->ReleaseMCMultiplicity();
            GetExTDR_DNNMult->ReleaseMCMultiplicity();
            GetExBeta->ReleaseMCMultiplicity();
            GetExSingle->ReleaseMCMultiplicity();
            GetExPerfect->ReleaseMCMultiplicity();
        }
        
        // Add them to the Mother FairTask:
        run->AddTask(GetExDNN);
        run->AddTask(GetExDNN_STOF);
        run->AddTask(GetExScoringPlus);
        run->AddTask(GetExTDR);
        run->AddTask(GetExTDR_DNNMult);
        run->AddTask(GetExBeta);
        run->AddTask(GetExSingle);
        run->AddTask(GetExPerfect);

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
