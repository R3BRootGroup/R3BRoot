void R3BTradMethClass::ErrorMessage(TString const Message)
{
    TString Output = "### TRAD. METHOD ERROR ###: " + Message + "\n";
    if (PrintErrors==kTRUE) {cout << Output.Data();}
    Errors.push_back(Output);
}

void R3BTradMethClass::PrintAllErrors()
{
    for (Int_t k = 0; k<Errors.size(); ++k)
    {
        cout << Errors.at(k).Data();
    }
}

void R3BTradMethClass::DisableErrorPrinting()
{
    PrintErrors = kFALSE;
}

void R3BTradMethClass::EnableErrorPrinting()
{
    PrintErrors = kTRUE;
}
    
Bool_t R3BTradMethClass::ContainsNoErrors()
{
    if (Errors.size()==0) {return kTRUE;}
    else {return kFALSE;}
}
