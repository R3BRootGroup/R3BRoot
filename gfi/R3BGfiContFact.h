#ifndef R3BGFICONTFACT_H
#define R3BGFICONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BGfiContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BGfiContFact();
  ~R3BGfiContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BGfiContFact,0) // Factory for all GFI parameter containers
};

#endif  /* !R3BGFICONTFACT_H */
