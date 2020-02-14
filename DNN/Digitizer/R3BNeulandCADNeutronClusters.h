#ifndef R3BNEULANDCADNEUTRONCLUSTERS_H
#define R3BNEULANDCADNEUTRONCLUSTERS_H

#include "FairMCPoint.h"
#include "FairTask.h"
#include "R3BNeulandCADCluster.h"
#include "TCAConnector.h"

class TClonesArray;
class TH1D;

class R3BNeulandCADNeutronClusters : public FairTask
{
  public:
    R3BNeulandCADNeutronClusters(TString input, TString outputPrimary, TString outputSecondary, Double_t maxDist);
    ~R3BNeulandCADNeutronClusters() override = default;

    // No copy and no move is allowed (Rule of three/five)
    R3BNeulandCADNeutronClusters(const R3BNeulandCADNeutronClusters&) = delete;            // copy constructor
    R3BNeulandCADNeutronClusters(R3BNeulandCADNeutronClusters&&) = delete;                 // move constructor
    R3BNeulandCADNeutronClusters& operator=(const R3BNeulandCADNeutronClusters&) = delete; // copy assignment
    R3BNeulandCADNeutronClusters& operator=(R3BNeulandCADNeutronClusters&&) = delete;      // move assignment
    
    // Multiple digitizer runs:
    void SetTag(TString const Tag) {OutputNameTag = Tag;}

  protected:
    InitStatus Init() override;
    void Finish() override;

  public:
    void Exec(Option_t*) override;

  private:
    Double_t fMaxDist;

    TCAInputConnector<FairMCPoint> fPrimaryNeutronInteractionPoints;
    TCAInputConnector<R3BNeulandCADCluster> fClusters;
    TCAOutputConnector<R3BNeulandCADCluster> fPrimaryClusters;
    TCAOutputConnector<R3BNeulandCADCluster> fPrimaryClustersMultiple;
    TCAOutputConnector<R3BNeulandCADCluster> fPrimaryClustersSO;
    TCAOutputConnector<R3BNeulandCADCluster> fPrimaryClustersMO;
    TCAOutputConnector<R3BNeulandCADCluster> fSecondaryClusters;
    
    TString OutputNameTag;

    TH1D* fhNeutronClusterDistance;
    TH1D* fhNeutronClusterUsage;
    TH1D* fhUnmatchedClusters;

  public:
    ClassDefOverride(R3BNeulandCADNeutronClusters, 0);
};

#endif // R3BNEULANDNEUTRONCLUSTERS_H
