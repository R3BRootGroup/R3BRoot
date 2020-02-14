Nucleus pion0()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("pion0");
    ThisNuc.SetZ(0);
    ThisNuc.SetA(0);
    ThisNuc.SetLifeTime("s",8.4e-17);
    ThisNuc.SetAbundance("Dimless",1.0);
    ThisNuc.SetMass("clean","MeV",134.97666);
    ThisNuc.SetPDGCode(111);
    return ThisNuc;
}
