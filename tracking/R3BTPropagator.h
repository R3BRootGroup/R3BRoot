
#ifndef R3B_T_PROPAGATOR
#define R3B_T_PROPAGATOR

#include "TObject.h"
#include "TVector3.h"
#include "TCanvas.h"

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

    Bool_t PropagateToDetector(R3BTrackingParticle* particle,
                               R3BTrackingDetector* detector);
    
    Bool_t PropagateToDetectorBackward(R3BTrackingParticle* particle,
                                       R3BTrackingDetector* detector);

    Bool_t PropagateToPlane(R3BTrackingParticle* particle,
                            const TVector3& v1,
                            const TVector3& v2,
                            const TVector3& v3);
    
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
