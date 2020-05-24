#ifndef R3BROOT_R3BNEULANDMULTIPLICITYFIXED_H
#define R3BROOT_R3BNEULANDMULTIPLICITYFIXED_H

#include "FairTask.h"
#include "R3BNeulandMultiplicity.h"

class R3BNeulandMultiplicityFixed : public FairTask
{
  public:
    R3BNeulandMultiplicityFixed(const int mult, TString output = "NeulandMultiplicity");
    ~R3BNeulandMultiplicityFixed() override;

    void Exec(Option_t*) override;

  protected:
    InitStatus Init() override;

  private:
    const int fMult;
    R3BNeulandMultiplicity* fMultiplicity;
    TString fOutputName;

    ClassDefOverride(R3BNeulandMultiplicityFixed, 0)
};

#endif // R3BROOT_R3BNEULANDMULTIPLICITYFIXED_H
