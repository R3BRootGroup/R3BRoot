#ifndef R3BDBCONTFACT_H
#define R3BDBCONTFACT_H

#include "FairContFact.h"               // for R3BDBContFact, etc

#include "Rtypes.h"                     // for R3BDBContFact::Class, etc

class FairParSet;

class R3BDBContFact : public FairContFact
{
  public:
    R3BDBContFact();
    ~R3BDBContFact() {}
    FairParSet* createContainer(FairContainer* c);
  private:
    void setAllContainers();

    ClassDef( R3BDBContFact,0); // Factory for parameter containers
};

#endif  /* !R3BDBCONTFACT_H */
