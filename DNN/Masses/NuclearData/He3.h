Nucleus He3()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("He3");
    ThisNuc.SetZ(2);
    ThisNuc.SetA(3);
    ThisNuc.SetLifeTime("sec",1e10);
    ThisNuc.SetAbundance("Dimless",0.00000134);
    ThisNuc.SetBindingEnergy("PerN","MeV",2.572680910);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.SetLevelReference(Key,"ABDF"); ThisNuc.AddLevelAngMom(Key,0.5,"+",kTRUE); ThisNuc.SetLevelLifeTime(Key,1e99,"year");
    
    return ThisNuc;
}