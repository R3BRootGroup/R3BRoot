Nucleus Sn128()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Sn128");
    ThisNuc.SetZ(50);
    ThisNuc.SetA(128);
    ThisNuc.SetLifeTime("min",59.07);
    ThisNuc.SetAbundance("Dimless",0.0);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.41698);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.AddLevelAngMom(Key,0.0,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,59.07,"min");
    
    return ThisNuc;
}
