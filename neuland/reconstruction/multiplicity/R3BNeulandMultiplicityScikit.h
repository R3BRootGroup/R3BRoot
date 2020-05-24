#ifndef R3BROOT_R3BNEULANDMULTIPLICITYSCIKIT_H
#define R3BROOT_R3BNEULANDMULTIPLICITYSCIKIT_H

#include "FairTask.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandMultiplicity.h"
#include "TCAConnector.h"

class R3BNeulandMultiplicityScikit : public FairTask
{
  public:
    R3BNeulandMultiplicityScikit(TString model,
                                 TString inputCluster = "NeulandClusters",
                                 TString output = "NeulandMultiplicity");
    ~R3BNeulandMultiplicityScikit() override;

    void Exec(Option_t*) override;

  protected:
    InitStatus Init() override;

  private:
    TCAInputConnector<R3BNeulandCluster> fClusters;
    R3BNeulandMultiplicity* fMultiplicity;
    TString fOutputName;

    ClassDefOverride(R3BNeulandMultiplicityScikit, 0)
};

#endif // R3BROOT_R3BNEULANDMULTIPLICITYSCIKIT_H
