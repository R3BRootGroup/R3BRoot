#ifndef R3BMTOFCONTFACT_H
#define R3BMTOFCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BmTofContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BmTofContFact();
  ~R3BmTofContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BmTofContFact,0) // Factory for all MTOF parameter containers
};

#endif  /* !R3BMTOFCONTFACT_H */
