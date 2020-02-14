void R3BTradMethClass::LoadHistograms()
{
    // Loads the multiplicity histograms into the traditional method class.
    Histograms_Loaded = kFALSE;
    Bool_t ErrorTag = kFALSE;
    
    // We already know that the inputs class is properly linked. Hence:
    if (Inputs==0)
    {
        ErrorMessage("You should provide an inputs class to this class before calling LoadHistograms()!");
        ErrorTag = kTRUE;
    }
    else
    {
        // Retrieve all inputs:
        Inputs->ReadFile();
        
        // Next, read the filename where the calibration histograms are saved:
        TString FileName = Inputs->GetInputString("TheOutputPath") + "/TDR_Calibration_Histograms.root";
        
        // And obtain the maximum multiplicity:
        Max_Multiplicity = Inputs->GetInputInteger("ParticleGun_Multiplicity");
        
        // Next, declare the appropriate arrays (0 counts too, so increase the size by one):
        if (Max_Multiplicity>0)
        {
            // declare arrays:
            MultHistograms = new TH2D*[Max_Multiplicity+1];
            Ethresholds = new Double_t[Max_Multiplicity];
            Avg_Ethresholds = new Double_t[Max_Multiplicity];
            
            // Connect to the file:
            TFile* f = new TFile(FileName,"read");
            
            // Declare what we need:
            TString kstr = "";
            TString st = "";
            
            // Loop over all histograms:
            for (Int_t k = 0; k<(Max_Multiplicity+1); ++k)
            {
                // Convert k to a string:
                kstr = st.Itoa(k,10);
                
                // Obtain the histogram:
                MultHistograms[k] = (TH2D*) f->Get("TDR_Calibration_"+kstr);
                
                // Check that it worked:
                if ((MultHistograms[k]==0)||(MultHistograms[k]==nullptr))
                {
                    ErrorMessage("The histogram TDR_Calibration_"+kstr+" could not be located!");
                    ErrorTag = kTRUE;
                }
                
                // Reset the E-value:
                if (k!=Max_Multiplicity) 
                {
                    Ethresholds[k] = 0.0;
                    Avg_Ethresholds[k] = 0.0;
                }
            }
            
            // Check that no more histograms exist:
            TH1D* Test = 0;
            kstr = st.Itoa(Max_Multiplicity+1,10);
            Test = (TH1D*) f->Get("TDR_Calibration_"+kstr);
            
            if (!((Test==0)||(Test==nullptr)))
            {
                kstr = st.Itoa(Max_Multiplicity,10);
                ErrorMessage("There exist more calibration histograms than up to the multiplcity of "+kstr+" that was specified!");
                ErrorTag = kTRUE;
            }
            
            // reset Kappa too:
            Kappa = 0.0;
            
            // Done.
            if (ErrorTag==kFALSE)
            {
                Histograms_Loaded = kTRUE;
            }
        }
        else
        {
            ErrorMessage("You need to specify a maximum multiplicity above zero!");
            ErrorTag = kTRUE;
        }
        
        // Pass values to global parameters too:
        GLOBAL_Max_Multiplicity = Max_Multiplicity;
        GLOBAL_MultHistograms = MultHistograms;
        GLOBAL_Ethresholds = Ethresholds;
        GLOBAL_Kappa = Kappa;
    }
}
