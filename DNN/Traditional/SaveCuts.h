void R3BTradMethClass::SaveCuts()
{
    // This function saves the current cuts (obtained through
    // optimization, for example), to a .txt-file so they can 
    // be used & applied later on.
    
    if (Inputs==0)
    {
        ErrorMessage("You should provide an inputs class to this class before calling SaveCuts()!");
    }
    else
    {
        // Obtain the output path:
        TString PATH = Inputs->GetInputString("TheOutputPath");
        
        // Define filename:
        TString FileName = PATH + "/CutsFile.txt";
        
        // Create the file:
        std::ofstream TheFile (FileName.Data(), std::ofstream::out);
        
        // Write the data:
        TheFile << Kappa << "\n";
        
        for (Int_t k = 0; k<Max_Multiplicity; ++k)
        {
            TheFile << Ethresholds[k] << "\n";
        }
        
        // Close the file:
        TheFile.close();
        
        // Done.
    }
}

void R3BTradMethClass::LoadCuts()
{
    // This function loads the cuts from an exterior file.
    // Then you can use them without having to optimize them first.
    
    if (Inputs==0)
    {
        ErrorMessage("You should provide an inputs class to this class before calling LoadCuts()!");
    }
    else
    {
        // Obtain the output path:
        TString PATH = Inputs->GetInputString("TheOutputPath");
        
        // Define filename:
        TString FileName = PATH + "/CutsFile.txt";
        
        // Load the file:
        ifstream TheFile;
        TheFile.open(FileName.Data());
        
        // Check that it worked:
        if (TheFile.is_open())
        {
            // then, we can extract data from it:
            Int_t NLines = 0;
            std::string ThisLine;
            while (std::getline(TheFile,ThisLine)) {NLines = NLines + 1;}
            
            // Check that the file has the correct number of lines:
            if (NLines==(Max_Multiplicity+1))
            {
                // Then the cuts can be used for this situation:
                TheFile.close();
                TheFile.open(FileName.Data());
                
                // Retrieve Kappa:
                TheFile >> Kappa;
                
                // Retrieve energies:
                for (Int_t k = 0; k<Max_Multiplicity; ++k)
                {
                    std::getline(TheFile,ThisLine);
                    TheFile >> Ethresholds[k];
                }
                
                // Close the file:
                TheFile.close();
                
                // Pass varaibles to the global ones:
                GLOBAL_Max_Multiplicity = Max_Multiplicity;
                GLOBAL_MultHistograms = MultHistograms;
                GLOBAL_Ethresholds = Ethresholds;
                GLOBAL_Kappa = Kappa;
                
                // Tag that we now have valid cuts:
                Cuts_Initialized = kTRUE;
            }
            else
            {
                TString st = "";
                ErrorMessage("The File corresponds to "+st.Itoa(NLines,10)+" Histograms, while the class currently contains "+st.Itoa(Max_Multiplicity+1,10)+" histograms!");
            }
        }
        else
        {
            ErrorMessage("No file containing cuts information could be found!");
        }
    }
}
