/******************************************************************************
 *   Copyright (C) 2022 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2022-2024 Members of R3B Collaboration                     *
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
// -----                   R3BAlpideMappedData                 -----
// -----          Created 11/10/2021 by J.L. Rodriguez         -----
// -----------------------------------------------------------------

#pragma once

#include <TObject.h>
#include <cstdint>
#include <iostream>
#include <string>

class R3BAlpideMappedData : public TObject
{
  public:
    // Default Constructor
    R3BAlpideMappedData() = default;

    /** Standard Constructor
     *@param senId    Sensor unique identifier
     *@param reg      Region identifier
     *@param ads      Address identifier, Mosaic ID
     *@param chip     Chip identifier
     *@param row      Row identifier
     *@param col      Column identifier
     **/
    explicit R3BAlpideMappedData(uint16_t senId, uint16_t reg, uint16_t ads, uint16_t chip, uint16_t row, uint16_t col);

    // Destructor
    virtual ~R3BAlpideMappedData() = default;

    // Accessors with [[nodiscard]]
    [[nodiscard]] inline const uint16_t GetSensorId() const { return fSenId; }
    [[nodiscard]] inline const uint16_t GetReg() const { return fReg; }
    [[nodiscard]] inline const uint16_t GetAds() const { return fAds; }
    [[nodiscard]] inline const uint16_t GetChip() const { return fChip; }
    [[nodiscard]] inline const uint16_t GetRow() const { return fRow; }
    [[nodiscard]] inline const uint16_t GetCol() const { return fCol; }

    // Support for printing
    [[nodiscard]] std::string toString() const;
    void Print(const Option_t*) const override;

  protected:
    uint16_t fSenId = 0; // Sensor unique identifier
    uint16_t fReg = 0;   // Region identifier from 1 to 32
    uint16_t fAds = 0;   // Address identifier from 1 to 1024 or Mosaic ID from 1 to 8
    uint16_t fChip = 0;  // Chip identifier
    uint16_t fRow = 0;   // Row identifier from 1 to 512
    uint16_t fCol = 0;   // Column identifier from 1 to 1024

  public:
    ClassDefOverride(R3BAlpideMappedData, 2)
};

// Operator overloading for printing R3BAlpideMappedData
std::ostream& operator<<(std::ostream& os, const R3BAlpideMappedData& data);
