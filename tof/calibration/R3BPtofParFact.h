#ifndef R3BPTOFPARFACT_H
#define R3BPTOFPARFACT_H

#include "FairContFact.h"

#include "Rtypes.h"

class FairParSet;

class R3BPtofParFact : public FairContFact
{
  public:
    /**
     * Default constructor.
     */
    R3BPtofParFact();
    
    /**
     * Destructor.
     */
    ~R3BPtofParFact()
    {
    }
    
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

    ClassDef(R3BPtofParFact, 0);
};

#endif /* !R3BPTOFPARFACT_H */
