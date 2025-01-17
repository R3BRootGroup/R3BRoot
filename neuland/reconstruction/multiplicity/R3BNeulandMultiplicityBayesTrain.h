#pragma once

#include "FairTask.h"
#include "R3BMCTrack.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandMultiplicityBayesPar.h"
#include <R3BIOConnector.h>

class R3BNeulandMultiplicityBayesTrain : public FairTask
{
  public:
    explicit R3BNeulandMultiplicityBayesTrain(std::string_view clusters = "NeulandClusters",
                                              std::string_view tracks = "NeulandPrimaryTracks");

  private:
    R3B::InputVectorConnector<R3BNeulandCluster> fClusters;
    R3B::InputVectorConnector<R3BMCTrack> fTracks;

    R3BNeulandMultiplicityBayesPar* fPar = nullptr;

    void Exec(Option_t* /*option*/) override;
    void FinishTask() override;
    auto Init() -> InitStatus override;
    void SetParContainers() override;
    ClassDefOverride(R3BNeulandMultiplicityBayesTrain, 1)
};
