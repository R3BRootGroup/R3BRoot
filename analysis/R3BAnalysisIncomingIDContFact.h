
#ifndef R3BAnalysisIncomingIDContFact_H
#define R3BAnalysisIncomingIDContFact_H

#include "FairContFact.h"

class FairContainer;

class R3BAnalysisIncomingIDContFact : public FairContFact
{
  private:
    void setAllContainers();

  public:
    R3BAnalysisIncomingIDContFact();
    ~R3BAnalysisIncomingIDContFact() {}
    FairParSet* createContainer(FairContainer*);
    ClassDef(R3BAnalysisIncomingIDContFact, 0)
};

#endif /* R3BMusicContFact_H */
