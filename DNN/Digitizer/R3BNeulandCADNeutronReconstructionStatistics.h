#ifndef R3BROOT_R3BNEULANDCADNEUTRONRECONSTRUCTIONSTATISTICS_H
#define R3BROOT_R3BNEULANDCADNEUTRONRECONSTRUCTIONSTATISTICS_H

#include "FairTask.h"
#include "R3BNeulandCADCluster.h"
#include "R3BNeulandCADNeutron.h"
#include "TCAConnector.h"
#include "TH1D.h"
#include <iostream>

class R3BNeulandCADNeutronReconstructionStatistics : public FairTask
{
  public:
    R3BNeulandCADNeutronReconstructionStatistics(const TString primary = "NeulandNeutronClusters",
                                              const TString secondary = "NeulandPrimaryClusters",
                                              const TString predicted = "NeulandSecondaryClusters",
                                              std::ostream& out = std::cout);
    ~R3BNeulandCADNeutronReconstructionStatistics() override = default;

    // No copy and no move is allowed (Rule of three/five)
    R3BNeulandCADNeutronReconstructionStatistics(const R3BNeulandCADNeutronReconstructionStatistics&) =
        delete;                                                                                      // copy constructor
    R3BNeulandCADNeutronReconstructionStatistics(R3BNeulandCADNeutronReconstructionStatistics&&) = delete; // move constructor
    R3BNeulandCADNeutronReconstructionStatistics& operator=(const R3BNeulandCADNeutronReconstructionStatistics&) =
        delete; // copy assignment
    R3BNeulandCADNeutronReconstructionStatistics& operator=(R3BNeulandCADNeutronReconstructionStatistics&&) =
        delete; // move assignment

  protected:
    InitStatus Init() override;
    void Finish() override;

  public:
    void Exec(Option_t*) override;

  private:
    TCAInputConnector<R3BNeulandCADCluster> fPrimaryClusters;
    TCAInputConnector<R3BNeulandCADCluster> fSecondaryClusters;
    TCAInputConnector<R3BNeulandCADNeutron> fPredictedNeutrons;
    TString fPredictedName;
    TH1D* fhTP;
    TH1D* fhFP;
    TH1D* fhFN;
    TH1D* fhTN;
    TH1D* fhF1;
    int fTP;
    int fFP;
    int fFN;
    int fTN;
    std::ostream& fOut;
    std::vector<int> fMult;

  public:
    ClassDefOverride(R3BNeulandCADNeutronReconstructionStatistics, 0);
};

#endif // R3BROOT_R3BNEULANDNEUTRONRECONSTRUCTIONSTATISTICS_H
