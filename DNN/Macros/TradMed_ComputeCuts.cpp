void TradMed_ComputeCuts()
{
    // Connect to InputsFile:
    R3BInputClass* Inputs = new R3BInputClass();
    Inputs->DisableErrorPrinting();
    Inputs->LinkFile("../InputFiles/InputsFile.txt");
    Inputs->ReadFile();
    
    // Next, execute the traditional method:
    R3BTradMethClass* TradMed = new R3BTradMethClass();
    TradMed->DisableErrorPrinting();
    TradMed->LinkInputClass(Inputs);
    TradMed->LoadHistograms();
    //TradMed->InitializeCuts();
    //TradMed->Optimization_IncludeZero();
    //TradMed->OptimizeCuts();
    TradMed->DoMultipleOptimizations(10);
    TradMed->SaveCuts();
    TradMed->PlotHistograms();
    TradMed->PrintMatrix();
    // TradMed->Illustrate_EqualDist(10);
    
    if (TradMed->ContainsNoErrors()==kFALSE)
    {
        TradMed->PrintAllErrors();
    }

    // Done.
}
