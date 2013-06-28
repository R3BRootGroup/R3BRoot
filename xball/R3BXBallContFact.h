#ifndef R3BXBALLCONTFACT_H
#define R3BXBALLCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BXBallContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BXBallContFact();
  ~R3BXBallContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BXBallContFact,0) // Factory for all CALO parameter containers
};

#endif  /* !R3BXBALLCONTFACT_H */
