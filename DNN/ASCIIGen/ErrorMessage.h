void R3BASCIIFileGenerator::ErrorMessage(TString const Message)
{
    TString Output = "### ASCII GENERATOR ERROR ###: " + Message + "\n";
    if (PrintErrors==kTRUE) {cout << Output.Data();}
    Errors.push_back(Output);
}

void R3BASCIIFileGenerator::PrintAllErrors()
{
    for (Int_t k = 0; k<Errors.size(); ++k)
    {
        cout << Errors.at(k).Data();
    }
}

void R3BASCIIFileGenerator::DisableErrorPrinting()
{
    PrintErrors = kFALSE;
}

void R3BASCIIFileGenerator::EnableErrorPrinting()
{
    PrintErrors = kTRUE;
}
    
Bool_t R3BASCIIFileGenerator::ContainsNoErrors()
{
    if (Errors.size()==0) {return kTRUE;}
    else {return kFALSE;}
}
