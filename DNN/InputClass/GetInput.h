Double_t R3BInputClass::GetInputDouble(TString const Description, TString const Unit)
{
    // Retrieves a Double_t input:
    Double_t Answer = 0.0;
    
    // begin with identifying the index of the description:
    Int_t Index = -1;
    
    for (Int_t k = 0; k<Double_Descriptions.size(); ++k)
    {
        if (Double_Descriptions.at(k)==Description)
        {
            Index = k;
        }
    }
    
    // Check if the input exists:
    if (Index==-1)
    {
        ErrorMessage("The Double_t-input <"+Description+"> is not present in the class!");
    }
    else
    {
        // Next, as an extra security check, make sure that the units match:
        if (Double_Units.at(Index)==Unit)
        {
            // Then, we are OK:
            Answer = Double_Values.at(Index);
        }
        else
        {
            // Then, the unit is incorrect:
            ErrorMessage("The Double_t-input <"+Description+"> does not have a unit ["+Unit+"]!");
        }
    }
    
    // Finally, return the answer:
    return Answer;
}

Int_t R3BInputClass::GetInputInteger(TString const Description)
{
    // Retrieves an Int_t input:
    Int_t Answer = 0;
    
    // begin with identifying the index of the description:
    Int_t Index = -1;
    
    for (Int_t k = 0; k<Integer_Descriptions.size(); ++k)
    {
        if (Integer_Descriptions.at(k)==Description)
        {
            Index = k;
        }
    }
    
    // Check if the input exists:
    if (Index==-1)
    {
        ErrorMessage("The Int_t-input <"+Description+"> is not present in the class!");
    }
    else
    {
        // Then, we can get the value:
        Answer = Integer_Values.at(Index);
    }
    
    // Finally, return the answer:
    return Answer;
}

Bool_t R3BInputClass::GetInputBoolian(TString const Description)
{
    // Retrieves a Bool_t input:
    Bool_t Answer = kFALSE;
    
    // begin with identifying the index of the description:
    Int_t Index = -1;
    
    for (Int_t k = 0; k<Boolian_Descriptions.size(); ++k)
    {
        if (Boolian_Descriptions.at(k)==Description)
        {
            Index = k;
        }
    }
    
    // Check if the input exists:
    if (Index==-1)
    {
        ErrorMessage("The Bool_t-input <"+Description+"> is not present in the class!");
    }
    else
    {
        // Then, we can get the value:
        Answer = Boolian_Values.at(Index);
    }
    
    // Finally, return the answer:
    return Answer;
}

TString R3BInputClass::GetInputString(TString const Description)
{
    // Retrieves an Int_t input:
    TString Answer = "";
    
    // begin with identifying the index of the description:
    Int_t Index = -1;
    
    for (Int_t k = 0; k<String_Descriptions.size(); ++k)
    {
        if (String_Descriptions.at(k)==Description)
        {
            Index = k;
        }
    }
    
    // Check if the input exists:
    if (Index==-1)
    {
        ErrorMessage("The TString-input <"+Description+"> is not present in the class!");
    }
    else
    {
        // Then, we can get the value:
        Answer = String_Values.at(Index);
    }
    
    // Finally, return the answer:
    return Answer;
}
