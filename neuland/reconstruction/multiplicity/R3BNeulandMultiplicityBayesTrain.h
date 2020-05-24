#ifndef R3BROOT_R3BNEULANDMULTIPLICITYBAYESTRAIN_H
#define R3BROOT_R3BNEULANDMULTIPLICITYBAYESTRAIN_H

#include "FairTask.h"
#include "R3BMCTrack.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandMultiplicityBayesPar.h"
#include "TCAConnector.h"

class R3BNeulandMultiplicityBayesTrain : public FairTask
{
  public:
    R3BNeulandMultiplicityBayesTrain(TString clusters = "NeulandClusters", TString tracks = "NeulandPrimaryTracks");
    ~R3BNeulandMultiplicityBayesTrain() override;

    void Exec(Option_t*) override;
    void FinishTask() override;

  protected:
    InitStatus Init() override;
    void SetParContainers() override;

  private:
    TCAInputConnector<R3BNeulandCluster> fClusters;
    TCAInputConnector<R3BMCTrack> fTracks;

    R3BNeulandMultiplicityBayesPar* fPar;

    ClassDefOverride(R3BNeulandMultiplicityBayesTrain, 0)
};

#endif // R3BROOT_R3BNEULANDMULTIPLICITYBAYESTRAIN_H
