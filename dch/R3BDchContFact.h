#ifndef R3BDCHCONTFACT_H
#define R3BDCHCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BDchContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BDchContFact();
  ~R3BDchContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BDchContFact,0) // Factory for all DCH parameter containers
};

#endif  /* !R3BDCHCONTFACT_H */
