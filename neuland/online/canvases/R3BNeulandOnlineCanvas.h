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

#include <R3BDataMonitor.h>
#include <R3BNeulandCommon.h>
#include <R3BNeulandTriggerTypes.h>
#include <string>

constexpr auto NEULAND_DEFAULT_BAR_NUM = 1300;
namespace R3B::Neuland
{
    class OnlineSpectra;
    class OnlineCanvas
    {
      public:
        virtual ~OnlineCanvas() = default;
        OnlineCanvas(const OnlineCanvas&) = default;
        OnlineCanvas(OnlineCanvas&&) = default;
        auto operator=(const OnlineCanvas&) -> OnlineCanvas& = delete;
        auto operator=(OnlineCanvas&&) -> OnlineCanvas& = delete;

        explicit OnlineCanvas(std::string_view name)
            : name_{ name }
        {
        }

        void SetTrigger(CalTrigger trigger) { trigger_type_ = trigger; }
        void SetOnlineSpectra(OnlineSpectra* spectra) { online_spectra_ = spectra; };
        [[nodiscard]] auto GetBarNumber() const -> int { return bar_numbers_; }
        [[nodiscard]] auto GetOnlineSpectra() const -> OnlineSpectra* { return online_spectra_; }

        [[nodiscard]] auto IsTriggered() const -> bool;

        auto CreateNewCanvas(DataMonitor& histograms) -> DataMonitorCanvas&
        {
            return histograms.create_canvas(
                name_, name_, top_left_coordinate.first, top_left_coordinate.second, width_along_x, width_along_y);
        }

        virtual void DataInit() = 0;
        virtual void CanvasInit(DataMonitor& histograms) = 0;
        virtual void CanvasFill(DataMonitor& histograms) = 0;
        virtual void CanvasFinish(){};

      private:
        std::string name_;
        CalTrigger trigger_type_ = CalTrigger::all;
        OnlineSpectra* online_spectra_ = nullptr;
        int bar_numbers_ = NEULAND_DEFAULT_BAR_NUM;

        static constexpr auto top_left_coordinate = std::make_pair(10, 10);
        static constexpr auto width_along_x = 850;
        static constexpr auto width_along_y = 850;
    };
} // namespace R3B::Neuland
