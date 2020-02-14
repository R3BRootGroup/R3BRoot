Nucleus Cd110()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Cd110");
    ThisNuc.SetZ(48);
    ThisNuc.SetA(110);
    ThisNuc.SetLifeTime("sec",1e10);
    ThisNuc.SetAbundance("Dimless",0.1249);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.5512825);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.SetLevelReference(Key,"ABCDEFGHIJKLMNPQRS"); ThisNuc.AddLevelAngMom(Key,0,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,1e99,"year");
    
    return ThisNuc;
}
