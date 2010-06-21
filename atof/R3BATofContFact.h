#ifndef R3BATOFCONTFACT_H
#define R3BATOFCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BATofContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BATofContFact();
  ~R3BATofContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BATofContFact,0) // Factory for all TOF parameter containers
};

#endif  /* !R3BATOFCONTFACT_H */
