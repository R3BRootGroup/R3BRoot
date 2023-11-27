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

#include "R3BDataMonitor.h"
#include <FairRootFileSink.h>
#include <FairRunOnline.h>
#include <FairRuntimeDb.h>
#include <fmt/format.h>

namespace R3B
{
    DataMonitorCanvas::DataMonitorCanvas(DataMonitor* monitor)
        : monitor_{ monitor }
    {
    }

    void DataMonitorCanvas::draw()
    {
        for (auto& [div_num, hists] : histograms_)
        {
            canvas_->cd(div_num);
            for (auto iter = hists.begin(); iter != hists.end(); ++iter)
            {
                const auto* option = (*iter)->GetOption();
                if (iter == hists.begin())
                {

                    (*iter)->Draw(option);
                }
                else
                {
                    (*iter)->Draw(fmt::format("same {}", option).c_str());
                }
            }
        }
    }
    void DataMonitorCanvas::reset()
    {
        for (auto& [div_num, hists] : histograms_)
        {
            for (auto* hist : hists)
            {
                hist->Reset();
            }
        }
    }

    auto DataMonitor::get(const std::string& histName) -> TH1*
    {
        if (auto hist = histograms_.find(histName); hist != histograms_.end())
        {
            return hist->second.get();
        }
        throw R3B::logic_error(fmt::format("Histogram with the name {} doesn't exist!", histName));
    }

    auto DataMonitor::get_canvas(const std::string& histName) -> DataMonitorCanvas&
    {
        if (auto canvas = canvases_.find(histName); canvas != canvases_.end())
        {
            return canvas->second;
        }
        throw R3B::logic_error(fmt::format("Canvas with the name {} doesn't exist!", histName));
    }

    void DataMonitor::save(std::string_view folderName, FairSink* sinkFile)
    {
        auto* hist_dir = get_hist_dir(sinkFile);
        auto* new_dir = folderName.empty() ? hist_dir : hist_dir->mkdir(folderName.data(), "", true);
        if (new_dir == nullptr)
        {
            throw R3B::runtime_error(
                fmt::format("Failed to create a sub directory {} for the histrogams!", folderName));
        }

        for (const auto& [_, histogram] : histograms_)
        {
            new_dir->WriteObject(histogram.get(), histogram->GetName(), "overwrite");
        }
        // old_dir->cd();
    }

    auto DataMonitor::add_hist(std::unique_ptr<TH1> hist) -> TH1*
    {
        const auto* histName = hist->GetName();
        if (histograms_.find(std::string{ histName }) != histograms_.end())
        {
            throw R3B::logic_error(
                fmt::format("Histogram with the name {} has been already added. Please use different name!", histName));
        }
        const auto [it, is_success] = histograms_.insert({ std::string{ histName }, std::move(hist) });
        return it->second.get();
    }

    auto DataMonitor::get_hist_dir(FairSink* sinkFile) -> TDirectory*
    {
        auto* rootSinkFile = dynamic_cast<FairRootFileSink*>(sinkFile);
        if (rootSinkFile == nullptr)
        {
            throw R3B::logic_error("Cannot save the histograms as the output file is not a root file!");
        }
        auto* rootFile = rootSinkFile->GetRootFile();
        auto* hist_dir = rootFile->mkdir(DEFAULT_HIST_MONITOR_DIR, "", true);
        if (hist_dir == nullptr)
        {
            throw R3B::runtime_error("Cannot create a directory for the histrogams!");
        }
        return hist_dir;
    }

    void DataMonitor::draw_canvases()
    {
        for (auto& [canvas_name, canvas] : canvases_)
        {
            canvas.draw();
        }
    }

    void DataMonitor::register_canvases(FairRun* run)
    {
        if (auto* run_online = dynamic_cast<FairRunOnline*>(run); run_online != nullptr)
        {
            for (auto& [name, canvas] : canvases_)
            {
                run_online->AddObject(canvas.get_canvas());
            }
        }
    }

    void DataMonitor::reset_all_histograms()
    {
        for (auto& [name, hist] : histograms_)
        {
            hist->Reset();
        }
    }
} // namespace R3B
