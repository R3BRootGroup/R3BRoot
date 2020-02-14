Nucleus C13()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("C13");
    ThisNuc.SetZ(6);
    ThisNuc.SetA(13);
    ThisNuc.SetLifeTime("sec",1e10);
    ThisNuc.SetAbundance("Dimless",0.0107);
    ThisNuc.SetBindingEnergy("PerN","MeV",7.469849);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.SetLevelReference(Key,"ABGHIJLMNQRSVWXYZabcdefghijklmnopqr"); ThisNuc.AddLevelAngMom(Key,0.5,"-",kTRUE); ThisNuc.SetLevelLifeTime(Key,1e99,"year");
    
    return ThisNuc;
}
