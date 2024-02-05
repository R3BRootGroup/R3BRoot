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

#include "R3BNeulandOnlineSpectra2.h"
#include <FairRunOnline.h>
#include <R3BDataMonitor.h>
#include <TH2.h>
#include <THttpServer.h>
#include <fmt/format.h>

namespace R3B::Neuland
{
    OnlineSpectra::OnlineSpectra(std::string_view name, int iVerbose)
        : CalibrationTask(name, iVerbose)
    {
    }
    OnlineSpectra::~OnlineSpectra() { SaveAllHists(); }

    void OnlineSpectra::HistogramInit(DataMonitor& histograms)
    {
        ActionToSpectra([&histograms](OnlineCanvas* spec) { spec->CanvasInit(histograms); });

        histograms.draw_canvases();
        if (auto* run = FairRunOnline::Instance(); run != nullptr)
        {
            histograms.register_canvases(run);
        }
    }

    void OnlineSpectra::ExtraInit(FairRootManager* /*rootMan*/)
    {
        ActionToSpectra([](OnlineCanvas* spec) { spec->DataInit(); });

        if (auto* run = FairRunOnline::Instance(); run != nullptr)
        {
            init_http_server(run);
        }
    }

    void OnlineSpectra::SetExtraPar(FairRuntimeDb* rtdb) {}

    void OnlineSpectra::TriggeredExec()
    {
        auto& histograms = GetHistMonitor();
        ActionToSpectra(
            [&histograms](OnlineCanvas* spec)
            {
                if (spec->IsTriggered())
                {
                    spec->CanvasFill(histograms);
                }
            });
    }

    void OnlineSpectra::init_http_server(FairRunOnline* run)
    {
        const auto reset_neuland_cmd = fmt::format(R"(/Tasks/{}/->ResetHistos())", GetName());
        const auto reset_neuland_mapped_cmd = fmt::format(R"(/Tasks/{}/->ResetHistosMapped())", GetName());
        const auto save_neuland_hists_cmd = fmt::format(R"(/Tasks/{}/->SaveAllHists())", GetName());

        run->GetHttpServer()->Register("/Tasks", this);
        run->GetHttpServer()->RegisterCommand("/Tasks/Reset_Neuland", reset_neuland_cmd.c_str());
        run->GetHttpServer()->RegisterCommand("/Tasks/Reset_Neuland_Mapped", reset_neuland_mapped_cmd.c_str());
        run->GetHttpServer()->RegisterCommand("/Tasks/save_all_histograms", save_neuland_hists_cmd.c_str());
    }
    void OnlineSpectra::ResetHistos()
    {
        SaveAllHists();
        GetHistMonitor().reset_all_hists();
    }

    void OnlineSpectra::ResetHistosMapped() { GetHistMonitor().get_canvas("NeulandMapped").reset(); }
    void OnlineSpectra::SaveAllHists() { GetHistMonitor().save_all_hists(); }
} // namespace R3B::Neuland

ClassImp(R3B::Neuland::OnlineSpectra);
