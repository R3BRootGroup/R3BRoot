Nucleus antiproton()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("antiproton");
    ThisNuc.SetZ(-1);
    ThisNuc.SetA(1);
    ThisNuc.SetLifeTime("year",1e10);
    ThisNuc.SetAbundance("Dimless",1.0);
    ThisNuc.SetMass("clean","MeV",938.272081358);
    ThisNuc.SetPDGCode(-2212);
    return ThisNuc;
}
