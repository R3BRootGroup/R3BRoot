
#include "R3BTrackingParticle.h"
#include "R3BTrackingDetector.h"
#include "FairLogger.h"

R3BTrackingParticle::R3BTrackingParticle()
    : fCharge(0.)
    , fStartPosition(0., 0., 0.)
    , fStartMomentum(0., 0., 0.)
    , fStartBeta(0.)
    , fMass(0.)
    , fPosition(0., 0., 0.)
    , fMomentum(0., 0., 0.)
    , fBeta(0.)
    , fLength(0.)
    , fChi2(0.)
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
    , fStartBeta(beta)
    , fMass(mass)
    , fPosition(x, y, z)
    , fMomentum(px, py, pz)
    , fBeta(beta)
    , fLength(0.)
    , fChi2(0.)
{
}

void R3BTrackingParticle::SetStartBeta(const Double_t& startBeta) { fStartBeta = startBeta; }

void R3BTrackingParticle::SetBeta(const Double_t& beta) { fBeta = beta; }

void R3BTrackingParticle::SetMass(const Double_t& mass) { fMass = mass; }

void R3BTrackingParticle::UpdateMomentum()
{
    Double_t startMom = fMass * fStartBeta * GetStartGamma();
    Double_t mom = fMass * fBeta * GetGamma();

    TVector3 direction1 = fStartMomentum.Unit();
    TVector3 direction2 = fMomentum.Unit();

    fStartMomentum = startMom * direction1;
    fMomentum = mom * direction2;
}

void R3BTrackingParticle::Reset()
{
    fLength = 0.;
    fPosition = fStartPosition;
    fMomentum = fStartMomentum;
    fBeta = fStartBeta;
}

R3BTrackingParticle::~R3BTrackingParticle() {}

void R3BTrackingParticle::PassThroughDetector(R3BTrackingDetector* det, Double_t weight)
{
    Double_t eloss = weight * det->GetEnergyLoss(this) * 1e-3;
    fBeta = fBeta - DeltaEToDeltaBeta(eloss);
    //    UpdateMomentum();
    Double_t mom = fMomentum.Mag();
    Double_t etot = TMath::Sqrt(fMomentum.Mag2() + fMass * fMass);
    Double_t mom2 = TMath::Sqrt(TMath::Power(etot - eloss, 2) - fMass * fMass);
    fMomentum = fMomentum * (mom2 / mom);
}

void R3BTrackingParticle::PassThroughDetectorBackward(R3BTrackingDetector* det, Double_t weight)
{
    Double_t eloss = weight * det->GetEnergyLoss(this) * 1e-3;
    fBeta = fBeta + DeltaEToDeltaBeta(eloss);
    //    UpdateMomentum();
    Double_t mom = fMomentum.Mag();
    Double_t etot = TMath::Sqrt(fMomentum.Mag2() + fMass * fMass);
    Double_t mom2 = TMath::Sqrt(TMath::Power(etot + eloss, 2) - fMass * fMass);
    fMomentum = fMomentum * (mom2 / mom);
}

Double_t R3BTrackingParticle::DeltaEToDeltaBeta(Double_t eloss)
{
    Double_t etot = TMath::Sqrt(fMomentum.Mag2() + fMass * fMass);
    Double_t d_beta = 1. / fBeta * TMath::Power(fMass / etot, 2) * eloss / etot;
    return d_beta;
}

ClassImp(R3BTrackingParticle)
