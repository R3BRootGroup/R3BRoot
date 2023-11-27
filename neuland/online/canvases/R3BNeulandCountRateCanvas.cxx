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

#include "R3BNeulandCountRateCanvas.h"
#include <R3BNeulandOnlineSpectra2.h>
#include <algorithm>
#include <range/v3/numeric.hpp>
#include <range/v3/view.hpp>

namespace rng = ranges;

constexpr auto BEGIN_VIEW_RATIO = 0.1;
constexpr auto END_VIEW_RATIO = 0.9;

namespace R3B::Neuland
{

    void CountRateCanvas::DataInit()
    {
        mapped_data_.init();
        hit_data_.init();
        cal_data_.init();
    }

    void CountRateCanvas::CanvasInit(DataMonitor& histograms)
    {
        auto& canvas = CreateNewCanvas(histograms);

        canvas.divide(2, 2);
        constexpr auto start_graph_data_size = 100;

        los_count_rate_ = canvas.add<1, TGraph>("los_count_rate", start_graph_data_size);
        los_count_rate_->SetTitle("Count rates for LOS");
        los_count_rate_->GetXaxis()->SetTitle("time (min)");
        los_count_rate_->GetYaxis()->SetTitle("counts per min");

        map_count_rate_ = canvas.add<2, TGraph>("map_count_rate", start_graph_data_size);
        map_count_rate_->SetTitle("Count rates for map level");
        map_count_rate_->GetXaxis()->SetTitle("time (min)");
        map_count_rate_->GetYaxis()->SetTitle("counts per min");

        cal_count_rate_ = canvas.add<3, TGraph>("cal_count_rate", start_graph_data_size);
        cal_count_rate_->SetTitle("Count rates for cal level");
        cal_count_rate_->GetXaxis()->SetTitle("time (min)");
        cal_count_rate_->GetYaxis()->SetTitle("counts per min");

        hit_count_rate_ = canvas.add<4, TGraph>("hit_count_rate", start_graph_data_size);
        hit_count_rate_->SetTitle("Count rates for onspill hit level");
        hit_count_rate_->GetXaxis()->SetTitle("time (min)");
        hit_count_rate_->GetYaxis()->SetTitle("counts per min");
    }

    void CountRateCanvas::CanvasFill(DataMonitor& /*histograms*/)
    {
        los_count();
        map_count();
        cal_count();
        hit_count();

        // auto* x_axis = los_count_rate_->GetXaxis();
        // auto range_begin = x_axis->GetBinCenter(x_axis->GetFirst());
        // const auto range_end = x_axis->GetBinCenter(x_axis->GetLast());
        // fmt::print("****min: {}, max: {}, padmax: {}\n", range_begin, range_end, los_count_rate_.pad()->GetUxmax());
        calculate_count_rate();
    }

    void CountRateCanvas::los_count()
    {

        auto* event_header = GetOnlineSpectra()->GetEventHeader();
        const auto time_start = event_header->GetTStart();
        if (not std::isnan(time_start))
        {
            ++los_counter_;
        }
    }

    void CountRateCanvas::map_count()
    {
        map_counter_ += rng::accumulate(
            mapped_data_.get(),
            int{ 0 },
            [](auto init, const auto& plane_signals) -> int
            {
                return init + rng::accumulate(plane_signals | rng::views::values,
                                              int{ 0 },
                                              [](auto init_bar, const auto bar_signals) -> int {
                                                  return init_bar +
                                                         static_cast<int>(std::min(bar_signals.left.size(),
                                                                                   bar_signals.right.size()));
                                              });
            },
            &PaddleTamexMappedData::bars);
    }

    void CountRateCanvas::cal_count()
    {
        cal_counter_ += rng::accumulate(
            cal_data_.get(),
            int{ 0 },
            [](auto init, const auto& bar_signals)
            { return init + static_cast<int>(std::min(bar_signals.left.size(), bar_signals.right.size())); });
    }

    void CountRateCanvas::hit_count()
    {
        auto* online_spectra = GetOnlineSpectra();
        auto is_on_spill = CheckTriggerWithTpat(CalTrigger::onspill,
                                                online_spectra->GetEventHeader()->GetTpat(),
                                                online_spectra->GetBasePar()->GetOffSpillTpatPos());
        const auto time_start = online_spectra->GetEventHeader()->GetTStart();
        if (is_on_spill and not std::isnan(time_start))
        {
            hit_counter_ += static_cast<int>(hit_data_.size());
        }
    }

