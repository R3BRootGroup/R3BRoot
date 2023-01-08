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

#include "R3BPhaseSpaceGenerator.h"
#include "R3BDistribution2D.h"
#include "R3BDistribution3D.h"

#include "FairLogger.h"
#include "FairPrimaryGenerator.h"
#include "FairRunSim.h"

#include "TDatabasePDG.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVector3.h"

#include <numeric>

R3BPhaseSpaceGenerator::R3BPhaseSpaceGenerator(unsigned int seed)
    : fErel_keV(R3BDistribution1D::Delta(100))
    , fTotMass(0)
    , fRngGen(seed)
{
}

bool R3BPhaseSpaceGenerator::Init()
{
    if (fMasses.size() < 2)
        LOG(fatal) << "R3BPhaseSpaceGenerator::Init: Not enough Particles! At least two are required.";

    fTotMass = std::accumulate(fMasses.begin(), fMasses.end(), 0.);
    return true;
}

bool R3BPhaseSpaceGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{
    const auto pos_cm =
        Beam.GetVertexDistribution().GetRandomValues({ fRngGen.Rndm(), fRngGen.Rndm(), fRngGen.Rndm() });
    const auto spread_mRad = Beam.GetSpreadDistribution().GetRandomValues({ fRngGen.Rndm(), fRngGen.Rndm() });
    const auto beamBeta = Beam.GetBetaDistribution().GetRandomValues({ fRngGen.Rndm() })[0];
    const auto erel_GeV = fErel_keV.GetRandomValues({ fRngGen.Rndm() })[0] * (1e-6);

    const auto TotE_GeV = erel_GeV + fTotMass;
    TLorentzVector InitVec(0.0, 0.0, 0.0, TotE_GeV);
    fPhaseSpace.SetDecay(InitVec, fMasses.size(), fMasses.data());
    fPhaseSpace.Generate();

    TVector3 beamVector(0, 0, beamBeta);
    beamVector.RotateX(spread_mRad[0] * 1e-3);
    beamVector.RotateZ(spread_mRad[1] * 1e-3);

    const auto nParticles = fPDGCodes.size();

    for (auto i = 0; i < nParticles; i++)
    {
        auto p = fPhaseSpace.GetDecay(i);
        p->Boost(beamVector);

        const auto totalEnergy = sqrt(p->Mag() * p->Mag() + fMasses[i] * fMasses[i]);

        primGen->AddTrack(
            fPDGCodes.at(i), p->Px(), p->Py(), p->Pz(), pos_cm[0], pos_cm[1], pos_cm[2], -1, true, totalEnergy);
    }
    return true;
}

void R3BPhaseSpaceGenerator::addParticle(const int pdgCode, const double mass)
{
    fPDGCodes.push_back(pdgCode);
    fMasses.push_back(mass);
}
