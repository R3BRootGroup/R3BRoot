#ifndef R3BROOT_R3BNEULANDMULTIPLICITYBAYES_H
#define R3BROOT_R3BNEULANDMULTIPLICITYBAYES_H

#include "FairTask.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandMultiplicity.h"
#include "R3BNeulandMultiplicityBayesPar.h"
#include "TCAConnector.h"

class R3BNeulandMultiplicityBayes : public FairTask
{
  public:
    R3BNeulandMultiplicityBayes(TString input = "NeulandClusters", TString output = "NeulandMultiplicity");
    ~R3BNeulandMultiplicityBayes() override;

    void Exec(Option_t*) override;

  protected:
    InitStatus Init() override;
    void SetParContainers() override;

  private:
    TCAInputConnector<R3BNeulandCluster> fClusters;
    R3BNeulandMultiplicity* fMultiplicity;
    TString fOutputName;

    R3BNeulandMultiplicityBayesPar* fPar;

    ClassDefOverride(R3BNeulandMultiplicityBayes, 0)
};

#endif // R3BROOT_R3BNEULANDMULTIPLICITYBAYES_H
