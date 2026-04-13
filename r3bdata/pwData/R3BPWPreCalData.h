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

class R3BPWPreCalData : public TObject
{
  public:
    // Default constructor
    R3BPWPreCalData() = default;

    explicit R3BPWPreCalData(UShort_t DetId, UShort_t channelId, double Time, double Tot); // NOLINT

    // Accessors
    [[nodiscard]] UShort_t GetDetId() const { return fDetId; }
    [[nodiscard]] UShort_t GetChannelId() const { return fChannelId; }
    [[nodiscard]] double GetTime() const { return fTime; }
    [[nodiscard]] double GetTot() const { return fTot; }

  private:
    UShort_t fDetId = 0;         // Channel unique identifier
    UShort_t fChannelId = 0;     // Channel unique identifier
    double fTime = std::nan(""); // Fine time
    double fTot = std::nan("");  // Coarse time

  public:
    ClassDefOverride(R3BPWPreCalData, 1);
};
