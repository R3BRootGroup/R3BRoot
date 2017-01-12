void ConnectParFileToRuntimeDb(const TString parFile, FairRuntimeDb* rtdb)
{
    FairParRootFileIo* io = new FairParRootFileIo();
    io->open(parFile);
    rtdb->setFirstInput(io);
    rtdb->setOutput(io);
    rtdb->saveOutput();
}

void ConnectParFilesToRuntimeDb(const TString parFile, const TString secondInput, FairRuntimeDb* rtdb)
{
    FairParRootFileIo* io = new FairParRootFileIo();
    io->open(parFile);

    FairParRootFileIo* io2 = new FairParRootFileIo();
    io2->open(secondInput);

    rtdb->setFirstInput(io);
    rtdb->setSecondInput(io2);

    rtdb->setOutput(io);
    rtdb->saveOutput();
}

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
        , nMax(4)
    {
    }

    TString GetNeut() const
    {
        // return TString::Format("%dSn_%dn_%dAMeV_%dkeV_noSn.dat", 132 - nNeutrons, nNeutrons, energy, erel);
        return TString::Format("%dSn_%dn_%dAMeV_%dkeV.dat", 132 - nNeutrons, nNeutrons, energy, erel);
    }

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
    Int_t nMax;
};
