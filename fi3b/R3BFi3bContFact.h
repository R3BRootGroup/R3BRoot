#ifndef R3BFI3bCONTFACT_H
#define R3BFI3bCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BFi3bContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BFi3bContFact();
  ~R3BFi3bContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BFi3bContFact,0) // Factory for all Fi4 parameter containers
};

#endif  /* !R3BFI3bCONTFACT_H */
