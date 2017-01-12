#ifndef R3BROOT_R3BNEULANDNEUTRONRECONSTRUCTIONSTATISTICS_H
#define R3BROOT_R3BNEULANDNEUTRONRECONSTRUCTIONSTATISTICS_H

#include "FairTask.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandNeutron.h"
#include "TCAConnector.h"
#include "TH1D.h"

class R3BNeulandNeutronReconstructionStatistics : public FairTask
{
  public:
    R3BNeulandNeutronReconstructionStatistics(const TString primary = "NeulandNeutronClusters",
                                              const TString secondary = "NeulandPrimaryClusters",
                                              const TString predicted = "NeulandSecondaryClusters");
    ~R3BNeulandNeutronReconstructionStatistics() override = default;

    // No copy and no move is allowed (Rule of three/five)
    R3BNeulandNeutronReconstructionStatistics(const R3BNeulandNeutronReconstructionStatistics&) =
        delete;                                                                                      // copy constructor
    R3BNeulandNeutronReconstructionStatistics(R3BNeulandNeutronReconstructionStatistics&&) = delete; // move constructor
    R3BNeulandNeutronReconstructionStatistics& operator=(const R3BNeulandNeutronReconstructionStatistics&) =
        delete; // copy assignment
    R3BNeulandNeutronReconstructionStatistics& operator=(R3BNeulandNeutronReconstructionStatistics&&) =
        delete; // move assignment

  protected:
    InitStatus Init() override;
    void Finish() override;

  public:
    void Exec(Option_t*) override;

  private:
    TCAInputConnector<R3BNeulandCluster> fPrimaryClusters;
    TCAInputConnector<R3BNeulandCluster> fSecondaryClusters;
    TCAInputConnector<R3BNeulandNeutron> fPredictedNeutrons;
    TString fPredictedName;
    TH1D* fTP;
    TH1D* fFP;
    TH1D* fFN;
    TH1D* fTN;
    TH1D* fF1;

  public:
    ClassDefOverride(R3BNeulandNeutronReconstructionStatistics, 0);
};

#endif // R3BROOT_R3BNEULANDNEUTRONRECONSTRUCTIONSTATISTICS_H
