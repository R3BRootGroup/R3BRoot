#ifndef R3BFRAGMENTFITTERGENERIC
#define R3BFRAGMENTFITTERGENERIC

#include "Rtypes.h"

class R3BTrackingParticle;
class R3BTrackingSetup;
class R3BTPropagator;

class R3BFragmentFitterGeneric
{
public:
    R3BFragmentFitterGeneric();
    virtual ~R3BFragmentFitterGeneric();
    
    virtual void Init(R3BTPropagator* prop = nullptr, Bool_t energyLoss = kTRUE) = 0;
    
    virtual Int_t FitTrack(R3BTrackingParticle*, R3BTrackingSetup*) = 0;
    
    virtual Int_t FitTrackBackward(R3BTrackingParticle*, R3BTrackingSetup*) = 0;
    
    ClassDef(R3BFragmentFitterGeneric, 1)
};

#endif
