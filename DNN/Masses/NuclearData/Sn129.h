Nucleus Sn129()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Sn129");
    ThisNuc.SetZ(50);
    ThisNuc.SetA(129);
    ThisNuc.SetLifeTime("min",2.23);
    ThisNuc.SetAbundance("Dimless",0.0);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.39282);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.AddLevelAngMom(Key,1.5,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,2.23,"min");
    
    return ThisNuc;
}
