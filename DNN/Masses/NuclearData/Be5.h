Nucleus Be5()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Be5");
    ThisNuc.SetZ(4);
    ThisNuc.SetA(5);
    ThisNuc.SetLifeTime("ns",0.001);
    ThisNuc.SetAbundance("Dimless",0.0);
    ThisNuc.SetBindingEnergy("PerN","MeV",0.0);
    
    return ThisNuc;
}
