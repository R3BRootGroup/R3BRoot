Nucleus Sb132()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Sb132");
    ThisNuc.SetZ(51);
    ThisNuc.SetA(132);
    ThisNuc.SetLifeTime("min",2.79);
    ThisNuc.SetAbundance("Dimless",0.0);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.372344);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.AddLevelAngMom(Key,4.0,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,2.79,"min");
    
    return ThisNuc;
}
