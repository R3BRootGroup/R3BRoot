void CrossSections_7LiPN::ErrorMessage(TString const Message)
{
    TString Output = "### 7Li(p,n)7Be CROSS SECTION ERROR ###: " + Message + "\n";
    if (PrintErrors==kTRUE) {cout << Output.Data();}
    Errors.push_back(Output);
}

void CrossSections_7LiPN::PrintAllErrors()
{
    for (Int_t k = 0; k<Errors.size(); ++k)
    {
        cout << Errors.at(k).Data();
    }
}

void CrossSections_7LiPN::DisableErrorPrinting()
{
    PrintErrors = kFALSE;
}

void CrossSections_7LiPN::EnableErrorPrinting()
{
    PrintErrors = kTRUE;
}
    
Bool_t CrossSections_7LiPN::ContainsNoErrors()
{
    if (Errors.size()==0) {return kTRUE;}
    else {return kFALSE;}
}
