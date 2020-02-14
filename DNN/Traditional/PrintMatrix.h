void R3BTradMethClass::PrintMatrix()
{
    // This function saves the neutron separation matrix into
    // a .txt-file as output format. The cuts being used
    // are also printed.
    
    if (!((Cuts_Initialized==kTRUE)&&(Histograms_Loaded==kTRUE)))
    {
        ErrorMessage("You need both loaded histograms and valid cuts to print a neutron separation matrix!");
    }
    else
    {
        // Obtain the output path:
        TString PATH = Inputs->GetInputString("TheOutputPath");
        
        // Define filename:
        TString FileName = PATH + "/SeparationMatrix.txt";
        
        // Create the file:
        std::ofstream TheFile (FileName.Data(), std::ofstream::out);
        
        // begin by printing the matrix:
        TheFile << "NEUTRON SEPARATION MATRIX:\n\n";
        TheFile << "               True neutron multiplicity:\n";
        
        // print true multiplicities:
        TString st = "";
        TString kstr = "";
        TheFile << "               ";
        for (Int_t k = 0; k<(Max_Multiplicity+1); ++k)
        {
            kstr = st.Itoa(k,10);
            TheFile << kstr << "n    ";
        }
        TheFile << "\n";
        
        // print matrix:
        TString Percent = "";
        TString rowstr = "";
        TString colstr = "";
        Double_t ThisEff = 0.0;
        
        for (Int_t krow = 0; krow<(Max_Multiplicity+1); ++krow)
        {
            // print directive:            
            if (krow==0) {TheFile << "obtained   ";}
            else if (krow==1) {TheFile << "neutron    ";}
            else if (krow==2) {TheFile << "mult.      ";}
            else {TheFile << "           ";}
            
            // print obtained multiplicities:
            rowstr = st.Itoa(krow,10);
            TheFile << rowstr << "n  ";
            
            // print matrix:
            for (Int_t kcol = 0; kcol<(Max_Multiplicity+1); ++kcol)
            {
                // Compute efficiency:
                // Range = row (what we think) & matrix = true mult.
                ThisEff = HistIntegral(kcol,krow);
                
                // Convert to string:
                Percent = RoundOff(ThisEff,2);
                Percent = Percent + "%";
                
                // print it:
                if (Percent.Length()==2) {TheFile << " ";}
                TheFile << Percent.Data() << "   ";
            }
            
            // skip to the next line:
            TheFile << "\n";
        }
        
        // Now we have printed the matrix. Print the cuts:
        TheFile << "\n\n\n";
        TheFile << "CALIBRATION CUTS USED TO COMPUTE THIS MATRIX:\n\n";
        TheFile << "Kappa = " << Kappa << " [#clusters/MeV]\n";
        
        for (Int_t k = 0; k<Max_Multiplicity; ++k)
        {
            kstr = st.Itoa(k,10);
            TheFile << kstr.Data() << "n Energy Cut = " << Ethresholds[k] << " [MeV]\n";
        }
        
        // Done. Close the File:
        TheFile.close();
    }
}
