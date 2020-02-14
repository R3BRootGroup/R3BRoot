Nucleus C12()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("C12");
    ThisNuc.SetZ(6);
    ThisNuc.SetA(12);
    ThisNuc.SetLifeTime("sec",1e10);
    ThisNuc.SetAbundance("Dimless",0.9893);
    ThisNuc.SetBindingEnergy("PerN","MeV",7.680144);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.SetLevelReference(Key,"ABCDEFGHILMNOPQRSTUVWXYZabcdefghij"); ThisNuc.AddLevelAngMom(Key,0,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,1e99,"year");
    
    return ThisNuc;
}
