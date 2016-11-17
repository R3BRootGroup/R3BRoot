
#ifndef R3BTCONTFACT_H
#define R3BTCONTFACT_H

#include "FairContFact.h"

#include "Rtypes.h"

class FairParSet;

/**
 * Factory class for all TGeo parameter containers.
 * Currently supported:
 * @author D. Kresan
 * @since November 11, 2016
 */
class R3BTContFact : public FairContFact
{
  public:
    /**
     * Default constructor.
     */
    R3BTContFact();
    
    /**
     * Destructor.
     */
    ~R3BTContFact()
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

    ClassDef(R3BTContFact, 1);
};

#endif /* !R3BTCONTFACT_H */
