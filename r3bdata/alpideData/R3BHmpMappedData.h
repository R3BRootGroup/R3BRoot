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

// ---------------------------------------------------------------
// -----                 R3BHmpMappedData                    -----
// -----          Created 17/04/2025 by L. Rose              -----
// ---------------------------------------------------------------

#pragma once

#include <TObject.h>
#include <cstdint>
#include <iostream>
#include <string>

class R3BHmpMappedData : public TObject
{
  public:
    // Default Constructor
    R3BHmpMappedData() = default;

    /** Standard Constructor
      HMP_TSYS_LO // Slow control computer time Low bits
      HMP_TSYS_HI // Slow control computer time High bits
      HMP_VOLT1 // Voltage channel 1
      HMP_VOLT2 // Voltage channel 2
      HMP_VOLT3 // Voltage channel 3
      HMP_VOLT4 // Voltage channel 4
      HMP_CURR1 // Current channel 1
      HMP_CURR2 // Current channel 2
      HMP_CURR3 // Current channel 3
      HMP_CURR4 // Current channel 4
     **/

    explicit R3BHmpMappedData(uint32_t TSYS_LO,
                              uint32_t TSYS_HI,
                              uint32_t volt1,
                              uint32_t curr1,
                              uint32_t volt2,
                              uint32_t curr2,
                              uint32_t volt3,
                              uint32_t curr3,
                              uint32_t volt4,
                              uint32_t curr4);

    // Destructor
    virtual ~R3BHmpMappedData() = default;

    // Accessors with [[nodiscard]]
    [[nodiscard]] inline const uint32_t GetTSLO() const { return fTSLO; }
    [[nodiscard]] inline const uint32_t GetTSHI() const { return fTSHI; }
    [[nodiscard]] inline const uint32_t GetVolt1() const { return fVolt1; }
    [[nodiscard]] inline const uint32_t GetCurr1() const { return fCurr1; }
    [[nodiscard]] inline const uint32_t GetVolt2() const { return fVolt2; }
    [[nodiscard]] inline const uint32_t GetCurr2() const { return fCurr2; }
    [[nodiscard]] inline const uint32_t GetVolt3() const { return fVolt3; }
    [[nodiscard]] inline const uint32_t GetCurr3() const { return fCurr3; }
    [[nodiscard]] inline const uint32_t GetVolt4() const { return fVolt4; }
    [[nodiscard]] inline const uint32_t GetCurr4() const { return fCurr4; }

    // Support for printing
    [[nodiscard]] std::string toString() const;
    void Print(const Option_t*) const override;

  protected:
    uint32_t fTSLO = 0;
    uint32_t fTSHI = 0;
    uint32_t fVolt1 = 0;
    uint32_t fCurr1 = 0;
    uint32_t fVolt2 = 0;
    uint32_t fCurr2 = 0;
    uint32_t fVolt3 = 0;
    uint32_t fCurr3 = 0;
    uint32_t fVolt4 = 0;
    uint32_t fCurr4 = 0;

  public:
    ClassDefOverride(R3BHmpMappedData, 1); // NOLINT
};

// Operator overloading for printing R3BHmpMappedData
std::ostream& operator<<(std::ostream& os, const R3BHmpMappedData& data);
