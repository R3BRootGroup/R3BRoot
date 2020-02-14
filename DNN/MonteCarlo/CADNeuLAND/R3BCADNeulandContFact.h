#ifndef R3BCADNEULANDCONTFACT_H
#define R3BCADNEULANDCONTFACT_H

#include "FairContFact.h"

class FairContainer;

/**
 * NeuLAND parameter container factory
 * @author Jan Mayer
 *
 * Lots of super brittle boilerplate code that should really not be necessary
 * TODO: Make FairRoot better so all of this can be deleted.
 */

class R3BCADNeulandContFact : public FairContFact
{
  private:
    void setAllContainers();

  public:
    R3BCADNeulandContFact();
    FairParSet* createContainer(FairContainer*) override;
    ClassDefOverride(R3BCADNeulandContFact, 0)
};

#endif // R3BCADNEULANDCONTFACT_H
