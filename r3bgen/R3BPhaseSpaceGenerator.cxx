#include "R3BPhaseSpaceGenerator.h"
#include "R3BDistribution1D.h"
#include "R3BDistribution2D.h"
#include "R3BDistribution3D.h"

#include "FairLogger.h"
#include "FairPrimaryGenerator.h"
#include "FairRunSim.h"

#include "TDatabasePDG.h"
#include "TLorentzVector.h"
#include "TVector3.h"

#include <numeric>

R3BPhaseSpaceGenerator::R3BPhaseSpaceGenerator(UInt_t seed) 
    : fVertex_cm(R3BDistribution3D::Delta(0, 0, 0))
    , fBeamSpread_mRad({R3BDistribution1D::Delta(0), R3BDistribution1D::Flat(0, 2*TMath::Pi()*1e3)})
    , fBeamEnergy_AMeV(R3BDistribution1D::Delta(500))
    , fErel_keV(R3BDistribution1D::Delta(100))    
    , fRngGen(seed)
{
}

void R3BPhaseSpaceGenerator::AddParticle(const Int_t PDGCode){
    auto particle = TDatabasePDG::Instance()->GetParticle(PDGCode);
    if (particle == nullptr){
        LOG(FATAL) << __func__ << ": No such particle: " << PDGCode;
        return;
    }
    fMasses.push_back(particle->Mass());
    fPDGCodes.push_back(particle->PdgCode());
}

void R3BPhaseSpaceGenerator::AddHeavyIon(const FairIon& ion){
    auto run = FairRunSim::Instance();
    if (run == nullptr){
        LOG(WARNING) << __func__ << ": No FairRunSim";
    }
    else{
        run->AddNewIon((FairIon*)ion.Clone());
    }
    fMasses.push_back(ion.GetMass());
    fPDGCodes.push_back(1000000000 + 10 * 1000 * ion.GetZ() + 10 * ion.GetA());
}

Bool_t R3BPhaseSpaceGenerator::Init(){
    fTotMass = std::accumulate(fMasses.begin(), fMasses.end(), 0.);

    return kTRUE;
}

Bool_t R3BPhaseSpaceGenerator::ReadEvent(FairPrimaryGenerator* primGen){  
    const auto pos_cm = fVertex_cm.GetRandomValues({fRngGen.Rndm(), fRngGen.Rndm(), fRngGen.Rndm()});
    const auto spread_mRad = fBeamSpread_mRad.GetRandomValues({fRngGen.Rndm(), fRngGen.Rndm()});
    const auto beamEnergy_GeV = fBeamEnergy_AMeV.GetRandomValues({fRngGen.Rndm()})[0] * 1e-3;
    const auto erel_GeV = fErel_keV.GetRandomValues({fRngGen.Rndm()})[0] * (1e-6);

    const auto TotE_GeV = erel_GeV + fTotMass;
    const auto gamma = 1 + beamEnergy_GeV / 0.931494028;
    const auto beta = std::sqrt(1 - 1/(gamma*gamma));
    TLorentzVector Init(0.0, 0.0, 0.0, TotE_GeV);
    fPhaseSpace.SetDecay(Init, fMasses.size(), fMasses.data());
    fPhaseSpace.Generate();

    const size_t nParticles = fPDGCodes.size();

    TVector3 beam(0, 0, beta);
    beam.RotateX(spread_mRad[0]*1e-3);
    beam.RotateZ(spread_mRad[1]*1e-3);

    for (size_t i = 0; i < nParticles; i++){
        TLorentzVector* p = fPhaseSpace.GetDecay(i);
        p->Boost(beam);
        primGen->AddTrack(fPDGCodes.at(i), p->Px(), p->Py(), p->Pz(), pos_cm[0], pos_cm[1], pos_cm[2]);
    }
    return true;
}
