#ifndef R3BTRACONTFACT_H
#define R3BTRACONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BTraContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BTraContFact();
  ~R3BTraContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BTraContFact,0) // Factory for all TRA parameter containers
};

#endif  /* !R3BTRACONTFACT_H */
