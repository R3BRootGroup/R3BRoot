void GenerateExperimentalInput()
{
    // Re-initialize the Inputs-class:
    R3BInputClass* Inputs = new R3BInputClass();
    Inputs->DisableErrorPrinting();
    Inputs->LinkFile("../InputFiles/InputsFile.txt");
    
    // Check if the File exists:
    if (Inputs->IsLinked()==kFALSE)
    {
        cout << "### FATAL: Inputs-file could not be found!\n"
             << "### FATAL: New default-file was generated!\n\n";
             
        // Then, recreate it:
        Inputs->CreateFile("../InputFiles/InputsFile.txt");
        cout << "\nNOTE: Inputs-file was regenerated from scratch!\n\n";
    }
    else
    {
        cout << "\nOK. Inputs-file exists. No need to regenerate it.\n\n";
    }
    
    // Then, try again:
    Inputs->LinkFile("../InputFiles/InputsFile.txt");
    Inputs->ReadFile();
    
    // Next, adjust the inputs so that we can do Network Training:
    TString OutputPath = Inputs->GetInputString("TheOutputPath");
    if (OutputPath.Contains("/DNN_Training/")==kTRUE) {OutputPath.ReplaceAll("/DNN_Training/","");}
    if (OutputPath.Contains("/DNN_Validation/")==kTRUE) {OutputPath.ReplaceAll("/DNN_Validation/","");}
    if (OutputPath.Contains("/DNN_Experiment/")==kTRUE) {OutputPath.ReplaceAll("/DNN_Experiment/","");}
    OutputPath = OutputPath + "/DNN_Experiment/";
    Inputs->ModifyString("TheOutputPath",OutputPath);
    
    Inputs->ModifyInteger("ParticleGun_StartEventReading_SnDataFiles",9000);
    Inputs->ModifyInteger("ParticleGun_EndEventReading_SnDataFiles",9999);
    Inputs->ModifyInteger("NeuLAND_Digitizer_nDifferentRuns",1);
    Inputs->ModifyBoolian("NeuLAND_TranslateToSignals_Mark_TruePrimarySignals",kFALSE);
    Inputs->ModifyBoolian("NeuLAND_DNNTextFile_ValidationMode",kTRUE);
    Inputs->ModifyBoolian("ParticleGun_Uniformize_Detected_Multiplicity",kFALSE);
    Inputs->ModifyInteger("R3BRoot_nEvents",10);
    
    if (Inputs->GetInputString("NeuLAND_DNNTextFile_NormalizationMethod")=="Elena")
    {
        Inputs->ModifyInteger("ParticleGun_StartEventReading_SnDataFiles",0);
        Inputs->ModifyInteger("ParticleGun_EndEventReading_SnDataFiles",9999);
    }
    
    // Then, write these modifications back to the file:
    Inputs->WriteFile();
    
    // Done. Print error log in the end:
    Inputs->PrintAllErrors();
    delete Inputs;
}
