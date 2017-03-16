#include "runconfig.C"

void calibr(const TString path,
            const Int_t distance,
            const Int_t energy,
            const Int_t erel,
            const Int_t nDoublePlanes,
            const Int_t nNeutrons)
{
    Neuland::Neutron2DCalibr cal = Neuland::Neutron2DCalibr();
    for (Int_t n = 1; n <= nNeutrons; n++)
    {
        const RunConfig cfg(path, distance, energy, erel, nDoublePlanes, n);
        cal.AddClusterFile(cfg.GetDigi());
    }

    std::vector<Double_t> vslope = { 0.04, 0.001, 0.001, 10 };
    std::vector<Double_t> vdistance = { Double_t(energy) / 60., 0.5, Double_t(energy) / 100., Double_t(energy) / 10. };
    std::vector<Double_t> vdist_off = { 3, 0.5, 3, 6 };
    cal.Optimize(vslope, vdistance, vdist_off);

    const RunConfig cfg(path, distance, energy, erel, nDoublePlanes, nNeutrons);
    cal.WriteParameterFile(cfg.GetNcut());
    std::ofstream o(TString(cfg.GetNcut()).ReplaceAll(".root", ".dat"));
    cal.Print(o);
    cal.Draw(TString(cfg.GetNcut()).ReplaceAll(".root", ".pdf"));
}
