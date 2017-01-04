
#ifndef R3B_FRAGMENTFITTER_H
#define R3B_FRAGMENTFITTER_H

#include "FairTask.h"

class TClonesArray;
class R3BFieldPar;
class R3BTPropagator;
class R3BTrackingDetector;
class R3BTrackingParticle;

class R3BFragmentFitter : public FairTask
{
  public:
    R3BFragmentFitter(const char* name, Bool_t vis = kFALSE, Int_t verbose = 1);
    virtual ~R3BFragmentFitter();

    virtual InitStatus Init();
    virtual InitStatus ReInit();

    virtual void SetParContainers();

    virtual void Exec(const Option_t* = "");

    virtual void Finish();

    void FitFragment(R3BTrackingParticle* particle);

    Double_t TrackFragment(R3BTrackingParticle* particle);

    Double_t DbetaDx(R3BTrackingParticle* candidate);
    Double_t DmDx(R3BTrackingParticle* candidate);
    Double_t Velocity(R3BTrackingParticle* candidate);

  private:
    Bool_t InitPropagator();

    R3BFieldPar* fFieldPar;
    R3BTPropagator* fPropagator;
    TClonesArray* fArrayMCTracks;  // simulation output??? To compare?
    TClonesArray* fArrayDetectors; // array of R3BTrackingDetector
    Int_t fNEvents;
    Bool_t fVis;

    ClassDef(R3BFragmentFitter, 1)
};

#endif
