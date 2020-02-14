void ChangeInputs()
{
    // User control over the Inputs:
    R3BInputClass* Inputs = new R3BInputClass();
    Inputs->DisableErrorPrinting();
    Inputs->LinkFile("../InputFiles/InputsFile.txt");

    if (Inputs->IsLinked()==kFALSE)
    {
        cout << "### FATAL: Inputs-file could not be found!\n"
             << "### FATAL: New default-file was generated!\n\n";
        Inputs->CreateFile("../InputFiles/InputsFile.txt");
    }
    else
    {
        Inputs->ReadFile();
        cout << "\n\n";
        
        cout << "Inputs-file was located.\n\n";
        
        cout << "Do you want an overview of all inputs [y/n]:";
        TString Answer1 = "";
        cin >> Answer1;
        if (Answer1=="y") {Inputs->Print(kTRUE);}
        
        cout << "Do you want to add a new input [y/n]:";
        TString Answer2 = "";
        cin >> Answer2;
        if (Answer2=="y")
        {
            cout << "What type of input?\n"
                 << "1) Double_t\n"
                 << "2) Int_t\n"
                 << "3) Bool_t\n"
                 << "4) TString\n"
                 << "==> Answer [1/2/3/4]:";
                 Int_t Answer3 = 0;
                 cin >> Answer3;
                 
                 if (Answer3==1)
                 {
                     cout << "Give Description: ";
                     TString Answer4 = "";
                     cin >> Answer4;
                     cout << "Give Unit: ";
                     TString Answer5 = "";
                     cin >> Answer5;
                     cout << "Give Value: ";
                     Double_t Answer6 = 0.0;
                     cin >> Answer6;
                     Inputs->AddInputDouble(Answer4,Answer5,Answer6);
                 }
                 else if (Answer3==2)
                 {
                     cout << "Give Description: ";
                     TString Answer7 = "";
                     cin >> Answer7;
                     cout << "Give Value: ";
                     Int_t Answer8 = 0;
                     cin >> Answer8;
                     Inputs->AddInputInteger(Answer7,Answer8);
                 }
                 else if (Answer3==3)
                 {
                     cout << "Give Description: ";
                     TString Answer9 = "";
                     cin >> Answer9;
                     cout << "Give Value [kTRUE/kFALSE]: ";
                     TString Answer10 = "";
                     cin >> Answer10;
                     Bool_t BoolValue = kTRUE;
                     if (Answer10=="kTRUE") {BoolValue = kTRUE;}
                     else if (Answer10=="kFALSE") {BoolValue = kFALSE;}
                     else {Inputs->ErrorMessage("Boolian input value was NOT properly specified!");}
                     Inputs->AddInputBoolian(Answer9,BoolValue);
                 }
                 else if (Answer3==4)
                 {
                     cout << "Give Description: ";
                     TString Answer11 = "";
                     cin >> Answer11;
                     cout << "Give Value: ";
                     TString Answer12 = "";
                     cin >> Answer12;
                     Inputs->AddInputString(Answer11,Answer12);
                 }
                 else
                 {
                     Inputs->ErrorMessage("The type of input you wanted to change was not specified!");
                 }
        }
        
        cout << "Do you want to modify an existing input [y/n]:";
        TString Answer13 = "";
        cin >> Answer13;
        if (Answer13=="y")
        {
            cout << "What type of input?\n"
                 << "1) Double_t\n"
                 << "2) Int_t\n"
                 << "3) Bool_t\n"
                 << "4) TString\n"
                 << "==> Answer [1/2/3/4]:";
                 Int_t Answer14 = 0;
                 cin >> Answer14;
                 
                 if (Answer14==1)
                 {
                     cout << "Give Existing Description: ";
                     TString Answer15 = "";
                     cin >> Answer15;
                     cout << "Give New Unit: ";
                     TString Answer16 = "";
                     cin >> Answer16;
                     cout << "Give New Value: ";
                     Double_t Answer17 = 0.0;
                     cin >> Answer17;
                     Inputs->ModifyDouble(Answer15,Answer16,Answer17);
                 }
                 else if (Answer14==2)
                 {
                     cout << "Give Existing Description: ";
                     TString Answer18 = "";
                     cin >> Answer18;
                     cout << "Give New Value: ";
                     Int_t Answer19 = 0;
                     cin >> Answer19;
                     Inputs->ModifyInteger(Answer18,Answer19);
                 }
                 else if (Answer14==3)
                 {
                     cout << "Give Existing Description: ";
                     TString Answer20 = "";
                     cin >> Answer20;
                     cout << "Give New Value [kTRUE/kFALSE]: ";
                     TString Answer21 = "";
                     cin >> Answer21;
                     Bool_t BoolValue = kTRUE;
                     if (Answer21=="kTRUE") {BoolValue = kTRUE;}
                     else if (Answer21=="kFALSE") {BoolValue = kFALSE;}
                     else {Inputs->ErrorMessage("Boolian input value was NOT properly specified!");}
                     Inputs->ModifyBoolian(Answer20,BoolValue);
                 }
                 else if (Answer14==4)
                 {
                     cout << "Give Existing Description: ";
                     TString Answer22 = "";
                     cin >> Answer22;
                     cout << "Give New Value: ";
                     TString Answer23 = "";
                     cin >> Answer23;
                     Inputs->ModifyString(Answer22,Answer23);
                 }
                 else
                 {
                     Inputs->ErrorMessage("The type of input you wanted to change was not specified!");
                 }
        }
        
        cout << "Do you want to remove an input [y/n]:";
        TString Answer24 = "";
        cin >> Answer24;
        if (Answer24=="y")
        {
            cout << "Give Existing Description: ";
            TString Answer25 = "";
            cin >> Answer25;
            Inputs->EraseInput(Answer25);
        }
        
        Inputs->WriteFile();
        cout << "\n\n ### ALL REQUESTS PROCESSED ### \n\n";
        Inputs->Print(kTRUE);
        cout << "\n\n ### RERUN PROGRAM TO MAKE OTHER MODIFICATIONS ### \n\n";
    }
        
    // Print error log in the end:
    Inputs->PrintAllErrors();
    delete Inputs;
}
