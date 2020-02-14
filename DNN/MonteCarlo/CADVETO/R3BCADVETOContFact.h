#ifndef R3BCADVETOCONTFACT_H
#define R3BCADVETOCONTFACT_H

#include "FairContFact.h"

class FairContainer;

/**
 * VETO parameter container factory
 * @author Jan Mayer
 *
 * Lots of super brittle boilerplate code that should really not be necessary
 * TODO: Make FairRoot better so all of this can be deleted.
 */

class R3BCADVETOContFact : public FairContFact
{
  private:
    void setAllContainers();

  public:
    R3BCADVETOContFact();
    FairParSet* createContainer(FairContainer*) override;
    ClassDefOverride(R3BCADVETOContFact, 0)
};

#endif // R3BCADVETOCONTFACT_H
