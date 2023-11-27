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

#include "R3BNeulandHitXYCanvas.h"
#include <R3BNeulandCommon.h>
#include <R3BNeulandOnlineSpectra2.h>
#include <TH2.h>
#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>

namespace R3B::Neuland
{
    void HitXYCanvas::DataInit() { hit_data_.init(); }
    void HitXYCanvas::CanvasInit(DataMonitor& histograms)
    {
        auto& canvas = CreateNewCanvas(histograms);
        auto bar_numbers = GetBarNumber();
        auto num_of_plane = static_cast<int>(bar_numbers / BarsPerPlane);

        canvas.divide(5, 6);

        constexpr auto XY_BIN_SIZE = 300;
        constexpr auto XY_MAX = 150;

        h_xy_per_plane_.reserve(num_of_plane);
        for (const auto& plane_id : ranges::views::iota(0, num_of_plane))
        {
            const auto hist_name = fmt::format("hHitXYPlane{}", plane_id);
            const auto hist_title = fmt::format("Hit XY Plane{}", plane_id);
            auto hist = canvas.add<TH2D>(
                plane_id + 1, hist_name, hist_title, XY_BIN_SIZE, -XY_MAX, XY_MAX, XY_BIN_SIZE, -XY_MAX, XY_MAX);
            h_xy_per_plane_.push_back(hist);
        }
    }

    void HitXYCanvas::CanvasFill(DataMonitor& /*histograms*/)
    {
        auto* rand_gen = GetOnlineSpectra()->GetRandomGenerator();
        for (const auto& hit : hit_data_)
        {
            const auto module_id = hit.module_id;
            const auto plane_id = GetPlaneID(module_id);
            const auto bar_width_error = rand_gen->Uniform(-BarSize_XY / 2., BarSize_XY / 2.);
            // TODO: introduce calibration errors
            if (IsPlaneHorizontal(plane_id))
            {
                h_xy_per_plane_.at(plane_id)->Fill(hit.position.X(), hit.position.Y() + bar_width_error);
            }
            else
            {
                h_xy_per_plane_.at(plane_id)->Fill(hit.position.X() + bar_width_error, hit.position.Y());
            }
        }
    }
    void HitXYCanvas::CanvasFinish() {}
} // namespace R3B::Neuland
