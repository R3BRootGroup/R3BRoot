Bool_t NormalizationClass::ReadNormFile(TString const TheFileName)
{
    // Reads the NormParams.txt-file and transfers the content to this class.
    TString FileName = TheFileName;
    FileName.ReplaceAll(".txt","_NormParams.txt");
    
    // Connect to the file:
    ifstream TheFile;
    TheFile.open(FileName.Data());
    if (TheFile.is_open())
    {
        // Clean the vector:
        TheNormParameters.clear();
        
        // Declare what we need:
        std::string ThisLine;
        TString ThisLine_ROOT;
        NormUnit ThisUnit;
        Int_t NLines = 0;
        Char_t* LineArray;
        Int_t EndPoint;
        TString ParName;
        TString ParOPT;
        TString ParValue_str;
        Double_t ParValue;
        
        // Start reading:
        while (std::getline(TheFile,ThisLine)) 
        {
            // Extract the line from the file:
            NLines = NLines + 1;
            ThisLine_ROOT = ThisLine;
            
            // Extract the name:
            ThisLine_ROOT.ReplaceAll("NAME=","");
            EndPoint = ThisLine_ROOT.Index(";");
            LineArray = (Char_t*) ThisLine_ROOT.Data();
            ParName = "";
            for (Int_t n = 0; n<EndPoint; ++n)
            {
                ParName = ParName + LineArray[n];
            }
            ThisUnit.Name = ParName;
            
            // Extract the option:
            ThisLine_ROOT.ReplaceAll("OPTION=","");
            EndPoint = ThisLine_ROOT.Index(";");
            LineArray = (Char_t*) ThisLine_ROOT.Data();
            ParOPT = "";
            for (Int_t n = 0; n<EndPoint; ++n)
            {
                ParOPT = ParOPT + LineArray[n];
            }
            ThisUnit.Normalization_Option = ParOPT;
            
            // Extract 4 values:
            for (Int_t ii = 0; ii<4; ++ii)
            {
                if (ii==0) {ThisLine_ROOT.ReplaceAll("OLD_MEANSHIFT=","");}
                if (ii==1) {ThisLine_ROOT.ReplaceAll("OLD_SCALEFACTOR=","");}
                if (ii==2) {ThisLine_ROOT.ReplaceAll("NEW_MEANSHIFT=","");}
                if (ii==3) {ThisLine_ROOT.ReplaceAll("NEW_SCALEFACTOR=","");}
                
                EndPoint = ThisLine_ROOT.Index(";");
                LineArray = (Char_t*) ThisLine_ROOT.Data();
                ParValue_str = "";
                for (Int_t n = 0; n<EndPoint; ++n)
                {
                    ParValue_str = ParValue_str + LineArray[n];
                }
                ParValue = ParValue_str.Atof();
                
                if (ii==0) {ThisUnit.Old_MeanShift = ParValue;}
                if (ii==1) {ThisUnit.Old_ScaleFactor = ParValue;}
                if (ii==2) {ThisUnit.New_MeanShift = ParValue;}
                if (ii==3) {ThisUnit.New_ScaleFactor = ParValue;}
            }
            
            // Next, push it back into the vector:
            TheNormParameters.push_back(ThisUnit);
        }
        
        // Reading is now done.
        return kTRUE;
    }
    else
    {
        cout << "### NORMALISATION CLASS ERROR: The NormParams.txt-file does not exist!\n";
        return kFALSE;
    }
    
    return kFALSE;
}
            
        
