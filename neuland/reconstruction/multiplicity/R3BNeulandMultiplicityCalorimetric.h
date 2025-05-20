#pragma once

#include "FairTask.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandMultiplicity.h"
#include "R3BNeulandMultiplicityCalorimetricPar.h"
#include <R3BIOConnector.h>
#include <Rtypes.h>
#include <RtypesCore.h>
#include <TString.h>
#include <memory>
#include <string_view>

class R3BNeulandMultiplicityCalorimetric : public FairTask
{
  public:
    explicit R3BNeulandMultiplicityCalorimetric(std::string_view input = "NeulandClusters",
                                                std::string_view output = "NeulandMultiplicity");
    void Exec(Option_t* /*option*/) override;

  private:
    R3B::InputVectorConnector<R3BNeulandCluster> fClusters;
    std::unique_ptr<R3BNeulandMultiplicity> fMultiplicity;
    R3BNeulandMultiplicity* multiplicity_par_ptr = nullptr;
    TString fOutputName;

    R3BNeulandMultiplicityCalorimetricPar* fPar;

    auto Init() -> InitStatus override;

    ClassDefOverride(R3BNeulandMultiplicityCalorimetric, 1);
};
