void GenerateOutputPath()
{
    // Re-initialize the Inputs-class:
    R3BInputClass* Inputs = new R3BInputClass();
    Inputs->DisableErrorPrinting();
    Inputs->LinkFile("../InputFiles/InputsFile.txt");
    Inputs->ReadFile();
    
    // Obtain the Output Path:
    TString OutputPath = Inputs->GetInputString("TheOutputPath");
    if (OutputPath.Contains("/DNN_Training/")==kTRUE) {OutputPath.ReplaceAll("/DNN_Training/","");}
    if (OutputPath.Contains("/DNN_Validation/")==kTRUE) {OutputPath.ReplaceAll("/DNN_Validation/","");}
    if (OutputPath.Contains("/DNN_Experiment/")==kTRUE) {OutputPath.ReplaceAll("/DNN_Experiment/","");}
    
    // Check if all went well:
    if (Inputs->ContainsNoErrors()==kTRUE)
    {
        // Then, recreate a .txt-file:
        std::ofstream TheFile ("../InputFiles/OutputPath.txt", std::ofstream::out);
        
        // And write to it:
        TheFile << OutputPath;
        
        // Then, close up:
        TheFile.close();
    }
    else
    {
        // Then, print where it went wrong:
        Inputs->PrintAllErrors();
    }
}
