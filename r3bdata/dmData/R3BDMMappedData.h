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
#include <cstdint>

class R3BDMMappedData : public TObject
{
  public:
    // Default Constructor
    R3BDMMappedData() = default;

    explicit R3BDMMappedData(UShort_t DetId,
                             UShort_t ChannelId,
                             uint64_t FineTime,
                             uint64_t CoarseTime,
                             UShort_t Edge,
                             uint64_t Epoch,
                             UShort_t TDC); // NOLINT

    // Getters
    [[nodiscard]] UShort_t GetDetId() const { return fDetId; }
    [[nodiscard]] UShort_t GetChannelId() const { return fChannelId; }
    [[nodiscard]] uint64_t GetFineTime() const { return fFineTime; }
    [[nodiscard]] uint64_t GetCoarseTime() const { return fCoarseTime; }
    [[nodiscard]] UShort_t GetEdge() const { return fEdge; }
    [[nodiscard]] uint64_t GetEpoch() const { return fEpoch; }
    [[nodiscard]] UShort_t GetTDC() const { return fTDC; }

  private:
    UShort_t fDetId = 0;       // Detector unique identifier
    UShort_t fChannelId = 0;   // Channel unique identifier
    uint64_t fFineTime = 0.;   // Fine time
    uint64_t fCoarseTime = 0.; // Coarse time
    UShort_t fEdge = 0;        // Leading or Trailing
    uint64_t fEpoch = 0.;      // Epoch counter
    UShort_t fTDC = 0;         // TDC identifier

  public:
    ClassDefOverride(R3BDMMappedData, 1);
};
