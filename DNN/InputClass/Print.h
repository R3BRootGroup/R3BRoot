void R3BInputClass::Print(Bool_t const PrintContent = kFALSE)
{
    // Default printing function:
    cout << "\n\n---------------------------\n";
    cout << "==> Inputs Class Content:\n";
    cout << "Class is linked: ";
        if (Linked==kTRUE) {cout << "YES";}
        else {cout << "NO ";}
    cout << "\n";
    cout << "Name of the linked file: " << FileName << "\n";
    cout << "Number of lines in the file: " << NLines << "\n";
    cout << "Errors are printed on-the-fly: ";
        if (PrintErrors==kTRUE) {cout << "YES";}
        else {cout << "NO";}
    cout << "\n";
    cout << "---------------------------\n";
    
    // Print class input parameters, if we want them:
    if (PrintContent==kTRUE)
    {
    cout << "===========================\n";
    cout << "---------------------------\n";
    cout << "==> Double_t type Inputs:\n";
    for (Int_t k = 0; k<Double_Descriptions.size(); ++k)
    {
        cout << Double_Descriptions.at(k) << " = " << Double_Values.at(k) << " [" + Double_Units.at(k) + "]\n";
    }
    cout << "---------------------------\n";
    cout << "==> Int_t type Inputs:\n";
    for (Int_t k = 0; k<Integer_Descriptions.size(); ++k)
    {
        cout << Integer_Descriptions.at(k) << " = " << Integer_Values.at(k) << "\n";
    }
    cout << "---------------------------\n";
    cout << "==> TString type Inputs:\n";
    for (Int_t k = 0; k<String_Descriptions.size(); ++k)
    {
        cout << String_Descriptions.at(k) << " = " << String_Values.at(k) << "\n";
    }
    cout << "---------------------------\n";
    cout << "==> Bool_t type Inputs:\n";
    for (Int_t k = 0; k<Boolian_Descriptions.size(); ++k)
    {
        cout << Boolian_Descriptions.at(k) << " = ";
        if (Boolian_Values.at(k)==kTRUE) {cout << "kTRUE";}
        else {cout << "kFALSE";}
        cout << "\n";
    }
    cout << "---------------------------\n";
    }
    
    // End with a few white lines:
    cout << "\n\n";
}
    
