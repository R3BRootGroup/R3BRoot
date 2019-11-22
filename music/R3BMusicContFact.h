// ------------------------------------------------------------------
// -----             R3BMusicContFact source file               -----
// -----         Created 24/11/19  by J.L. Rodriguez-Sanchez    -----
// ------------------------------------------------------------------

#ifndef R3BMusicContFact_H
#define R3BMusicContFact_H

#include "FairContFact.h"

class FairContainer;

class R3BMusicContFact : public FairContFact
{
  private:
    void setAllContainers();

  public:
    R3BMusicContFact();
    ~R3BMusicContFact() {}
    FairParSet* createContainer(FairContainer*);
    void activateParIo(FairParIo* io);
    ClassDef(R3BMusicContFact, 0) // Factory for all R3BMUSIC parameter containers
};

#endif /* R3BMusicContFact_H */
