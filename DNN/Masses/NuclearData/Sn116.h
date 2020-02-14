Nucleus Sn116()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Sn116");
    ThisNuc.SetZ(50);
    ThisNuc.SetA(116);
    ThisNuc.SetLifeTime("sec",1e10);
    ThisNuc.SetAbundance("Dimless",0.1454);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.523116210);
    ThisNuc.SetSp("MeV",9.27862);
    ThisNuc.SetSn("MeV",9.56348);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.AddLevelAngMom(Key,0,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,1e99,"year");
    
    return ThisNuc;
}
