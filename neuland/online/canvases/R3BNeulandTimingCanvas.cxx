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

#include "R3BNeulandTimingCanvas.h"
#include <R3BNeulandCommon.h>
#include <R3BNeulandOnlineSpectra2.h>
#include <TH2.h>
#include <range/v3/view.hpp>

namespace rng = ranges;

namespace R3B::Neuland
{
    void TimingCanvas::DataInit()
    {
        hit_data_.init();
        cal_data_.init();
    }

    void TimingCanvas::CanvasInit(DataMonitor& histograms)
    {
        auto& canvas = CreateNewCanvas(histograms);
        auto bar_numbers = GetBarNumber();
        auto num_of_plane = static_cast<int>(bar_numbers / BarsPerPlane);

        canvas.divide(2, 2);

        constexpr auto TOF_BIN_SIZE = 3000;
        constexpr auto TOF_MIN = -100;
        constexpr auto TOF_MAX = 500;

        constexpr auto X_BIN_SIZE = 1000;
        constexpr auto X_MAX = 200;

        constexpr auto TDC_BIN_SIZE = 1000;
        constexpr auto TDC_MAX = 10000;

        hTofvsZ_ =
            canvas.add<1, TH2D>("hTofvsZ", "Tof vs Z", num_of_plane, 0, num_of_plane, TOF_BIN_SIZE, TOF_MIN, TOF_MAX);
        hTofvsZ_.pad()->SetLogz();
        hNeuLANDvsStart_ = canvas.add<2, TH2D>("hNeuLANDvsStart",
                                               "hNeuLANDvsStart",
                                               3 * TDC_BIN_SIZE,
                                               -TDC_MAX,
                                               4 * TDC_MAX,
                                               TDC_BIN_SIZE,
                                               -TDC_MAX,
                                               TDC_MAX);
        hTOFc_ = canvas.add<3, TH1D>("hTOFc", "hTOFc", TOF_BIN_SIZE, TOF_MIN, TOF_MAX);
        hTOFc_.pad()->SetLogy();
        hTofcvsX_ =
            canvas.add<4, TH2D>("hTofcvsX", "Tofc vs X", X_BIN_SIZE, -X_MAX, X_MAX, TOF_BIN_SIZE, TOF_MIN, TOF_MAX);
        hTofcvsX_.pad()->SetLogz();
    }

    void TimingCanvas::hit_data_fill()
    {
        const auto distance_to_target = GetOnlineSpectra()->GetDistanceToTarget();
        auto* rand_gen = GetOnlineSpectra()->GetRandomGenerator();
        for (const auto& hit : hit_data_)
        {
            const auto module_id = hit.module_id;
            const auto plane_id = GetPlaneID(module_id);
            const auto bar_width_error = rand_gen->Uniform(-BarSize_XY / 2., BarSize_XY / 2.);

            const auto time = hit.time;
            const auto correc_time = time - (hit.position.Mag() - distance_to_target) / CLight;
            const auto pos_x = IsPlaneHorizontal(plane_id) ? hit.position.X() : hit.position.X() + bar_width_error;

            hTofvsZ_->Fill(plane_id, time);
            hTofcvsX_->Fill(pos_x, correc_time);
            if (hit.energy > 0.)
            {
                hTOFc_->Fill(correc_time);
            }
        }
    }

    void TimingCanvas::cal_data_fill()
    {
        auto* event_header = GetOnlineSpectra()->GetEventHeader();
        const auto time_start = event_header->GetTStart();
        for (const auto& cal_signal :
             cal_data_ |
                 rng::views::transform([](const auto& bar_signal)
                                       { return ranges::views::concat(bar_signal.left, bar_signal.right); }) |
                 ranges::views::join)
        {
            const auto time_cal = cal_signal.leading_time - cal_signal.trigger_time;
            hNeuLANDvsStart_->Fill(time_start, time_cal.value);
        }
    }

    void TimingCanvas::CanvasFill(DataMonitor& /*histograms*/)
    {
        cal_data_fill();
        hit_data_fill();
    }

    void TimingCanvas::CanvasFinish() {}
} // namespace R3B::Neuland
