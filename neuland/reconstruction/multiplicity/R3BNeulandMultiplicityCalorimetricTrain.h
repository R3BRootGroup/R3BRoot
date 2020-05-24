#ifndef R3BROOT_R3BNEULANDMULTIPLICITYCALORIMETRICTRAIN_H
#define R3BROOT_R3BNEULANDMULTIPLICITYCALORIMETRICTRAIN_H

#include "FairTask.h"
#include "R3BMCTrack.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandMultiplicityCalorimetricPar.h"
#include "TCAConnector.h"
#include "TCutG.h"
#include "TH2D.h"

class R3BNeulandMultiplicityCalorimetricTrain : public FairTask
{
  public:
    R3BNeulandMultiplicityCalorimetricTrain(TString clusters = "NeulandClusters",
                                            TString tracks = "NeulandPrimaryTracks");
    ~R3BNeulandMultiplicityCalorimetricTrain() override;

    void Exec(Option_t*) override;
    void FinishTask() override;
    void Print(Option_t* = "") const override;

    void SetEdepOpt(double init, double step, double low, double high) { fEdepOpt = { init, step, low, high }; }
    void SetNclusterOpt(double init, double step, double low, double high) { fNclusterOpt = { init, step, low, high }; }
    void SetWeight(double w) { fWeight = w; }

  protected:
    InitStatus Init() override;

  private:
    TCutG* GetCut(unsigned int nNeutrons, double edep, double ncluster);
    double WastedEfficiency(const double* d);
    void Optimize();

    TH2D* GetOrBuildHist(unsigned int i);

    TCAInputConnector<R3BNeulandCluster> fClusters;
    TCAInputConnector<R3BMCTrack> fTracks;

    R3BNeulandMultiplicityCalorimetricPar* fPar;

    std::array<double, 4> fEdepOpt;
    std::array<double, 4> fNclusterOpt;
    double fWeight;

    std::map<unsigned int, TH2D*> fHists;
    std::map<unsigned int, TCutG*> fCuts;

    ClassDefOverride(R3BNeulandMultiplicityCalorimetricTrain, 0)
};

#endif // R3BROOT_R3BNEULANDMULTIPLICITYCALORIMETRICTRAIN_H
