#ifndef R3BFI10CONTFACT_H
#define R3BFI10CONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BFi10ContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BFi10ContFact();
  ~R3BFi10ContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BFi10ContFact,0) // Factory for all Fi4 parameter containers
};

#endif  /* !R3BFI10CONTFACT_H */
