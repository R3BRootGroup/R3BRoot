#ifndef R3BLANDTSYNCCONTFACT_H
#define R3BLANDTSYNCCONTFACT_H

#include "FairContFact.h"

#include "Rtypes.h"

class FairParSet;

class R3BLandTSyncContFact : public FairContFact
{
  public:
    /**
     * Default constructor.
     */
    R3BLandTSyncContFact();
    
    /**
     * Destructor.
     */
    ~R3BLandTSyncContFact()
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

    ClassDef(R3BLandTSyncContFact, 0);
};

#endif /* !R3BLANDTSYNCCONTFACT_H */
