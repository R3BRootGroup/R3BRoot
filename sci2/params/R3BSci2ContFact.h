
#ifndef R3BSci2ContFact_H
#define R3BSci2ContFact_H

#include "FairContFact.h"

class FairContainer;

class R3BSci2ContFact : public FairContFact
{
  private:
    void setAllContainers();

  public:
    R3BSci2ContFact();
    ~R3BSci2ContFact() {}
    FairParSet* createContainer(FairContainer*);
    ClassDef(R3BSci2ContFact, 0)
};

#endif
