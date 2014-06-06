#ifndef R3BDBLOSCONTFACT_H
#define R3BDBLOSCONTFACT_H

#include "FairContFact.h"               // for R3BDBLosContFact, etc

#include "Rtypes.h"                     // for R3BDBLosContFact::Class, etc

class FairParSet;

class R3BDBLosContFact : public FairContFact
{
  public:
    R3BDBLosContFact();
    ~R3BDBLosContFact() {}
    FairParSet* createContainer(FairContainer* c);
  private:
    void setAllContainers();

    ClassDef( R3BDBLosContFact,0); // Factory for parameter containers
};

#endif  /* !R3BDBLOSCONTFACT_H */
