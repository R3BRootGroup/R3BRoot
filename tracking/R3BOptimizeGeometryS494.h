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

#ifndef R3B_OPTIMIZEGEOMETRYS494_H
#define R3B_OPTIMIZEGEOMETRYS494_H

#include "FairTask.h"

#include <string>
#include <vector>
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "Math/Minimizer.h"
#include "Minuit2/Minuit2Minimizer.h"
#include <TLorentzVector.h>

class TClonesArray;
class R3BFieldPar;
class R3BTPropagator;
class R3BTrackingDetector;
class R3BTrackingParticle;
class R3BTrackingSetup;
class R3BFragmentFitterGeneric;

class TH1F;
class TH2F;

class R3BOptimizeGeometryS494 : public FairTask
{
  public:
    R3BOptimizeGeometryS494(const char* name, Bool_t vis = kFALSE, Int_t verbose = 1);
    virtual ~R3BOptimizeGeometryS494();

    virtual InitStatus Init();
    virtual InitStatus ReInit();

    virtual void SetParContainers();

    virtual void Exec(const Option_t* = "");

    double Chi2();

    virtual void Finish();

    void SetFragmentFitter(R3BFragmentFitterGeneric* fitter) { fFitter = fitter; }
    void SetEnergyLoss(Bool_t energyLoss) { fEnergyLoss = energyLoss; }
    void SetSimu(Int_t simu) {fSimu = simu;}  
    void SetForward(Bool_t forward) {fForward = forward;}
    void SetOptimizeGeometry(Int_t optimizeGeometry) {fOptimizeGeometry = optimizeGeometry;}
    void SetBfield(Double_t Bfield) {fBfield = Bfield;}
    inline Int_t GetNEvents() const { return fNEvents; }


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
    std::vector<TClonesArray*> fArrayHits;
    Int_t fNEvents;

    R3BFragmentFitterGeneric* fFitter;
    Bool_t fEnergyLoss;
	Bool_t fSimu;
	Bool_t fOptimizeGeometry;
    Bool_t fForward;
	Int_t fSide;
    Double_t fAfterGladResolution;
    Int_t eventCounter = 0;
    Int_t counter1 = 0, fNEvents_nonull=0, counterHe=0, counterC=0;    
    Int_t fNEventsLeft = 0;
    Int_t fNEventsRight=0;
	Double_t w[14];
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
    TLorentzVector Oxygen; 
    TVector3 pos16O0;
    TVector3 pos16O3;    
                                           
    Bool_t fVis;
//   	Double_t amu = 0.93149410242;
   	Double_t amu = 0.931494028;   // Gev/c**2
   	//0.938272;
   	Double_t totalChi2Mass = 0;
   	Double_t totalChi2P = 0;
   	Double_t totalEvents = 0;
    Double_t fBfield;
    Int_t maxevent;
    Bool_t fWriteOut;
    Double_t minChi2;
    Double_t minChi2_12C;
    TLorentzVector alphaP, carbonP;
    TVector3 p12C;
    Double_t mHe = 3727.409;
	Double_t mC = 11174.950;
	Double_t x_l[8];
    Double_t y_l[8];
    Double_t det_hit_x[8];
    Double_t det_hit_y[8];
    Double_t det_hit_xC[8];
    Double_t det_hit_yC[8];
    Int_t fNwriteout=0;

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
    TH1F* fh_x_res[8];
    TH1F* fh_x_pull[8];
    TH1F* fh_y_res[8];
    TH1F* fh_y_pull[8];
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
    TH2F* fh_p_vs_ch2;
    TH2F* fh_xfi30_fi23a_track;
    TH2F* fh_xfi30_fi32_track;
    TH2F* fh_xfi30_tofd_track;
    TH2F* fh_xfi31_fi23a_track;
    TH2F* fh_xfi31_fi33_track;
    TH2F* fh_xfi31_tofd_track;
            
    TH2F* fh_xfi30_fi23a_exp;
    TH2F* fh_xfi30_fi32_exp;
    TH2F* fh_xfi30_tofd_exp;
    TH2F* fh_xfi31_fi23a_exp;
    TH2F* fh_xfi31_fi33_exp;
    TH2F* fh_xfi31_tofd_exp;   
    TH2F* fh_yC_vs_yHe_Tofd;
    TH2F* fh_yC_vs_yHe_Tofd_exp;
    TH2F* fh_tofd_track_exp;
    TH2F* fh_fi23b_track_exp;
    TH2F* fh_yFi23b_tofd_track;
    TH2F* fh_yFi23b_tofd_exp;
    TH2F* fh_pyC_vs_pyHe; 
    TH2F* fh_A_overZ;
    TH2F* fh_theta_16O;
    TH2F* fh_phi_16O;
    TH2F* fh_xfi23a_target_track;
    TH2F* fh_yfi23b_target_track;
    TH2F* fh_xy_target_C;
    TH2F* fh_xy_target_He;
    TH2F* fh_dxdy;
    TH2F* fh_mass_corel;
    TH2F* fh_mass_vs_ch2;
    
    ClassDef(R3BOptimizeGeometryS494, 1)
};

#endif
