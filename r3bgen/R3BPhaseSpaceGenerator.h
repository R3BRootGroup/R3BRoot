#ifndef R3BROOT_R3BPHASESPACEGENERATOR_H
#define R3BROOT_R3BPHASESPACEGENERATOR_H

// Wrapper for TGenPhaseSpace

#include "FairGenerator.h"
#include "FairIon.h"
#include "TGenPhaseSpace.h"
#include <string>
#include <vector>

class R3BPhaseSpaceGenerator : public FairGenerator
{
  public:
    R3BPhaseSpaceGenerator();

    void AddParticle(const Int_t PDGCode);
    void AddHeavyIon(const FairIon& ion);

    void SetBeamEnergyAMeV(const Double_t EBeam_AMeV);
    void SetErelkeV(const Double_t Erel_keV) { fErel_keV = Erel_keV; }

    Bool_t Init() override;
    Bool_t ReadEvent(FairPrimaryGenerator* primGen) override;
    // FairGenerator* CloneGenerator() const override;

  private:
    Double_t fGamma;
    Double_t fBeta;
    Double_t fErel_keV;
    TGenPhaseSpace fPhaseSpace;
    std::vector<Int_t> fPDGCodes;
    std::vector<Double_t> fMasses;

    ClassDefOverride(R3BPhaseSpaceGenerator, 1);
};

#endif // R3BROOT_R3BPHASESPACEGENERATOR_H
