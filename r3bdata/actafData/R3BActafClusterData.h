/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -----------------------------------------------------------------
// -----             R3BActafClusterData source file           -----
// -----      Created 12/04/26 by J.L. Rodriguez-Sanchez       -----
// -----------------------------------------------------------------

#pragma once

#include <Rtypes.h>
#include <RtypesCore.h>
#include <TMath.h>
#include <TObject.h>
#include <TVector3.h>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <string>
#include <vector>

class R3BActafClusterData : public TObject
{
  public:
    // Default Constructor
    R3BActafClusterData() = default;

    /** Standard Constructor
     *@param side         Upstream(1) or downstream(2) sides
     *@param xpos         X-position for the 1st pad of the cluster
     *@param ypos         Y-position for the 1st pad of the cluster
     *@param zpos         Z-position for the 1st pad of the cluster
     *@param track        track for each cluster
     *@param energy       Deposited energy for each pad
     *@param padlist      List with pads inside the cluster
     *@param chi2polarfit Chi2 of the polar angle fit
     **/
    explicit R3BActafClusterData(UInt_t side,
                                 double xpos,
                                 double ypos,
                                 double zpos,
                                 double energy,
                                 const TVector3& track,
                                 std::vector<uint16_t> padlist,
                                 double chi2polarfit);

    // Accessors with [[nodiscard]]
    [[nodiscard]] UInt_t GetSide() const { return fSide; }
    [[nodiscard]] double GetXpos() const { return fXPos; }
    [[nodiscard]] double GetYpos() const { return fYPos; }
    [[nodiscard]] double GetZpos() const { return fZPos; }
    [[nodiscard]] double GetEnergy() const { return fEnergy; }
    [[nodiscard]] const TVector3& GetTrack() const { return fTrack; }
    [[nodiscard]] double GetTheta() const { return fTrack.Theta() * TMath::RadToDeg(); }
    [[nodiscard]] double GetPhi() const { return fTrack.Phi() * TMath::RadToDeg(); }
    [[nodiscard]] size_t GetNbOfPads() const { return fPadList.size(); }
    [[nodiscard]] const std::vector<uint16_t>& GetPadList() const { return fPadList; }
    [[nodiscard]] double GetChi2PolarFit() const { return fChi2PolarFit; }

    // Support for printing
    [[nodiscard]] std::string toString() const;
    void Print(const Option_t* /*option*/) const override;

  private:
    UInt_t fSide = 0;  // Side number
    double fXPos = 0.; // in cm
    double fYPos = 0.; // in cm
    double fZPos = 0.; // in cm
    double fEnergy = std::nan("");
    TVector3 fTrack;
    std::vector<uint16_t> fPadList;
    double fChi2PolarFit = 0.;

  public:
    ClassDefOverride(R3BActafClusterData, 1);
};

// Operator overloading for printing R3BActafClusterData
std::ostream& operator<<(std::ostream& output, const R3BActafClusterData& data);
