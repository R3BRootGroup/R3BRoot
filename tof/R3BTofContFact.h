#ifndef R3BTOFCONTFACT_H
#define R3BTOFCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BTofContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BTofContFact();
  ~R3BTofContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BTofContFact,0) // Factory for all TOF parameter containers
};

#endif  /* !R3BTOFCONTFACT_H */
