void Step3_Calibr(const TString& base = "./output/1400cm_30dp")
{
    Neuland::Neutron2DCalibr cal = Neuland::Neutron2DCalibr();
    cal.SetClusterFile(1, base + "_1n.digi.root");
    cal.SetClusterFile(2, base + "_2n.digi.root");
    cal.SetClusterFile(3, base + "_3n.digi.root");
    cal.SetClusterFile(4, base + "_4n.digi.root");
    cal.Optimize();
    cal.WriteParameterFile(base + ".neutroncuts.par.root");
    cal.Print(std::ofstream(base + ".dat"));
    cal.Draw(base + ".neutroncuts.pdf");
}
