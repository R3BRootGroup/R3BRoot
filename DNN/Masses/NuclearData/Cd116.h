Nucleus Cd116()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Cd116");
    ThisNuc.SetZ(48);
    ThisNuc.SetA(116);
    ThisNuc.SetLifeTime("sec",1e10);
    ThisNuc.SetAbundance("Dimless",0.0749);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.512350610);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.AddLevelAngMom(Key,0,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,3.3e19,"year");

    return ThisNuc;
}