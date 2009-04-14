#ifndef R3BPASSIVECONTFACT_H
#define R3BPASSIVECONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BPassiveContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BPassiveContFact();
  ~R3BPassiveContFact() {}
  FairParSet* createContainer(FairContainer*);
  ClassDef( R3BPassiveContFact,0) // Factory for all Passive parameter containers
};

#endif  /* !R3BPASSIVECONTFACT_H */
