void GenerateMaxMultiplicity()
{
    // Re-initialize the Inputs-class:
    R3BInputClass* Inputs = new R3BInputClass();
    Inputs->DisableErrorPrinting();
    Inputs->LinkFile("../InputFiles/InputsFile.txt");
    Inputs->ReadFile();
    
    // Obtain the Output Path:
    Int_t MaxMultiplicity = Inputs->GetInputInteger("ParticleGun_Multiplicity");
    
    // Check if all went well:
    if (Inputs->ContainsNoErrors()==kTRUE)
    {
        // Then, recreate a .txt-file:
        std::ofstream TheFile ("../InputFiles/MaxMultiplicity.txt", std::ofstream::out);
        
        // And write to it:
        TheFile << MaxMultiplicity;
        
        // Then, close up:
        TheFile.close();
    }
    else
    {
        // Then, print where it went wrong:
        Inputs->PrintAllErrors();
    }
}
