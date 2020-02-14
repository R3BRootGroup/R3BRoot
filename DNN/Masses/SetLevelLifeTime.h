void Nucleus::SetLevelLifeTime(Int_t const Key, Double_t const Time, TString const Unit)
{
    // Adds a lifetime to an existing energy level of a nucleus with given units. 
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
        if (Unit=="sec") {Levels.at(Index).LifeTime = Time;}
        else if (Unit=="s") {Levels.at(Index).LifeTime = Time;}
        else if (Unit=="ms") {Levels.at(Index).LifeTime = Time/1000.0;}
        else if (Unit=="fs") {Levels.at(Index).LifeTime = Time/1e15;}
        else if (Unit=="us") {Levels.at(Index).LifeTime = Time/1e6;}
        else if (Unit=="ns") {Levels.at(Index).LifeTime = Time/1e9;}
        else if (Unit=="ps") {Levels.at(Index).LifeTime = Time/1e12;}
        else if (Unit=="min") {Levels.at(Index).LifeTime = Time*60.0;}
        else if (Unit=="hour") {Levels.at(Index).LifeTime = Time*60.0*60.0;}
        else if (Unit=="day") {Levels.at(Index).LifeTime = Time*60.0*60.0*24.0;}
        else if (Unit=="week") {Levels.at(Index).LifeTime = Time*60.0*60.0*24.0*7.0;}
        else if (Unit=="month") {Levels.at(Index).LifeTime = Time*60.0*60.0*24.0*365.0/12.0;}
        else if (Unit=="year") {Levels.at(Index).LifeTime = Time*60.0*60.0*24.0*365.0;}
        else 
        {
            cout << "NUCEAR LEVEL ERROR: The unit <"<<Unit<<"> was not supported by Nucleus.SetLevelLifeTime()!\n\n";
        }
    }
}
