Nucleus Be7()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Be7");
    ThisNuc.SetZ(4);
    ThisNuc.SetA(7);
    ThisNuc.SetLifeTime("day",53.22);
    ThisNuc.SetAbundance("Dimless",0.0);
    ThisNuc.SetBindingEnergy("PerN","MeV",5.371548);
    
    // Add the most important states:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV");    ThisNuc.SetLevelReference(Key,"CDEFGHIJ"); ThisNuc.AddLevelAngMom(Key,1.5,"-",kTRUE); ThisNuc.SetLevelLifeTime(Key,53.22,"day");
    ++Key; ThisNuc.AddEnergyLevel(Key,429.08,"keV"); ThisNuc.SetLevelReference(Key,"CDEFGHIJ"); ThisNuc.AddLevelAngMom(Key,0.5,"-",kTRUE); ThisNuc.SetLevelLifeTime(Key,133.0,"fs");
    
    
    return ThisNuc;
}
