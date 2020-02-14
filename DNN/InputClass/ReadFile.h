void R3BInputClass::ReadFile()
{
    // Test if we can read:
    if (Linked==kFALSE)
    {
        ErrorMessage("You did not link a .txt-file to the class. Reading is impossible!");
    }
    else
    {
    
    // Loads the full textfile into the vectors. Begin by clearing everything out:
    Double_Descriptions.clear();
    Double_Units.clear();
    Double_Values.clear();
    Integer_Descriptions.clear();
    Integer_Values.clear();
    String_Descriptions.clear();
    String_Values.clear();
    Boolian_Descriptions.clear();
    Boolian_Values.clear();
    
    // Next, begin by oping the file:
    ifstream TheFile;
    TheFile.open(FileName.Data());
    
    // Then, start reading from it:
    std::string ThisLine;
    TString ThisLine_ROOT;
    Char_t* LineArray;
    Int_t EndPoint;
    
    TString Description = "";
    TString Type = "";
    TString Unit = "";
    TString Value = "";
    
    for (Int_t k = 0; k<NLines; ++k)
    {
        // get the line:
        std::getline(TheFile,ThisLine);
        ThisLine_ROOT = ThisLine;
        
        // Next, we start to break it down one-by-one.
        
        // Begin by extracting the description:
        ThisLine_ROOT.ReplaceAll("DESCRIPTION=","");
        
        EndPoint = ThisLine_ROOT.Index(";");
        LineArray = (Char_t*) ThisLine_ROOT.Data();
        Description = "";
        for (Int_t n = 0; n<EndPoint; ++n)
        {
            Description = Description + LineArray[n];
        }
        
        // Next, extract the type:
        ThisLine_ROOT.ReplaceAll(Description,"");
        ThisLine_ROOT.ReplaceAll("; TYPE=","");
        EndPoint = ThisLine_ROOT.Index(";");
        LineArray = (Char_t*) ThisLine_ROOT.Data();
        Type = "";
        for (Int_t n = 0; n<EndPoint; ++n)
        {
            Type = Type + LineArray[n];
        }
        
        // Now, proceed differently for different types:
        if (Type=="double")
        {
            // Then, we must extract the unit too:
            ThisLine_ROOT.ReplaceAll(Type,"");
            ThisLine_ROOT.ReplaceAll("; UNIT=","");
            EndPoint = ThisLine_ROOT.Index(";");
            LineArray = (Char_t*) ThisLine_ROOT.Data();
            Unit = "";
            for (Int_t n = 0; n<EndPoint; ++n)
            {
                Unit = Unit + LineArray[n];
            }
            
            // And then, extract the value:
            ThisLine_ROOT.ReplaceAll(Unit,"");
            ThisLine_ROOT.ReplaceAll("; VALUE=","");
            EndPoint = ThisLine_ROOT.Index(";");
            LineArray = (Char_t*) ThisLine_ROOT.Data();
            Value = "";
            for (Int_t n = 0; n<EndPoint; ++n)
            {
                Value = Value + LineArray[n];
            }
            
            // Next, add these values to the vectors:
            Double_Descriptions.push_back(Description);
            Double_Units.push_back(Unit);
            Double_Values.push_back(Value.Atof());
        }
        else
        {
            // Then, we just need to extract the value:
            ThisLine_ROOT.ReplaceAll(Type,"");
            ThisLine_ROOT.ReplaceAll("; VALUE=","");
            EndPoint = ThisLine_ROOT.Index(";");
            LineArray = (Char_t*) ThisLine_ROOT.Data();
            Value = "";
            for (Int_t n = 0; n<EndPoint; ++n)
            {
                Value = Value + LineArray[n];
            }
            
            // Next, add this to the vectors:
            if (Type=="int")
            {
                Integer_Descriptions.push_back(Description);
                Integer_Values.push_back(Value.Atoi());
            }
            else if (Type=="string")
            {
                String_Descriptions.push_back(Description);
                String_Values.push_back(Value);
            }
            else if (Type=="bool")
            {
                Boolian_Descriptions.push_back(Description);
                if (Value=="kTRUE") {Boolian_Values.push_back(kTRUE);}
                else if (Value=="kFALSE") {Boolian_Values.push_back(kFALSE);}
                else {ErrorMessage("Boolian Input <"+Description+"> has an invalid value <"+Value+">!");}
            }
            else
            {
                ErrorMessage("Input <"+Description+"> as an unsupported type <"+Type+">!");
            }
        }
        
        // Done. Close all blocks:
    }
    }
}
    
