void R3B_DNN_Plots::ErrorMessage(TString const Message)
{
    TString Output = "### PLOTS-CLASS ERROR ###: " + Message + "\n";
    if (PrintErrors==kTRUE) {cout << Output.Data();}
    Errors.push_back(Output);
}

void R3B_DNN_Plots::PrintAllErrors()
{
    for (Int_t k = 0; k<Errors.size(); ++k)
    {
        cout << Errors.at(k).Data();
    }
}

void R3B_DNN_Plots::DisableErrorPrinting()
{
    PrintErrors = kFALSE;
}

void R3B_DNN_Plots::EnableErrorPrinting()
{
    PrintErrors = kTRUE;
}
    
Bool_t R3B_DNN_Plots::ContainsNoErrors()
{
    if (Errors.size()==0) {return kTRUE;}
    else {return kFALSE;}
}
