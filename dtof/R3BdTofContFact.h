#ifndef R3BDTOFCONTFACT_H
#define R3BDTOFCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BdTofContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BdTofContFact();
  ~R3BdTofContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BdTofContFact,0) // Factory for all DTOF parameter containers
};

#endif  /* !R3BDTOFCONTFACT_H */
