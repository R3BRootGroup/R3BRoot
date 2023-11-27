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

#include "R3BNeulandHitCanvas.h"
#include <R3BNeulandCommon.h>
#include <R3BNeulandOnlineSpectra2.h>
#include <TH2.h>

namespace R3B::Neuland
{
    void HitCanvas::DataInit() { hit_data_.init(); }
    void HitCanvas::CanvasInit(DataMonitor& histograms)
    {
        auto& canvas = CreateNewCanvas(histograms);
        auto bar_numbers = GetBarNumber();

        canvas.divide(2, 2);

        constexpr auto ENERGY_BIN_SIZE = 2000;
        constexpr auto ENERGY_MAX = 120.;

        constexpr auto TIME_DIFF_BIN_SIZE = 1000;
        constexpr auto TIME_DIFF_MAX = 60.;

        constexpr auto TOF_BIN_SIZE = 6000;
        constexpr auto TOF_MIN = -100;
        constexpr auto TOF_MAX = 400;

        hHitEvsBar_ = canvas.add<1, TH2D>("hHitEvsBar",
                                          "HitLevel: Energy vs Bars ",
                                          bar_numbers,
                                          -0.5,
                                          bar_numbers + 0.5,
                                          ENERGY_BIN_SIZE,
                                          0,
                                          ENERGY_MAX);

        hTdiffvsBar_ = canvas.add<2, TH2D>("hTdiffvsBar",
                                           "Tdiff vs Bars",
                                           bar_numbers,
                                           -0.5,
                                           bar_numbers + 0.5,
                                           TIME_DIFF_BIN_SIZE,
                                           -TIME_DIFF_MAX,
                                           TIME_DIFF_MAX);

        hTofvsBar_ = canvas.add<3, TH2D>(
            "hTofvsBar", "Tof vs Bars", bar_numbers, -0.5, bar_numbers + 0.5, TOF_BIN_SIZE, TOF_MIN, TOF_MAX);
        hTofvsEhit_ = canvas.add<4, TH2D>(
            "hTofvsEhit", "Tof vs Ehit", ENERGY_BIN_SIZE, 0., ENERGY_MAX, TOF_BIN_SIZE, TOF_MIN, TOF_MAX);
    }

    void HitCanvas::CanvasFill(DataMonitor& /*histograms*/)
    {
        // if (hit_data_.size() > 0)
        // {
        //     const auto event_num = GetOnlineSpectra()->GetEventHeader()->GetEventno();
        //     fmt::print("hit spectra event number: {}, size: {} -----------------\n", event_num, hit_data_.size());
        // }

        for (const auto& hit : hit_data_)
        {
            const auto& module_id = hit.module_id;
            hHitEvsBar_->Fill(module_id, hit.energy);
            // std::cout << "energy: " << hit.energy << "\n";
            hTdiffvsBar_->Fill(module_id, hit.tdc_left - hit.tdc_right);

            if (hit.energy > 0.)
            {
                hTofvsBar_->Fill(module_id, hit.time);
                hTofvsEhit_->Fill(hit.energy, hit.time);
            }
        }
    }
    void HitCanvas::CanvasFinish() {}
} // namespace R3B::Neuland
