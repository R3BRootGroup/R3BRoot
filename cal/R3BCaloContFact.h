#ifndef R3BCALOCONTFACT_H
#define R3BCALOCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BCaloContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BCaloContFact();
  ~R3BCaloContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BCaloContFact,0) // Factory for all CALO parameter containers
};

#endif  /* !R3BCALOCONTFACT_H */
