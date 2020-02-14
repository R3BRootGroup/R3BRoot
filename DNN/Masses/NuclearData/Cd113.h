Nucleus Cd113()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Cd113");
    ThisNuc.SetZ(48);
    ThisNuc.SetA(113);
    ThisNuc.SetLifeTime("sec",1e10);
    ThisNuc.SetAbundance("Dimless",0.1222);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.5269864);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.SetLevelReference(Key,"ABCDEFGHIJKLMNO"); ThisNuc.AddLevelAngMom(Key,0.5,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,8.0e15,"year");

    return ThisNuc;
}
