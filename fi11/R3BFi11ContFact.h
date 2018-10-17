#ifndef R3BFI11CONTFACT_H
#define R3BFI11CONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BFi11ContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BFi11ContFact();
  ~R3BFi11ContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BFi11ContFact,0) // Factory for all Fi4 parameter containers
};

#endif  /* !R3BFI11CONTFACT_H */
