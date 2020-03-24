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

#include "R3BPhaseSpaceGenerator.h"
#include "FairLogger.h"
#include "FairPrimaryGenerator.h"
#include "FairRunSim.h"
#include "G4NistManager.hh"
#include "R3BDistribution1D.h"
#include "R3BDistribution2D.h"
#include "R3BDistribution3D.h"
#include "TDatabasePDG.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include <numeric>

R3BPhaseSpaceGenerator::R3BPhaseSpaceGenerator(unsigned int seed)
    : fVertex_cm(R3BDistribution3D::Delta(0, 0, 0))
    , fBeamSpread_mRad({ R3BDistribution1D::Delta(0), R3BDistribution1D::Flat(0, 2 * TMath::Pi() * 1e3) })
    , fBeamEnergy_AMeV(R3BDistribution1D::Delta(500))
    , fErel_keV(R3BDistribution1D::Delta(100))
    , fTotMass(0)
    , fRngGen(seed)
{
}

void R3BPhaseSpaceGenerator::AddParticle(const int PDGCode)
{
    auto particle = TDatabasePDG::Instance()->GetParticle(PDGCode);
    if (particle == nullptr)
    {
        LOG(FATAL) << __func__ << ": No such particle: " << PDGCode;
        return;
    }
    fMasses.push_back(particle->Mass());
    fPDGCodes.push_back(particle->PdgCode());
}

void R3BPhaseSpaceGenerator::AddHeavyIon(int z, int a)
{
    const double mass = G4NistManager::Instance()->GetIsotopeMass(z, a) / CLHEP::GeV;
    auto run = FairRunSim::Instance();
    if (run == nullptr)
    {
        LOG(WARNING) << __func__ << ": No FairRunSim";
    }
    else
    {
        run->AddNewIon(new FairIon(TString::Format("Ion_%d_%d", a, z), z, a, z, 0., mass));
    }
    fMasses.push_back(mass);
    fPDGCodes.push_back(1000000000 + 10 * 1000 * z + 10 * a);
}

bool R3BPhaseSpaceGenerator::Init()
{
    fTotMass = std::accumulate(fMasses.begin(), fMasses.end(), 0.);
    return true;
}

bool R3BPhaseSpaceGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{
    const auto pos_cm = fVertex_cm.GetRandomValues({ fRngGen.Rndm(), fRngGen.Rndm(), fRngGen.Rndm() });
    const auto spread_mRad = fBeamSpread_mRad.GetRandomValues({ fRngGen.Rndm(), fRngGen.Rndm() });
    const auto beamEnergy_GeV = fBeamEnergy_AMeV.GetRandomValues({ fRngGen.Rndm() })[0] * 1e-3;
    const auto erel_GeV = fErel_keV.GetRandomValues({ fRngGen.Rndm() })[0] * (1e-6);

    const auto TotE_GeV = erel_GeV + fTotMass;
    const auto gamma = 1 + beamEnergy_GeV / 0.931494028;
    const auto beta = std::sqrt(1 - 1 / (gamma * gamma));
    TLorentzVector InitVec(0.0, 0.0, 0.0, TotE_GeV);
    fPhaseSpace.SetDecay(InitVec, fMasses.size(), fMasses.data());
    fPhaseSpace.Generate();

    const size_t nParticles = fPDGCodes.size();

    TVector3 beam(0, 0, beta);
    beam.RotateX(spread_mRad[0] * 1e-3);
    beam.RotateZ(spread_mRad[1] * 1e-3);

    for (size_t i = 0; i < nParticles; i++)
    {
        auto p = fPhaseSpace.GetDecay(i);
        p->Boost(beam);
        primGen->AddTrack(fPDGCodes.at(i), p->Px(), p->Py(), p->Pz(), pos_cm[0], pos_cm[1], pos_cm[2]);
    }
    return true;
}
