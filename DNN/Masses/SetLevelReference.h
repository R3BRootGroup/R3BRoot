void Nucleus::SetLevelReference(Int_t const Key, TString const Refs)
{
    // Adds an NNDC reference-value to an existing energy level of a nucleus.
    // First, Check that this key exists:
    Int_t ComparisonCounter = 0;
    Int_t Size = (Int_t) Levels.size();
    Int_t Index = -1;
    
    if (Size>0)
    {
        for (Int_t k = 0; k<Size; ++k)
        {
            if (Levels.at(k).Key==Key) 
            {
                ComparisonCounter = ComparisonCounter + 1;
                Index = k;
            }
        }
    }
    
    if (ComparisonCounter!=1)
    {
        cout << "NUCEAR LEVEL ERROR: This energy level does not exist!\n\n";
    }
    else
    {
        Levels.at(Index).References = Levels.at(Index).References + Refs;
    }
}

// ==============================================================================

void Nucleus::ClearLevelReference(Int_t const Key)
{
    // Cleans all reference-values of an existing energy level of a nucleus.
    // First, Check that this key exists:
    Int_t ComparisonCounter = 0;
    Int_t Size = (Int_t) Levels.size();
    Int_t Index = -1;
    
    if (Size>0)
    {
        for (Int_t k = 0; k<Size; ++k)
        {
            if (Levels.at(k).Key==Key) 
            {
                ComparisonCounter = ComparisonCounter + 1;
                Index = k;
            }
        }
    }
    
    if (ComparisonCounter!=1)
    {
        cout << "NUCEAR LEVEL ERROR: This energy level does not exist!\n\n";
    }
    else
    {
        Levels.at(Index).References = "";
    }
}