Int_t Nucleus::GetNElevels()
{
    Int_t Size = (Int_t) Levels.size();
    return Size;
}

EnergyLevel Nucleus::GetEnergyLevel(Int_t const Index)
{
    // Returns an energy level according to its index.
    Int_t Size = (Int_t) Levels.size();
    EnergyLevel Answer;
    
    if ((Index>=0)&&(Index<Size))
    {
        Answer = Levels.at(Index);
    }
    else
    {
        cout << "NUCLEAR LEVEL ERROR: You asked for a level with the wrong index!\n\n";
    }
    
    return Answer;
}