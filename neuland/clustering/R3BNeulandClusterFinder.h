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

#include <memory>
#include "TClonesArray.h"
#include "FairTask.h"
#include "R3BNeulandDigi.h"
#include "ClusteringEngine.h"

class R3BNeulandClusterFinder : public FairTask
{
  public:
    R3BNeulandClusterFinder(const Double_t dx = 2. * 7.5,
                            const Double_t dy = 2. * 7.5,
                            const Double_t dz = 3. * 7.5,
                            const Double_t dt = 3.,
                            const TString input = "NeulandDigis",
                            const TString output = "NeulandClusters");

    ~R3BNeulandClusterFinder() = default;

  private:
    // No copy and no move is allowed (Rule of three/five)
    R3BNeulandClusterFinder(const R3BNeulandClusterFinder&);            // copy constructor
    R3BNeulandClusterFinder(R3BNeulandClusterFinder&&);                 // move constructor
    R3BNeulandClusterFinder& operator=(const R3BNeulandClusterFinder&); // copy assignment
    R3BNeulandClusterFinder& operator=(R3BNeulandClusterFinder&&);      // move assignment

  protected:
    InitStatus Init() override;
    void Finish() override;

  public:
    void Exec(Option_t*) override;

  private:
    Neuland::ClusteringEngine<R3BNeulandDigi> fClusteringEngine;

    TString fInput;
    TString fOutput;

    TClonesArray* fDigis;                    // non-owning
    std::unique_ptr<TClonesArray> fClusters; // owning

  public:
    ClassDefOverride(R3BNeulandClusterFinder, 0);
};

#endif // R3BNEULANDCLUSTERFINDER_H_NEW
