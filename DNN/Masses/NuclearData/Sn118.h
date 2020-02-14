Nucleus Sn118()
{
    Nucleus ThisNuc;
    ThisNuc.SetName("Sn118");
    ThisNuc.SetZ(50);
    ThisNuc.SetA(118);
    ThisNuc.SetLifeTime("s",1e10);
    ThisNuc.SetAbundance("Dimless",0.2422);
    ThisNuc.SetBindingEnergy("PerN","MeV",8.516534);
    ThisNuc.SetSp("MeV",9.999);
    ThisNuc.SetSn("MeV",9.32642);    
    
    // Return the final answer:
    return ThisNuc;
}