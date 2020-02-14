void R3BInputClass::ModifyDouble(TString const Description, TString const Unit, Double_t const Value)
{
    // Modifies an existing Double_t input to a new unit and value.
    
    // First, read the file content:
    if (Linked==kTRUE)
    {
        ReadFile();
        
        // Then, search for the right inpput:
        Int_t Index = -1;
        
        for (Int_t k = 0; k<Double_Descriptions.size(); ++k)
        {
            if (Double_Descriptions.at(k)==Description)
            {
                Index = k;
            }
        }
        
        // Make the modification:
        if (Index==-1)
        {
            ErrorMessage("The Double_t input <"+Description+"> does not exist in the current file!");
        }
        else
        {
            // Then, we can modify it:
            Double_Units.at(Index) = Unit;
            Double_Values.at(Index) = Value;
        }
        
        // And write it to the file:
        WriteFile();
    }
    else
    {
        ErrorMessage("No .txt-file was linked to the class!");
    }
}

void R3BInputClass::ModifyInteger(TString const Description, Int_t const Value)
{
    // Modifies an existing Int_t input to a new unit and value.
    
    // First, read the file content:
    if (Linked==kTRUE)
    {
        ReadFile();
        
        // Then, search for the right inpput:
        Int_t Index = -1;
        
        for (Int_t k = 0; k<Integer_Descriptions.size(); ++k)
        {
            if (Integer_Descriptions.at(k)==Description)
            {
                Index = k;
            }
        }
        
        // Make the modification:
        if (Index==-1)
        {
            ErrorMessage("The Int_t input <"+Description+"> does not exist in the current file!");
        }
        else
        {
            // Then, we can modify it:
            Integer_Values.at(Index) = Value;
        }
        
        // And write it to the file:
        WriteFile();
    }
    else
    {
        ErrorMessage("No .txt-file was linked to the class!");
    }
}

void R3BInputClass::ModifyBoolian(TString const Description, Bool_t const Value)
{
    // Modifies an existing Bool_t input to a new unit and value.
    
    // First, read the file content:
    if (Linked==kTRUE)
    {
        ReadFile();
        
        // Then, search for the right input:
        Int_t Index = -1;
        
        for (Int_t k = 0; k<Boolian_Descriptions.size(); ++k)
        {
            if (Boolian_Descriptions.at(k)==Description)
            {
                Index = k;
            }
        }
        
        // Make the modification:
        if (Index==-1)
        {
            ErrorMessage("The Bool_t input <"+Description+"> does not exist in the current file!");
        }
        else
        {
            // Then, we can modify it:
            Boolian_Values.at(Index) = Value;
        }
        
        // And write it to the file:
        WriteFile();
    }
    else
    {
        ErrorMessage("No .txt-file was linked to the class!");
    }
}

void R3BInputClass::ModifyString(TString const Description, TString const Value)
{
    // Modifies an existing Bool_t input to a new unit and value.
    
    // First, read the file content:
    if (Linked==kTRUE)
    {
        ReadFile();
        
        // Then, search for the right input:
        Int_t Index = -1;
        
        for (Int_t k = 0; k<String_Descriptions.size(); ++k)
        {
            if (String_Descriptions.at(k)==Description)
            {
                Index = k;
            }
        }
        
        // Make the modification:
        if (Index==-1)
        {
            ErrorMessage("The TString input <"+Description+"> does not exist in the current file!");
        }
        else
        {
            // Then, we can modify it:
            String_Values.at(Index) = Value;
        }
        
        // And write it to the file:
        WriteFile();
    }
    else
    {
        ErrorMessage("No .txt-file was linked to the class!");
    }
}
