#ifndef R3BROOT_R3BNEULANDNEUTRONSCHEAT_H
#define R3BROOT_R3BNEULANDNEUTRONSCHEAT_H

#include "FairTask.h"
#include "R3BNeulandHit.h"
#include "R3BNeulandMultiplicity.h"
#include "R3BNeulandNeutron.h"
#include "TCAConnector.h"

class R3BNeulandNeutronsCheat : public FairTask
{
  public:
    R3BNeulandNeutronsCheat(TString inputMult = "NeulandMultiplicity",
                            TString inputHit = "NeulandPrimaryHits",
                            TString output = "NeulandNeutrons");
    ~R3BNeulandNeutronsCheat() override = default;
    void Exec(Option_t*) override;

  protected:
    InitStatus Init() override;

  private:
    const TString fInputMultName;
    const R3BNeulandMultiplicity* fMultiplicity;     //!
    TCAInputConnector<R3BNeulandHit> fHits;          //!
    TCAOutputConnector<R3BNeulandNeutron> fNeutrons; //!

    ClassDefOverride(R3BNeulandNeutronsCheat, 0)
};

#endif // R3BROOT_R3BNEULANDNEUTRONSCHEAT_H
