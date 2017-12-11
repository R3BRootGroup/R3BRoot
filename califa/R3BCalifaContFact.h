#ifndef R3BCALIFACONTFACT_H
#define R3BCALIFACONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BCalifaContFact : public FairContFact {
 private:
  void setAllContainers();
 public:
  R3BCalifaContFact();
  ~R3BCalifaContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BCalifaContFact,0) // Factory for all CALIFA parameter containers
    };

#endif  /* !R3BCALIFACONTFACT_H */
