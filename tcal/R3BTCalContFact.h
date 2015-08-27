#ifndef R3BTCALCONTFACT_H
#define R3BTCALCONTFACT_H

#include "FairContFact.h"

#include "Rtypes.h"

class FairParSet;

class R3BTCalContFact : public FairContFact
{
  public:
    R3BTCalContFact();
    ~R3BTCalContFact()
    {
    }
    FairParSet* createContainer(FairContainer* c);

  private:
    void setAllContainers();

    ClassDef(R3BTCalContFact, 1);
};

#endif /* !R3BTCALCONTFACT_H */
