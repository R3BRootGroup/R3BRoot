Nucleus Mg26()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Mg26");
    ThisNuc.SetZ(12);
    ThisNuc.SetA(26);
    ThisNuc.SetLifeTime("sec",1e10);
    ThisNuc.SetAbundance("Dimless",0.1101);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.333870110);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.SetLevelReference(Key,"ABCDEFGHIKLMOPQRSTU"); ThisNuc.AddLevelAngMom(Key,0,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,1e99,"year");
    
    return ThisNuc;
}