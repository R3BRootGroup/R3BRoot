void CrossSections_7LiPN::ReadEfficiencies()
{
    // Read efficiencies from the file:
    ifstream EffTextFile;
    
    // Define filename:
    TString TextFileName = OutputPath + "/CrossSection_Efficiencies_" + BranchTitle;
    if (Experiment_Is110MeV==kTRUE) {TextFileName = TextFileName + "_110MeV";}
    if (Experiment_Is250MeV==kTRUE) {TextFileName = TextFileName + "_250MeV";}
    
    // NOTE: Always read from validation:
    TextFileName = TextFileName + "_Validation.txt";
    
    if (UseExternalEfficiencies==kTRUE)
    {
        TextFileName = OutputPath + "/" + External_EffFile;
    }
    
    // Open the file:
    EffTextFile.open(TextFileName.Data());
    
    if (EffTextFile.is_open())
    {
        // Then we can move on:
        Double_t ThisEpslon;
        Double_t ThisEpsion_Error;
        Double_t ThisEpsion_Systematic;
        
        // Clean efficiency vectors:
        Epsilon.clear();
        Epsilon_Error.clear();
        Epsilon_Systematic.clear();
        
        // Read the file:
        std::string ThisLine;
        
        if (PeakAreas.size()<=0)
        {
            ErrorMessage("We cannot read efficiencies if there are no peak areas computed!");
        }
        else
        {
            for (Int_t k = 0; k<PeakAreas.size(); ++k)
            {
                // Pass parameters:
                EffTextFile >> ThisEpslon;
                EffTextFile >> ThisEpsion_Error;
                EffTextFile >> ThisEpsion_Systematic;
                
                Epsilon.push_back(ThisEpslon);
                Epsilon_Error.push_back(ThisEpsion_Error);
                Epsilon_Systematic.push_back(ThisEpsion_Systematic);
                
                // Move to next line:
                std::getline(EffTextFile,ThisLine);
                
                // Done.
            }
        }
        
        // Close the file:
        EffTextFile.close();
    }
    else
    {
        ErrorMessage("You tried to read efficiencies that were not computed before!");
    }
}
