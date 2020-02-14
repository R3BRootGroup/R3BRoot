void R3BInputClass::AddInputDouble(TString const Description, TString const Unit, Double_t const Value)
{
    // Adds an input of type Double to the file.
    TString ValueString = RoundOff(Value,4);
    
    // Check if the inputs do not suffer from invalid strings:
    Bool_t ContainsInvalidString = kFALSE;
    
    if (Description.Contains("DESCRIPTION")==kTRUE) {ContainsInvalidString = kTRUE; ErrorMessage("The input <"+Description+"> cannot contain <DESCRIPTION>!");}
    if (Description.Contains("VALUE")==kTRUE)       {ContainsInvalidString = kTRUE; ErrorMessage("The input <"+Description+"> cannot contain <VALUE>!");}
    if (Description.Contains("TYPE")==kTRUE)        {ContainsInvalidString = kTRUE; ErrorMessage("The input <"+Description+"> cannot contain <TYPE>!");}
    if (Description.Contains("UNIT")==kTRUE)        {ContainsInvalidString = kTRUE; ErrorMessage("The input <"+Description+"> cannot contain <UNIT>!");}
    if (Description.Contains("=")==kTRUE)           {ContainsInvalidString = kTRUE; ErrorMessage("The input <"+Description+"> cannot contain <=>!");}
    if (Description.Contains(";")==kTRUE)           {ContainsInvalidString = kTRUE; ErrorMessage("The input <"+Description+"> cannot contain <;>!");}
    
    if (Unit.Contains("DESCRIPTION")==kTRUE)        {ContainsInvalidString = kTRUE; ErrorMessage("The input unit <"+Unit+"> cannot contain <DESCRIPTION>!");}
    if (Unit.Contains("VALUE")==kTRUE)              {ContainsInvalidString = kTRUE; ErrorMessage("The input unit <"+Unit+"> cannot contain <VALUE>!");}
    if (Unit.Contains("TYPE")==kTRUE)               {ContainsInvalidString = kTRUE; ErrorMessage("The input unit <"+Unit+"> cannot contain <TYPE>!");}
    if (Unit.Contains("UNIT")==kTRUE)               {ContainsInvalidString = kTRUE; ErrorMessage("The input unit <"+Unit+"> cannot contain <UNIT>!");}
    if (Unit.Contains("=")==kTRUE)                  {ContainsInvalidString = kTRUE; ErrorMessage("The input unit <"+Unit+"> cannot contain <=>!");}
    if (Unit.Contains(";")==kTRUE)                  {ContainsInvalidString = kTRUE; ErrorMessage("The input unit <"+Unit+"> cannot contain <;>!");}
    
    // The next step, is to see if this input has already been defined.
    Bool_t InputExists = kFALSE;
    
    if (Linked==kTRUE)
    {
        std::ifstream TheFile;
        TheFile.open(FileName.Data());
        
        std::string ThisLine;
        TString ThisLine_ROOT;
        
        for (Int_t k = 0; k<NLines; ++k)
        {
            std::getline(TheFile,ThisLine);
            ThisLine_ROOT = ThisLine;
            
            if (ThisLine_ROOT.Contains(Description)==kTRUE)
            {
                InputExists = kTRUE;
                ErrorMessage("The input <"+Description+"> already exists!");
            }
        }
        
        // Next, we can add it to the file:
        if ((ContainsInvalidString==kFALSE)&&(InputExists==kFALSE))
        {
            // Then, we can add the input to the file.
    
            // Define the total string:
            TString Total = "DESCRIPTION=" + Description + "; TYPE=double; UNIT=" + Unit + "; VALUE=" + ValueString + ";\n";
        
            // open the file in update mode & add:
            std::ofstream ThisFile;
            ThisFile.open(FileName.Data(), std::ios::app);
            ThisFile << Total.Data();
            ThisFile.close();
        }
        else
        {
            ErrorMessage("The input <"+Description+"> was not added to the file, due to a previous error!");
        }
        
        // Then, also read the new content:
        ReadFile();
    }
    else
    {
        ErrorMessage("The input <"+Description+"> could not be added, because you did not successfully link this class to a .txt-file!");
    }
}

