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

#include "R3BDataMonitorCanvas.h"
#include "R3BLogger.h"
#include "R3BShared.h"
#include <FairRun.h>
#include <R3BException.h>
#include <map>
#include <string>

class FairRun;

namespace R3B
{
    constexpr auto DEFAULT_HIST_MONITOR_DIR = "HistMonitor";
    class DataMonitor
    {
      public:
        DataMonitor() = default;

        template <typename... Args>
        auto create_canvas(std::string_view canvas_name, std::string_view canvas_title, Args&&... args)
            -> DataMonitorCanvas&;

        // Use R3B::make_hist to create unique_ptr<TH1>
        auto add_hist(std::unique_ptr<TH1> hist) -> TH1* { return add_to_map(std::move(hist), histograms_); }
        auto add_graph(std::string_view graph_name, std::unique_ptr<TGraph> graph) -> TGraph*
        {
            graph->SetName(graph_name.data());
            return add_to_map(std::move(graph), graphs_);
        }

        template <typename... Args>
        [[nodiscard]] auto add_graph(std::string_view graph_name, Args&&... args) -> TGraph*
        {
            return add_graph(graph_name, std::make_unique<TGraph>(std::forward<Args>(args)...));
        }
        template <typename Hist, typename... Args>
        auto add_hist(std::string_view histName, std::string_view histTitle, Args&&... args) -> Hist*;

        // TODO: these methods should be removed
        auto get(const std::string& histName) -> TH1*;
        auto get_canvas(const std::string& histName) -> DataMonitorCanvas&;

        void save(std::string_view folderName = "", FairSink* sinkFile = FairRun::Instance()->GetSink());
        void draw_canvases();
        void register_canvases(FairRun* run);
        void reset_all_hists();
        void save_all(std::string_view filename = "");

      private:
        std::string save_filename_{ "histograms_save" };
        std::map<std::string, std::unique_ptr<TH1>> histograms_;
        std::map<std::string, std::unique_ptr<TGraph>> graphs_;
        // std::vector<std::unique_ptr<TGraph>> graphs_;
        std::map<std::string, DataMonitorCanvas> canvases_;
        static auto get_hist_dir(FairSink* sinkFile) -> TDirectory*;

        template <typename ElementType, typename ContainerType>
        static auto add_to_map(std::unique_ptr<ElementType> element, ContainerType& container) -> ElementType*;
        template <typename ElementType, typename ContainerType>
        static auto add_to_vector(std::unique_ptr<ElementType> element, ContainerType& container) -> ElementType*
        {
            auto* ptr = element.get();
            container.emplace_back(std::move(element));
            return ptr;
        }
    };

    template <typename... Args>
    DataMonitorCanvas::DataMonitorCanvas(DataMonitor* monitor, Args&&... args)
        : monitor_{ monitor }
        , canvas_(std::make_unique<TCanvas>(std::forward<Args>(args)...))
    {
    }

    template <int div_num, typename ElementType, typename... Args>
    constexpr auto DataMonitorCanvas::add(Args&&... args) -> CanvasElement<ElementType>
    {
        static_assert(div_num > 0, "Division number of the histogram must be larger than 0!");
        return add<ElementType>(div_num, std::forward<Args>(args)...);
    }

    template <typename ElementType, typename ContainerType>
    auto DataMonitor::add_to_map(std::unique_ptr<ElementType> element, ContainerType& container) -> ElementType*
    {
        const auto* element_name = element->GetName();
        if (container.find(std::string{ element_name }) != container.end())
        {
            throw R3B::logic_error(fmt::format(
                "Element with the name {} has been already added. Please use different name!", element_name));
        }
        const auto [it, is_success] = container.insert({ std::string{ element_name }, std::move(element) });
        return it->second.get();
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

    template <typename ElementType, typename... Args>
    constexpr auto DataMonitorCanvas::add(int div_num, Args&&... args) -> CanvasElement<ElementType>
    {
        ElementType* figure = nullptr;
        if constexpr (std::is_same_v<TGraph, ElementType>)
        {
            figure = monitor_->add_graph<ElementType>(std::forward<Args>(args)...);
        }
        else
        {
            figure = monitor_->add_hist<ElementType>(std::forward<Args>(args)...);
        }

        if (figures_.find(div_num) != figures_.end())
        {
            figures_[div_num].emplace_back(figure);
        }
        else
        {
            figures_.emplace(div_num, std::vector<DrawableElementPtr>{ figure });
        }
        if constexpr (std::is_base_of_v<TH2, ElementType>)
        {
            const auto* option = figure->GetOption();
            figure->SetOption(fmt::format("{} COLZ", option).c_str());
        }
        auto* pad = canvas_->GetPad(div_num);
        return CanvasElement{ figure, pad };
    }

} // namespace R3B
