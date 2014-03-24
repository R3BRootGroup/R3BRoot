#ifndef R3BCALOCONTFACT_H
#define R3BCALOCONTFACT_H

#include "FairContFact.h"               // for R3BCaloContFact, etc

#include "Rtypes.h"                     // for R3BCaloContFact::Class, etc

class FairParSet;

class R3BCaloContFact : public FairContFact
{
  public:
    R3BCaloContFact();
    ~R3BCaloContFact() {}
    FairParSet* createContainer(FairContainer* c);
  private:
    void setAllContainers();

    ClassDef( R3BCaloContFact,0); // Factory for parameter containers
};

#endif  /* !R3BCALOCONTFACT_H */
