void Nucleus::SetLevelBF(Int_t const Key, Double_t const BF)
{
    // Adds a BF-value to an existing energy level of a nucleus with given units. 
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
        Levels.at(Index).BF = BF;
        Levels.at(Index).BF_spec = kTRUE;
    }
}