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

#include "R3BNeulandHitCosmicCanvas.h"
#include <R3BNeulandCommon.h>
#include <R3BNeulandOnlineSpectra2.h>
#include <TH2.h>
#include <range/v3/algorithm.hpp>

constexpr auto REFERENCE_BAR_ID_FRONT = 624;
constexpr auto REFERENCE_BAR_ID_BACK = 674;

namespace R3B::Neuland
{
    void HitCosmicCanvas::DataInit() { hit_data_.init(); }
    void HitCosmicCanvas::CanvasInit(DataMonitor& histograms)
    {
        auto& canvas = CreateNewCanvas(histograms);
        auto bar_numbers = GetBarNumber();
        canvas.divide(2, 3);

        constexpr auto TIME_BIN_SIZE = 1000;
        constexpr auto ENERGY_BIN_SIZE = 2000;
        constexpr auto ENERGY_MAX = 120.;
        constexpr auto TIME_MAX = 20.;

        hHitEvsBarCosmics_ = canvas.add<1, TH2D>("hHitEvsBarCosmics",
                                                 "HitLevel: Energy vs Bars cosmics",
                                                 bar_numbers,
                                                 -0.5,
                                                 bar_numbers + 0.5,
                                                 ENERGY_BIN_SIZE,
                                                 0,
                                                 ENERGY_MAX);

        hTdiffvsBarCosmics_ = canvas.add<2, TH2D>("hTdiffvsBarCosmics",
                                                  "Tdiff vs Bars cosmics",
                                                  bar_numbers,
                                                  -0.5,
                                                  bar_numbers + 0.5,
                                                  TIME_BIN_SIZE,
                                                  -ENERGY_MAX,
                                                  ENERGY_MAX);

        hDTBack_ = canvas.add<3, TH2D>("hDT675",
                                       "Thit - Thit675 vs Bars cosmics",
                                       bar_numbers,
                                       -0.5,
                                       bar_numbers + 0.5,
                                       TIME_BIN_SIZE,
                                       -TIME_MAX,
                                       TIME_MAX);

        hDTBackc_ = canvas.add<4, TH2D>("hDT675c",
                                        "Thit - Thit675 vs Bars cosmics corrected",
                                        bar_numbers,
                                        -0.5,
                                        bar_numbers + 0.5,
                                        TIME_BIN_SIZE,
                                        -TIME_MAX,
                                        TIME_MAX);

        hDTFront_ = canvas.add<5, TH2D>("hDT625",
                                        "Thit - Thit625 vs Bars cosmics",
                                        bar_numbers,
                                        -0.5,
                                        bar_numbers + 0.5,
                                        TIME_BIN_SIZE,
                                        -TIME_MAX,
                                        TIME_MAX);

        hDTFrontc_ = canvas.add<6, TH2D>("hDT625c",
                                         "Thit - Thit625 vs Bars cosmics corrected",
                                         bar_numbers,
                                         -0.5,
                                         bar_numbers + 0.5,
                                         TIME_BIN_SIZE,
                                         -TIME_MAX,
                                         TIME_MAX);
    }

    void HitCosmicCanvas::CanvasFill(DataMonitor& /*histograms*/)
    {
        for (const auto& hit : hit_data_)
        {
            const auto module_id = hit.module_id;
            hHitEvsBarCosmics_->Fill(module_id, hit.energy);
            // std::cout << "energy: " << hit.energy << "\n";
            hTdiffvsBarCosmics_->Fill(module_id, hit.tdc_left - hit.tdc_right);

            auto fill_data_ref = [this, &hit, module_id](auto module_id_ref, auto& hist, auto& hist_c)
            {
                if (module_id == module_id_ref)
                {
                    return;
                }

                if (auto res = ranges::find_if(
                        hit_data_.get(), [&module_id_ref](const auto& ele) { return ele.module_id == module_id_ref; });
                    res != hit_data_.get().end())
                {
                    const auto& hit_ref = *(res);
                    const auto time_mean = (hit.tdc_left + hit.tdc_right) / 2;
                    const auto time_mean_ref = (hit_ref.tdc_left + hit_ref.tdc_right) / 2;
                    const auto time_diff = time_mean - time_mean_ref;

                    const auto distance = hit.position - hit_ref.position;
                    const auto time_c = std::copysign(distance.Mag() / CLight, distance.Y());

                    hist->Fill(module_id, time_diff);
                    hist_c->Fill(module_id, time_diff + time_c);
                }
            };

            fill_data_ref(REFERENCE_BAR_ID_FRONT, hDTFront_, hDTFrontc_);
            fill_data_ref(REFERENCE_BAR_ID_BACK, hDTBack_, hDTBackc_);
        }
    }
    void HitCosmicCanvas::CanvasFinish() {}
} // namespace R3B::Neuland
