void R3BInputClass::EraseInput(TString const Description)
{
    // This function erases a specific input from the file.
    
    // Check if the class is linked:
    if (Linked==kTRUE)
    {
        // Then, read the file content:
        ReadFile();
        
        // Next, search what input is it:
        Int_t Index_Double = -1;
        Int_t Index_Int = -1;
        Int_t Index_Bool = -1;
        Int_t Index_String = -1;
        
        for (Int_t k = 0; k<Double_Descriptions.size(); ++k)
        {
            if (Double_Descriptions.at(k)==Description)
            {
                Index_Double = k;
            }
        }
        
        for (Int_t k = 0; k<Integer_Descriptions.size(); ++k)
        {
            if (Integer_Descriptions.at(k)==Description)
            {
                Index_Int = k;
            }
        }
        
        for (Int_t k = 0; k<String_Descriptions.size(); ++k)
        {
            if (String_Descriptions.at(k)==Description)
            {
                Index_Bool = k;
            }
        }
        
        for (Int_t k = 0; k<Boolian_Descriptions.size(); ++k)
        {
            if (Boolian_Descriptions.at(k)==Description)
            {
                Index_String = k;
            }
        }
        
        // Then, the input that we found should be removed:
        if (Index_Double!=-1)
        {
            // re-locate the end:
            Int_t Size = (Int_t) Double_Descriptions.size();
            
            Double_Descriptions.at(Index_Double) = Double_Descriptions.at(Size-1);
            Double_Units.at(Index_Double) = Double_Units.at(Size-1);
            Double_Values.at(Index_Double) = Double_Values.at(Size-1);
            
            Double_Descriptions.resize(Size-1);
            Double_Units.resize(Size-1);
            Double_Values.resize(Size-1);
        }
        else if (Index_Int!=-1)
        {
            // re-locate the end:
            Int_t Size = (Int_t) Integer_Descriptions.size();
            
            Integer_Descriptions.at(Index_Double) = Integer_Descriptions.at(Size-1);
            Integer_Values.at(Index_Double) = Integer_Values.at(Size-1);
            
            Integer_Descriptions.resize(Size-1);
            Integer_Values.resize(Size-1);
        }
        else if (Index_Bool!=-1)
        {
            // re-locate the end:
            Int_t Size = (Int_t) Boolian_Descriptions.size();
            
            Boolian_Descriptions.at(Index_Double) = Boolian_Descriptions.at(Size-1);
            Boolian_Values.at(Index_Double) = Boolian_Values.at(Size-1);
            
            Boolian_Descriptions.resize(Size-1);
            Boolian_Values.resize(Size-1);
        }
        else if (Index_String!=-1)
        {
            // re-locate the end:
            Int_t Size = (Int_t) String_Descriptions.size();
            
            String_Descriptions.at(Index_Double) = String_Descriptions.at(Size-1);
            String_Values.at(Index_Double) = String_Values.at(Size-1);
            
            String_Descriptions.resize(Size-1);
            String_Values.resize(Size-1);
        }
        else
        {
            ErrorMessage("The input <"+Description+"> could not be located in the .txt-file!");
        }
        
        // next, write this to the file:
        WriteFile();
        
        // Done.
    }
    else
    {
        ErrorMessage("The class was not linked to a .txt-file!");
    }
}
            
