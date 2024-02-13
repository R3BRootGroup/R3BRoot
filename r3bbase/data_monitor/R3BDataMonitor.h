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
        auto add_hist(std::unique_ptr<TH1> hist) -> TH1* { return add(std::move(hist), histograms_); }
        auto add_graph(std::unique_ptr<TGraph> graph) -> TGraph* { return add(std::move(graph), graphs_); }

        template <typename... Args>
        [[nodiscard]] auto add_graph(Args&&... args) -> TGraph*
        {
            return add_graph(std::make_unique<TGraph>(std::forward<Args>(args)...));
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
        void save_all_hists(std::string_view filename = "");

      private:
        std::string save_filename_{ "histograms_save" };
        std::map<std::string, std::unique_ptr<TH1>> histograms_;
        std::map<std::string, std::unique_ptr<TGraph>> graphs_;
        std::map<std::string, DataMonitorCanvas> canvases_;
        static auto get_hist_dir(FairSink* sinkFile) -> TDirectory*;

        template <typename ElementType, typename ContainerType>
        static auto add(std::unique_ptr<ElementType> element, ContainerType& container) -> ElementType*;
    };

} // namespace R3B

#include "TemplateImp.h"
