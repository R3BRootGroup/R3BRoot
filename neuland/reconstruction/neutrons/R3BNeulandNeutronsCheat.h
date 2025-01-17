#pragma once

#include "FairTask.h"
#include "R3BNeulandHit.h"
#include "R3BNeulandMultiplicity.h"
#include "R3BNeulandNeutron.h"
#include <R3BIOConnector.h>

class R3BNeulandNeutronsCheat : public FairTask
{
  public:
    explicit R3BNeulandNeutronsCheat(std::string_view inputMult = "NeulandMultiplicity",
                                     std::string_view inputHit = "NeulandPrimaryHits",
                                     std::string_view output = "NeulandNeutrons");

  protected:
  private:
    std::string fInputMultName;
    const R3BNeulandMultiplicity* fMultiplicity;             //!
    R3B::InputVectorConnector<R3BNeulandHit> fHits;          //!
    R3B::OutputVectorConnector<R3BNeulandNeutron> fNeutrons; //!

    auto Init() -> InitStatus override;
    void Exec(Option_t* /*option*/) override;
    ClassDefOverride(R3BNeulandNeutronsCheat, 1)
};
