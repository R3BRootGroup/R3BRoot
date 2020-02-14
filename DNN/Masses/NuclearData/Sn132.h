Nucleus Sn132()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Sn132");
    ThisNuc.SetZ(50);
    ThisNuc.SetA(132);
    ThisNuc.SetLifeTime("sec",39.7);
    ThisNuc.SetAbundance("Dimless",0.0);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.354872);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.AddLevelAngMom(Key,0,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,39.7,"sec");
    
    return ThisNuc;
}
