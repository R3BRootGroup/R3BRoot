Nucleus Cd106()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Cd106");
    ThisNuc.SetZ(48);
    ThisNuc.SetA(106);
    ThisNuc.SetLifeTime("sec",1e10);
    ThisNuc.SetAbundance("Dimless",0.0125);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.53904710);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.SetLevelReference(Key,"ABCDEFGHIJK"); ThisNuc.AddLevelAngMom(Key,0,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,1e99,"year");
    
    return ThisNuc;
}