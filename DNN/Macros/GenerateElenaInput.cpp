void GenerateElenaInput()
{
    // Simply create an Elena Inputs-file:
    R3BInputClass* Inputs = new R3BInputClass();
    Inputs->DisableErrorPrinting();
    Inputs->CreateElenaFile("../InputFiles/InputsFile.txt");
    Inputs->PrintAllErrors();
    delete Inputs;
}
