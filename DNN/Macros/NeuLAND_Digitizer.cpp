inline void ConnectParFileToRuntimeDb(const TString parFile, FairRuntimeDb* rtdb)
{
    auto io = new FairParRootFileIo();
    io->open(parFile);
    rtdb->setFirstInput(io);
    rtdb->setOutput(io);
    rtdb->saveOutput();
}

void NeuLAND_Digitizer(Int_t const TotalNumberOfThreads = 1, Int_t const CurrentThread = 0)
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
    Int_t nDigiRuns = Inputs->GetInputInteger("NeuLAND_Digitizer_nDifferentRuns");
    Bool_t NeuLAND_IsOriginal = Inputs->GetInputBoolian("NeuLAND_Use_Original");
    Bool_t UseVETO = Inputs->GetInputBoolian("VETO_Include_in_SETUP");
    Bool_t UseNEBULA = Inputs->GetInputBoolian("NEBULA_Include_in_SETUP");
    Bool_t UseNEBVETO = Inputs->GetInputBoolian("NEBULA_VET_Include_in_SETUP");
    
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
        run->SetInputFile(MCFile);
        run->SetOutputFile(DigiFile);
        ConnectParFileToRuntimeDb(ParFile,run->GetRuntimeDb());
  
        // Create Jan Mayers digitizer. One for each run:
        TString st = "";
        TString kstr = "";
        TString OutputNameTag = "";
        R3BNeulandCADDigitizer** NeuLAND_Digitizer = new R3BNeulandCADDigitizer*[nDigiRuns];
        R3BNeulandCADDigitizer** VETO_Digitizer = new R3BNeulandCADDigitizer*[nDigiRuns];
        R3BNeulandCADDigitizer** NEBULA_Digitizer = new R3BNeulandCADDigitizer*[nDigiRuns];
        R3BNeulandCADDigitizer** NEBVETO_Digitizer = new R3BNeulandCADDigitizer*[nDigiRuns];
        
        for (Int_t k = 0; k<nDigiRuns; ++k)
        {
            // Create each digitizer:
            NeuLAND_Digitizer[k] = new R3BNeulandCADDigitizer();
            
            // Supply each digitizer with an individual name tag:
            kstr = st.Itoa(k,10);
            OutputNameTag = "_DigiRun_" + kstr;
            if (k==0) {OutputNameTag = "";} // So there is always one without tags that can be found by other tasks.
            NeuLAND_Digitizer[k]->SetTag(OutputNameTag);
            
            // Supply each digitizer with inputs:
            if (NeuLAND_IsOriginal==kTRUE) {NeuLAND_Digitizer[k]->SetDetector("NeuLAND");}
            else {NeuLAND_Digitizer[k]->SetDetector("CAD_NeuLAND");}
            NeuLAND_Digitizer[k]->SetParameters(Inputs);
            NeuLAND_Digitizer[k]->SetNevents(nEvents);
            NeuLAND_Digitizer[k]->LinkInputClass(Inputs);
            
            // Add each digitizer to the Mother FairTask:
            run->AddTask(NeuLAND_Digitizer[k]);
            
            // Same for VETO digitizers:
            if (UseVETO==kTRUE)
            {
                VETO_Digitizer[k] = new R3BNeulandCADDigitizer();
                VETO_Digitizer[k]->SetDetector("CAD_VETO");
                VETO_Digitizer[k]->SetTag(OutputNameTag);
                VETO_Digitizer[k]->SetParameters(Inputs);
                VETO_Digitizer[k]->SetNevents(nEvents);
                VETO_Digitizer[k]->LinkInputClass(Inputs);
                run->AddTask(VETO_Digitizer[k]);
            }
            
            // For NEBULA digitizers too:
            if (UseNEBULA==kTRUE)
            {
                NEBULA_Digitizer[k] = new R3BNeulandCADDigitizer();
                NEBULA_Digitizer[k]->SetDetector("CAD_NEBULA");
                NEBULA_Digitizer[k]->SetTag(OutputNameTag);
                NEBULA_Digitizer[k]->SetParameters(Inputs);
                NEBULA_Digitizer[k]->SetNevents(nEvents);
                NEBULA_Digitizer[k]->LinkInputClass(Inputs);
                run->AddTask(NEBULA_Digitizer[k]);
            }
            
            // And for NEBULA VETO digitizers:
            if (UseNEBVETO==kTRUE)
            {
                NEBVETO_Digitizer[k] = new R3BNeulandCADDigitizer();
                NEBVETO_Digitizer[k]->SetDetector("CAD_NEBULA_VETO");
                NEBVETO_Digitizer[k]->SetTag(OutputNameTag);
                NEBVETO_Digitizer[k]->SetParameters(Inputs);
                NEBVETO_Digitizer[k]->SetNevents(nEvents);
                NEBVETO_Digitizer[k]->LinkInputClass(Inputs);
                run->AddTask(NEBVETO_Digitizer[k]);
            }
            
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
        cout << "Output file written: " << DigiFile << endl;
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
