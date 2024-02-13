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

#include "R3BDataMonitor.h"
#include "R3BDataMonitorCanvas.h"

namespace R3B
{
    template <typename... Args>
    DataMonitorCanvas::DataMonitorCanvas(DataMonitor* monitor, Args&&... args)
        : monitor_{ monitor }
        , canvas_(std::make_unique<TCanvas>(std::forward<Args>(args)...))
    {
    }

    template <int div_num, typename Hist, typename... Args>
    constexpr auto DataMonitorCanvas::add(Args&&... args) -> CanvasElement<Hist>
    {
        static_assert(div_num > 0, "Division number of the histogram must be larger than 0!");
        return add<Hist>(div_num, std::forward<Args>(args)...);
    }

    template <typename ElementType, typename ContainerType>
    auto DataMonitor::add(std::unique_ptr<ElementType> element, ContainerType& container) -> ElementType*
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
        auto* figure = monitor_->add_hist<ElementType>(std::forward<Args>(args)...);
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
