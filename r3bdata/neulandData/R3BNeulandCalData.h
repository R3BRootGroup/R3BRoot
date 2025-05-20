/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
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
#include <Rtypes.h>
#include <RtypesCore.h>
#include <iostream>

class R3BNeulandCalData : public TObject
{
  public:
    R3BNeulandCalData() = default;
    R3BNeulandCalData(int barId, int side, double time, double triggertime, double qdc);

    [[nodiscard]] auto GetBarId() const -> int { return fBarId; }
    [[nodiscard]] auto GetSide() const -> int { return fSide; }
    [[nodiscard]] auto GetTime() const -> double { return fTime; }
    [[nodiscard]] auto GetTriggerTime() const -> double { return fTriggerTime; }
    [[nodiscard]] auto GetQdc() const -> double { return fQdc; }

    void Print(const Option_t* /*option*/) const override;

  private:
    int fBarId = 0;
    int fSide = 0;
    double fTime = 0.;
    double fTriggerTime = 0.;
    double fQdc = 0.;

    ClassDefOverride(R3BNeulandCalData, 2)
};

auto operator<<(std::ostream&, const R3BNeulandCalData&) -> std::ostream&; // Support easy printing
