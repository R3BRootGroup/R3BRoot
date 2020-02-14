void R3BTextFileGenerator::SetBoolianOutputs(TString const Option)
{
    // Defines the numbers for yes/no outputs:
    
    if (Option=="NormalSigmoid") 
    {
        Output_ISYES = 1.0;
        Output_ISNO = 0.0;
    }
    else if (Option=="SymmetricSigmoid") 
    {
        Output_ISYES = 1.0;
        Output_ISNO = -1.0;
    }
    else
    {
        Output_ISYES = 1.0;
        Output_ISNO = 0.0;
    }
}

void R3BTextFileGenerator::SetBoolianInputs(TString const Option)
{
    // Defines the numbers for yes/no outputs:
    
    if (Option=="NormalSigmoid") 
    {
        Input_ISYES = 1.0;
        Input_ISNO = 0.0;
    }
    else if (Option=="SymmetricSigmoid") 
    {
        Input_ISYES = 1.0;
        Input_ISNO = -1.0;
    }
    else
    {
        Input_ISYES = 1.0;
        Input_ISNO = 0.0;
    }
}
