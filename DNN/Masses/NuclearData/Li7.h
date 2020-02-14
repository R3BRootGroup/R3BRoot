Nucleus Li7()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Li7");
    ThisNuc.SetZ(3);
    ThisNuc.SetA(7);
    ThisNuc.SetLifeTime("sec",1e10);
    ThisNuc.SetAbundance("Dimless",0.9241);
    ThisNuc.SetBindingEnergy("PerN","MeV",5.6064390);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.SetLevelReference(Key,"ABDFGHIJKLMNPQRSUVWXYZa"); ThisNuc.AddLevelAngMom(Key,1.5,"-",kTRUE); ThisNuc.SetLevelLifeTime(Key,1e99,"year");
    
    return ThisNuc;
}
