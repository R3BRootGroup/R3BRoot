#ifndef R3BSTARTRACKCONTFACT_H
#define R3BSTARTRACKCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BStartrackContFact : public FairContFact {
private:
  void setAllContainers();
public:
  R3BStartrackContFact();
  ~R3BStartrackContFact() {}
  FairParSet* createContainer(FairContainer*);
  void  activateParIo(FairParIo* io);
  ClassDef( R3BStartrackContFact,0) // Factory for all TRA parameter containers
};

#endif  /* !R3BTRACONTFACT_H */
