#ifndef R3BROOT_R3BPHASESPACEGENERATOR_H
#define R3BROOT_R3BPHASESPACEGENERATOR_H

// Wrapper for TGenPhaseSpace

#include <string>
#include <vector>
#include <functional>

#include "TGenPhaseSpace.h"
#include "TRandom3.h"
#include "TGraph.h"
#include "TF1.h"

#include "FairGenerator.h"
#include "FairIon.h"

#include "R3BDouble.h"

class R3BPhaseSpaceGenerator : public FairGenerator
{
  public:
    R3BPhaseSpaceGenerator();

    void AddParticle(const Int_t PDGCode);
    void AddHeavyIon(const FairIon& ion);

    void SetBeamEnergyAMeV(const Double_t EBeam_AMeV);
    void SetBeamEnergyDistAMeV(const std::function<Double_t(Double_t)> dist, const Double_t minE, const Double_t maxE,
                         const Int_t samples = 1000);
    void SetBeamEnergyDistAMeV(const TGraph& dist, const Double_t minE = -1., const Double_t maxE = -1.,
                         const Int_t samples = 1000);
    void SetBeamEnergyDistAMeV(const TF1& dist, const Double_t minE = -1., const Double_t maxE = -1.,
                         const Int_t samples = 1000);
                         
    void SetErelkeV(const Double_t Erel_keV);              
    void SetErelDistkeV(const std::function<Double_t(Double_t)> dist, const Double_t minE, const Double_t maxE,
                         const Int_t samples = 1000);
    void SetErelDistkeV(const TGraph& dist, const Double_t minE = -1., const Double_t maxE = -1.,
                         const Int_t samples = 1000);
    void SetErelDistkeV(const TF1& dist, const Double_t minE = -1., const Double_t maxE = -1.,
                         const Int_t samples = 1000);

    R3BDouble* GetBeamEnergyAMeV() { return &fBeamEnergy_AMeV;}                    
    R3BDouble* GetErelkeV() { return &fErel_keV;}
    R3BDouble* GetBeamBeta() { return &fBeta;}
    R3BDouble* GetBeamGamma() { return &fGamma;}
    
    Bool_t Init() override;
    Bool_t ReadEvent(FairPrimaryGenerator* primGen) override;
    // FairGenerator* CloneGenerator() const override;

  private:
    R3BDouble fGamma;
    R3BDouble fBeta;
    R3BDouble fBeamEnergy_AMeV;
    R3BDouble fErel_keV;
    TGraph fBeamEnergyLookup;
    TGraph fErelLookup;
    TRandom3 fRngGen;
    Bool_t fConstBeamEnergy;
    Bool_t fConstErel;
    TGenPhaseSpace fPhaseSpace;
    std::vector<Int_t> fPDGCodes;
    std::vector<Double_t> fMasses;

    TGraph SetupLookupGraph(std::function<Double_t(Double_t)> dist, const Double_t minE, const Double_t maxE, const Int_t samples);

    ClassDefOverride(R3BPhaseSpaceGenerator, 1);
};

#endif // R3BROOT_R3BPHASESPACEGENERATOR_H
