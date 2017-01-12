#ifndef R3BNEULANDCLUSTERFINDER_H_NEW
#define R3BNEULANDCLUSTERFINDER_H_NEW

/**
 * NeuLAND cluster finder task
 * @author Jan Mayer
 *
 * For each event, get the R3BNeulandDigis and group them into R3BNeulandClusters using the Neuland Clustering Engine.
 *   Input:  Branch NeulandDigis    = TClonesArray("R3BNeulandDigi")
 *   Output: Branch NeulandClusters = TClonesArray("R3BNeulandCluster")
 *
 */

#include "ClusteringEngine.h"
#include "FairTask.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandDigi.h"
#include "TCAConnector.h"

class R3BNeulandClusterFinder : public FairTask
{
  public:
    R3BNeulandClusterFinder(const Double_t dx = 1. * 7.5,
                            const Double_t dy = 1. * 7.5,
                            const Double_t dz = 2. * 7.5,
                            const Double_t dt = 1.,
                            const TString input = "NeulandDigis",
                            const TString output = "NeulandClusters");

    ~R3BNeulandClusterFinder() override = default;

    // No copy and no move is allowed (Rule of three/five)
    R3BNeulandClusterFinder(const R3BNeulandClusterFinder&) = delete;            // copy constructor
    R3BNeulandClusterFinder(R3BNeulandClusterFinder&&) = delete;                 // move constructor
    R3BNeulandClusterFinder& operator=(const R3BNeulandClusterFinder&) = delete; // copy assignment
    R3BNeulandClusterFinder& operator=(R3BNeulandClusterFinder&&) = delete;      // move assignment

  protected:
    InitStatus Init() override;

  public:
    void Exec(Option_t*) override;

  private:
    Neuland::ClusteringEngine<R3BNeulandDigi> fClusteringEngine;
    TCAInputConnector<R3BNeulandDigi> fDigis;
    TCAOutputConnector<R3BNeulandCluster> fClusters;

    ClassDefOverride(R3BNeulandClusterFinder, 0);
};

#endif // R3BNEULANDCLUSTERFINDER_H_NEW
