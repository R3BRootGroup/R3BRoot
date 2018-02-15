#ifndef R3BFI4CONTFACT_H
#define R3BFI4CONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BFi4ContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BFi4ContFact();
  ~R3BFi4ContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BFi4ContFact,0) // Factory for all Fi4 parameter containers
};

#endif  /* !R3BFI4CONTFACT_H */
