
#ifndef R3B_TRACKING_PARTICLE
#define R3B_TRACKING_PARTICLE

#include "TObject.h"
#include "TVector3.h"
#include "TMath.h"

class R3BTrackingDetector;

class R3BTrackingParticle : public TObject
{
  public:
    R3BTrackingParticle();

    R3BTrackingParticle(Double_t charge,
                        Double_t x,
                        Double_t y,
                        Double_t z,
                        Double_t px,
                        Double_t py,
                        Double_t pz,
                        Double_t beta,
                        Double_t mass);

    virtual ~R3BTrackingParticle();

    Double_t GetCharge() const { return fCharge; }
    void SetCharge(Double_t charge) { fCharge = charge; }

    void GetPosition(Double_t* xyz) const
    {
        xyz[0] = fPosition.X();
        xyz[1] = fPosition.Y();
        xyz[2] = fPosition.Z();
    }
    void GetMomentum(Double_t* p) const
    {
        p[0] = fMomentum.X();
        p[1] = fMomentum.Y();
        p[2] = fMomentum.Z();
    }
    void GetCosines(Double_t* cos) const
    {
        cos[3] = fMomentum.Mag();
        cos[0] = fMomentum.X() / cos[3];
        cos[1] = fMomentum.Y() / cos[3];
        cos[2] = fMomentum.Z() / cos[3];
    }

    void SetPosition(const TVector3& pos) { fPosition = pos; }
    void SetPosition(Double_t* xyz) { fPosition.SetXYZ(xyz[0], xyz[1], xyz[2]); }
    void SetMomentum(const TVector3& mom) { fMomentum = mom; }
    void SetMomentum(Double_t* p) { fMomentum.SetXYZ(p[0], p[1], p[2]); }
    void SetCosines(Double_t* cos) { fMomentum.SetXYZ(cos[0] * cos[3], cos[1] * cos[3], cos[2] * cos[3]); }

    void SetStartBeta(const Double_t& startBeta);
    void SetBeta(const Double_t& beta);
    void SetMass(const Double_t& mass);
    void UpdateMomentum();

    void AddStep(Double_t step) { fLength += step; }

    void SetChi2(Double_t chi2) { fChi2 = chi2; }

    Double_t GetX() const { return fPosition.X(); }
    Double_t GetY() const { return fPosition.Y(); }
    Double_t GetZ() const { return fPosition.Z(); }
    const TVector3& GetPosition() const { return fPosition; }

    const TVector3& GetMomentum() const { return fMomentum; }

    const TVector3& GetStartPosition() const { return fStartPosition; }
    const TVector3& GetStartMomentum() const { return fStartMomentum; }
    Double_t GetStartBeta() const { return fStartBeta; }
    Double_t GetStartGamma() const { return TMath::Sqrt(1. / (1. - fStartBeta * fStartBeta)); }

    Double_t GetBeta() const { return fBeta; }
    Double_t GetGamma() const { return TMath::Sqrt(1. / (1. - fBeta * fBeta)); }
    Double_t GetMass() const { return fMass; }
    Double_t GetLength() const { return fLength; }

    Double_t GetChi2() const { return fChi2; }

    void PassThroughDetector(R3BTrackingDetector* det, Double_t weight = 1.);
    void PassThroughDetectorBackward(R3BTrackingDetector* det, Double_t weight = 1.);

    void SetStartPosition(const TVector3& startPosition) { fStartPosition = startPosition; }
    void SetStartMomentum(const TVector3& startMomentum) { fStartMomentum = startMomentum; }

    Double_t DeltaEToDeltaBeta(Double_t eloss);

    void Reset();

  private:
    Double_t fCharge;
    TVector3 fStartPosition;
    TVector3 fStartMomentum;
    Double_t fStartBeta;
    Double_t fMass;

    TVector3 fPosition;
    TVector3 fMomentum;
    Double_t fBeta;
    Double_t fLength;

    Double_t fChi2;

    ClassDef(R3BTrackingParticle, 1)
};

#endif
