Nucleus Be8()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Be8");
    ThisNuc.SetZ(4);
    ThisNuc.SetA(8);
    ThisNuc.SetLifeTime("year",1e10);
    ThisNuc.SetAbundance("Dimless",0.0);
    ThisNuc.SetBindingEnergy("PerN","MeV",7.062435);
    
    // Add the most important states:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV");    ThisNuc.SetLevelReference(Key,"ABCDEHIJKLPQRSUVWYZabcdef"); ThisNuc.AddLevelAngMom(Key,0.0,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,1e10,"year");
    
    return ThisNuc;
}
