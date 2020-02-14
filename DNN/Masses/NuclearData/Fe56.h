Nucleus Fe56()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Fe56");
    ThisNuc.SetZ(26);
    ThisNuc.SetA(56);
    ThisNuc.SetLifeTime("sec",1e10);
    ThisNuc.SetAbundance("Dimless",0.91754);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.790342);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.SetLevelReference(Key,"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefg"); ThisNuc.AddLevelAngMom(Key,0,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,1e99,"year");
    
    return ThisNuc;
}