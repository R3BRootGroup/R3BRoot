Nucleus Zr90()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Zr90");
    ThisNuc.SetZ(40);
    ThisNuc.SetA(90);
    ThisNuc.SetLifeTime("year",1e10);
    ThisNuc.SetAbundance("Dimless",0.5145);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.70998021);
    return ThisNuc;
}