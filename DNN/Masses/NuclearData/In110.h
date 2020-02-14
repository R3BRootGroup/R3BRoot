Nucleus In110()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("In110");
    ThisNuc.SetZ(49);
    ThisNuc.SetA(110);
    ThisNuc.SetLifeTime("sec",4.9*3600.0);
    ThisNuc.SetAbundance("Dimless",0.0);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.5089210);
    return ThisNuc;
}