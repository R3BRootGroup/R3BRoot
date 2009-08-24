#ifndef R3BCHIMERACONTFACT_H
#define R3BCHIMERACONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BChimeraContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BChimeraContFact();
  ~R3BChimeraContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BChimeraContFact,0) // Factory for all TOF parameter containers
};

#endif  /* !R3BCHIMERACONTFACT_H */
