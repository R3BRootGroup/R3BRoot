#ifndef R3BROOT_R3BNEULANDMULTIPLICITYBAYES_H
#define R3BROOT_R3BNEULANDMULTIPLICITYBAYES_H

#include "FairTask.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandMultiplicity.h"
#include "R3BNeulandMultiplicityBayesPar.h"
#include <R3BIOConnector.h>

class R3BNeulandMultiplicityBayes : public FairTask
{
  public:
    explicit R3BNeulandMultiplicityBayes(std::string_view input = "NeulandClusters",
                                         std::string_view output = "NeulandMultiplicity");

  private:
    R3B::InputVectorConnector<R3BNeulandCluster> fClusters;
    std::unique_ptr<R3BNeulandMultiplicity> fMultiplicity;
    R3BNeulandMultiplicity* multiplicity_par_ptr = nullptr;
    std::string fOutputName;

    R3BNeulandMultiplicityBayesPar* fPar = nullptr;

    auto Init() -> InitStatus override;
    void SetParContainers() override;
    void Exec(Option_t* /*option*/) override;

    ClassDefOverride(R3BNeulandMultiplicityBayes, 0)
};

#endif // R3BROOT_R3BNEULANDMULTIPLICITYBAYES_H
