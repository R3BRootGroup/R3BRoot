Nucleus Au197()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Au197");
    ThisNuc.SetZ(79);
    ThisNuc.SetA(197);
    ThisNuc.SetLifeTime("sec",1e10);
    ThisNuc.SetAbundance("Dimless",1.0);
    ThisNuc.SetBindingEnergy("PerN","MeV",7.9156603);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.SetLevelReference(Key,"ABCDEFGHIJKLM"); ThisNuc.AddLevelAngMom(Key,1.5,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,1e99,"year");
    return ThisNuc;
}