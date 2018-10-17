#ifndef R3BFIBERCONTFACT_H
#define R3BFIBERCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BFiberContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BFiberContFact();
  ~R3BFiberContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BFiberContFact,0) // Factory for all Fiber parameter containers
};

#endif  /* !R3BTOFCONTFACT_H */
