void GenerateSAMURAIInputs()
{
    // Simply create an Inputs-file:
    R3BInputClass* Inputs = new R3BInputClass();
    Inputs->DisableErrorPrinting();
    Inputs->CreateSAMURAIFile("../InputFiles/InputsFile.txt");
    Inputs->PrintAllErrors();
    delete Inputs;
}
