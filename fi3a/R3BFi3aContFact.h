#ifndef R3BFI3aCONTFACT_H
#define R3BFI3aCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BFi3aContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BFi3aContFact();
  ~R3BFi3aContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BFi3aContFact,0) // Factory for all Fi4 parameter containers
};

#endif  /* !R3BFI3aCONTFACT_H */
