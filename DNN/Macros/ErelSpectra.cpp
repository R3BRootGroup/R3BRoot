inline void ConnectParFileToRuntimeDb(const TString parFile, FairRuntimeDb* rtdb)
{
    auto io = new FairParRootFileIo();
    io->open(parFile);
    rtdb->setFirstInput(io);
    rtdb->setOutput(io);
    rtdb->saveOutput();
}

void ErelSpectra(Int_t const TotalNumberOfThreads = 1)
{
    // This computes all the Relative Energy Spectra.
    
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
    
    // Retrieve other inputs:
    Bool_t SimulationData_IsAvailable = Inputs->GetInputBoolian("NeuLAND_TranslateToSignals_Mark_TruePrimarySignals");
    Bool_t UseNEBULA = Inputs->GetInputBoolian("NEBULA_Include_in_SETUP");
    
    // Corrent the number of events for MT effects:
    if (TotalNumberOfThreads>1)
    {
        nEvents = nEvents/TotalNumberOfThreads;
        nEvents = nEvents*TotalNumberOfThreads;
    }
    
    // Then, execute the macro if there are no errors:
    if (Inputs->ContainsNoErrors()==kTRUE)
    {
        // Activate the timer:
        TStopwatch timer;
        timer.Start();

        // Create the Master FairTask:
        FairRunAna* run = new FairRunAna();
        run->SetInputFile(SignalFile);
        run->AddFriend(FinalFile);
        ConnectParFileToRuntimeDb(ParFile,run->GetRuntimeDb());
        
        // Re-initialize .Txt-file:
        TString TextFileName = TheOutputPath + "/ErelSpectrum_Integrals.txt";
        std::ofstream Creation (TextFileName.Data(), std::ofstream::out);
        Creation.close();
  
        // Create different classes for each type of reconstruction method:
        R3BErelSpectrum* ESpect_01 = new R3BErelSpectrum(); ESpect_01->LinkInputClass(Inputs); ESpect_01->SetNevents(nEvents); ESpect_01->SetBranchName("NeutronTracks_PerfectMethod_Signals");
        R3BErelSpectrum* ESpect_02 = new R3BErelSpectrum(); ESpect_02->LinkInputClass(Inputs); ESpect_02->SetNevents(nEvents); ESpect_02->SetBranchName("NeutronTracks_PerfectMethod_Clusters");
        R3BErelSpectrum* ESpect_03 = new R3BErelSpectrum(); ESpect_03->LinkInputClass(Inputs); ESpect_03->SetNevents(nEvents); ESpect_03->SetBranchName("MCNeutronTracks");
        
        // Exp. allowed reconstructions:
        R3BErelSpectrum* ESpect_04 = new R3BErelSpectrum(); ESpect_04->LinkInputClass(Inputs); ESpect_04->SetNevents(nEvents); ESpect_04->SetBranchName("TheNeutronTracks");
        R3BErelSpectrum* ESpect_05 = new R3BErelSpectrum(); ESpect_05->LinkInputClass(Inputs); ESpect_05->SetNevents(nEvents); ESpect_05->SetBranchName("NeutronTracks_TradMed_Clusters_CutsMult");
        R3BErelSpectrum* ESpect_06 = new R3BErelSpectrum(); ESpect_06->LinkInputClass(Inputs); ESpect_06->SetNevents(nEvents); ESpect_06->SetBranchName("NeutronTracks_TradMed_Clusters_DNNMult");
        R3BErelSpectrum* ESpect_07 = new R3BErelSpectrum(); ESpect_07->LinkInputClass(Inputs); ESpect_07->SetNevents(nEvents); ESpect_07->SetBranchName("NeutronTracks_TradMed_Clusters_PerfectMult");
        R3BErelSpectrum* ESpect_08 = new R3BErelSpectrum(); ESpect_08->LinkInputClass(Inputs); ESpect_08->SetNevents(nEvents); ESpect_08->SetBranchName("NeutronTracks_ScoringPlus_Signals_CutsMult");
        R3BErelSpectrum* ESpect_09 = new R3BErelSpectrum(); ESpect_09->LinkInputClass(Inputs); ESpect_09->SetNevents(nEvents); ESpect_09->SetBranchName("NeutronTracks_ScoringPlus_Signals_DNNMult");
        R3BErelSpectrum* ESpect_10 = new R3BErelSpectrum(); ESpect_10->LinkInputClass(Inputs); ESpect_10->SetNevents(nEvents); ESpect_10->SetBranchName("NeutronTracks_ScoringPlus_Signals_PerfectMult");
        R3BErelSpectrum* ESpect_11 = new R3BErelSpectrum(); ESpect_11->LinkInputClass(Inputs); ESpect_11->SetNevents(nEvents); ESpect_11->SetBranchName("NeutronTracks_ScoringPlus_Clusters_CutsMult");
        R3BErelSpectrum* ESpect_12 = new R3BErelSpectrum(); ESpect_12->LinkInputClass(Inputs); ESpect_12->SetNevents(nEvents); ESpect_12->SetBranchName("NeutronTracks_ScoringPlus_Clusters_DNNMult");
        R3BErelSpectrum* ESpect_13 = new R3BErelSpectrum(); ESpect_13->LinkInputClass(Inputs); ESpect_13->SetNevents(nEvents); ESpect_13->SetBranchName("NeutronTracks_ScoringPlus_Clusters_PerfectMult");
        R3BErelSpectrum* ESpect_14 = new R3BErelSpectrum(); ESpect_14->LinkInputClass(Inputs); ESpect_14->SetNevents(nEvents); ESpect_14->SetBranchName("NeutronTracks_DNNScoringPlus");
        R3BErelSpectrum* ESpect_15 = new R3BErelSpectrum(); ESpect_15->LinkInputClass(Inputs); ESpect_15->SetNevents(nEvents); ESpect_15->SetBranchName("NeutronTracks_DNNScoringPlus_SingleTOF");
        R3BErelSpectrum* ESpect_16 = new R3BErelSpectrum(); ESpect_16->LinkInputClass(Inputs); ESpect_16->SetNevents(nEvents); ESpect_16->SetBranchName("NeutronTracks_DNNScoringPlus_Max");
        R3BErelSpectrum* ESpect_17 = new R3BErelSpectrum(); ESpect_17->LinkInputClass(Inputs); ESpect_17->SetNevents(nEvents); ESpect_17->SetBranchName("NeutronTracks_BetaReconstruction_NeuLAND");
        
        // Add task to the mother FairTask:
        if (SimulationData_IsAvailable==kTRUE)
        {
            run->AddTask(ESpect_01);
            run->AddTask(ESpect_02);
            run->AddTask(ESpect_03);
        }
        
        run->AddTask(ESpect_04);
        run->AddTask(ESpect_05);
        run->AddTask(ESpect_06);
        run->AddTask(ESpect_07);
        run->AddTask(ESpect_08);
        run->AddTask(ESpect_09);
        run->AddTask(ESpect_10);
        run->AddTask(ESpect_11);
        run->AddTask(ESpect_12);
        run->AddTask(ESpect_13);
        run->AddTask(ESpect_14);
        run->AddTask(ESpect_15);
        run->AddTask(ESpect_16);
        run->AddTask(ESpect_17);
        
        if (UseNEBULA==kTRUE)
        {
            R3BErelSpectrum* ESpect_N1 = new R3BErelSpectrum(); ESpect_N1->LinkInputClass(Inputs); ESpect_N1->SetNevents(nEvents); ESpect_N1->SetBranchName("NeutronTracks_BetaReconstruction_NEBULA");
            R3BErelSpectrum* ESpect_N2 = new R3BErelSpectrum(); ESpect_N2->LinkInputClass(Inputs); ESpect_N2->SetNevents(nEvents); ESpect_N2->SetBranchName("NeutronTracks_BetaReconstruction_Combined");
            run->AddTask(ESpect_N1);
            run->AddTask(ESpect_N2);
        }
       

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
    }

    // Done.
}
