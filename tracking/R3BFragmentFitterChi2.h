#ifndef R3BFRAGMENTFITTERCHI2
#define R3BFRAGMENTFITTERCHI2

#include "R3BFragmentFitterGeneric.h"

#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "Minuit2/Minuit2Minimizer.h"

class R3BFragmentFitterChi2 : public R3BFragmentFitterGeneric
{
public:
    R3BFragmentFitterChi2();
    ~R3BFragmentFitterChi2();
    
    void Init(R3BTPropagator* prop = nullptr, Bool_t energyLoss = kTRUE);
    
    Int_t FitTrack(R3BTrackingParticle*, R3BTrackingSetup*);
    
    Int_t FitTrackBackward(R3BTrackingParticle*, R3BTrackingSetup*);
    
    Double_t TrackFragment(R3BTrackingParticle* particle,
                           Bool_t energyLoss,
                           Double_t& devTof,
                           Double_t& time,
                           Double_t& chi2);
    Double_t DbetaDx(R3BTrackingParticle* candidate);
    Double_t DbetaChi2(R3BTrackingParticle* candidate);
    Double_t DbetaDt(R3BTrackingParticle* candidate);
    Double_t DmDx(R3BTrackingParticle* candidate, Bool_t energy_loss);
    Double_t DmDxTof(R3BTrackingParticle* candidate, Bool_t energy_loss);
    Double_t DmDt(R3BTrackingParticle* candidate, Bool_t energy_loss);
    Double_t Velocity(R3BTrackingParticle* candidate);

private:
    ROOT::Math::Minimizer* fMinimum;
    R3BTPropagator* fPropagator;
    
    ClassDef(R3BFragmentFitterChi2, 1)
};

#endif