void R3BInputClass::AddInputString(TString const Description, TString const Value)
{
    // Check if the inputs do not suffer from invalid strings:
    Bool_t ContainsInvalidString = kFALSE;
    
    if (Description.Contains("DESCRIPTION")==kTRUE) {ContainsInvalidString = kTRUE; ErrorMessage("The input <"+Description+"> cannot contain <DESCRIPTION>!");}
    if (Description.Contains("VALUE")==kTRUE)       {ContainsInvalidString = kTRUE; ErrorMessage("The input <"+Description+"> cannot contain <VALUE>!");}
    if (Description.Contains("TYPE")==kTRUE)        {ContainsInvalidString = kTRUE; ErrorMessage("The input <"+Description+"> cannot contain <TYPE>!");}
    if (Description.Contains("UNIT")==kTRUE)        {ContainsInvalidString = kTRUE; ErrorMessage("The input <"+Description+"> cannot contain <UNIT>!");}
    if (Description.Contains("=")==kTRUE)           {ContainsInvalidString = kTRUE; ErrorMessage("The input <"+Description+"> cannot contain <=>!");}
    if (Description.Contains(";")==kTRUE)           {ContainsInvalidString = kTRUE; ErrorMessage("The input <"+Description+"> cannot contain <;>!");}
    
    if (Value.Contains("DESCRIPTION")==kTRUE)        {ContainsInvalidString = kTRUE; ErrorMessage("The input value <"+Value+"> cannot contain <DESCRIPTION>!");}
    if (Value.Contains("VALUE")==kTRUE)              {ContainsInvalidString = kTRUE; ErrorMessage("The input value <"+Value+"> cannot contain <VALUE>!");}
    if (Value.Contains("TYPE")==kTRUE)               {ContainsInvalidString = kTRUE; ErrorMessage("The input value <"+Value+"> cannot contain <TYPE>!");}
    if (Value.Contains("UNIT")==kTRUE)               {ContainsInvalidString = kTRUE; ErrorMessage("The input value <"+Value+"> cannot contain <UNIT>!");}
    if (Value.Contains("=")==kTRUE)                  {ContainsInvalidString = kTRUE; ErrorMessage("The input value <"+Value+"> cannot contain <=>!");}
    if (Value.Contains(";")==kTRUE)                  {ContainsInvalidString = kTRUE; ErrorMessage("The input value <"+Value+"> cannot contain <;>!");}
    
    // The next step, is to see if this input has already been defined.
    Bool_t InputExists = kFALSE;
    
    if (Linked==kTRUE)
    {
        std::ifstream TheFile;
        TheFile.open(FileName.Data());
        
        std::string ThisLine;
        TString ThisLine_ROOT;
        
        for (Int_t k = 0; k<NLines; ++k)
        {
            std::getline(TheFile,ThisLine);
            ThisLine_ROOT = ThisLine;
            
            if (ThisLine_ROOT.Contains(Description)==kTRUE)
            {
                InputExists = kTRUE;
                ErrorMessage("The input <"+Description+"> already exists!");
            }
        }
        
        // Next, we can add it to the file:
        if ((ContainsInvalidString==kFALSE)&&(InputExists==kFALSE))
        {
            // Then, we can add the input to the file.
    
            // Define the total string:
            TString Total = "DESCRIPTION=" + Description + "; TYPE=string; VALUE=" + Value + ";\n";
        
            // open the file in update mode & add:
            std::ofstream ThisFile;
            ThisFile.open(FileName.Data(), std::ios::app);
            ThisFile << Total.Data();
            ThisFile.close();
        }
        else
        {
            ErrorMessage("The input <"+Description+"> was not added to the file, due to a previous error!");
        }
        
        // Then, also read the new content:
        ReadFile();
    }
    else
    {
        ErrorMessage("The input <"+Description+"> could not be added, because you did not successfully link this class to a .txt-file!");
    }
}

