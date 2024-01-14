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
// -----                   R3BAlpideCalData                    -----
// -----          Created 28/01/2022 by J.L. Rodriguez         -----
// -----------------------------------------------------------------

#pragma once

#include <TObject.h>
#include <cstdint>
#include <iostream>
#include <string>

class R3BAlpideCalData : public TObject
{
  public:
    // Default Constructor
    R3BAlpideCalData() = default;

    /** Standard Constructor
     *@param senId    Sensor unique identifier
     *@param col    Longitudinal position
     *@param row    Transverse position
     **/
    explicit R3BAlpideCalData(uint16_t senId, uint16_t row, uint16_t col);

    // Destructor
    virtual ~R3BAlpideCalData() = default;

    // Accessors with [[nodiscard]]
    [[nodiscard]] inline const uint16_t GetSensorId() const { return fSenId; }
    [[nodiscard]] inline const uint16_t GetRow() const { return fRow; }
    [[nodiscard]] inline const uint16_t GetCol() const { return fCol; }

    // Support for printing
    [[nodiscard]] std::string toString() const;
    void Print(const Option_t*) const override;

  protected:
    uint16_t fSenId = 0;
    uint16_t fRow = 0;
    uint16_t fCol = 0;

  public:
    ClassDefOverride(R3BAlpideCalData, 1)
};

// Operator overloading for printing R3BAlpideCalData
std::ostream& operator<<(std::ostream& os, const R3BAlpideCalData& data);
