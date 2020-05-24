#ifndef R3BROOT_R3BNEULANDMULTIPLICITYCHEAT_H
#define R3BROOT_R3BNEULANDMULTIPLICITYCHEAT_H

#include "FairTask.h"
#include "R3BNeulandHit.h"
#include "R3BNeulandMultiplicity.h"
#include "TCAConnector.h"

class R3BNeulandMultiplicityCheat : public FairTask
{
  public:
    R3BNeulandMultiplicityCheat(TString input = "NeulandPrimaryHits", TString output = "NeulandMultiplicity");
    ~R3BNeulandMultiplicityCheat() override;

    void Exec(Option_t*) override;

  protected:
    InitStatus Init() override;

  private:
    TCAInputConnector<R3BNeulandHit> fPrimaryHits;
    R3BNeulandMultiplicity* fMultiplicity;
    TString fOutputName;

    ClassDefOverride(R3BNeulandMultiplicityCheat, 0)
};

#endif // R3BROOT_R3BNEULANDMULTIPLICITYCHEAT_H
