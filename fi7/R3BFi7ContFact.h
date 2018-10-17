#ifndef R3BFI7CONTFACT_H
#define R3BFI7CONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BFi7ContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BFi7ContFact();
  ~R3BFi7ContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BFi7ContFact,0) // Factory for all Fi4 parameter containers
};

#endif  /* !R3BFI7CONTFACT_H */
