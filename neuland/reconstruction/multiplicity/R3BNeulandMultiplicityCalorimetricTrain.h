#pragma once

#include "FairTask.h"
#include "R3BMCTrack.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandHit.h"
#include "R3BNeulandMultiplicityCalorimetricPar.h"
#include "TCutG.h"
#include <R3BIOConnector.h>
#include <Rtypes.h>
#include <RtypesCore.h>
#include <array>
#include <map>
#include <string_view>

class TH2D;

class R3BNeulandMultiplicityCalorimetricTrain : public FairTask
{
  public:
    explicit R3BNeulandMultiplicityCalorimetricTrain(std::string_view clusters = "NeulandClusters",
                                                     std::string_view tracks = "NeulandPrimaryTracks",
                                                     std::string_view phits = "NeulandPrimaryHits");

    R3BNeulandMultiplicityCalorimetricTrain(const R3BNeulandMultiplicityCalorimetricTrain&) = delete;
    R3BNeulandMultiplicityCalorimetricTrain(R3BNeulandMultiplicityCalorimetricTrain&&) = delete;
    auto operator=(const R3BNeulandMultiplicityCalorimetricTrain&) -> R3BNeulandMultiplicityCalorimetricTrain& = delete;
    auto operator=(R3BNeulandMultiplicityCalorimetricTrain&&) -> R3BNeulandMultiplicityCalorimetricTrain& = delete;
    ~R3BNeulandMultiplicityCalorimetricTrain() override;

    void SetEdepOpt(double init, double step, double low, double high) { fEdepOpt = { init, step, low, high }; }
    void SetEdepOffOpt(double init, double step, double low, double high) { fEdepOffOpt = { init, step, low, high }; }
    void SetNclusterOpt(double init, double step, double low, double high) { fNclusterOpt = { init, step, low, high }; }
    void SetNclusterOffOpt(double init, double step, double low, double high)
    {
        fNclusterOffOpt = { init, step, low, high };
    }
    void SetWeight(double weight) { fWeight = weight; }
    void SetUseHits(bool is_hit_used) { fUseHits = is_hit_used; }

  private:
    auto GetCut(unsigned int nNeutrons, double edep, double edepoff, double ncluster, double nclusteroff) -> TCutG*;
    auto WastedEfficiency(const double* cut) -> double;
    void Optimize();

    auto GetOrBuildHist(unsigned int index) -> TH2D*;

    R3B::InputVectorConnector<R3BNeulandCluster> fClusters;
    R3B::InputVectorConnector<R3BMCTrack> fTracks;
    R3B::InputVectorConnector<R3BNeulandHit> fPHits;

    R3BNeulandMultiplicityCalorimetricPar* fPar;

    bool fUseHits;

    std::array<double, 4> fEdepOpt;
    std::array<double, 4> fEdepOffOpt;
    std::array<double, 4> fNclusterOpt;
    std::array<double, 4> fNclusterOffOpt;
    double fWeight;

    std::map<unsigned int, TH2D*> fHists;

    // TODO: use unique_ptr
    std::map<unsigned int, TCutG*> fCuts;

    void Exec(Option_t* /*option*/) override;
    void FinishTask() override;
    void Print(Option_t* /*option*/ = "") const override;
    auto Init() -> InitStatus override;
    ClassDefOverride(R3BNeulandMultiplicityCalorimetricTrain, 1)
};
