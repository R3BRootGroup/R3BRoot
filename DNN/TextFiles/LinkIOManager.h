void R3BTextFileGenerator::LinkIOManager(FairRootManager* io)
{
    // Links the FairRootManager* ioman to an external FairRootManager*.
    
    // Set default value:
    IO_Manager_IsLinked = kFALSE;
    
    // Begin by testing whether the fArray is null-pointer:
    if ((io==0)||(io==nullptr))
    {
        ErrorMessage("You tried to link ioman to an empty pointer!");
    }
    else
    {
        // Then, we will assume that it worked:
        ioman = io;
        IO_Manager_IsLinked = kTRUE;
    }
}
