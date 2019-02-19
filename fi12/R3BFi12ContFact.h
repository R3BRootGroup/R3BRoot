#ifndef R3BFI12CONTFACT_H
#define R3BFI12CONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BFi12ContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BFi12ContFact();
  ~R3BFi12ContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BFi12ContFact,0) // Factory for all Fi4 parameter containers
};

#endif  /* !R3BFI12CONTFACT_H */
