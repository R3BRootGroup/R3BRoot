Nucleus gamma()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("gamma");
    ThisNuc.SetZ(0);
    ThisNuc.SetA(0);
    ThisNuc.SetLifeTime("year",1e10);
    ThisNuc.SetAbundance("Dimless",1.0);
    ThisNuc.SetMass("clean","MeV",0.0);
    ThisNuc.SetPDGCode(22);
    return ThisNuc;
}
