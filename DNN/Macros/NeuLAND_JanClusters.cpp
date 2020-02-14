inline void ConnectParFileToRuntimeDb(const TString parFile, FairRuntimeDb* rtdb)
{
    auto io = new FairParRootFileIo();
    io->open(parFile);
    rtdb->setFirstInput(io);
    rtdb->setOutput(io);
    rtdb->saveOutput();
}

void NeuLAND_JanClusters(Int_t const TotalNumberOfThreads = 1, Int_t const CurrentThread = 0)
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
    
    // Retrieve other inputs:
    Int_t nDigiRuns = Inputs->GetInputInteger("NeuLAND_Digitizer_nDifferentRuns");
    if (nDigiRuns<1) {nDigiRuns = 1;}
    Double_t XRadius = Inputs->GetInputDouble("NeuLAND_ClusterRadius_X","cm");
    Double_t YRadius = Inputs->GetInputDouble("NeuLAND_ClusterRadius_Y","cm");
    Double_t ZRadius = Inputs->GetInputDouble("NeuLAND_ClusterRadius_Z","cm");
    Double_t TimeRadius = Inputs->GetInputDouble("NeuLAND_ClusterRadius_Time","ns");
    Double_t MinimizationMarking_MaxDistance = Inputs->GetInputDouble("NeuLAND_Translator_MinimizationMarking_MaxDistance","cm");
    Bool_t MinimizationMarking_AllowMaxDist = Inputs->GetInputBoolian("NeuLAND_Translator_MinimizationMarking_AllowMaxDist");
    if (MinimizationMarking_AllowMaxDist==kFALSE) {MinimizationMarking_MaxDistance = 1e99;}
    TString MarkSignals_Method = Inputs->GetInputString("NeuLAND_Translator_MarkPrimSignals");
    
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
    if ((Inputs->ContainsNoErrors()==kTRUE)&&(MarkSignals_Method=="JanMayer"))
    {
        // Activate the timer:
        TStopwatch timer;
        timer.Start();

        // Create the Master FairTask:
        FairRunAna* run = new FairRunAna();
        run->SetInputFile(MCFile);
        run->AddFriend(DigiFile);
        run->SetOutputFile(JanFile);
        ConnectParFileToRuntimeDb(ParFile,run->GetRuntimeDb());
  
        // This FairTask performs Jan Mayers analysis, exactly as he programmed it,
        // using his classes (and NOT the ones from the DNN module).
        
        // Create the tasks for each run:
        TString st = "";
        TString kstr = "";
        TString OutputNameTag = "";
        R3BHitConvertor** DigiConvertor = new R3BHitConvertor*[nDigiRuns];
        R3BNeulandCADClusterFinder** ClusterFinder = new R3BNeulandCADClusterFinder*[nDigiRuns];
        R3BNeulandCADNeutronClusters** ClusterMarker = new R3BNeulandCADNeutronClusters*[nDigiRuns];
        R3BNeutronConvertor** NeuConvertor = new R3BNeutronConvertor*[nDigiRuns];
        R3BNeulandCADNeutronReconstructionStatistics** Stats = new R3BNeulandCADNeutronReconstructionStatistics*[nDigiRuns];
        std::ofstream* MultEvaluation = new std::ofstream[nDigiRuns];
        
        for (Int_t k = 0; k<nDigiRuns; ++k)
        {
            // Supply each task with an individual name tag:
            kstr = st.Itoa(k,10);
            OutputNameTag = "_DigiRun_" + kstr;
            if (k==0) {OutputNameTag = "";} // So there is always one without tags that can be found by other tasks.
            
            // Convert digis to hits (which is what Jan Mayers software needs):
            DigiConvertor[k] = new R3BHitConvertor();
            DigiConvertor[k]->SetNevents(nEvents);
            DigiConvertor[k]->LinkInputClass(Inputs);
            DigiConvertor[k]->SetTag(OutputNameTag);
        
            // Generate the NeuLAND clusters from NeuLAND hits:
            ClusterFinder[k] = new R3BNeulandCADClusterFinder(XRadius,YRadius,ZRadius,TimeRadius,"NeulandHits","NeulandClusters");
            ClusterFinder[k]->SetTag(OutputNameTag);
        
            // And then, label these clusters as primary or secondary:
            ClusterMarker[k] = new R3BNeulandCADNeutronClusters("NeulandClusters","NeulandPrimaryClusters","NeulandSecondaryClusters",MinimizationMarking_MaxDistance);
            ClusterMarker[k]->SetTag(OutputNameTag);
            
            // Convert the marked clusters to neuland neutrons:
            NeuConvertor[k] = new R3BNeutronConvertor();
            NeuConvertor[k]->SetNevents(nEvents);
            NeuConvertor[k]->LinkInputClass(Inputs);
            NeuConvertor[k]->SetTag(OutputNameTag);
            
            // Next, generate .txt-files as output:
            MultEvaluation[k] = std::ofstream((TheOutputPath+"/Jan_Matrix_Output_"+OutputNameTag+".txt").Data(), std::ofstream::out);
            Stats[k] = new R3BNeulandCADNeutronReconstructionStatistics("NeulandPrimaryClustersMultiple","NeulandSecondaryClusters","NeulandNeutronsCheat",MultEvaluation[k]);
            
            // Add all elements to the Mother FairTask:
            run->AddTask(DigiConvertor[k]);
            run->AddTask(ClusterFinder[k]);
            run->AddTask(ClusterMarker[k]);
            run->AddTask(NeuConvertor[k]);
            run->AddTask(Stats[k]);
        }
  
        // Run the FairTasks:
        run->Init();
        run->Run(0,nEvents);
        
        for (Int_t k = 0; k<nDigiRuns; ++k) {MultEvaluation[k].close();}

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
