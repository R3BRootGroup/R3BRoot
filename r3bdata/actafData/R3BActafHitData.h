/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025 Members of R3B Collaboration                          *
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
// -----             R3BActafHitData source file               -----
// -----      Created 14/09/25 by J.L. Rodriguez-Sanchez       -----
// -----------------------------------------------------------------

#pragma once

#include <TObject.h>
#include <TVector3.h>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>

class R3BActafHitData : public TObject
{
  public:
    // Default Constructor
    R3BActafHitData() = default;

    /** Standard Constructor
     *@param pad          Pad number
     *@param side         Upstream(1) or downstream(2) sides
     *@param ring         ring from 1 to 8
     *@param xpos         X-position for each pad
     *@param ypos         Y-position for each pad
     *@param zpos         Z-position for each pad
     *@param track        track for each pad
     **/
    explicit R3BActafHitData(UInt_t pad,
                             UInt_t side,
                             UInt_t ring,
                             double xpos,
                             double ypos,
                             double zpos,
                             double energy,
                             TVector3 track);

    // Destructor
    virtual ~R3BActafHitData() = default;

    // Accessors with [[nodiscard]]
    [[nodiscard]] inline const UInt_t& GetPad() const { return fPad; }
    [[nodiscard]] inline const UInt_t& GetSide() const { return fSide; }
    [[nodiscard]] inline const UInt_t& GetRing() const { return fRing; }
    [[nodiscard]] inline const double& GetXpos() const { return fXPos; }
    [[nodiscard]] inline const double& GetYpos() const { return fYPos; }
    [[nodiscard]] inline const double& GetZpos() const { return fZPos; }
    [[nodiscard]] inline const double& GetEnergy() const { return fEnergy; }
    [[nodiscard]] inline const TVector3 GetTrack() const { return fTrack; }

    // Support for printing
    [[nodiscard]] std::string toString() const;
    void Print(const Option_t*) const override;

  protected:
    UInt_t fPad = 0;   // Pad number
    UInt_t fSide = 0;  // Side number
    UInt_t fRing = 0;  // Ring number
    double fXPos = 0.; // in cm
    double fYPos = 0.; // in cm
    double fZPos = 0.; // in cm
    double fEnergy = std::nan("");
    TVector3 fTrack;

  public:
    ClassDefOverride(R3BActafHitData, 1);
};

// Operator overloading for printing R3BActafHitData
std::ostream& operator<<(std::ostream& os, const R3BActafHitData& data);
