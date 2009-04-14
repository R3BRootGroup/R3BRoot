#ifndef R3BCALCONTFACT_H
#define R3BCALCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BCalContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BCalContFact();
  ~R3BCalContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BCalContFact,0) // Factory for all CALO parameter containers
};

#endif  /* !R3BCALCONTFACT_H */
