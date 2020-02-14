Nucleus Li3()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Li3");
    ThisNuc.SetZ(3);
    ThisNuc.SetA(3);
    ThisNuc.SetLifeTime("ns",0.001);
    ThisNuc.SetAbundance("Dimless",0.0);
    ThisNuc.SetBindingEnergy("PerN","MeV",0.0);
    
    return ThisNuc;
}
