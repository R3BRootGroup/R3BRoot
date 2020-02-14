void R3BInputClass::LinkFile(TString const Name)
{
    // Links the .txt-file to the Spectrum Class.
    
    // Set the default:
    Linked = kFALSE;
    NLines = 0;
    
    // Pass the input to the class:
    FileName = Name;
    
    // Next, open the file:
    ifstream TheFile;
    TheFile.open(FileName.Data());
    
    // Test if it worked:
    if (TheFile.is_open()==kTRUE)
    {
        // Then, we get the number of lines:
        std::string ThisLine;
        while (std::getline(TheFile,ThisLine)) {NLines = NLines + 1;}
        
        // And, then we close the file again:
        TheFile.close();
        
        // But note that this FileName worked:
        Linked = kTRUE;
    }
    else
    {
        ErrorMessage("The file you wanted to link to does not exist!");
    }
    
    // done.
}
    
