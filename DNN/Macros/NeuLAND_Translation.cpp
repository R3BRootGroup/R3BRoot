inline void ConnectParFileToRuntimeDb(const TString parFile, FairRuntimeDb* rtdb)
{
    auto io = new FairParRootFileIo();
    io->open(parFile);
    rtdb->setFirstInput(io);
    rtdb->setOutput(io);
    rtdb->saveOutput();
}

void NeuLAND_Translation(Int_t const TotalNumberOfThreads = 1, Int_t const CurrentThread = 0)
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
    
    // Retrieve other things that we need:
    Bool_t TagSims = Inputs->GetInputBoolian("NeuLAND_TranslateToSignals_Mark_TruePrimarySignals");
    TString LinkMethod = Inputs->GetInputString("NeuLAND_Translator_MarkPrimSignals");
    Int_t nDigiRuns = Inputs->GetInputInteger("NeuLAND_Digitizer_nDifferentRuns");
    if (nDigiRuns<1) {nDigiRuns = 1;}
    
    // Take care of multithreading:
    if (TotalNumberOfThreads>1)
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
    
    // Then, execute the macro if the Inputs Manager has no errors:
    if (Inputs->ContainsNoErrors()==kTRUE)
    {
        // Activate the timer:
        TStopwatch timer;
        timer.Start();

        // Create the Master FairTask:
        FairRunAna* run = new FairRunAna();
        run->SetInputFile(DigiFile);
        run->SetOutputFile(SignalFile);
        if (TagSims==kTRUE) {run->AddFriend(MCFile);}
        if ((TagSims==kTRUE)&&(LinkMethod=="JanMayer")) {run->AddFriend(JanFile);}
        ConnectParFileToRuntimeDb(ParFile,run->GetRuntimeDb());
        
        // Create a R3BNeuLANDTranslator for each run:
        TString st = "";
        TString kstr = "";
        TString OutputNameTag = "";
        R3BNeuLANDTranslator** Translator = new R3BNeuLANDTranslator*[nDigiRuns];
  
        for (Int_t k = 0; k<nDigiRuns; ++k)
        {
            // Create each Translator:
            Translator[k] = new R3BNeuLANDTranslator();
            
            // Supply each Translator with an individual name tag:
            kstr = st.Itoa(k,10);
            OutputNameTag = "_DigiRun_" + kstr;
            if (k==0) {OutputNameTag = "";} // So there is always one without tags that can be found by other tasks.
            Translator[k]->SetTag(OutputNameTag);
            
            // Supply each Translator with inputs:
            Translator[k]->LinkInputClass(Inputs);
            Translator[k]->SetNevents(nEvents);
            Translator[k]->SetMT(TotalNumberOfThreads,CurrentThread);
            
            // Add the translators to the Mother FairTask:
            run->AddTask(Translator[k]);
            
            // Done.
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
