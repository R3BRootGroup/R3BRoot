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
 * for testing CALIFA. Originally copied from FairBoxGenerator.
 *
 * Updated to include vertex distributions:
 *   - SetXYZ(x,y,z)     : central vertex position [cm]
 *   - SetDxDyDz(sx,sy,sz): Gaussian sigma for x/y, uniform half-width for z
 *                          i.e. z is sampled in [z-sz, z+sz]
 *
 * For backward compatibility, SetBoxXYZ(...) is retained, which samples a
 * uniform box distribution.
 *
 * Beam angular divergence:
 *   - SetBeamAngularDivergence(sx_mrad, sy_mrad): a Gaussian
 *     distribution the fragment beam around the z-axis, characterised by
 *     1-sigma projected angles (in mrad) in the x-z and y-z planes.
 *     The Lorentz boost is applied along the sampled beam direction from this Gaussian
 *     rather than a fixed z-axis, giving correct lab-frame angles of
 *     the gammas seen by CALIFA.
 *     Requires SetLorentzBoost() to be set; has no effect otherwise.
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
    ~R3BCALIFATestGenerator() override = default;

    // Modifiers
    void SetPDGType(Int_t pdg) { fPDGType = pdg; }
    void SetMultiplicity(Int_t mult) { fMult = mult; }

    void SetPRange(double pmin = 0., double pmax = 10.)
    {
        fPMin = pmin;
        fPMax = pmax;
        fPRangeIsSet = true;
    }

    void SetPtRange(double ptmin = 0., double ptmax = 10.)
    {
        fPtMin = ptmin;
        fPtMax = ptmax;
        fPtRangeIsSet = true;
    }

    void SetPhiRange(double phimin = 0., double phimax = 360.)
    {
        fPhiMin = phimin;
        fPhiMax = phimax;
    }

    void SetEtaRange(double etamin = -5., double etamax = 7.)
    {
        fEtaMin = etamin;
        fEtaMax = etamax;
        fEtaRangeIsSet = true;
    }

    void SetYRange(double ymin = -5., double ymax = 7.)
    {
        fYMin = ymin;
        fYMax = ymax;
        fYRangeIsSet = true;
    }

    void SetThetaRange(double thetamin = 0., double thetamax = 180.)
    {
        fThetaMin = thetamin;
        fThetaMax = thetamax;
        fThetaRangeIsSet = true;
    }

    void SetCosTheta() { fCosThetaIsSet = true; }

    // Vertex configuration: same conventions as R3BINCLRootGenerator
    void SetXYZ(Double32_t x = 0., Double32_t y = 0., Double32_t z = 0.)
    {
        fX = x;
        fY = y;
        fZ = z;
        fPointVtxIsSet = true;
    }

    /**
     * Set dispersion around the central vertex point.
     * - x sampled as Gaussian(x, sx)
     * - y sampled as Gaussian(y, sy)
     * - z sampled uniformly in [z-sz, z+sz]
     */
    void SetDxDyDz(Double32_t sx = 0., Double32_t sy = 0., Double32_t sz = 0.)
    {
        fDX = sx;
        fDY = sy;
        fDZ = sz;
        fDispersionVtxIsSet = true;
    }

    /**
     * Backward-compatible box vertex.
     * Samples x,y,z uniformly in the specified box.
     */
    void SetBoxXYZ(Double32_t x1 = 0.,
                   Double32_t y1 = 0.,
                   Double32_t z1 = 0.,
                   Double32_t x2 = 0.,
                   Double32_t y2 = 0.,
                   Double32_t z2 = 0.)
    {
        fX1 = x1;
        fY1 = y1;
        fZ1 = z1;
        fX2 = x2;
        fY2 = y2;
        fZ2 = z2;
        fBoxVtxIsSet = true;
    }

    void SetDebug(bool debug = false) { fDebug = debug; }

    void SetLorentzBoost(double beta)
    {
        SetFragmentVelocity(beta);
        fLorentzBoostIsSet = true;
    }

    void SetFragmentVelocity(double beta = 0., double dispersion = 0.);

    /**
     * Set 1-sigma projected angular divergence of the beam around the z-axis.
     * @param sigmaX_mrad  Gaussian sigma of the x-z projection angle [mrad]
     * @param sigmaY_mrad  Gaussian sigma of the y-z projection angle [mrad]
     *
     * Per event a random beam direction is sampled:
     *   n = (sin(thetaX), sin(thetaY), sqrt(1 - sin²(thetaX) - sin²(thetaY)))
     * and the Lorentz boost is applied along n instead of the fixed z-axis.
     * Requires SetLorentzBoost() to be set; ignored otherwise.
     */
    void SetBeamAngularDivergence(double sigmaX_mrad = 0., double sigmaY_mrad = 0.)
    {
        fBeamDivX = sigmaX_mrad * 1e-3; // convert mrad -> rad
        fBeamDivY = sigmaY_mrad * 1e-3;
        fBeamAngularDivIsSet = true;
    }

    void SetNuclearDecayChain() { fNuclearDecayChainIsSet = true; }
    void SetDecayChainPoint(double gammaEnergy = 0., double branchingRatio = 0.);

    Bool_t Init() override;

    /** Creates an event with given type and multiplicity.
     **@param primGen pointer to the FairPrimaryGenerator
     **/
    Bool_t ReadEvent(FairPrimaryGenerator* primGen) override;

  private:
    void SampleVertex(Double_t& vx, Double_t& vy, Double_t& vz) const;

    /** Sample a beam direction unit vector for the current event.
     *  When fBeamAngularDivIsSet, spreads the beam by Gaussian projected angles.
     *  Otherwise returns the nominal z-axis (0, 0, 1). */
    void SampleBeamDirection(Double_t& nx, Double_t& ny, Double_t& nz) const;

    /** Apply a Lorentz boost along the unit vector (nx, ny, nz) to the
     *  momentum (px, py, pz), using fBetaOfEmittingFragment / fGammaFactor.
     *  Handles both massive particles and photons (fPDGType == 22). */
    void ApplyLorentzBoost(Double_t& px, Double_t& py, Double_t& pz, Double_t nx, Double_t ny, Double_t nz) const;

    Int_t fPDGType = 22;
    Int_t fMult = 1;

    double fPDGMass = 0.;
    double fPtMin = 0., fPtMax = 0.;
    double fPhiMin = 0., fPhiMax = 360.;
    double fEtaMin = 0., fEtaMax = 0.;
    double fYMin = 0., fYMax = 0.;
    double fPMin = 0., fPMax = 0.;
    double fThetaMin = 0., fThetaMax = 180.;

    // Central vertex coordinates [cm]
    double fX = 0., fY = 0., fZ = 0.;
    // INCL-like dispersions: x/y Gaussian sigma, z uniform half-width [cm]
    double fDX = 0., fDY = 0., fDZ = 0.;
    // Explicit box vertex coordinates [cm]
    double fX1 = 0., fY1 = 0., fZ1 = 0., fX2 = 0., fY2 = 0., fZ2 = 0.;

    double sumBranchingRatios = 0.;

    bool fEtaRangeIsSet = false;     // True if eta range is set
    bool fYRangeIsSet = false;       // True if rapidity range is set
    bool fThetaRangeIsSet = true;    // True if theta range is set
    bool fCosThetaIsSet = false;     // True if uniform distribution in
                                     // cos(theta) is set (default -> not set)
    bool fPtRangeIsSet = false;      // True if transverse momentum range is set
    bool fPRangeIsSet = false;       // True if abs.momentum range is set
    bool fPointVtxIsSet = true;      // True if point vertex is set
    bool fDispersionVtxIsSet = true; // True if x,y,z dispersion around vertex is set
    bool fBoxVtxIsSet = false;       // True if box vertex is set
    bool fDebug = false;             // Debug switch

    // CALIFA-specific
    std::vector<double> fGammaEnergies;        // gamma energies in GeV
    std::vector<double> fGammaBranchingRatios; // branching ratios

    double fBetaOfEmittingFragment = 0.; // Velocity of the fragment emitting the photons
    double fGammaFactor = 1.;            // Lorentz gamma of the fragment emitting the photons

    bool fLorentzBoostIsSet = false;      // True if Lorentz Boost is set
    bool fNuclearDecayChainIsSet = false; // True if a nuclear decay chain is set

    // Beam angular divergence (projected 1-sigma angles in x-z and y-z planes)
    double fBeamDivX = 0.; // [rad]
    double fBeamDivY = 0.; // [rad]
    bool fBeamAngularDivIsSet = false;

  public:
    ClassDefOverride(R3BCALIFATestGenerator, 4); // NOLINT
};
