/******************************************************************************
 *   Copyright (C) 2010 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2010-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BCALIFATestGenerator.h"
#include "R3BLogger.h"

#include <FairPrimaryGenerator.h>

#include <TDatabasePDG.h>
#include <TMath.h>
#include <TParticlePDG.h>
#include <TRandom.h>

#include <iomanip>
#include <iostream>
#include <sstream>

R3BCALIFATestGenerator::R3BCALIFATestGenerator()
    : R3BCALIFATestGenerator(22, 1)
{
    // Default constructor
}

R3BCALIFATestGenerator::R3BCALIFATestGenerator(Int_t pdgid, Int_t mult)
    : fPDGType(pdgid)
    , fMult(mult)
{
}

Bool_t R3BCALIFATestGenerator::Init()
{
    // Initialize generator
    R3BLOG_IF(fatal, fPhiMax - fPhiMin > 360., "phi range is too wide: " << fPhiMin << "<phi<" << fPhiMax);
    R3BLOG_IF(fatal, fPRangeIsSet && fPtRangeIsSet, "Cannot set P and Pt ranges simultaneously");
    R3BLOG_IF(fatal, fPRangeIsSet && fYRangeIsSet, "Cannot set P and Y ranges simultaneously");

    if ((fThetaRangeIsSet && fYRangeIsSet) || (fThetaRangeIsSet && fEtaRangeIsSet) || (fYRangeIsSet && fEtaRangeIsSet))
    {
        R3BLOG(fatal, "Cannot set Y, Theta or Eta ranges simultaneously");
    }

    R3BLOG_IF(fatal,
              fBoxVtxIsSet && (fPointVtxIsSet || fDispersionVtxIsSet),
              "Cannot set explicit box vertex together with SetXYZ/SetDxDyDz vertex conventions");
    R3BLOG_IF(fatal,
              fDispersionVtxIsSet && !fPointVtxIsSet,
              "SetDxDyDz requires a central vertex to be defined first via SetXYZ");

    // CALIFA specifics
    R3BLOG_IF(fatal, fBetaOfEmittingFragment > 1., "beta of fragment larger than 1!");
    R3BLOG_IF(fatal,
              fBeamAngularDivIsSet && !fLorentzBoostIsSet,
              "SetBeamAngularDivergence has no effect without SetLorentzBoost");

    sumBranchingRatios = 0.;
    for (size_t i = 0; i < fGammaEnergies.size(); ++i)
    {
        if (fGammaBranchingRatios[i] > 1.)
        {
            R3BLOG(fatal, "gamma branching ratio in position " << i << " larger than 1!");
        }
        sumBranchingRatios += fGammaBranchingRatios[i];
    }
    R3BLOG_IF(warn, sumBranchingRatios > 1., "gamma branching ratio sum larger than 1!");

    TDatabasePDG* pdgBase = TDatabasePDG::Instance();
    TParticlePDG* particle = pdgBase->GetParticle(fPDGType);
    R3BLOG_IF(fatal, !particle, "PDG code " << fPDGType << " not defined.");
    fPDGMass = particle->Mass();
    return kTRUE;
}

void R3BCALIFATestGenerator::SampleVertex(Double_t& vx, Double_t& vy, Double_t& vz) const
{
    vx = 0.;
    vy = 0.;
    vz = 0.;

    if (fBoxVtxIsSet)
    {
        vx = gRandom->Uniform(fX1, fX2);
        vy = gRandom->Uniform(fY1, fY2);
        vz = gRandom->Uniform(fZ1, fZ2);
        return;
    }

    if (fPointVtxIsSet)
    {
        if (fDispersionVtxIsSet)
        {
            vx = gRandom->Gaus(fX, fDX);
            vy = gRandom->Gaus(fY, fDY);
            vz = gRandom->Uniform(fZ - fDZ, fZ + fDZ);
        }
        else
        {
            vx = fX;
            vy = fY;
            vz = fZ;
        }
    }
}

void R3BCALIFATestGenerator::SampleBeamDirection(Double_t& nx, Double_t& ny, Double_t& nz) const
{
    nx = 0.;
    ny = 0.;
    nz = 1.;

    if (!fBeamAngularDivIsSet)
        return;

    // Sample projected angles in the x-z and y-z planes from a Gaussian
    // centered on zero (nominal beam axis)
    const Double_t thetaX = gRandom->Gaus(0., fBeamDivX);
    const Double_t thetaY = gRandom->Gaus(0., fBeamDivY);
    nx = TMath::Sin(thetaX);
    ny = TMath::Sin(thetaY);
    // Ensure unit vector; clamp argument to avoid sqrt of negative from
    // floating-point rounding when divergence angles are very large.
    const Double_t nz2 = 1. - nx * nx - ny * ny;
    nz = (nz2 > 0.) ? TMath::Sqrt(nz2) : 0.;
}

void R3BCALIFATestGenerator::ApplyLorentzBoost(Double_t& px,
                                               Double_t& py,
                                               Double_t& pz,
                                               Double_t nx,
                                               Double_t ny,
                                               Double_t nz) const
{
    // General Lorentz boost along unit vector n = (nx, ny, nz):
    //   E'  = gamma * (E  + beta * (n . p))
    //   p'  = p  +  n * [(gamma - 1) * (n . p)  +  gamma * beta * E]
    //
    // fGammaFactor stores sqrt(1 - beta^2) = 1/gamma, matching the
    // convention used in SetFragmentVelocity.

    const Double_t E = (fPDGType == 22) ? TMath::Sqrt(px * px + py * py + pz * pz)
                                        : TMath::Sqrt(px * px + py * py + pz * pz + fPDGMass * fPDGMass);

    const Double_t gamma = 1. / fGammaFactor;
    const Double_t beta = fBetaOfEmittingFragment;
    const Double_t pDotN = px * nx + py * ny + pz * nz;
    const Double_t coeff = (gamma - 1.) * pDotN + gamma * beta * E;

    px += nx * coeff;
    py += ny * coeff;
    pz += nz * coeff;
}

Bool_t R3BCALIFATestGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{
    // Generate one event: produce primary particles emitted from one vertex.
    // Primary particles are distributed uniformly along
    // those kinematics variables which were limited by setters.
    // if SetCosTheta() function is used, the distribution will be uniform in
    // cos(theta)

    Double32_t pabs = 0., phi = 0., pt = 0., theta = 0., eta = 0., y = 0., mt = 0.;
    Double32_t px = 0., py = 0., pz = 0.;

    // Sample the boost direction once for the whole event: all de-excitation
    // photons from a single fragment share the same beam direction.
    Double_t beamNx = 0., beamNy = 0., beamNz = 1.;
    if (fLorentzBoostIsSet)
        SampleBeamDirection(beamNx, beamNy, beamNz);

    // Generate particles
    for (Int_t k = 0; k < fMult; ++k)
    {
        Double_t vx = 0., vy = 0., vz = 0.;
        SampleVertex(vx, vy, vz);

        phi = gRandom->Uniform(fPhiMin, fPhiMax) * TMath::DegToRad();

        if (fPRangeIsSet)
            pabs = gRandom->Uniform(fPMin, fPMax);
        else if (fPtRangeIsSet)
            pt = gRandom->Uniform(fPtMin, fPtMax);

        if (fThetaRangeIsSet)
        {
            if (fCosThetaIsSet)
                theta = acos(gRandom->Uniform(cos(fThetaMin * TMath::DegToRad()), cos(fThetaMax * TMath::DegToRad())));
            else
                theta = gRandom->Uniform(fThetaMin, fThetaMax) * TMath::DegToRad();
        }
        else if (fEtaRangeIsSet)
        {
            eta = gRandom->Uniform(fEtaMin, fEtaMax);
            theta = 2. * TMath::ATan(TMath::Exp(-eta));
        }
        else if (fYRangeIsSet)
        {
            y = gRandom->Uniform(fYMin, fYMax);
            mt = TMath::Sqrt(fPDGMass * fPDGMass + pt * pt);
            pz = mt * TMath::SinH(y);
        }

        if (fThetaRangeIsSet || fEtaRangeIsSet)
        {
            if (fPRangeIsSet)
            {
                pz = pabs * TMath::Cos(theta);
                pt = pabs * TMath::Sin(theta);
            }
            else if (fPtRangeIsSet)
            {
                pz = pt / TMath::Tan(theta);
            }
        }

        px = pt * TMath::Cos(phi);
        py = pt * TMath::Sin(phi);

        if (fBoxVtxIsSet)
        {
            fX = gRandom->Uniform(fX1, fX2);
            fY = gRandom->Uniform(fY1, fY2);
            fZ = gRandom->Uniform(fZ1, fZ2);
        }

        if (fNuclearDecayChainIsSet)
        {
            LOG_IF(fatal, fPDGType != 22) << "PDG code " << fPDGType << " is not a gamma!";
            for (size_t i = 0; i < fGammaEnergies.size(); ++i)
            {
                const auto br = gRandom->Uniform();
                if (br < fGammaBranchingRatios[i])
                {
                    pabs = fGammaEnergies[i];
                    phi = gRandom->Uniform(fPhiMin, fPhiMax) * TMath::DegToRad();
                    theta =
                        acos(gRandom->Uniform(cos(fThetaMin * TMath::DegToRad()), cos(fThetaMax * TMath::DegToRad())));
                    pz = pabs * TMath::Cos(theta);
                    pt = pabs * TMath::Sin(theta);
                    px = pt * TMath::Cos(phi);
                    py = pt * TMath::Sin(phi);

                    const auto gammaMomentum = TMath::Sqrt(px * px + py * py + pz * pz);
                    px = px * fGammaEnergies[i] / gammaMomentum;
                    py = py * fGammaEnergies[i] / gammaMomentum;
                    pz = pz * fGammaEnergies[i] / gammaMomentum;

                    if (fLorentzBoostIsSet)
                    {
                        ApplyLorentzBoost(px, py, pz, beamNx, beamNy, beamNz);
                    }

                    primGen->AddTrack(fPDGType, px, py, pz, vx, vy, vz);

                    if (sumBranchingRatios <= 1.)
                        break;
                }
            }
            continue;
        }

        if (fLorentzBoostIsSet)
        {
            ApplyLorentzBoost(px, py, pz, beamNx, beamNy, beamNz);
        }

        if (fDebug)
        {
            std::ostringstream oss;
            oss << "CALIFATestGen: kf=" << fPDGType << ", p=(" << std::fixed << std::setprecision(2) << px << ", " << py
                << ", " << pz << ") GeV"
                << ", x=(" << std::setprecision(2) << vx << ", " << vy << ", " << vz << ") cm";
            if (fLorentzBoostIsSet && fBeamAngularDivIsSet)
            {
                oss << ", beam_n=(" << std::setprecision(4) << beamNx << ", " << beamNy << ", " << beamNz << ")";
            }
            R3BLOG(info, oss.str());
        }

        primGen->AddTrack(fPDGType, px, py, pz, vx, vy, vz);
    }

    return kTRUE;
}

void R3BCALIFATestGenerator::SetFragmentVelocity(double beta, double dispersion)
{
    // Keep original convention: sample once when the setter is called.
    // This matches the current class behavior and avoids unexpected API changes.
    R3BLOG(info, "Set a beta of " << beta << " with a sigma dispersion of " << dispersion);
    fBetaOfEmittingFragment = gRandom->Gaus(beta, dispersion);
    fGammaFactor = TMath::Sqrt(1. - fBetaOfEmittingFragment * fBetaOfEmittingFragment);
}

void R3BCALIFATestGenerator::SetDecayChainPoint(double gammaEnergy, double branchingRatio)
{
    R3BLOG(info, "Set gamma energy " << gammaEnergy << " with a branching ratio of " << branchingRatio);
    fGammaEnergies.push_back(gammaEnergy / 1000.); // GeV
    fGammaBranchingRatios.push_back(branchingRatio);
}

ClassImp(R3BCALIFATestGenerator)
