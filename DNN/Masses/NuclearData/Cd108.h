Nucleus Cd108()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Cd108");
    ThisNuc.SetZ(48);
    ThisNuc.SetA(108);
    ThisNuc.SetLifeTime("sec",1e10);
    ThisNuc.SetAbundance("Dimless",0.0089);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.55002010);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.SetLevelReference(Key,"ABCDEGHIKLMNOPQRST"); ThisNuc.AddLevelAngMom(Key,0,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,1e99,"year");
    
    return ThisNuc;
}
