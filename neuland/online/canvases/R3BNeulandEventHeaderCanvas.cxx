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

#include "R3BNeulandEventHeaderCanvas.h"
#include "R3BNeulandOnlineSpectra2.h"
#include <fmt/format.h>

namespace R3B::Neuland
{
    void EventHeaderCanvas::DataInit() {}
    void EventHeaderCanvas::CanvasInit(DataMonitor& histograms)
    {
        auto& canvas = CreateNewCanvas(histograms);
        canvas.divide(1, 2);
        h_tpats_ = canvas.add<1, TH1I>("tpats", "tpat value distribution", 1, 0., 0.);
        h_tpats_.pad()->SetLogy();

        h_spill_stat_ = canvas.add<2, TH1I>("spill", "spill statistics", 1, 0., 0.);
        h_spill_stat_.pad()->SetLogy();
    }
    void EventHeaderCanvas::CanvasFill(DataMonitor& /*histograms*/)
    {
        auto* eventHeader = GetOnlineSpectra()->GetEventHeader();
        const auto offspill_bit = GetOnlineSpectra()->GetBasePar()->GetOffSpillTpatPos();
        const auto tpat = eventHeader->GetTpat();
        h_tpats_->Fill(fmt::format("{:016b}", tpat).c_str(), 1);

        if (CheckTriggerWithTpat(CalTrigger::onspill, tpat, offspill_bit))
        {
            h_spill_stat_->Fill("onspill", 1);
        }
        else if (CheckTriggerWithTpat(CalTrigger::offspill, tpat, offspill_bit))
        {
            h_spill_stat_->Fill("offspill", 1);
        }
        else
        {
            h_spill_stat_->Fill("others", 1);
        }
    }
    void EventHeaderCanvas::CanvasFinish() {}
} // namespace R3B::Neuland
