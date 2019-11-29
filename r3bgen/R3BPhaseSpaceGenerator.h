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

#ifndef R3BROOT_R3BPHASESPACEGENERATOR_H
#define R3BROOT_R3BPHASESPACEGENERATOR_H

// Wrapper for TGenPhaseSpace

#include <string>
#include <vector>

#include "TGenPhaseSpace.h"
#include "TRandom3.h"

#include "FairGenerator.h"
#include "FairIon.h"

#include "R3BDistribution.h"

class R3BPhaseSpaceGenerator : public FairGenerator
{
  public:
    R3BPhaseSpaceGenerator(UInt_t seed = 0);

    void AddParticle(const Int_t PDGCode);
    void AddHeavyIon(const FairIon& ion);

    void SetVertexDistribution_cm(R3BDistribution<3> vertexDistribution) { fVertex_cm = vertexDistribution; }
    R3BDistribution<3>& GetVertexDistribution_cm() { return fVertex_cm; }

    void SetBeamSpread_mRad(R3BDistribution<2> beamSpreadDistribution) { fBeamSpread_mRad = beamSpreadDistribution; }
    R3BDistribution<2>& GetBeamSpread_mRad() { return fBeamSpread_mRad; }

    void SetBeamEnergyDistribution_AMeV(R3BDistribution<1> beamEnergyDistribution)
    {
        fBeamEnergy_AMeV = beamEnergyDistribution;
    }
    R3BDistribution<1>& GetBeamEnergyDistribution_AMeV() { return fBeamEnergy_AMeV; }

    void SetErelDistribution_keV(R3BDistribution<1> ErelDistribution) { fErel_keV = ErelDistribution; }
    R3BDistribution<1>& GetErelDistribution_keV() { return fErel_keV; }

    Bool_t Init() override;
    Bool_t ReadEvent(FairPrimaryGenerator* primGen) override;
    // FairGenerator* CloneGenerator() const override;

  private:
    R3BDistribution<3> fVertex_cm;       //!
    R3BDistribution<2> fBeamSpread_mRad; //!
    R3BDistribution<1> fBeamEnergy_AMeV; //!
    R3BDistribution<1> fErel_keV;        //!

    Double_t fTotMass;
    TRandom3 fRngGen;
    TGenPhaseSpace fPhaseSpace;
    std::vector<Int_t> fPDGCodes;
    std::vector<Double_t> fMasses;

    ClassDefOverride(R3BPhaseSpaceGenerator, 2);
};

#endif // R3BROOT_R3BPHASESPACEGENERATOR_H
