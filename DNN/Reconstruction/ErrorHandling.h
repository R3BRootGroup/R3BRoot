// Error messages:
void AllScorers::ErrorMessage(TString const Message)
{
    TString Output = "### SCORINGCLASS ERROR ###: " + Message + "\n";
    if (PrintErrors==kTRUE) {cout << Output.Data();}
    Errors.push_back(Output);
}

void AllScorers::PrintAllErrors()
{
    for (Int_t k = 0; k<Errors.size(); ++k)
    {
        cout << Errors.at(k).Data();
    }
}

void AllScorers::DisableErrorPrinting()
{
    PrintErrors = kFALSE;
}

void AllScorers::EnableErrorPrinting()
{
    PrintErrors = kTRUE;
}
    
Bool_t AllScorers::ContainsNoErrors()
{
    if (Errors.size()==0) {return kTRUE;}
    else {return kFALSE;}
}


