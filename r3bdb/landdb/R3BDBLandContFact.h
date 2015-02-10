#ifndef R3BDBLANDCONTFACT_H
#define R3BDBLANDCONTFACT_H

#include "FairContFact.h"               // for R3BDBLandContFact, etc

#include "Rtypes.h"                     // for R3BDBLandContFact::Class, etc

class FairParSet;

class R3BDBLandContFact : public FairContFact
{
  public:
    R3BDBLandContFact();
    ~R3BDBLandContFact() {}
    FairParSet* createContainer(FairContainer* c);

  private:
    void setAllContainers();

    ClassDef( R3BDBLandContFact,0); // Factory for parameter containers
};

#endif  /* !R3BDBLANDCONTFACT_H */