void R3BInputClass::AddInputInteger(TString const Description, Int_t const Value)
{
    // Adds an input of type Double to the file.
    TString st = "";
    TString ValueString = st.Itoa(Value,10);
    
    // Check if the inputs do not suffer from invalid strings:
    Bool_t ContainsInvalidString = kFALSE;
    
    if (Description.Contains("DESCRIPTION")==kTRUE) {ContainsInvalidString = kTRUE; ErrorMessage("The input <"+Description+"> cannot contain <DESCRIPTION>!");}
    if (Description.Contains("VALUE")==kTRUE)       {ContainsInvalidString = kTRUE; ErrorMessage("The input <"+Description+"> cannot contain <VALUE>!");}
    if (Description.Contains("TYPE")==kTRUE)        {ContainsInvalidString = kTRUE; ErrorMessage("The input <"+Description+"> cannot contain <TYPE>!");}
    if (Description.Contains("UNIT")==kTRUE)        {ContainsInvalidString = kTRUE; ErrorMessage("The input <"+Description+"> cannot contain <UNIT>!");}
    if (Description.Contains("=")==kTRUE)           {ContainsInvalidString = kTRUE; ErrorMessage("The input <"+Description+"> cannot contain <=>!");}
    if (Description.Contains(";")==kTRUE)           {ContainsInvalidString = kTRUE; ErrorMessage("The input <"+Description+"> cannot contain <;>!");}
    
    // The next step, is to see if this input has already been defined.
    Bool_t InputExists = kFALSE;
    
    if (Linked==kTRUE)
    {
        std::ifstream TheFile;
        TheFile.open(FileName.Data());
        
        std::string ThisLine;
        TString ThisLine_ROOT;
        
        for (Int_t k = 0; k<NLines; ++k)
        {
            std::getline(TheFile,ThisLine);
            ThisLine_ROOT = ThisLine;
            
            if (ThisLine_ROOT.Contains(Description)==kTRUE)
            {
                InputExists = kTRUE;
                ErrorMessage("The input <"+Description+"> already exists!");
            }
        }
        
        // Next, we can add it to the file:
        if ((ContainsInvalidString==kFALSE)&&(InputExists==kFALSE))
        {
            // Then, we can add the input to the file.
    
            // Define the total string:
            TString Total = "DESCRIPTION=" + Description + "; TYPE=int; VALUE=" + ValueString + ";\n";
        
            // open the file in update mode & add:
            std::ofstream ThisFile;
            ThisFile.open(FileName.Data(), std::ios::app);
            ThisFile << Total.Data();
            ThisFile.close();
        }
        else
        {
            ErrorMessage("The input <"+Description+"> was not added to the file, due to a previous error!");
        }
        
        // Then, also read the new content:
        ReadFile();
    }
    else
    {
        ErrorMessage("The input <"+Description+"> could not be added, because you did not successfully link this class to a .txt-file!");
    }
}

void R3BInputClass::AddInputBoolian(TString const Description, Bool_t const Value)
{
    // Check if the inputs do not suffer from invalid strings:
    Bool_t ContainsInvalidString = kFALSE;
    
    if (Description.Contains("DESCRIPTION")==kTRUE) {ContainsInvalidString = kTRUE; ErrorMessage("The input <"+Description+"> cannot contain <DESCRIPTION>!");}
    if (Description.Contains("VALUE")==kTRUE)       {ContainsInvalidString = kTRUE; ErrorMessage("The input <"+Description+"> cannot contain <VALUE>!");}
    if (Description.Contains("TYPE")==kTRUE)        {ContainsInvalidString = kTRUE; ErrorMessage("The input <"+Description+"> cannot contain <TYPE>!");}
    if (Description.Contains("UNIT")==kTRUE)        {ContainsInvalidString = kTRUE; ErrorMessage("The input <"+Description+"> cannot contain <UNIT>!");}
    if (Description.Contains("=")==kTRUE)           {ContainsInvalidString = kTRUE; ErrorMessage("The input <"+Description+"> cannot contain <=>!");}
    if (Description.Contains(";")==kTRUE)           {ContainsInvalidString = kTRUE; ErrorMessage("The input <"+Description+"> cannot contain <;>!");}
    
    // The next step, is to see if this input has already been defined.
    Bool_t InputExists = kFALSE;
    
    if (Linked==kTRUE)
    {
        std::ifstream TheFile;
        TheFile.open(FileName.Data());
        
        std::string ThisLine;
        TString ThisLine_ROOT;
        
        for (Int_t k = 0; k<NLines; ++k)
        {
            std::getline(TheFile,ThisLine);
            ThisLine_ROOT = ThisLine;
            
            if (ThisLine_ROOT.Contains(Description)==kTRUE)
            {
                InputExists = kTRUE;
                ErrorMessage("The input <"+Description+"> already exists!");
            }
        }
        
        // Next, we can add it to the file:
        if ((ContainsInvalidString==kFALSE)&&(InputExists==kFALSE))
        {
            // Then, we can add the input to the file.
    
            // Define the total string:
            TString Total;
            if (Value==kTRUE)  {Total = "DESCRIPTION=" + Description + "; TYPE=bool; VALUE=" + "kTRUE" + ";\n";}
            if (Value==kFALSE) {Total = "DESCRIPTION=" + Description + "; TYPE=bool; VALUE=" + "kFALSE" + ";\n";}
        
            // open the file in update mode & add:
            std::ofstream ThisFile;
            ThisFile.open(FileName.Data(), std::ios::app);
            ThisFile << Total.Data();
            ThisFile.close();
        }
        else
        {
            ErrorMessage("The input <"+Description+"> was not added to the file, due to a previous error!");
        }
        
        // Then, also read the new content:
        ReadFile();
    }
    else
    {
        ErrorMessage("The input <"+Description+"> could not be added, because you did not successfully link this class to a .txt-file!");
    }
}
    
