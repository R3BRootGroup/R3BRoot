Nucleus electron()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("electron");
    ThisNuc.SetZ(-1);
    ThisNuc.SetA(0);
    ThisNuc.SetLifeTime("year",1e10);
    ThisNuc.SetAbundance("Dimless",1.0);
    ThisNuc.SetMass("clean","MeV",0.510998946131);
    ThisNuc.SetPDGCode(11);
    return ThisNuc;
}
