Nucleus Sn122()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Sn122");
    ThisNuc.SetZ(50);
    ThisNuc.SetA(122);
    ThisNuc.SetLifeTime("sec",1e10);
    ThisNuc.SetAbundance("Dimless",0.0463);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.48790920);
    ThisNuc.SetSp("MeV",11.390);
    ThisNuc.SetSn("MeV",8.8154);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.SetLevelReference(Key,"ABCDEFGHIJKLMNOP"); ThisNuc.AddLevelAngMom(Key,0,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,1e99,"year");
    
    return ThisNuc;
}