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

#include "R3BNeulandCalCanvas.h"
#include <R3BNeulandCommon.h>
#include <R3BNeulandOnlineSpectra2.h>
#include <TH2.h>

namespace R3B::Neuland
{
    // void CalOnlineCanvas::DataInit() {}
    // void CalOnlineCanvas::CanvasInit(HistMonitor& histograms) {}
    // void CalOnlineCanvas::CanvasFill(HistMonitor& histograms) {}
    // void CalOnlineCanvas::CanvasFinish() {}

    void CalCanvas::DataInit() { cal_data_.init(); }
    void CalCanvas::CanvasInit(DataMonitor& histograms)
    {
        auto& canvas = CreateNewCanvas(histograms);
        auto bar_numbers = GetBarNumber();

        canvas.divide(2, 2);

        constexpr auto TIME_BIN_SIZE = 2000;
        constexpr auto TIME_MIN = -5000.;
        constexpr auto TIME_MAX = 15000.;
        constexpr auto TOT_BIN_SIZE = 600;

        canvas.add<1, TH2D>("hCalTvsBar_L",
                            "CalLevel_left: Time vs Bars  ",
                            bar_numbers,
                            0.5,
                            bar_numbers + 0.5,
                            TIME_BIN_SIZE,
                            TIME_MIN,
                            TIME_MAX);
        canvas.add<2, TH2D>("hCalTvsBar_R",
                            "CalLevel_right: Time vs Bars  ",
                            bar_numbers,
                            0.5,
                            bar_numbers + 0.5,
                            TIME_BIN_SIZE,
                            TIME_MIN,
                            TIME_MAX);
        canvas.add<3, TH2D>("hCalToTvsBar_L",
                            "CalLevel_left: ToT vs Bars",
                            bar_numbers,
                            0.5,
                            bar_numbers + 0.5,
                            TOT_BIN_SIZE,
                            0.,
                            TOT_BIN_SIZE + 0.);
        canvas.add<4, TH2D>("hCalToTvsBar_R",
                            "CalLevel_right: ToT vs Bars",
                            bar_numbers,
                            0.5,
                            bar_numbers + 0.5,
                            TOT_BIN_SIZE,
                            0.,
                            TOT_BIN_SIZE + 0.);
    }

    void CalCanvas::CanvasFill(DataMonitor& histograms)
    {
        for (const auto& bar_signal : cal_data_)
        {
            const auto module_num = bar_signal.module_num;
            for (const auto& left_signal : bar_signal.left)
            {
                histograms.get("hCalTvsBar_L")
                    ->Fill(module_num, (left_signal.leading_time - left_signal.trigger_time).value);
                histograms.get("hCalToTvsBar_L")->Fill(module_num, left_signal.time_over_threshold.value);
            }
            for (const auto& right_signal : bar_signal.right)
            {
                histograms.get("hCalTvsBar_R")
                    ->Fill(module_num, (right_signal.leading_time - right_signal.trigger_time).value);
                histograms.get("hCalToTvsBar_R")->Fill(module_num, right_signal.time_over_threshold.value);
            }
        }
    }
    void CalCanvas::CanvasFinish() {}
} // namespace R3B::Neuland
