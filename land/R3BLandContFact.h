#ifndef R3BLANDCONTFACT_H
#define R3BLANDCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BLandContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BLandContFact();
  ~R3BLandContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BLandContFact,0) // Factory for all LAND parameter containers
};

#endif  /* !R3BLANDCONTFACT_H */
