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

#ifndef R3BFRAGMENTFITTERCHI2S494
#define R3BFRAGMENTFITTERCHI2S494

#include "R3BFragmentFitterGeneric.h"

#include "Math/Factory.h"
#include "Math/Functor.h"
#include "Math/Minimizer.h"
#include "Minuit2/Minuit2Minimizer.h"
#include "Math/GenAlgoOptions.h"
#include "TMatrixD.h"    
#include "TDecompLU.h"
#include "TMatrixDSym.h" 
#include "TVectorD.h"  
#include <vector>  

class R3BTrackingParticle;
class R3BTrackingSetup;
class R3BTPropagator;

class R3BFragmentFitterChi2S494 : public R3BFragmentFitterGeneric
{
  public:
    R3BFragmentFitterChi2S494();
    virtual ~R3BFragmentFitterChi2S494();
    
    virtual void Init(R3BTPropagator* prop = nullptr, Bool_t energyLoss = kTRUE);

    virtual Int_t FitTrack(R3BTrackingParticle*, R3BTrackingSetup*);
    
    virtual Int_t FitTrackBeta(R3BTrackingParticle*, R3BTrackingSetup*);

    virtual Int_t FitTrackMomentumForward(R3BTrackingParticle*, R3BTrackingSetup*);

    virtual Int_t FitTrackMomentumKalmanFilter(R3BTrackingParticle*, R3BTrackingSetup*);
    
    virtual Int_t FitTrackMomentumKalmanFilterRTS(R3BTrackingParticle*, R3BTrackingSetup*);
    
    Bool_t RunRTSBackwardSmoother(TMatrixD& smoothed_x_target, 
                                                      TMatrixD& smoothed_P_target);
    
    
    // Define the data structure for holding plane history
	struct TwoFilterDataPlane {
		Int_t detID;
		TMatrixD x_fwd;     // Forward filtered state x_{k|k}
		TMatrixD P_fwd;     // Forward filtered covariance P_{k|k}
		TMatrixD x_bwd;     // Backward predicted state x_{k|k+1}
		TMatrixD P_bwd;     // Backward predicted covariance P_{k|k+1}
		
		TMatrixD x_smooth;  // Output: Smoothed state x_{k|N}
		TMatrixD P_smooth;  // Output: Smoothed covariance P_{k|N}

		// Default constructor allocating 5x1 state and 5x5 covariance matrices
		TwoFilterDataPlane(Int_t id = -1) 
			: detID(id), 
			  x_fwd(5, 1), P_fwd(5, 5), 
			  x_bwd(5, 1), P_bwd(5, 5),
			  x_smooth(5, 1), P_smooth(5, 5) {}
	};

    Bool_t RunWeightedMeansSmoother(std::vector<TwoFilterDataPlane>& history, 
								   TMatrixD& smoothed_x_fi23, 
                                   TMatrixD& smoothed_P_fi23);
                                                      
    virtual Int_t FitTrackMomentumForwardMinuit(R3BTrackingParticle*, R3BTrackingSetup*);
    
    virtual Int_t FitTrackMomentumBackward(R3BTrackingParticle*, R3BTrackingSetup*);

    virtual Int_t FitTrackBackward(R3BTrackingParticle*, R3BTrackingSetup*);
    
    virtual Int_t FitTrackBackward2D(R3BTrackingParticle*, R3BTrackingSetup*);

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
    /** RTS Smoother
	 * Container holding recorded state vectors, covariances, and 
	 * accumulated transport matrices at each detector plane during the forward pass.
	 */
	struct RTSDataPlane {
		Int_t detID;         // Detector plane index/identifier
		TMatrixD x_pred;     // x_{k|k-1}  (5x1 State BEFORE hit update)
		TMatrixD P_pred;     // P_{k|k-1}  (5x5 Covariance BEFORE hit update)
		TMatrixD x_post;     // x_{k|k}    (5x1 State AFTER hit update)
		TMatrixD P_post;     // P_{k|k}    (5x5 Covariance AFTER hit update)
		TMatrixD F_trans;    // F_k        (5x5 Net transport matrix from plane k-1 to k)

		// --- ADDED FOR RTS SMOOTHED CALCULATIONS ---
		TMatrixD x_smooth;   // x_{k|N}    (5x1 Fully Smoothed State)
		TMatrixD P_smooth;   // P_{k|N}    (5x5 Fully Smoothed Covariance)

		RTSDataPlane(Int_t id = -1) 
			: detID(id), x_pred(5, 1), P_pred(5, 5), x_post(5, 1), P_post(5, 5), 
			  F_trans(5, 5), x_smooth(5, 1), P_smooth(5, 5) {}
	};
   
  private:
    ROOT::Math::Minimizer* fMinimum;
    ROOT::Math::Minimizer* minimum_m;
    ROOT::Math::Minimizer* minimum_g;
    
    /**
	 * Safely inverts a square TMatrixD in-place.
	 * 
	 * @param mat Matrix to be inverted in-place.
	 * @return true if inversion succeeded and matrix is valid, false if singular/invalid.
	 */
	Bool_t SafeInvertMatrix(TMatrixD& mat) {
		TDecompLU lu(mat);
		if (!lu.Decompose()) {
			return kFALSE; // Matrix is singular / non-invertible
		}
		return lu.Invert(mat); // Inverts mat in-place safely
	}
	std::vector<RTSDataPlane> fRTSHistory;
    
    R3BTPropagator* fPropagator;
   	Double_t amu = 0.931494028;   // Gev/c**2
    TVector3 pinv;
    Double_t rad_length = 40.; //radiation length in cm

    ClassDef(R3BFragmentFitterChi2S494, 1)
};

#endif
