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

/**  R3BCALIFATestGenerator.h
 *@author H. Alvarez Pol <hector.alvarez@usc.es>
 *
 * The R3BCALIFATestGenerator generates gammas with different options
 * for testing CALIFA. Copies from FairBoxGenerator (I tried to derive from it,
 * but requires a modification of their data members to protected).
 * Derived from FairGenerator.
 **/

#pragma once

#include <FairGenerator.h>

#include <iostream>
#include <vector>

class FairPrimaryGenerator;

class R3BCALIFATestGenerator : public FairGenerator
{
  public:
    // Default constructor
    R3BCALIFATestGenerator();

    /** Constructor with PDG-ID, multiplicity
     **@param pdgid Particle type (PDG encoding)
     **@param mult  Multiplicity (default is 1)
     **/
    R3BCALIFATestGenerator(Int_t pdgid, Int_t mult = 1);

    // Destructor
    ~R3BCALIFATestGenerator() = default;

    // Modifiers
    void SetPDGType(Int_t pdg) { fPDGType = pdg; };

    void SetMultiplicity(Int_t mult) { fMult = mult; };

    void SetPRange(double pmin = 0, double pmax = 10)
    {
        fPMin = pmin;
        fPMax = pmax;
        fPRangeIsSet = true;
    }

    void SetPtRange(double ptmin = 0, double ptmax = 10)
    {
        fPtMin = ptmin;
        fPtMax = ptmax;
        fPtRangeIsSet = true;
    };

    void SetPhiRange(double phimin = 0, double phimax = 360)
    {
        fPhiMin = phimin;
        fPhiMax = phimax;
    };

    void SetEtaRange(double etamin = -5, double etamax = 7)
    {
        fEtaMin = etamin;
        fEtaMax = etamax;
        fEtaRangeIsSet = true;
    };

    void SetYRange(double ymin = -5, double ymax = 7)
    {
        fYMin = ymin;
        fYMax = ymax;
        fYRangeIsSet = true;
    };

    void SetThetaRange(double thetamin = 0, double thetamax = 180)
    {
        fThetaMin = thetamin;
        fThetaMax = thetamax;
        fThetaRangeIsSet = true;
    };

    void SetCosTheta() { fCosThetaIsSet = true; };

    void SetXYZ(Double32_t x = 0, Double32_t y = 0, Double32_t z = 0)
    {
        fX = x;
        fY = y;
        fZ = z;
        fPointVtxIsSet = true;
    }

    void SetBoxXYZ(Double32_t x1 = 0,
                   Double32_t y1 = 0,
                   Double32_t z1 = 0,
                   Double32_t x2 = 0,
                   Double32_t y2 = 0,
                   Double32_t z2 = 0)
    {
        fX1 = x1;
        fY1 = y1;
        fZ1 = z1;
        fX2 = x2;
        fY2 = y2;
        fZ2 = z2;
        fBoxVtxIsSet = kTRUE;
    }

    void SetDebug(bool debug = false) { fDebug = debug; }

    void SetLorentzBoost(double beta)
    {
        SetFragmentVelocity(beta);
        fLorentzBoostIsSet = true;
    };

    void SetFragmentVelocity(double beta = 0., double dispersion = 0.);

    void SetNuclearDecayChain() { fNuclearDecayChainIsSet = true; };

    void SetDecayChainPoint(double gammaEnergy = 0, double branchingRatio = 0);

    /** Initializer **/
    Bool_t Init() override;

    /** Creates an event with given type and multiplicity.
     **@param primGen pointer to the FairPrimaryGenerator
     **/
    Bool_t ReadEvent(FairPrimaryGenerator* primGen) override;

  private:
    Int_t fPDGType; // Particle type (PDG encoding)
    Int_t fMult;    // Multiplicity

    double fPDGMass = 0.;                                              // Particle mass [GeV]
    double fPtMin = 0., fPtMax = 0.;                                   // Transverse momentum range [GeV]
    double fPhiMin = 0., fPhiMax = 360.;                               // Azimuth angle range [degree]
    double fEtaMin = 0., fEtaMax = 0.;                                 // Pseudorapidity range in lab system
    double fYMin = 0., fYMax = 0.;                                     // Rapidity range in lab system
    double fPMin = 0., fPMax = 0.;                                     // Momentum range in lab system
    double fThetaMin = 0., fThetaMax = 180.;                           // Polar angle range in lab system [degree]
    double fX = 0., fY = 0., fZ = 0.;                                  // Point vertex coordinates [cm]
    double fX1 = 0., fY1 = 0., fZ1 = 0., fX2 = 0., fY2 = 0., fZ2 = 0.; // Box vertex coords (x1,y1,z1)->(x2,y2,z2)
    double sumBranchingRatios = 0.;

    bool fEtaRangeIsSet = false;  // True if eta range is set
    bool fYRangeIsSet = false;    // True if rapidity range is set
    bool fThetaRangeIsSet = true; // True if theta range is set
    bool fCosThetaIsSet = false;  // True if uniform distribution in
                                  // cos(theta) is set (default -> not set)
    bool fPtRangeIsSet = false;   // True if transverse momentum range is set
    bool fPRangeIsSet = false;    // True if abs.momentum range is set
    bool fPointVtxIsSet = false;  // True if point vertex is set
    bool fBoxVtxIsSet = false;    // True if box vertex is set
    bool fDebug = false;          // Debug switch

    // SPECIFIC OF CALIFA
    std::vector<double> fGammaEnergies;        // Gamma energies for the nuclear decay chain in GeV UNITS
    std::vector<double> fGammaBranchingRatios; // Gamma branching ratios for the nuclear decay chain

    double fBetaOfEmittingFragment = 0.; // Velocity of the fragment emitting the gammas
    double fGammaFactor = 1.;            // Velocity of the fragment emitting the gammas

    bool fLorentzBoostIsSet = false;      // True if Lorentz Boost is set
    bool fNuclearDecayChainIsSet = false; // True if a nuclear decay chain is set

  public:
    ClassDefOverride(R3BCALIFATestGenerator, 2); // NOLINT
};
