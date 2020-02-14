Nucleus Sn126()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Sn126");
    ThisNuc.SetZ(50);
    ThisNuc.SetA(126);
    ThisNuc.SetLifeTime("year",2.3e5);
    ThisNuc.SetAbundance("Dimless",0.0);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.44352);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.AddLevelAngMom(Key,0.0,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,2.3e5,"year");
    
    return ThisNuc;
}
