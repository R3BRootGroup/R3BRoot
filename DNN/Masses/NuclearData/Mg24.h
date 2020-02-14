Nucleus Mg24()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Mg24");
    ThisNuc.SetZ(12);
    ThisNuc.SetA(24);
    ThisNuc.SetLifeTime("sec",1e10);
    ThisNuc.SetAbundance("Dimless",0.7899);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.260709010);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.SetLevelReference(Key,"ABCDEFGHIJKLMNOPQR"); ThisNuc.AddLevelAngMom(Key,0,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,1e99,"year");
    
    return ThisNuc;
}