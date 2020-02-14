#ifndef R3BCADNEBULACONTFACT_H
#define R3BCADNEBULACONTFACT_H

#include "FairContFact.h"

class FairContainer;

/**
 * NEBULA parameter container factory
 * @author Jan Mayer
 *
 * Lots of super brittle boilerplate code that should really not be necessary
 * TODO: Make FairRoot better so all of this can be deleted.
 */

class R3BCADNEBULAContFact : public FairContFact
{
  private:
    void setAllContainers();

  public:
    R3BCADNEBULAContFact();
    FairParSet* createContainer(FairContainer*) override;
    ClassDefOverride(R3BCADNEBULAContFact, 0)
};

#endif // R3BCADNEBULACONTFACT_H
