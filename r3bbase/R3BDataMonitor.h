#pragma once
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

#include "R3BLogger.h"
#include "R3BShared.h"
#include <FairRun.h>
#include <R3BException.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <fmt/core.h>
#include <map>

class FairRun;

namespace R3B
{
    class DataMonitor;

    template <typename ElementType>
    class CanvasElement
    {

      public:
        CanvasElement() = default;
        CanvasElement(ElementType* hist, TVirtualPad* pad)
            : hist_{ hist }
            , pad_{ pad }
        {
        }
        auto* operator->() { return hist_; }
        auto* pad() { return pad_; }

      private:
        ElementType* hist_ = nullptr;
        TVirtualPad* pad_ = nullptr;
    };

    class DataMonitorCanvas
    {
      public:
        explicit DataMonitorCanvas(DataMonitor* monitor);

        template <typename... Args>
        explicit DataMonitorCanvas(DataMonitor* monitor, Args&&... args);

        template <int div_num = 1, typename Hist, typename... Args>
        auto add(Args&&... args) -> CanvasElement<Hist>;

        template <typename Hist, typename... Args>
        constexpr auto add(int div_num, Args&&... args) -> CanvasElement<Hist>;

        template <typename... Args>
        void divide(Args&&... args)
        {
            canvas_->Divide(args...);
        }

        auto get_canvas() -> TCanvas* { return canvas_.get(); }

        void draw();
        void reset();

      private:
        DataMonitor* monitor_ = nullptr;
        std::unique_ptr<TCanvas> canvas_;
        std::map<int, std::vector<TH1*>> histograms_;
    };

    constexpr auto DEFAULT_HIST_MONITOR_DIR = "HistMonitor";
    class DataMonitor
    {
      public:
        DataMonitor() = default;

        template <typename Hist, typename... Args>
        auto add_hist(std::string_view histName, std::string_view histTitle, Args&&... args) -> Hist*;

        template <typename... Args>
        auto create_canvas(std::string_view canvas_name, std::string_view canvas_title, Args&&... args)
            -> DataMonitorCanvas&;

        // Use R3B::make_hist to create unique_ptr<TH1>
        auto add_hist(std::unique_ptr<TH1> hist) -> TH1*;

        auto get(const std::string& histName) -> TH1*;
        auto get_canvas(const std::string& histName) -> DataMonitorCanvas&;
        void save(std::string_view folderName = "", FairSink* sinkFile = FairRun::Instance()->GetSink());
        void draw_canvases();
        void register_canvases(FairRun* run);
        void reset_all_histograms();

      private:
        std::map<std::string, std::unique_ptr<TH1>> histograms_;
        std::map<std::string, DataMonitorCanvas> canvases_;
        static auto get_hist_dir(FairSink* sinkFile) -> TDirectory*;
    };

    template <typename... Args>
    DataMonitorCanvas::DataMonitorCanvas(DataMonitor* monitor, Args&&... args)
        : monitor_{ monitor }
        , canvas_(std::make_unique<TCanvas>(std::forward<Args>(args)...))
    {
    }

    template <int div_num, typename Hist, typename... Args>
    auto DataMonitorCanvas::add(Args&&... args) -> CanvasElement<Hist>
    {
        static_assert(div_num > 0, "Division number of the histogram must be larger than 0!");
        return add<Hist>(div_num, std::forward<Args>(args)...);
    }

    template <typename Hist, typename... Args>
    constexpr auto DataMonitorCanvas::add(int div_num, Args&&... args) -> CanvasElement<Hist>
    {
        auto* hist = monitor_->add_hist<Hist>(std::forward<Args>(args)...);
        // R3BLOG(debug, fmt::format("add a histogram named {} to the canvas {}", hist->GetName(), canvas_->GetName()));
        if (histograms_.find(div_num) != histograms_.end())
        {
            histograms_[div_num].push_back(hist);
        }
        else
        {
            histograms_.emplace(div_num, std::vector<TH1*>{ hist });
        }
        if constexpr (std::is_base_of_v<TH2, Hist>)
        {
            const auto* option = hist->GetOption();
            hist->SetOption(fmt::format("{} COLZ", option).c_str());
        }
        auto* pad = canvas_->GetPad(div_num);
        return CanvasElement{ hist, pad };
    }

    template <typename Hist, typename... Args>
    auto DataMonitor::add_hist(std::string_view histName, std::string_view histTitle, Args&&... args) -> Hist*
    {
        auto hist = R3B::make_hist<Hist>(histName.data(), histTitle.data(), std::forward<Args>(args)...);
        return static_cast<Hist*>(add_hist(std::move(hist)));
    }

    template <typename... Args>
    auto DataMonitor::create_canvas(std::string_view canvas_name, std::string_view canvas_title, Args&&... args)
        -> DataMonitorCanvas&
    {
        if (canvases_.find(std::string{ canvas_name }) != canvases_.end())
        {
            throw R3B::logic_error(fmt::format(
                "A canvas with the name {} has been already added. Please use different name!", canvas_name));
        }
        const auto [it, is_success] = canvases_.insert(
            { std::string{ canvas_name }, DataMonitorCanvas(this, canvas_name.data(), canvas_title.data(), args...) });
        return it->second;
    }
} // namespace R3B
