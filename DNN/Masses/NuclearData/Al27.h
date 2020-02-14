Nucleus Al27()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Al27");
    ThisNuc.SetZ(13);
    ThisNuc.SetA(27);
    ThisNuc.SetLifeTime("sec",1e10);
    ThisNuc.SetAbundance("Dimless",1.0);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.3315484);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.SetLevelReference(Key,"ABCEFGHIJKL"); ThisNuc.AddLevelAngMom(Key,5.5,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,1e99,"year");
    
    return ThisNuc;
}
