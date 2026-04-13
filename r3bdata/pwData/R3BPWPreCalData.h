/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                     *
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

#include "TObject.h"

class R3BPWPreCalData : public TObject
{
  public:
    /** Default constructor **/
    R3BPWPreCalData();

    explicit R3BPWPreCalData(UShort_t DetId, UShort_t channelId, double Time, double Tot);

    /** Destructor **/
    virtual ~R3BPWPreCalData() = default;

    /** Accessors **/
    [[nodiscard]] UShort_t GetDetId() const { return fDetId; }
    [[nodiscard]] UShort_t GetChannelId() const { return fChannelId; }
    [[nodiscard]] double GetTime() const { return fTime; }
    [[nodiscard]] double GetTot() const { return fTot; }

  private:
    UShort_t fDetId;     // Channel unique identifier
    UShort_t fChannelId; // Channel unique identifier
    double fTime;        // Fine time
    double fTot;         // Coarse time

  public:
    ClassDefOverride(R3BPWPreCalData, 1)
};
