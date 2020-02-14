Nucleus Sn131()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Sn131");
    ThisNuc.SetZ(50);
    ThisNuc.SetA(131);
    ThisNuc.SetLifeTime("sec",56.0);
    ThisNuc.SetAbundance("Dimless",0.0);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.36252);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.AddLevelAngMom(Key,1.5,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,56.0,"sec");
    
    return ThisNuc;
}
