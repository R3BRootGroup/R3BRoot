/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#ifndef R3B_FRAGMENTTRACKERS494_H
#define R3B_FRAGMENTTRACKERS494_H

#include "FairTask.h"

#include <string>
#include <vector>

class TClonesArray;
class R3BFieldPar;
class R3BTPropagator;
class R3BTrackingDetector;
class R3BTrackingParticle;
class R3BTrackingSetup;
class R3BFragmentFitterGeneric;

class TH1F;
class TH2F;

class R3BFragmentTrackerS494 : public FairTask
{
  public:
    R3BFragmentTrackerS494(const char* name, Bool_t vis = kFALSE, Int_t verbose = 1);
    virtual ~R3BFragmentTrackerS494();

    virtual InitStatus Init();
    virtual InitStatus ReInit();

    virtual void SetParContainers();

    virtual void Exec(const Option_t* = "");

    virtual void FinishEvent();
    
    virtual void Finish();

    void SetFragmentFitter(R3BFragmentFitterGeneric* fitter) { fFitter = fitter; }
    void SetEnergyLoss(Bool_t energyLoss) { fEnergyLoss = energyLoss; }
    void SetSimu(Int_t simu) {fSimu = simu;}
    void SetForward(Bool_t forward) {fForward = forward;}
    void SetOptimizeGeometry(Int_t optimizeGeometry) {fOptimizeGeometry = optimizeGeometry;}
    
  private:
    Bool_t InitPropagator();

    R3BFieldPar* fFieldPar;
    R3BTPropagator* fPropagator;
    TClonesArray* fArrayMCTracks; // simulation output??? To compare?
    R3BTrackingSetup* fDetectors; // array of R3BTrackingDetector
    R3BTrackingSetup* fDetectorsLeft; // array of R3BTrackingDetector
    R3BTrackingSetup* fDetectorsRight; // array of R3BTrackingDetector
    std::vector<R3BTrackingParticle*> fFragments;
    TClonesArray* fArrayFragments;
    TClonesArray* fTrackItems;
    TClonesArray* fMCTrack;
    std::vector<TClonesArray*> fMappedItems;
    std::vector<TClonesArray*> fCalItems;
    std::vector<TClonesArray*> fArrayHits;
    Int_t fNofTrackItems;    
    Int_t fNEvents;
    Int_t fNEventsLeft;
    Int_t fNEventsRight;
    Int_t counter1 = 0;
    
    enum DetectorInstances
    {
        DET_FI_FIRST,
        DET_FI23A = DET_FI_FIRST,
        DET_FI23B,
        DET_FI30,
        DET_FI31,
        DET_FI32,
        DET_FI33,
        DET_FI_LAST = DET_FI33,
        DET_TOFD,
        DET_MAX
    };

#define NOF_FIB_DET (DET_FI_LAST - DET_FI_FIRST + 1)

    const char* fDetectorNames[DET_MAX + 1] = { "Fi23a", "Fi23b", "Fi30",
                                                "Fi31",   "Fi32", "Fi33", "Tofd", NULL };
                                                
    Bool_t fVis;
//   	Double_t amu = 0.93149410242;
   	Double_t amu = 0.931494028;   // Gev/c**2
   	//0.938272;
   	Double_t totalChi2Mass = 0;
   	Double_t totalChi2P = 0;
    Int_t maxevent;
    R3BFragmentFitterGeneric* fFitter;
    Bool_t fEnergyLoss;
    Bool_t fSimu;
    Bool_t fForward;
    Bool_t fOptimizeGeometry;
    Double_t fAfterGladResolution;
    Int_t eventCounter = 0;

    TH1F* fh_mult_fi23a;
    TH1F* fh_mult_fi23b;
    TH1F* fh_mult_fi30;
    TH1F* fh_mult_fi31;
    TH1F* fh_mult_fi32;
    TH1F* fh_mult_fi33;
    TH1F* fh_mult_tofd;
    TH1F* fh_eloss_fi23a_mc;
    TH1F* fh_eloss_fi23a;
    TH1F* fh_eloss_fi23b_mc;
    TH1F* fh_eloss_fi23b;
    TH1F* fh_eloss_fi30_mc;
    TH1F* fh_eloss_fi30;
    TH1F* fh_eloss_fi31_mc;
    TH1F* fh_eloss_fi31;
    TH1F* fh_eloss_fi32_mc;
    TH1F* fh_eloss_fi32;
    TH1F* fh_eloss_fi33_mc;
    TH1F* fh_eloss_fi33;
    TH1F* fh_ncand;
    TH1F* fh_x_res[7];
    TH1F* fh_x_pull[7];
    TH1F* fh_y_res[7];
    TH1F* fh_y_pull[7];
    TH1F* fh_A_reco1;
    TH1F* fh_A_reco2;
    TH1F* fh_mom_res;
    TH1F* fh_mom_res_x;
    TH1F* fh_mom_res_y;
    TH1F* fh_mom_res_z;
    TH1F* fh_mass_res;
    TH1F* fh_chi2;
    TH1F* fh_vz_res;
    TH1F* fh_beta_res;
    TH1F* fh_p;
    TH1F* fh_px;
    TH1F* fh_py;
    TH1F* fh_pz;
    TH1F* fh_px_l;
    TH1F* fh_py_l;
    TH1F* fh_pz_l;
    TH1F* fh_px_r;
    TH1F* fh_py_r;
    TH1F* fh_pz_r;
    
   TH2F* fh_A_overZ;

    ClassDef(R3BFragmentTrackerS494, 1)
};

#endif
