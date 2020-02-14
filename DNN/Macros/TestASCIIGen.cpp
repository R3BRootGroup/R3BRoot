void TestASCIIGen()
{
    // This macro tests the generation of an ASCII-file 
    // for the MC transport simulation.
    
    // Begin by creating an inputs class:
    R3BInputClass* Inputs = new R3BInputClass();
    Inputs->DisableErrorPrinting();
    Inputs->LinkFile("../InputFiles/InputsFile.txt");
    Inputs->ReadFile();
    Inputs->CheckPhysicsList();

    // Next, create an ASCII generator:
    R3BASCIIFileGenerator* ASCIIGen = new R3BASCIIFileGenerator();
    ASCIIGen->DisableErrorPrinting();
    ASCIIGen->LinkInputClass(Inputs);
    ASCIIGen->ObtainParticleGun();
    ASCIIGen->SetFileName(Inputs->GetInputString("TheOutputPath") + "ASCII_TestFile.dat"); // Not needed. Inputs also contains this.
    ASCIIGen->SetnEvents(20);                                                              // Not needed. Inputs also contains this.
    ASCIIGen->CreateFile();
        
    // Print error log in the end:
    Inputs->PrintAllErrors();
    delete Inputs;
    
    ASCIIGen->PrintAllErrors();
    delete ASCIIGen;
}
