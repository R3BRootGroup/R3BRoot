void Nucleus::AddEnergyLevel(Int_t const Key, Double_t const Energy, TString const Unit)
{
    // Adds an energy level to the nucleus with given units. First, compare that this
    // level key does not exist yet:
    Int_t ComparisonCounter = 0;
    Int_t Size = (Int_t) Levels.size();
    
    if (Size>0)
    {
        for (Int_t k = 0; k<Size; ++k)
        {
            if (Levels.at(k).Key==Key) {ComparisonCounter = ComparisonCounter + 1;}
        }
    }
    
    if (ComparisonCounter!=0)
    {
        cout << "NUCEAR LEVEL ERROR: You tried to overwrite an existing energy level! Key = " << Key << "\n\n";
    }
    else
    {
        // Create the new level:
        EnergyLevel NewLevel;
        
        // Set the key:
        NewLevel.Key = Key;
        
        // Set the energy:
        if (Unit=="MeV") {NewLevel.Energy = Energy;}
        else if (Unit=="keV") {NewLevel.Energy = Energy/1000.0;}
        else if (Unit=="J") {NewLevel.Energy = Energy/PhysConst.q_MeVJ;}
        else 
        {
            cout << "NUCEAR LEVEL ERROR: The unit <"<<Unit<<"> was not supported by Nucleus.AddEnergyLevel()!\n\n";
            NewLevel.Energy = -1.0;
        }
        
        // Defaults for the rest:
        NewLevel.LifeTime = -1.0;
        NewLevel.BGT = -1.0;
        NewLevel.BF = -1.0;
        NewLevel.BGT_spec = kFALSE;
        NewLevel.BF_spec = kFALSE;
        NewLevel.References = "";
        NewLevel.AngMom.clear();
        NewLevel.Gamma.clear();
        
        // And add it to the scheme:
        Levels.push_back(NewLevel);
    }
}