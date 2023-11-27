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

#include "R3BNeulandTJumpCanvas.h"
#include <R3BNeulandCommon.h>
#include <R3BNeulandOnlineSpectra2.h>
#include <TH2.h>
#include <range/v3/view.hpp>

constexpr auto EVENT_NUM_MAX = 10000000;

namespace R3B::Neuland
{
    void TJumpCanvas::DataInit() { cal_data_.init(); }
    void TJumpCanvas::CanvasInit(DataMonitor& histograms)
    {

        auto& canvas = CreateNewCanvas(histograms);
        auto bar_numbers = GetBarNumber();
        canvas.divide(2, 2);

        constexpr auto TSTART_BIN_SIZE = 1000;
        constexpr auto TSTART_MIN = -11000.;
        constexpr auto TSTART_MAX = 50000.;

        constexpr auto ZOOM_TIME_MAX = 200.;

        hTstart_ = canvas.add<1, TH1D>("hTstart", "Tstart", TSTART_BIN_SIZE, TSTART_MIN, TSTART_MAX);
        hTestJump_ = canvas.add<2, TH2D>(
            "hTestJump", "Test Jump", bar_numbers, 0.5, bar_numbers + 0.5, TSTART_BIN_SIZE, TSTART_MIN, -TSTART_MIN);
        hTestJump_.pad()->SetLogz();
        hTestJumpvsEvnt_ = canvas.add<3, TH2D>("hJumpsvsEvnt",
                                               "Jumps vs Evnt",
                                               TSTART_BIN_SIZE,
                                               0,
                                               EVENT_NUM_MAX,
                                               TSTART_BIN_SIZE,
                                               TSTART_MIN,
                                               -TSTART_MIN);
        hTestJumpvsEvnt_.pad()->SetLogz();

        hTestJumpvsEvntzoom_ = canvas.add<4, TH2D>("hJumpsvsEvntzoom",
                                                   "Jumps vs Evnt zoomed",
                                                   TSTART_BIN_SIZE,
                                                   0,
                                                   EVENT_NUM_MAX,
                                                   TSTART_BIN_SIZE,
                                                   -ZOOM_TIME_MAX,
                                                   ZOOM_TIME_MAX);
        hTestJumpvsEvntzoom_.pad()->SetLogz();
    }

    void TJumpCanvas::CanvasFill(DataMonitor& /*histograms*/)
    {
        auto* event_header = GetOnlineSpectra()->GetEventHeader();
        const auto event_number = static_cast<unsigned int>(event_header->GetEventno() % EVENT_NUM_MAX);
        if (event_number < last_event_num_)
        {
            hTestJumpvsEvnt_->Reset();
            hTestJumpvsEvntzoom_->Reset();
        }
        last_event_num_ = event_number;

        auto time_start = event_header->GetTStart();
        if (not std::isnan(time_start))
        {
            hTstart_->Fill(time_start);
            // fmt::print("tstart: {}\n", time_start);
        }

        for (const auto& bar_signal_one : cal_data_)
        {
            const auto module_num = bar_signal_one.module_num;
            for (const auto& signal_one : ranges::views::concat(bar_signal_one.left, bar_signal_one.right))
            {
                for (const auto& signal_other :
                     cal_data_ |
                         ranges::views::transform(
                             [](const auto& bar_signal)
                             { return ranges::views::concat(bar_signal.left, bar_signal.right); }) |
                         ranges::views::join)
                {
                    const auto time_one = signal_one.leading_time - signal_one.trigger_time;
                    const auto time_other = signal_other.leading_time - signal_other.trigger_time;
                    const auto time_jump = (time_one - time_other).value;
                    hTestJump_->Fill(module_num, time_jump);
                    hTestJumpvsEvnt_->Fill(event_number, time_jump);
                    hTestJumpvsEvntzoom_->Fill(event_number, time_jump);
                }
            }
        }
    }

    void TJumpCanvas::CanvasFinish() {}
} // namespace R3B::Neuland
