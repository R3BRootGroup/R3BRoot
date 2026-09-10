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

#ifndef R3BFRAGMENTFITTERGENERIC
#define R3BFRAGMENTFITTERGENERIC

#include "Rtypes.h"
#include "TMatrixD.h"    

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
    
    virtual Int_t FitTrackBeta(R3BTrackingParticle*, R3BTrackingSetup*) = 0;
    
    virtual Int_t FitTrackMomentumForward(R3BTrackingParticle*, R3BTrackingSetup*) = 0;
   
    virtual Int_t FitTrackMomentumBackward(R3BTrackingParticle*, R3BTrackingSetup*) = 0;
   
    virtual Int_t FitTrackMomentumKalmanFilter(R3BTrackingParticle*, R3BTrackingSetup*) = 0;

    virtual Int_t FitTrackBackward(R3BTrackingParticle*, R3BTrackingSetup*) = 0;

    virtual Int_t FitTrackMomentumForwardMinuit(R3BTrackingParticle*, R3BTrackingSetup*) = 0;
    
    virtual Int_t FitTrackBackward2D(R3BTrackingParticle*, R3BTrackingSetup*) = 0;
    
   virtual void SetCarbonTargetCovariance(const TMatrixD& cov) {
    fbestCarbonTargetCov.ResizeTo(5, 5); // FORCE 5x5 allocation!
    
    if (cov.GetNrows() == 5 && cov.GetNcols() == 5) {
        fbestCarbonTargetCov = cov;
    } else {
        // Fallback: If passed matrix isn't 5x5, zero it out safely
        fbestCarbonTargetCov.Zero();
    }
    fHasCarbonVertexCov = true; // Set flag to 1
	}

	virtual void ResetVertexCovariance() {
		fbestCarbonTargetCov.ResizeTo(5, 5);
		fbestCarbonTargetCov.Zero();
		fHasCarbonVertexCov = false; // Reset flag to 0
	}
    
    protected:
    // Make these protected so R3BFragmentFitterChi2S494 can read them directly
    TMatrixD fbestCarbonTargetCov; //! Do not stream to ROOT I/O
    bool fHasCarbonVertexCov;      //! Do not stream to ROOT I/O

    ClassDef(R3BFragmentFitterGeneric, 1)
};

#endif
