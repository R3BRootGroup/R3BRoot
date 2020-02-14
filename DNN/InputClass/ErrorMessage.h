void R3BInputClass::ErrorMessage(TString const Message)
{
    TString Output = "### INPUTS ERROR ###: " + Message + "\n";
    if (PrintErrors==kTRUE) {cout << Output.Data();}
    Errors.push_back(Output);
}

void R3BInputClass::PrintAllErrors()
{
    for (Int_t k = 0; k<Errors.size(); ++k)
    {
        cout << Errors.at(k).Data();
    }
}

void R3BInputClass::DisableErrorPrinting()
{
    PrintErrors = kFALSE;
}

void R3BInputClass::EnableErrorPrinting()
{
    PrintErrors = kTRUE;
}
    
Bool_t R3BInputClass::ContainsNoErrors()
{
    if (Errors.size()==0) {return kTRUE;}
    else {return kFALSE;}
}
