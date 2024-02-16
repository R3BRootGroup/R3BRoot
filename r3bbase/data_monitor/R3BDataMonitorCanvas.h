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
#include <TCanvas.h>
#include <TGraph.h>
#include <TH1.h>
#include <TH2.h>
#include <fmt/core.h>
#include <variant>

namespace R3B
{
    class DataMonitor;

    template <typename ElementType>
    class CanvasElement
    {

      public:
        CanvasElement() = default;
        CanvasElement(ElementType* element, TVirtualPad* pad)
            : element_{ element }
            , pad_{ pad }
        {
        }
        auto* get() { return element_; }
        auto* operator->() { return element_; }
        auto* pad() { return pad_; }

      private:
        ElementType* element_ = nullptr;
        TVirtualPad* pad_ = nullptr;
    };

    class DataMonitorCanvas
    {
      public:
        using DrawableElementPtr = std::variant<TH1*, TGraph*>;
        explicit DataMonitorCanvas(DataMonitor* monitor);

        template <typename... Args>
        explicit DataMonitorCanvas(DataMonitor* monitor, Args&&... args);

        template <int div_num = 1, typename Hist, typename... Args>
        [[nodiscard]] constexpr auto add(Args&&... args) -> CanvasElement<Hist>;

        template <typename Hist, typename... Args>
        [[nodiscard]] constexpr auto add(int div_num, Args&&... args) -> CanvasElement<Hist>;

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
        std::map<int, std::vector<DrawableElementPtr>> figures_;
    };
} // namespace R3B
