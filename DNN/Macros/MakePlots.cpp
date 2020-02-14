void MakePlots()
{
    // Connect to InputsFile:
    R3BInputClass* Inputs = new R3BInputClass();
    Inputs->DisableErrorPrinting();
    Inputs->LinkFile("../InputFiles/InputsFile.txt");
    Inputs->ReadFile();
    
    // Next, Generate the Plots-Class:
    R3B_DNN_Plots* ThePlots = new R3B_DNN_Plots();
    ThePlots->DisableErrorPrinting();
    ThePlots->LinkInputClass(Inputs);
    ThePlots->CheckFiles();
    ThePlots->SetScaleFactor(4.0);
    ThePlots->SetOutputPath(Inputs->GetInputString("TheOutputPath"));
    
    // Retrieve required inputs:
    Bool_t SimulationData_IsAvailable = Inputs->GetInputBoolian("NeuLAND_TranslateToSignals_Mark_TruePrimarySignals");
    
    // Then, create the plots that we want:
    ThePlots->Exclude_MC_Spectrum();
    ThePlots->Include_PerfectSignal_Spectrum();
    ThePlots->Include_PerfectCluster_Spectrum(); 
    ThePlots->Include_TradMed_CutsMult_Spectrum(); 
    ThePlots->Include_TradMed_DNNMult_Spectrum();
    ThePlots->Include_TradMed_PerfectMult_Spectrum();
    ThePlots->Exclude_SignalScoringPlus_CutsMult_Spectrum();
    ThePlots->Exclude_SignalScoringPlus_DNNMult_Spectrum();
    ThePlots->Exclude_SignalScoringPlus_PerfectMult_Spectrum();
    ThePlots->Include_ScoringPlus_CutsMult_Spectrum();
    ThePlots->Include_ScoringPlus_DNNMult_Spectrum();
    ThePlots->Include_ScoringPlus_PerfectMult_Spectrum();
    ThePlots->Include_DNNScoringPlus();
    ThePlots->Include_DNNScoringPlus_Max();
    
    if (SimulationData_IsAvailable==kTRUE)
    {
        ThePlots->ErelSpect(1,"Gun");
        ThePlots->ErelSpect(2,"Gun");
        ThePlots->ErelSpect(3,"Gun");
        ThePlots->ErelSpect(4,"Gun");
        ThePlots->ErelSpect(5,"Gun");
    }
    else
    {
        ThePlots->ErelSpect(1,"Exp");
        ThePlots->ErelSpect(2,"Exp");
        ThePlots->ErelSpect(3,"Exp");
        ThePlots->ErelSpect(4,"Exp");
        ThePlots->ErelSpect(5,"Exp");
    }

    // Done.
    Inputs->PrintAllErrors();
    ThePlots->PrintAllErrors();
}
