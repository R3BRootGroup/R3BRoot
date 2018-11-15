#ifndef R3BNEULANDPARFACT_H
#define R3BNEULANDPARFACT_H

#include "FairContFact.h"
#include "Rtypes.h"

class FairParSet;

class R3BNeulandParFact : public FairContFact
{
  public:
    /**
     * Default constructor.
     */
    R3BNeulandParFact();

    /**
     * Destructor.
     */
    ~R3BNeulandParFact() {}

    /**
     * Method to create a parameter container.
     * Called by FairRuntimeDB singleton class.
     * @param c a parameter container.
     * @return an instance of created parameter set.
     */
    FairParSet* createContainer(FairContainer* c);

  private:
    /**
     * Method to specify a list of parameter containers managed by this factory.
     * Support for additional detrectors has to be implemented in this function.
     */
    void setAllContainers();

    ClassDef(R3BNeulandParFact, 0);
};

#endif /* !R3BNEULANDPARFACT_H */
