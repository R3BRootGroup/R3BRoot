#pragma once

#include "FairTask.h"
#include "R3BNeulandHit.h"
#include "R3BNeulandMultiplicity.h"
#include <R3BIOConnector.h>
#include <Rtypes.h>
#include <RtypesCore.h>
#include <memory>
#include <string>
#include <string_view>

class R3BNeulandMultiplicityCheat : public FairTask
{
  public:
    explicit R3BNeulandMultiplicityCheat(std::string_view input = "NeulandPrimaryHits",
                                         std::string_view output = "NeulandMultiplicity");

  protected:
  private:
    R3B::InputVectorConnector<R3BNeulandHit> fPrimaryHits;
    std::unique_ptr<R3BNeulandMultiplicity> fMultiplicity;
    R3BNeulandMultiplicity* multiplicity_ptr_ = nullptr;
    std::string fOutputName;

    auto Init() -> InitStatus override;
    void Exec(Option_t* /*option*/) override;

    ClassDefOverride(R3BNeulandMultiplicityCheat, 1)
};
