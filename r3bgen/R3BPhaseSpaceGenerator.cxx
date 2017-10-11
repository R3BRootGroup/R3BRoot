#include "R3BPhaseSpaceGenerator.h"
#include "FairLogger.h"
#include "FairPrimaryGenerator.h"
#include "FairRunSim.h"
#include "TDatabasePDG.h"
#include <numeric>

Bool_t R3BPhaseSpaceGenerator::Init()
{
    const Double_t TotE_GeV = fErel_keV / (1000. * 1000.) + std::accumulate(fMasses.begin(), fMasses.end(), 0.);
    TLorentzVector Init(0.0, 0.0, 0.0, TotE_GeV);
    fPhaseSpace.SetDecay(Init, fMasses.size(), fMasses.data());
    return true;
}

Bool_t R3BPhaseSpaceGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{
    fPhaseSpace.Generate();

    const size_t nParticles = fPDGCodes.size();
    for (size_t i = 0; i < nParticles; i++)
    {
        const TLorentzVector* p = fPhaseSpace.GetDecay(i);

        // Apply boost
        const Double_t pz = fBeta * fGamma * p->E() + fGamma * p->Pz();

        primGen->AddTrack(fPDGCodes.at(i), p->Px(), p->Py(), pz, 0., 0., 0.);
    }
    return true;
}

void R3BPhaseSpaceGenerator::SetBeamEnergyAMeV(const Double_t EBeam_AMeV)
{
    fGamma = 1 + (EBeam_AMeV / 1000.) / 0.931494028; // MeV/A -> GeV/A
    fBeta = std::sqrt(1 - 1 / std::pow(fGamma, 2));
}

void R3BPhaseSpaceGenerator::AddParticle(const Int_t PDGCode)
{
    auto particle = TDatabasePDG::Instance()->GetParticle(PDGCode);
    if (particle == nullptr)
    {
        LOG(FATAL) << "R3BPhaseSpaceGenerator::AddParticle: No such particle: " << PDGCode << FairLogger::endl;
        return;
    }
    fMasses.push_back(particle->Mass());
    fPDGCodes.push_back(particle->PdgCode());
}

void R3BPhaseSpaceGenerator::AddHeavyIon(const FairIon& ion)
{
    auto run = FairRunSim::Instance();
    if (run == nullptr)
    {
        LOG(WARNING) << "R3BPhaseSpaceGenerator::AddIon: No FairRunSim" << FairLogger::endl;
    }
    else
    {
        run->AddNewIon((FairIon*)ion.Clone());
    }
    fMasses.push_back(ion.GetMass());
    fPDGCodes.push_back(1000000000 + 10 * 1000 * ion.GetZ() + 10 * ion.GetA());
}

R3BPhaseSpaceGenerator::R3BPhaseSpaceGenerator()
    : fGamma(1.)
    , fBeta(0.)
    , fErel_keV(0.)
{
}
