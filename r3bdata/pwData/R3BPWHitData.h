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

#pragma once

#include <Rtypes.h>
#include <RtypesCore.h>
#include <TObject.h>
#include <cmath>

class R3BPWHitData : public TObject
{
  public:
    // Default constructor
    R3BPWHitData() = default;

    explicit R3BPWHitData(UInt_t detId, UInt_t bar, double time, double pos, double charge, double Tof); // NOLINT

    // Accessors
    [[nodiscard]] UInt_t GetDetId() const { return fDetId; }
    [[nodiscard]] UInt_t GetChannelId() const { return fBar; }
    [[nodiscard]] double GetTime() const { return fTime; }
    [[nodiscard]] double GetPos() const { return fPos; }
    [[nodiscard]] double GetCharge() const { return fCharge; }
    [[nodiscard]] double GetTof() const { return fTof; }

    // Modifiers
    void SetDetId(UInt_t detId) { fDetId = detId; }
    void SetChannelId(UInt_t bar) { fBar = bar; }
    void SetPos(double pos) { fPos = pos; }
    void SetTime(double time) { fTime = time; }
    void SetCharge(double charge) { fCharge = charge; }

  private:
    // Basic Hit information
    UInt_t fDetId = 0;
    UInt_t fBar = 0;
    double fTime = std::nan("");
    double fPos = std::nan("");
    double fCharge = std::nan("");
    double fTof = std::nan(""); // Time-of-flight with respect to diamond detector

  public:
    ClassDefOverride(R3BPWHitData, 1);
};
