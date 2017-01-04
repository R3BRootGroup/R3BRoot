
#include "R3BTrackingParticle.h"

R3BTrackingParticle::R3BTrackingParticle()
    : fCharge(0.)
    , fStartPosition(0., 0., 0.)
    , fStartMomentum(0., 0., 0.)
    , fBeta(0.)
    , fMass(0.)
    , fPosition(0., 0., 0.)
    , fMomentum(0., 0., 0.)
    , fLength(0.)
{
}

R3BTrackingParticle::R3BTrackingParticle(Double_t charge,
                                         Double_t x,
                                         Double_t y,
                                         Double_t z,
                                         Double_t px,
                                         Double_t py,
                                         Double_t pz,
                                         Double_t beta,
                                         Double_t mass)
    : fCharge(charge)
    , fStartPosition(x, y, z)
    , fStartMomentum(px, py, pz)
    , fBeta(beta)
    , fMass(mass)
    , fPosition(x, y, z)
    , fMomentum(px, py, pz)
    , fLength(0.)
{
}

void R3BTrackingParticle::SetBeta(const Double_t& beta) { fBeta = beta; }

void R3BTrackingParticle::SetMass(const Double_t& mass) { fMass = mass; }

void R3BTrackingParticle::UpdateMomentum()
{
    Double_t mom = fMass * fBeta * GetGamma();

    TVector3 direction1 = fStartMomentum.Unit();
    TVector3 direction2 = fMomentum.Unit();

    fStartMomentum = mom * direction1;
    fMomentum = mom * direction2;
}

void R3BTrackingParticle::Reset()
{
    fLength = 0.;
    fPosition = fStartPosition;
    fMomentum = fStartMomentum;
}

R3BTrackingParticle::~R3BTrackingParticle() {}

ClassImp(R3BTrackingParticle)
