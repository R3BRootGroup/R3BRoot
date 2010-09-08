#ifndef R3BVETOCONTFACT_H
#define R3BVETOCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BVetoContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BVetoContFact();
  ~R3BVetoContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BVetoContFact,0) // Factory for all VETO parameter containers
};

#endif  /* !R3BVETOCONTFACT_H */
