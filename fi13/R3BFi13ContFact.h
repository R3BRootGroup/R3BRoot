#ifndef R3BFI13CONTFACT_H
#define R3BFI13CONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BFi13ContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BFi13ContFact();
  ~R3BFi13ContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BFi13ContFact,0) // Factory for all Fi4 parameter containers
};

#endif  /* !R3BFI13CONTFACT_H */
