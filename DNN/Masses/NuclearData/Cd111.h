Nucleus Cd111()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Cd111");
    ThisNuc.SetZ(48);
    ThisNuc.SetA(111);
    ThisNuc.SetLifeTime("sec",1e10);
    ThisNuc.SetAbundance("Dimless",0.1280);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.5370875);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.AddLevelAngMom(Key,0.5,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,1e99,"year");
    
    return ThisNuc;
}
  