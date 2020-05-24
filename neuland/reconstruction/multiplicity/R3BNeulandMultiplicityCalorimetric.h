#ifndef R3BROOT_R3BNEULANDMULTIPLICITYCALORIMETRIC_H
#define R3BROOT_R3BNEULANDMULTIPLICITYCALORIMETRIC_H

#include "FairTask.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandMultiplicity.h"
#include "R3BNeulandMultiplicityCalorimetricPar.h"
#include "TCAConnector.h"

class R3BNeulandMultiplicityCalorimetric : public FairTask
{
  public:
    R3BNeulandMultiplicityCalorimetric(TString input = "NeulandClusters", TString output = "NeulandMultiplicity");
    ~R3BNeulandMultiplicityCalorimetric() override;

    void Exec(Option_t*) override;

  protected:
    InitStatus Init() override;

  private:
    TCAInputConnector<R3BNeulandCluster> fClusters;
    R3BNeulandMultiplicity* fMultiplicity;
    TString fOutputName;

    R3BNeulandMultiplicityCalorimetricPar* fPar;

    ClassDefOverride(R3BNeulandMultiplicityCalorimetric, 0)
};

#endif // R3BROOT_R3BNEULANDMULTIPLICITYCALORIMETRIC_H
