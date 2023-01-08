/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#ifndef R3B_T_PROPAGATOR
#define R3B_T_PROPAGATOR

#include "TCanvas.h"
#include "TObject.h"
#include "TVector3.h"

class R3BGladFieldMap;
class FairRKPropagator;
class R3BTGeoPar;
class FairField;
class R3BTrackingParticle;
class R3BTrackingDetector;

class R3BTPropagator : public TObject
{
  public:
    R3BTPropagator(R3BGladFieldMap* field, Bool_t vis = kFALSE);

    virtual ~R3BTPropagator();

    inline void SetmTofGeo(R3BTGeoPar* geo) { fmTofGeo = geo; }

    Bool_t PropagateToDetector(R3BTrackingParticle* particle, R3BTrackingDetector* detector);

    Bool_t PropagateToDetectorBackward(R3BTrackingParticle* particle, R3BTrackingDetector* detector);

    Bool_t PropagateToPlane(R3BTrackingParticle* particle, const TVector3& v1, const TVector3& v2, const TVector3& v3);

    Bool_t PropagateToPlaneBackward(R3BTrackingParticle* particle,
                                    const TVector3& v1,
                                    const TVector3& v2,
                                    const TVector3& v3);

    Bool_t PropagateToPlaneRK(R3BTrackingParticle* particle,
                              const TVector3& v1,
                              const TVector3& v2,
                              const TVector3& v3);

    Bool_t LineIntersectPlane(const TVector3& pos,
                              const TVector3& mom,
                              const TVector3& v1,
                              const TVector3& normal,
                              TVector3& intersect);

    void SetVis(Bool_t vis = kTRUE) { fVis = vis; }

  private:
    FairRKPropagator* fFairProp;

    FairField* fField;

    R3BTGeoPar* fmTofGeo;

    Bool_t fVis;

    TVector3 fPlane1[3];
    TVector3 fPlane2[3];
    TVector3 fNorm1;
    TVector3 fNorm2;

    TCanvas* fc4;

    ClassDef(R3BTPropagator, 1)
};

#endif //! R3B_T_PROPAGATOR
