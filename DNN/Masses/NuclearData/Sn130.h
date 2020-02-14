Nucleus Sn130()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Sn130");
    ThisNuc.SetZ(50);
    ThisNuc.SetA(130);
    ThisNuc.SetLifeTime("min",3.72);
    ThisNuc.SetAbundance("Dimless",0.0);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.386816);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.AddLevelAngMom(Key,0.0,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,3.72,"min");
    
    return ThisNuc;
}
