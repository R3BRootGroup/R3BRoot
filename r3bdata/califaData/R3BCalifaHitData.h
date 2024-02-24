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

#pragma once

#include <TObject.h>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>

class R3BCalifaHitData : public TObject
{
  public:
    // Default constructor
    R3BCalifaHitData() = default;

    // Constructor with arguments  (explicit)
    explicit R3BCalifaHitData(double ene, double theta, double phi, double pid, uint8_t hittype, ULong64_t time);

    // Copy constructor
    R3BCalifaHitData(const R3BCalifaHitData&);

    R3BCalifaHitData& operator=(const R3BCalifaHitData&) { return *this; }

    // Destructor
    virtual ~R3BCalifaHitData() = default;

    // Accessors with [[nodiscard]]
    [[nodiscard]] inline const double& GetEnergy() const { return fEnergy; }
    [[nodiscard]] inline const double& GetTheta() const { return fTheta; }
    [[nodiscard]] inline const double& GetPhi() const { return fPhi; }
    [[nodiscard]] inline const double& GetPid() const { return fPid; }
    [[nodiscard]] inline const uint8_t& GetType() const { return fHitType; }
    [[nodiscard]] inline const ULong64_t& GetTime() const { return fTime; }

    // Modifiers
    inline void SetEnergy(double ene) { fEnergy = ene; }
    inline void SetTheta(double theta) { fTheta = theta; }
    inline void SetPhi(double phi) { fPhi = phi; }
    inline void SetPid(uint8_t pid) { fPid = pid; }
    inline void SetType(uint8_t id) { fHitType = id; }
    inline void SetTime(ULong64_t time) { fTime = time; }

    // Support for printing
    [[nodiscard]] std::string toString() const;
    void Print(const Option_t*) const override;

  protected:
    double fEnergy = std::nan(""); // Total energy deposited
    double fTheta = std::nan("");  // Reconstructed theta
    double fPhi = std::nan("");    // Reconstructed phi
    double fPid = std::nan("");    // Value of Nf/(Nf+Ns)
    uint8_t fHitType = 0;          // Hit type: 0 stopped particle, 1 punch-through, 2 gamma
    ULong64_t fTime = 0;           // WR time stamp

  public:
    ClassDefOverride(R3BCalifaHitData, 1)
};

// Operator overloading for printing R3BCalifaHitData
std::ostream& operator<<(std::ostream& os, const R3BCalifaHitData& data);
