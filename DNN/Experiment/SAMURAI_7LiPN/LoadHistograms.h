void CrossSections_7LiPN::LoadHistograms()
{
    // This function loads the histograms (per theta-bin)
    // into the class vectors, so that they can be analyzed.
    
    // Specify:
    // Bool_t Experiment_Is110MeV
    // Bool_t Experiment_Is250MeV
    // Bool_t Experiment_IsValidation
    // TString BranchTitle
    
    // Test if we have our inputs:
    if (Inputs_IsLinked==kTRUE)
    {
        // Obtain the output path:
        OutputPath = Inputs->GetInputString("TheOutputPath");
        if (OutputPath.Contains("/DNN_Training/")==kTRUE) {OutputPath.ReplaceAll("/DNN_Training/","");}
        if (OutputPath.Contains("/DNN_Validation/")==kTRUE) {OutputPath.ReplaceAll("/DNN_Validation/","");}
        if (OutputPath.Contains("/DNN_Experiment/")==kTRUE) {OutputPath.ReplaceAll("/DNN_Experiment/","");}
        if (OutputPath.Contains("/DNN_Experiment_Signal/")==kTRUE) {OutputPath.ReplaceAll("/DNN_Experiment_Signal/","");}
        if (OutputPath.Contains("/DNN_Experiment_Background/")==kTRUE) {OutputPath.ReplaceAll("/DNN_Experiment_Background/","");}
    
        // Compose the filenames:
        SignalFileName = OutputPath + "/DNN_Experiment_Signal/ExcitationEnergies.root";
        BackgroundFileName = OutputPath + "/DNN_Experiment_Background/ExcitationEnergies.root";
        Bool_t DoSubtraction = kTRUE;
        
        if (Experiment_IsValidation==kTRUE) 
        {
            SignalFileName = OutputPath + "/DNN_Validation/ExcitationEnergies.root";
            BackgroundFileName = OutputPath + "/DNN_DoesNotExits/Stupid.root";
            DoSubtraction = kFALSE;
        }
        
        cout << "### Signal FileName     = " << SignalFileName << "\n";
        cout << "### Background FileName = " << BackgroundFileName << "\n";
            
        // Connect to the files:
        FileStat_t stom;
        Int_t SigTest = gSystem->GetPathInfo(SignalFileName.Data(),stom);
        Int_t BackTest = gSystem->GetPathInfo(BackgroundFileName.Data(),stom);
    
        TFile* SigFile = 0;
        if (SigTest==0) {SigFile = new TFile(SignalFileName.Data(),"read"); cout << "Signal file was found!\n";}
        else {cout << "ERROR: Signal file could not be located!\n";}
    
        TFile* BackFile = 0;
        if (BackTest==0) {BackFile = new TFile(BackgroundFileName.Data(),"read"); cout << "Background file was found!\n";}
        else 
        {
            if (Experiment_IsValidation==kFALSE) {cout << "ERROR: Background file could not be located!\n";}
            if (Experiment_IsValidation==kTRUE)  {cout << "Background file was skipped for validation!\n";}
        }
    
        // Next, try to load as many histograms as possible:
        TString HistName_FirstPart = BranchTitle + "_Excitation_Energy_Bin_"; // ATTENTION: Underscore.
        TString HistName;
        TString st = "";
        TString kstr = "";
        Int_t BinNumber = 1;
        Bool_t WhileTest = kTRUE;
        if (SigTest!=0) {WhileTest = kFALSE;}
        TH1D* ThisHist;
    
        while (WhileTest==kTRUE)
        {
            // Compose the bin number:
            kstr = st.Itoa(BinNumber,10);
            HistName = HistName_FirstPart + kstr;
        
            // Display output:
            cout << HistName << " | ";
            
            // Try to find the signal histogram:
            if (SigTest==0) {ThisHist = (TH1D*) SigFile->Get(HistName.Data());} 
            else {WhileTest = kFALSE;}
        
            // See if it worked:
            if ((ThisHist==0)||(ThisHist==nullptr)) {WhileTest = kFALSE; cout << "SignalHist=ERROR";}
            else {SignalHists.push_back(ThisHist); cout << "SignalHist=found!";}
        
            // Try to find the background histogram:
            if ((WhileTest==kTRUE)&&(BackTest==0)) {ThisHist = (TH1D*) BackFile->Get(HistName.Data());}
            cout << " | ";
        
            // See if it worked:
            if ((ThisHist==0)||(ThisHist==nullptr)) {cout << "BackgroundHist=ERROR"; if (WhileTest==kTRUE) {BackgroundHists.push_back(0);}}
            else {BackgroundHists.push_back(ThisHist); cout << "BackgroundHist=found!";}
        
            // Update the bin number:
            BinNumber = BinNumber + 1;
            cout << "\n";
        }
    
        // Define the size:
        Int_t HistSize = SignalHists.size();
    
        // Define some parameters:
        Double_t BackScale = 1.0;
        if (Experiment_Is110MeV==kTRUE) {BackScale = 4461118.0/793763.0;}
        if (Experiment_Is250MeV==kTRUE) {BackScale = 4371700.0/1344424.0;}
    
        Double_t LowerBoundary = -20.0;
    
        Double_t UpperBoundary = 20.0;
        if (Experiment_Is110MeV==kTRUE) {UpperBoundary = 70.0;}
        if (Experiment_Is250MeV==kTRUE) {UpperBoundary = 100.0;}
        if (Experiment_IsValidation==kTRUE) {UpperBoundary = 40.0;}
    
        // Next, perform the background subtraction:
        TString NewHistName;
    
        for (Int_t k = 0; k<HistSize; ++k)
        {
            NewHistName = SignalHists.at(k)->GetName();
            NewHistName = NewHistName + "_Subtracted";
            ThisHist = SubtractHists(NewHistName,SignalHists.at(k),BackgroundHists.at(k),BackScale,LowerBoundary,UpperBoundary,DoSubtraction);
            
            if ((ThisHist!=0)&&(ThisHist!=nullptr))
            {
                SubtractedHists.push_back(ThisHist);
                cout << NewHistName << " | Background subtraction worked!\n";
            }
            else
            {
                cout << NewHistName << " | ERROR: Background subtraction failed!\n";
            }
        }
        
        // Done. All required histograms are loaded now.
    }
}
    
    
    
    
    
    
    
        
