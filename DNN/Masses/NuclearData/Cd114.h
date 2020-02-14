Nucleus Cd114()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Cd114");
    ThisNuc.SetZ(48);
    ThisNuc.SetA(114);
    ThisNuc.SetLifeTime("sec",1e10);
    ThisNuc.SetAbundance("Dimless",0.2873);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.5315124);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.SetLevelReference(Key,"ABCDEFGHIJKLMNOPQ"); ThisNuc.AddLevelAngMom(Key,0,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,1e99,"year");
    
    return ThisNuc;
}
        