/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2024 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -------------------------------------------------------------------
// -----               R3BTTTXCalData header file                -----
// -----      Created 11/01/2024 by J. L. Rodriguez-Sanchez      -----
// -------------------------------------------------------------------

#pragma once

#include <TObject.h>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>

class R3BTTTXCalData : public TObject
{
  public:
    // Default constructor
    R3BTTTXCalData() = default;

    // Constructor with arguments (explicit)
    explicit R3BTTTXCalData(uint8_t detID, uint8_t stripID, double time, double energy);

    // Destructor virtual
    virtual ~R3BTTTXCalData() = default;

    // Accessors with [[nodiscard]]
    [[nodiscard]] inline const uint8_t& GetDetID() const { return fDetID; }
    [[nodiscard]] inline const uint8_t& GetStripID() const { return fStripID; }
    [[nodiscard]] inline const double& GetTime() const { return fTime; }
    [[nodiscard]] inline const double& GetEnergy() const { return fEnergy; }

    // Modifiers
    inline void SetDetID(uint8_t id) { fDetID = id; }
    inline void SetStripID(uint8_t id) { fStripID = id; }
    inline void SetTime(double time) { fTime = time; }
    inline void SetEnergy(double energy) { fEnergy = energy; }

    // Support for printing
    [[nodiscard]] std::string toString() const;
    void Print(const Option_t*) const override;

  protected:
    uint8_t fDetID = 0;
    uint8_t fStripID = 0;
    double fTime = std::nan("");
    double fEnergy = std::nan("");

  public:
    ClassDefOverride(R3BTTTXCalData, 1)
};

// Operator overloading for printing R3BTTTXCalData
std::ostream& operator<<(std::ostream& os, const R3BTTTXCalData& data);
