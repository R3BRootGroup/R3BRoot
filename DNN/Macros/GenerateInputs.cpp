void GenerateInputs()
{
    // Simply create an Inputs-file:
    R3BInputClass* Inputs = new R3BInputClass();
    Inputs->DisableErrorPrinting();
    Inputs->CreateFile("../InputFiles/InputsFile.txt");
    Inputs->PrintAllErrors();
    delete Inputs;
}
