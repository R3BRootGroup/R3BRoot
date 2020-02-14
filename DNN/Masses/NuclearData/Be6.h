Nucleus Be6()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Be6");
    ThisNuc.SetZ(4);
    ThisNuc.SetA(6);
    ThisNuc.SetLifeTime("ns",0.001);
    ThisNuc.SetAbundance("Dimless",0.0);
    ThisNuc.SetBindingEnergy("PerN","MeV",4.4872);
    
    // Add the most important states:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV");    ThisNuc.SetLevelReference(Key,"BCD"); ThisNuc.AddLevelAngMom(Key,0.0,"+",kTRUE);
    
    return ThisNuc;
}
