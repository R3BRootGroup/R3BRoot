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

#include "R3BNeulandMappedCanvas.h"
#include <R3BLogger.h>
#include <R3BNeulandCommon.h>

namespace R3B::Neuland
{
    void MappedCanvas::DataInit() { mapped_data_.init(); }
    void MappedCanvas::CanvasInit(DataMonitor& histograms)
    {
        auto& canvas = CreateNewCanvas(histograms);
        auto bar_numbers = GetBarNumber();

        canvas.divide(1, 2);
        canvas.add<1, TH1D>("hMappedBar1fLE", "Mapped: Bars fine 1LE", bar_numbers, 0.5, bar_numbers + 0.5)
            ->SetLineColor(kBlue);
        canvas.add<1, TH1D>("hMappedBar1fTE", "Mapped: Bars fine 1TE", bar_numbers, 0.5, bar_numbers + 0.5)
            ->SetLineColor(kBlack);
        canvas.add<1, TH1D>("hMappedBar1cLE", "Mapped: Bars coarse 1LE", bar_numbers, 0.5, bar_numbers + 0.5)
            ->SetLineColor(kRed);
        canvas.add<1, TH1D>("hMappedBar1cTE", "Mapped: Bars coarse 1TE", bar_numbers, 0.5, bar_numbers + 0.5)
            ->SetLineColor(kOrange);

        canvas.add<2, TH1D>("hMappedBar2fLE", "Mapped: Bars fine 2LE", bar_numbers, 0.5, bar_numbers + 0.5)
            ->SetLineColor(kBlue);
        canvas.add<2, TH1D>("hMappedBar2fTE", "Mapped: Bars fine 2TE", bar_numbers, 0.5, bar_numbers + 0.5)
            ->SetLineColor(kBlack);
        canvas.add<2, TH1D>("hMappedBar2cLE", "Mapped: Bars coarse 2LE", bar_numbers, 0.5, bar_numbers + 0.5)
            ->SetLineColor(kRed);
        canvas.add<2, TH1D>("hMappedBar2cTE", "Mapped: Bars coarse 2TE", bar_numbers, 0.5, bar_numbers + 0.5)
            ->SetLineColor(kOrange);
    }

    void MappedCanvas::CanvasFill(DataMonitor& histograms)
    {
        auto fill_if_positive = [](double check_value, TH1* histogram, double fill_value)
        {
            if (check_value > 0)
            {
                histogram->Fill(fill_value);
            }
        };

        for (const auto& plane_data : mapped_data_)
        {
            const auto plane_num = plane_data.plane_num;
            for (const auto& [bar_num, bar_data] : plane_data.bars)
            {
                const auto module_num = Neuland_PlaneBar2ModuleNum(plane_num, bar_num);
                for (const auto& left_signal : bar_data.left)
                {
                    fill_if_positive(left_signal.leading.fine, histograms.get("hMappedBar1fLE"), module_num);
                    fill_if_positive(left_signal.leading.coarse, histograms.get("hMappedBar1cLE"), module_num);
                    fill_if_positive(left_signal.trailing.fine, histograms.get("hMappedBar1fTE"), module_num);
                    fill_if_positive(left_signal.trailing.coarse, histograms.get("hMappedBar1cTE"), module_num);
                }
                for (const auto& right_signal : bar_data.right)
                {
                    fill_if_positive(right_signal.leading.fine, histograms.get("hMappedBar2fLE"), module_num);
                    fill_if_positive(right_signal.leading.coarse, histograms.get("hMappedBar2cLE"), module_num);
                    fill_if_positive(right_signal.trailing.fine, histograms.get("hMappedBar2fTE"), module_num);
                    fill_if_positive(right_signal.trailing.coarse, histograms.get("hMappedBar2cTE"), module_num);
                }
            }
        }
    }

    void MappedCanvas::CanvasFinish() {}

} // namespace R3B::Neuland
