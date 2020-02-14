Nucleus neutron()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("neutron");
    ThisNuc.SetZ(0);
    ThisNuc.SetA(1);
    ThisNuc.SetLifeTime("sec",881.515);
    ThisNuc.SetAbundance("Dimless",1.0);
    ThisNuc.SetMass("clean","MeV",939.565413358);
    ThisNuc.SetPDGCode(2112);
    return ThisNuc;
}