    void CountRateCanvas::calculate_count_rate()
    {
        auto time_now = std::chrono::steady_clock::now();
        auto time_duration = std::chrono::duration_cast<std::chrono::seconds>(time_now - timer_);
        if ((time_duration) > counting_duration_)
        {
            constexpr auto minute_to_seconds = 60.;
            auto time_passed_seconds = std::chrono::duration_cast<std::chrono::seconds>(time_now - start_time_);
            auto time_passed_minutes = static_cast<double>(time_passed_seconds.count()) / minute_to_seconds;
            auto duration_minutes = static_cast<double>(time_duration.count()) / minute_to_seconds;
            los_count_rate_->SetPoint(los_count_rate_->GetN(), time_passed_minutes, los_counter_ / duration_minutes);
            map_count_rate_->SetPoint(map_count_rate_->GetN(), time_passed_minutes, map_counter_ / duration_minutes);
            cal_count_rate_->SetPoint(cal_count_rate_->GetN(), time_passed_minutes, cal_counter_ / duration_minutes);
            hit_count_rate_->SetPoint(hit_count_rate_->GetN(), time_passed_minutes, hit_counter_ / duration_minutes);
            adjust_graph_view();
            ResetCount();
        }
    }

    inline void CountRateCanvas::set_view(double width)
    {
        do_each_graph(
            [this, width](CanvasElement<TGraph>& graph)
            {
                const auto last_x = graph->GetPointX(graph->GetN() - 1);
                auto* x_axis = graph->GetXaxis();
                const auto range_min = x_axis->GetBinCenter(x_axis->GetFirst());
                const auto range_max = x_axis->GetBinCenter(x_axis->GetLast());
                auto range_begin = range_min;
                if (last_view_mode_ != RangeViewMode::two_hours)
                {
                    if (last_x > width / 2.)
                    {
                        range_begin = last_x - width / 2.;
                    }
                }
                else
                {
                    if (last_x > range_min + width * END_VIEW_RATIO)
                    {
                        range_begin = last_x - width * BEGIN_VIEW_RATIO;
                    }
                }
                if (range_max < range_begin + width)
                {
                    x_axis->SetLimits(0., range_begin + width);
                }
                x_axis->SetRangeUser(range_begin, range_begin + width);
                // graph.pad()->Modified();
                // graph.pad()->RangeChanged();
                graph.pad()->RedrawAxis();
                graph.pad()->Update();

            });
    }

    inline void CountRateCanvas::set_two_hour_view()
    {
        constexpr auto range_width_minutes = 120.;
        set_view(range_width_minutes);
        // auto* x_axis = los_count_rate_->GetXaxis();
        // auto range_begin = x_axis->GetBinCenter(x_axis->GetFirst());
        // const auto range_end = x_axis->GetBinCenter(x_axis->GetLast());
        // fmt::print("****min: {}, max: {}, padmax: {}\n", range_begin, range_end, los_count_rate_.pad()->GetUxmax());
    }

    inline void CountRateCanvas::set_four_hour_view()
    {
        constexpr auto range_width_minutes = 240.;
        set_view(range_width_minutes);
    }

    inline void CountRateCanvas::set_full_view()
    {
        do_each_graph(
            [](CanvasElement<TGraph>& graph)
            {
                auto* axis = graph->GetXaxis();
                axis->UnZoom();
                axis->SetCanExtend(true);
                graph.pad()->RedrawAxis();
            });
    }

    void CountRateCanvas::adjust_graph_view()
    {
        const auto view_mode = GetOnlineSpectra()->GetGraphViewMode();
        if (view_mode == last_view_mode_ and last_view_mode_ == RangeViewMode::full)
        {
            return;
        }
        switch (view_mode)
        {
            case RangeViewMode::full:
                set_full_view();
                break;
            case RangeViewMode::two_hours:
                set_two_hour_view();
                break;
            case RangeViewMode::four_hours:
                set_four_hour_view();
                break;
        }
        last_view_mode_ = view_mode;
        // auto* x_axis = los_count_rate_->GetXaxis();
        // auto range_begin = x_axis->GetBinCenter(x_axis->GetFirst());
        // const auto range_end = x_axis->GetBinCenter(x_axis->GetLast());
        // fmt::print("min: {}, max: {}\n", range_begin, range_end);
    }

    void CountRateCanvas::ResetCount()
    {
        timer_ = std::chrono::steady_clock::now();
        los_counter_ = 0;
        map_counter_ = 0;
        cal_counter_ = 0;
        hit_counter_ = 0;
    }

    void CountRateCanvas::CanvasFinish() {}
} // namespace R3B::Neuland
