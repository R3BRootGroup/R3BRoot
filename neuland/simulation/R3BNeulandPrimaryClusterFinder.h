#ifndef R3BROOT_R3BNEULANDPRIMARYCLUSTERFINDER_H
#define R3BROOT_R3BNEULANDPRIMARYCLUSTERFINDER_H

#include "FairTask.h"
#include "R3BMCTrack.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandHit.h"
#include "TCAConnector.h"

class R3BNeulandPrimaryClusterFinder : public FairTask
{
  public:
    explicit R3BNeulandPrimaryClusterFinder(TString primaryHits = "NeulandPrimaryHits",
                                            TString clusters = "NeulandClusters",
                                            TString primaryClusters = "NeulandPrimaryClusters",
                                            TString secondaryClusters = "NeulandSecondaryClusters");

    ~R3BNeulandPrimaryClusterFinder() override = default;

    // No copy and no move is allowed (Rule of three/five)
    R3BNeulandPrimaryClusterFinder(const R3BNeulandPrimaryClusterFinder&) = delete;
    R3BNeulandPrimaryClusterFinder(R3BNeulandPrimaryClusterFinder&&) = delete;
    R3BNeulandPrimaryClusterFinder& operator=(const R3BNeulandPrimaryClusterFinder&) = delete;
    R3BNeulandPrimaryClusterFinder& operator=(R3BNeulandPrimaryClusterFinder&&) = delete;

  protected:
    InitStatus Init() override;

  public:
    void Exec(Option_t*) override;

  private:
    TCAInputConnector<R3BNeulandHit> fPrimaryHits;
    TCAInputConnector<R3BNeulandCluster> fClusters;

    TCAOutputConnector<R3BNeulandCluster> fPrimaryClusters;
    TCAOutputConnector<R3BNeulandCluster> fSecondaryClusters;

    ClassDefOverride(R3BNeulandPrimaryClusterFinder, 0);
};

#endif // R3BROOT_R3BNEULANDPRIMARYCLUSTERFINDER_H
