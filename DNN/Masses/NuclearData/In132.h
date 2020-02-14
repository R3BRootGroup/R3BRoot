Nucleus In132()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("In132");
    ThisNuc.SetZ(49);
    ThisNuc.SetA(132);
    ThisNuc.SetLifeTime("sec",0.207);
    ThisNuc.SetAbundance("Dimless",0.0);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.2537);
    
    // Add the ground state:
    Int_t Key = 0;
    Key=0; ThisNuc.AddEnergyLevel(Key,0.0,"keV"); ThisNuc.AddLevelAngMom(Key,7.0,"-",kTRUE); ThisNuc.SetLevelLifeTime(Key,0.207,"sec");
    
    return ThisNuc;
}
