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

#ifndef R3BROOT_R3BPHASESPACEGENERATOR_H
#define R3BROOT_R3BPHASESPACEGENERATOR_H

// Wrapper for TGenPhaseSpace

#include "R3BBeamProperties.h"
#include "R3BDistribution.h"
#include "R3BDistribution1D.h"
#include "R3BParticleSelector.h"

#include "FairGenerator.h"
#include "FairIon.h"

#include "TGenPhaseSpace.h"
#include "TRandom3.h"

#include <string>
#include <vector>

class R3BPhaseSpaceGenerator : public FairGenerator, public R3BParticleSelector
{
  public:
    R3BPhaseSpaceGenerator(unsigned int seed = 0U);

    // realtive energy distribution in keV
    R3BDistribution<1>& GetErelDistribution() { return fErel_keV; }
    void SetErelDistribution(R3BDistribution<1> ErelDistribution) { fErel_keV = ErelDistribution; }

    bool Init() override;
    bool ReadEvent(FairPrimaryGenerator* primGen) override;

    R3BBeamProperties Beam;

  protected:
    void addParticle(const int pdgCode, const double mass) override;

  private:
    R3BDistribution<1> fErel_keV; //!

    double fTotMass;
    TRandom3 fRngGen;
    TGenPhaseSpace fPhaseSpace;
    std::vector<int> fPDGCodes;
    std::vector<double> fMasses;

    ClassDefOverride(R3BPhaseSpaceGenerator, 3);
};

#endif // R3BROOT_R3BPHASESPACEGENERATOR_H
