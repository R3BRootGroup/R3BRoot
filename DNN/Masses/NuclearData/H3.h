Nucleus H3()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("H3");
    ThisNuc.SetZ(1);
    ThisNuc.SetA(3);
    ThisNuc.SetLifeTime("sec",12.32*365.0*24.0*3600.0);
    ThisNuc.SetAbundance("Dimless",0.0);
    ThisNuc.SetBindingEnergy("PerN","MeV",2.827266110);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.SetLevelReference(Key,"AB"); ThisNuc.AddLevelAngMom(Key,0.5,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,12.32,"year");
    
    return ThisNuc;
}