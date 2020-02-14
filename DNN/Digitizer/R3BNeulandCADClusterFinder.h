#ifndef R3BNEULANDCADCLUSTERFINDER_H_NEW
#define R3BNEULANDCADCLUSTERFINDER_H_NEW

/**
 * NeuLAND cluster finder task
 * @author Jan Mayer
 *
 * For each event, get the R3BNeulandCADHits and group them into R3BNeulandCADClusters using the Neuland Clustering Engine.
 *   Input:  Branch NeulandHits     = TClonesArray("NeulandCADHit")
 *   Output: Branch NeulandClusters = TClonesArray("R3BNeulandCADCluster")
 *
 */

#include "ClusteringEngine.h"
#include "FairTask.h"
#include "R3BNeulandCADCluster.h"
#include "R3BNeulandCADHit.h"
#include "TCAConnector.h"

class R3BNeulandCADClusterFinder : public FairTask
{
  public:
    R3BNeulandCADClusterFinder(Double_t dx = 1. * 7.5,
                            Double_t dy = 1. * 7.5,
                            Double_t dz = 2. * 7.5,
                            Double_t dt = 1.,
                            TString input = "NeulandCADHits",
                            TString output = "NeulandCADClusters");

    ~R3BNeulandCADClusterFinder() override = default;

    // No copy and no move is allowed (Rule of three/five)
    R3BNeulandCADClusterFinder(const R3BNeulandCADClusterFinder&) = delete;            // copy constructor
    R3BNeulandCADClusterFinder(R3BNeulandCADClusterFinder&&) = delete;                 // move constructor
    R3BNeulandCADClusterFinder& operator=(const R3BNeulandCADClusterFinder&) = delete; // copy assignment
    R3BNeulandCADClusterFinder& operator=(R3BNeulandCADClusterFinder&&) = delete;      // move assignment
    
    // Multiple digitizer runs:
    void SetTag(TString const Tag) {OutputNameTag = Tag;}

  protected:
    InitStatus Init() override;

  public:
    void Exec(Option_t*) override;

  private:
    Neuland::ClusteringEngine<R3BNeulandCADHit> fClusteringEngine;
    TCAInputConnector<R3BNeulandCADHit> fDigis;
    TCAOutputConnector<R3BNeulandCADCluster> fClusters;
    
    TString OutputNameTag;

    ClassDefOverride(R3BNeulandCADClusterFinder, 0);
};

#endif // R3BNEULANDCLUSTERFINDER_H_NEW
