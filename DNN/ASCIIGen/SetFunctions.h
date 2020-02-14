void R3BASCIIFileGenerator::SetFileName(TString const Name)
{
    // Test that name contains a valid ASCII-name:
    if (Name.Contains(".dat")==kFALSE)
    {
        ErrorMessage("The user-specified filename does not contain <.dat>!");
    }
    else
    {
        ifstream TestFile_1;
        TestFile_1.open(Name.Data());

        if (TestFile_1.is_open()==kTRUE) 
        {
            // Then, this file exists. So it is a valid path-name.
            TestFile_1.close();
            FileName = Name;
        }
        else
        {
            // Then, try to create the file, to see if it is a valid path name:
            std::ofstream TestFile_2 (Name.Data(), std::ofstream::out);
            TestFile_2.close();
    
            ifstream TestFile_3;
            TestFile_3.open(Name.Data());
            
            if (TestFile_3.is_open()==kTRUE)
            {
                // Then, we are still OK:
                TestFile_3.close();
                FileName = Name;
            }
            else
            {
                // Then, Name is NOT a valid path name:
                ErrorMessage("The TString <"+Name+"> does NOT contain a valid path+filename!");
            }
        }
    }
}

void R3BASCIIFileGenerator::SetnEvents(Int_t const nn)
{
    if (nn>0)
    {
        R3BRoot_nEvents = nn;
    }
    else
    {
        ErrorMessage("If you re-specify the number of events, it should be positive!");
    }
}

void R3BASCIIFileGenerator::SetProfile(TString const Option)
{
    RandomizeOption = Option;
}
