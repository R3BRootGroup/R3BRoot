class RunConfig
{
  public:
    RunConfig(const TString path_,
              const Int_t distance_,
              const Int_t energy_,
              const Int_t erel_,
              const Int_t nDoublePlanes_,
              const Int_t nNeutrons_)
        : path(path_)
        , distance(distance_)
        , energy(energy_)
        , erel(erel_)
        , nDoublePlanes(nDoublePlanes_)
        , nNeutrons(nNeutrons_)
    {
    }

    TString GetNeut() const { return TString::Format("4He_%dn_%dAMeV_%dkeV.dat", nNeutrons, energy, erel); }

    TString GetGeom() const { return TString::Format("neuland_v2_%dcm_%ddp.geo.root", distance, nDoublePlanes); }

    TString GetSimu() const
    {
        return TString::Format(
            "%s/%dcm_%ddp_%dn_%dAMeV_%dkeV.simu.root", path.Data(), distance, nDoublePlanes, nNeutrons, energy, erel);
    }

    TString GetPara() const
    {
        return TString::Format(
            "%s/%dcm_%ddp_%dn_%dAMeV_%dkeV.para.root", path.Data(), distance, nDoublePlanes, nNeutrons, energy, erel);
    }

    TString GetDigi() const
    {
        return TString::Format(
            "%s/%dcm_%ddp_%dn_%dAMeV_%dkeV.digi.root", path.Data(), distance, nDoublePlanes, nNeutrons, energy, erel);
    }

    TString GetNcut() const
    {
        return TString::Format(
            "%s/%dcm_%ddp_%dAMeV_%dkeV.neutroncuts.para.root", path.Data(), distance, nDoublePlanes, energy, erel);
    }

    TString GetReco() const
    {
        return TString::Format(
            "%s/%dcm_%ddp_%dn_%dAMeV_%dkeV.reco.root", path.Data(), distance, nDoublePlanes, nNeutrons, energy, erel);
    }

    TString GetEval() const
    {
        return TString::Format(
            "%s/%dcm_%ddp_%dn_%dAMeV_%dkeV.eval.root", path.Data(), distance, nDoublePlanes, nNeutrons, energy, erel);
    }

  public:
    TString path;
    Int_t distance;
    Int_t energy;
    Int_t erel;
    Int_t nDoublePlanes;
    Int_t nNeutrons;
};
