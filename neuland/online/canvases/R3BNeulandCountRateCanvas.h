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

#include "R3BNeulandOnlineCanvas.h"
#include <R3BIOConnector.h>
#include <R3BNeulandCalData2.h>
#include <R3BNeulandHit.h>
#include <R3BPaddleTamexMappedData2.h>
#include <chrono>

constexpr auto DEFAULT_COUNTING_DURATION = std::chrono::seconds{ 10 };

namespace R3B::Neuland
{
    class CountRateCanvas : public OnlineCanvas
    {
      public:
        enum class RangeViewMode
        {
            full,
            four_hours,
            two_hours
        };

        explicit CountRateCanvas(std::string_view name)
            : OnlineCanvas(name)
        {
        }
        void ResetCount();

      private:
        InputVectorConnector<PaddleTamexMappedData> mapped_data_{ "NeulandMappedData" };
        InputVectorConnector<BarCalData> cal_data_{ "NeulandCalData" };
        InputVectorConnector<R3BNeulandHit> hit_data_{ "NeulandHits" };

        CanvasElement<TGraph> los_count_rate_;
        CanvasElement<TGraph> map_count_rate_;
        CanvasElement<TGraph> cal_count_rate_;
        CanvasElement<TGraph> hit_count_rate_;

        int los_counter_ = 0;
        int map_counter_ = 0;
        int cal_counter_ = 0;
        int hit_counter_ = 0;

        std::chrono::time_point<std::chrono::steady_clock> start_time_ = std::chrono::steady_clock::now();
        std::chrono::time_point<std::chrono::steady_clock> timer_;
        std::chrono::seconds counting_duration_ = DEFAULT_COUNTING_DURATION;
        RangeViewMode last_view_mode_ = RangeViewMode::full;

        void DataInit() override;
        void CanvasInit(DataMonitor& histograms) override;
        void CanvasFill(DataMonitor& histograms) override;
        void CanvasFinish() override;

        void los_count();
        void map_count();
        void cal_count();
        void hit_count();
        void calculate_count_rate();
        void adjust_graph_view();

        inline void set_view(double width);
        inline void set_full_view();
        inline void set_two_hour_view();
        inline void set_four_hour_view();

        template <typename UnaryOp>
        void do_each_graph(UnaryOp&& optn);
    };

    template <typename UnaryOp>
    void CountRateCanvas::do_each_graph(UnaryOp&& optn)
    {
        optn(los_count_rate_);
        optn(map_count_rate_);
        optn(cal_count_rate_);
        optn(hit_count_rate_);
    }
} // namespace R3B::Neuland
