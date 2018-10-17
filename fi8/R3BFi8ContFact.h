#ifndef R3BFI8CONTFACT_H
#define R3BFI8CONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BFi8ContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BFi8ContFact();
  ~R3BFi8ContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BFi8ContFact,0) // Factory for all Fi4 parameter containers
};

#endif  /* !R3BFI8CONTFACT_H */
