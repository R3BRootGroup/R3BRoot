Nucleus Te126()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Te126");
    ThisNuc.SetZ(52);
    ThisNuc.SetA(126);
    ThisNuc.SetLifeTime("sec",1e10);
    ThisNuc.SetAbundance("Dimless",0.1884);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.46324812);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.SetLevelReference(Key,"ABCDEFGHIJKLMNOPQ"); ThisNuc.AddLevelAngMom(Key,0,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,1e99,"year");
    
    return ThisNuc;
}