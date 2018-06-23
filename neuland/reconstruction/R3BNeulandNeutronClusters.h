#ifndef R3BNEULANDNEUTRONCLUSTERS_H
#define R3BNEULANDNEUTRONCLUSTERS_H

#include "FairMCPoint.h"
#include "FairTask.h"
#include "R3BNeulandCluster.h"
#include "TCAConnector.h"

class TClonesArray;
class TH1D;

class R3BNeulandNeutronClusters : public FairTask
{
  public:
    R3BNeulandNeutronClusters(TString input, TString outputPrimary, TString outputSecondary, Double_t maxDist);
    ~R3BNeulandNeutronClusters() override = default;

    // No copy and no move is allowed (Rule of three/five)
    R3BNeulandNeutronClusters(const R3BNeulandNeutronClusters&) = delete;            // copy constructor
    R3BNeulandNeutronClusters(R3BNeulandNeutronClusters&&) = delete;                 // move constructor
    R3BNeulandNeutronClusters& operator=(const R3BNeulandNeutronClusters&) = delete; // copy assignment
    R3BNeulandNeutronClusters& operator=(R3BNeulandNeutronClusters&&) = delete;      // move assignment

  protected:
    InitStatus Init() override;
    void Finish() override;

  public:
    void Exec(Option_t*) override;

  private:
    Double_t fMaxDist;

    TCAInputConnector<FairMCPoint> fPrimaryNeutronInteractionPoints;
    TCAInputConnector<R3BNeulandCluster> fClusters;
    TCAOutputConnector<R3BNeulandCluster> fPrimaryClusters;
    TCAOutputConnector<R3BNeulandCluster> fPrimaryClustersMultiple;
    TCAOutputConnector<R3BNeulandCluster> fPrimaryClustersSO;
    TCAOutputConnector<R3BNeulandCluster> fPrimaryClustersMO;
    TCAOutputConnector<R3BNeulandCluster> fSecondaryClusters;

    TH1D* fhNeutronClusterDistance;
    TH1D* fhNeutronClusterUsage;
    TH1D* fhUnmatchedClusters;

  public:
    ClassDefOverride(R3BNeulandNeutronClusters, 0);
};

#endif // R3BNEULANDNEUTRONCLUSTERS_H
