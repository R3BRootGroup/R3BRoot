#ifndef R3BFI6CONTFACT_H
#define R3BFI6CONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BFi6ContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BFi6ContFact();
  ~R3BFi6ContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BFi6ContFact,0) // Factory for all Fi4 parameter containers
};

#endif  /* !R3BFI6CONTFACT_H */
