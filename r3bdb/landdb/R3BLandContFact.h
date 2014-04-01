#ifndef R3BLANDCONTFACT_H
#define R3BLANDCONTFACT_H

#include "FairContFact.h"               // for R3BLandContFact, etc

#include "Rtypes.h"                     // for R3BLandContFact::Class, etc

class FairParSet;

class R3BLandContFact : public FairContFact
{
  public:
    R3BLandContFact();
    ~R3BLandContFact() {}
    FairParSet* createContainer(FairContainer* c);
  private:
    void setAllContainers();

    ClassDef( R3BLandContFact,0); // Factory for parameter containers
};

#endif  /* !R3BLANDCONTFACT_H */
