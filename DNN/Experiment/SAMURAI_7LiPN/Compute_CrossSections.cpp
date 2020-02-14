void Compute_CrossSections()
{
    // Connect to InputsFile:
    R3BInputClass* Inputs = new R3BInputClass();
    Inputs->DisableErrorPrinting();
    Inputs->LinkFile("../../InputFiles/InputsFile.txt");
    Inputs->ReadFile();
    
    // Next, perform the computation:
    Bool_t Is250MeV = kFALSE;
    TString BranchTitle = "NeutronTracks_DNNScoringPlus";
    // TString BranchTitle = "NeutronTracks_TradMed_Clusters_CutsMult";
    // TString BranchTitle = "NeutronTracks_SingleReconstruction_NeuLAND";
    Double_t MC_Epsilon = 0.351;
    Double_t MC_Epsilon_Error = 0.001;
    
    CrossSections_7LiPN* CrossVal = new CrossSections_7LiPN();
    CrossVal->SetBandPosition(0.3);
    CrossVal->SetLogScale();
    CrossVal->DisableErrorPrinting();
    CrossVal->LinkInputClass(Inputs);
    if (Is250MeV==kTRUE) {CrossVal->Select250MeV();}
    else {CrossVal->Select110MeV();}
    CrossVal->SetMCDetectionEfficiency(MC_Epsilon,MC_Epsilon_Error);
    CrossVal->SetBranchTitle(BranchTitle);
    CrossVal->AnalyzeValidation();
    
    CrossSections_7LiPN* CrossExp = new CrossSections_7LiPN();
    // CrossExp->UseCustomEfficiencies("DNN_Method_250MeV.txt");
    CrossExp->SetBandPosition(0.45);
    CrossExp->SetLogScale();
    CrossExp->DisableErrorPrinting();
    CrossExp->LinkInputClass(Inputs);
    if (Is250MeV==kTRUE) {CrossExp->Select250MeV();}
    else {CrossExp->Select110MeV();}
    CrossExp->SetMCDetectionEfficiency(MC_Epsilon,MC_Epsilon_Error);
    CrossExp->SetBranchTitle(BranchTitle);
    CrossExp->AnalyzeExperiment();
    
    // Finally, show the log:
    if (CrossVal->ContainsNoErrors()==kFALSE)
    {
        CrossVal->PrintAllErrors();
    }
    
    if (CrossExp->ContainsNoErrors()==kFALSE)
    {
        CrossExp->PrintAllErrors();
    }

    // Done.
}
