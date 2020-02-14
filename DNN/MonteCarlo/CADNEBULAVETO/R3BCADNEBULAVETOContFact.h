#ifndef R3BCADNEBULAVETOCONTFACT_H
#define R3BCADNEBULAVETOCONTFACT_H

#include "FairContFact.h"

class FairContainer;

/**
 * NEBULAVETO parameter container factory
 * @author Jan Mayer
 *
 * Lots of super brittle boilerplate code that should really not be necessary
 * TODO: Make FairRoot better so all of this can be deleted.
 */

class R3BCADNEBULAVETOContFact : public FairContFact
{
  private:
    void setAllContainers();

  public:
    R3BCADNEBULAVETOContFact();
    FairParSet* createContainer(FairContainer*) override;
    ClassDefOverride(R3BCADNEBULAVETOContFact, 0)
};

#endif // R3BCADNEBULAVETOCONTFACT_H
