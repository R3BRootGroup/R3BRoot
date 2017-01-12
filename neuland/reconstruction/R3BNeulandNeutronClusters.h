#ifndef R3BNEULANDNEUTRONCLUSTERS_H
#define R3BNEULANDNEUTRONCLUSTERS_H

#include "FairTask.h"

class TClonesArray;
class TH1D;

class R3BNeulandNeutronClusters : public FairTask
{
  public:
    R3BNeulandNeutronClusters(const TString input = "NeulandClusters",
                              const TString outputPrimary = "NeulandPrimaryClusters",
                              const TString outputSecondary = "NeulandSecondaryClusters",
                              const Double_t maxDist = 10.);
    ~R3BNeulandNeutronClusters();

  private:
    // No copy and no move is allowed (Rule of three/five)
    R3BNeulandNeutronClusters(const R3BNeulandNeutronClusters&);            // copy constructor
    R3BNeulandNeutronClusters(R3BNeulandNeutronClusters&&);                 // move constructor
    R3BNeulandNeutronClusters& operator=(const R3BNeulandNeutronClusters&); // copy assignment
    R3BNeulandNeutronClusters& operator=(R3BNeulandNeutronClusters&&);      // move assignment

  protected:
    InitStatus Init() override;
    void Finish() override;

  public:
    void Exec(Option_t*) override;

  private:
    TString fInput;
    TString fOutputPrimary;
    TString fOutputSecondary;

    Double_t fMaxDist;

    TH1D* fhNeutronClusterDistance;
    TH1D* fhNeutronClusterUsage;

    TClonesArray* fPrimaryNeutronInteractionPoints;
    TClonesArray* fClustersIn;
    TClonesArray* fPrimaryClusters;
    TClonesArray* fSecondaryClusters;

  public:
    ClassDefOverride(R3BNeulandNeutronClusters, 0);
};

#endif // R3BNEULANDNEUTRONCLUSTERS_H
