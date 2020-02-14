inline void ConnectParFileToRuntimeDb(const TString parFile, FairRuntimeDb* rtdb)
{
    auto io = new FairParRootFileIo();
    io->open(parFile);
    rtdb->setFirstInput(io);
    rtdb->setOutput(io);
    rtdb->saveOutput();
}

void NeuLAND_ScoreCollection(Int_t const TotalNumberOfThreads = 1, Int_t const CurrentThread = -1)
{
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
    
    // Retrieve extra inputs:
    Bool_t UseNEBULA = Inputs->GetInputBoolian("NEBULA_Include_in_SETUP");
    
    // Take care of multithreading:
    if (TotalNumberOfThreads>1)
    {
        if (CurrentThread>=0)
        {
            TString nst = "";
            TString nT_tot_str = nst.Itoa(TotalNumberOfThreads,10);
            TString nT_cur_str = nst.Itoa(CurrentThread,10);
            nEvents = nEvents/TotalNumberOfThreads;
        
            // Multithreading only goes as far as the clusterfile:
            MCFile = MCFile.ReplaceAll(".root","_nT"+nT_tot_str+"_cT"+nT_cur_str+".root");
            ParFile = ParFile.ReplaceAll(".root","_nT"+nT_tot_str+"_cT"+nT_cur_str+".root");
            VisFile = VisFile.ReplaceAll(".root","_nT"+nT_tot_str+"_cT"+nT_cur_str+".root");
            DigiFile = DigiFile.ReplaceAll(".root","_nT"+nT_tot_str+"_cT"+nT_cur_str+".root");
            JanFile = JanFile.ReplaceAll(".root","_nT"+nT_tot_str+"_cT"+nT_cur_str+".root");
            SignalFile = SignalFile.ReplaceAll(".root","_nT"+nT_tot_str+"_cT"+nT_cur_str+".root");
            ClusterFile = ClusterFile.ReplaceAll(".root","_nT"+nT_tot_str+"_cT"+nT_cur_str+".root");
            FilteredFile = FilteredFile.ReplaceAll(".root","_nT"+nT_tot_str+"_cT"+nT_cur_str+".root");
        }
        else
        {
            nEvents = nEvents/TotalNumberOfThreads;
            nEvents = nEvents*TotalNumberOfThreads;
        }
    }
    
    // Then, execute the macro if the Inputs Manager has no errors:
    if (Inputs->ContainsNoErrors()==kTRUE)
    {
        // Activate the timer:
        TStopwatch timer;
        timer.Start();

        // Create the Master FairTask:
        FairRunAna* run = new FairRunAna();
        run->SetInputFile(SignalFile);
        run->AddFriend(ClusterFile);
        run->SetOutputFile(TheOutputPath + "/TDR_Calibration_Histograms.root");
        ConnectParFileToRuntimeDb(ParFile,run->GetRuntimeDb());
  
        // Create the R3BScoreCollector:
        R3BScoreCollector* ScoreCol = new R3BScoreCollector();
        
        // Pass parameters:
        ScoreCol->LinkInputClass(Inputs);
        ScoreCol->SetNevents(nEvents);
        
        // Add it to the Mother of FairTasks:
        run->AddTask(ScoreCol);
        
        // same for NEBULA, if it is there:
        if (UseNEBULA==kTRUE)
        {
            R3BNEBULAScoreCollector* NEBScoreCol = new R3BNEBULAScoreCollector();
            NEBScoreCol->LinkInputClass(Inputs);
            NEBScoreCol->SetNevents(nEvents);
            run->AddTask(NEBScoreCol);
        }

        // Run the FairTasks:
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
}
