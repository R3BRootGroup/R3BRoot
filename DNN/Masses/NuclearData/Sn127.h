Nucleus Sn127()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Sn127");
    ThisNuc.SetZ(50);
    ThisNuc.SetA(127);
    ThisNuc.SetLifeTime("min",4.13);
    ThisNuc.SetAbundance("Dimless",0.0);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.42056);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.AddLevelAngMom(Key,1.5,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,4.13,"min");
    
    return ThisNuc;
}
