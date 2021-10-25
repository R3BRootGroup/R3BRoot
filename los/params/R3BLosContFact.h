
#ifndef R3BLosContFact_H
#define R3BLosContFact_H

#include "FairContFact.h"

class FairContainer;

class R3BLosContFact : public FairContFact
{
  private:
    void setAllContainers();

  public:
    R3BLosContFact();
    ~R3BLosContFact() {}
    FairParSet* createContainer(FairContainer*);
    ClassDef(R3BLosContFact, 0)
};

#endif
