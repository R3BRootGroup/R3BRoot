#ifndef R3BSTARTRACONTFACT_H
#define R3BSTARTRACONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BSTaRTraContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BSTaRTraContFact();
  ~R3BSTaRTraContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BSTaRTraContFact,0) // Factory for all TRA parameter containers
};

#endif  /* !R3BTRACONTFACT_H */
