
#ifndef R3B_FRAGMENTFITTER_H
#define R3B_FRAGMENTFITTER_H

#include "FairTask.h"

#include <vector>
#include <string>

#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "Minuit2/Minuit2Minimizer.h"

class TClonesArray;
class R3BFieldPar;
class R3BTPropagator;
class R3BTrackingDetector;
class R3BTrackingParticle;
class R3BTrackingSetup;

class TH1F;

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

    Int_t FitFragment(R3BTrackingParticle* particle);
    
    Int_t FitFragmentBackward(R3BTrackingParticle* particle);

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
    Bool_t InitPropagator();
    
    ROOT::Math::Minimizer* fMinimum;

    R3BFieldPar* fFieldPar;
    R3BTPropagator* fPropagator;
    TClonesArray* fArrayMCTracks; // simulation output??? To compare?
    R3BTrackingSetup* fDetectors; // array of R3BTrackingDetector
    std::vector<R3BTrackingParticle*> fFragments;
    TClonesArray* fArrayFragments;
    Int_t fNEvents;
    Bool_t fVis;
    
    Double_t fAfterGladResolution;

    TH1F* fh_mult_psp;
    TH1F* fh_mult_fi4;
    TH1F* fh_mult_fi5;
    TH1F* fh_mult_tofd;
    TH1F* fh_eloss_psp_mc;
    TH1F* fh_eloss_psp;
    TH1F* fh_eloss_fi4_mc;
    TH1F* fh_eloss_fi4;
    TH1F* fh_ncand;
    TH1F* fh_x_res[6];
    TH1F* fh_x_pull[6];
    TH1F* fh_A_reco1;
    TH1F* fh_A_reco2;
    TH1F* fh_mom_res;
    TH1F* fh_mass_res;
    TH1F* fh_chi2;
    TH1F* fh_vz_res;

    ClassDef(R3BFragmentFitter, 1)
};

#endif
