void R3BTextFileGenerator::ErrorMessage(TString const Message)
{
    TString Output = "### TEXTFILE GENERATOR ERROR ###: " + Message + "\n";
    if (PrintErrors==kTRUE) {cout << Output.Data();}
    Errors.push_back(Output);
}

void R3BTextFileGenerator::PrintAllErrors()
{
    for (Int_t k = 0; k<Errors.size(); ++k)
    {
        cout << Errors.at(k).Data();
    }
}

void R3BTextFileGenerator::DisableErrorPrinting()
{
    PrintErrors = kFALSE;
}

void R3BTextFileGenerator::EnableErrorPrinting()
{
    PrintErrors = kTRUE;
}
    
Bool_t R3BTextFileGenerator::ContainsNoErrors()
{
    if (Errors.size()==0) {return kTRUE;}
    else {return kFALSE;}
}
