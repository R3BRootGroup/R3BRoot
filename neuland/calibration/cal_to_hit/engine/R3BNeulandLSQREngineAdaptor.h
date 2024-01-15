/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
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

#include "R3BNeulandCosmicEngine.h"
#include <R3BNeulandCosmicTracker.h>
#include <R3BNeulandHitCalibrationEngine.h>
#include <R3BShared.h>

namespace R3B::Neuland::Calibration
{
    class LSQREngineAdaptor : public CosmicEngineInterface
    {
      public:
        LSQREngineAdaptor() = default;

      private:
        HitCalibrationEngine hit_cal_engine_;
        CosmicTracker cosmic_tracker_;
        Cal2HitPar hit_parameters_;

        void Init() override;
        void AddSignal(const BarCalData& signal) override;
        void Calibrate() override;
        void EndOfEvent(unsigned int event_num = 0) override;
        [[nodiscard]] auto ExtractParameters() -> Cal2HitPar override { return std::move(hit_parameters_); }
        void Reset() override
        {
            hit_cal_engine_.Reset();
            cosmic_tracker_.Reset();
        }

        // private non-virtual function
        void add_bar_signal(const BarCalData& barSignal, Side side);
    };

} // namespace R3B::Neuland::Calibration
