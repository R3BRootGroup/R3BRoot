Nucleus Mg25()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Mg25");
    ThisNuc.SetZ(12);
    ThisNuc.SetA(25);
    ThisNuc.SetLifeTime("sec",1e10);
    ThisNuc.SetAbundance("Dimless",0.1000);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.223502020);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.SetLevelReference(Key,"ABCDE GHIJKL"); ThisNuc.AddLevelAngMom(Key,2.5,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,1e99,"year");
    
    return ThisNuc;
}
