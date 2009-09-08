#ifndef ELILUMONCONTFACT_H
#define ELILUMONCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class ELILuMonContFact : public FairContFact {
private:
  void setAllContainers();
public:
  ELILuMonContFact();
  ~ELILuMonContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( ELILuMonContFact,0) // Factory for all TOF parameter containers
};

#endif  /* !ELILUMONCONTFACT_H */
