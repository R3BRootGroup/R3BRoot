#ifndef R3BNEULANDCONTFACT_H
#define R3BNEULANDCONTFACT_H

#include "FairContFact.h"

class FairContainer;

/**
 * NeuLAND parameter container factory
 * @author Jan Mayer
 *
 * Lots of super brittle boilerplate code that should really not be necessary
 * TODO: Make FairRoot better so all of this can be deleted.
 */

class R3BNeulandContFact : public FairContFact
{
  private:
    void setAllContainers();

  public:
    R3BNeulandContFact();
    FairParSet* createContainer(FairContainer*) override;
    ClassDefOverride(R3BNeulandContFact, 0)
};

#endif // R3BNEULANDCONTFACT_H
