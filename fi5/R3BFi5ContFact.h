#ifndef R3BFI5CONTFACT_H
#define R3BFI5CONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BFi5ContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BFi5ContFact();
  ~R3BFi5ContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BFi5ContFact,0) // Factory for all Fi4 parameter containers
};

#endif  /* !R3BFI5CONTFACT_H */
