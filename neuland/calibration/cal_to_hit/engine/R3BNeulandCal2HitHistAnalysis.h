#pragma once

#include "R3BDataMonitor.h"
#include "R3BLogger.h"
#include "R3BNeulandCalData2.h"
#include "R3BNeulandCalToHitPar.h"
#include "R3BNeulandCosmicEngine.h"
#include "R3BNeulandTSyncAnalysis.h"

#include <TH2.h>
#include <fmt/core.h>
#include <vector>

namespace R3B::Neuland::Calibration
{
    class HistAnalysis : public CosmicEngineInterface
    {
      public:
        HistAnalysis() = default;

      private:
        Cal2HitPar* cal_to_hit_par_ = nullptr;
        int minimum_hit_ = 1;
        TSyncEngine tsync_engine_;

        // histograms:
        TH2D* hist_time_diff_ = nullptr;

        // private virtual functions:
        void Init() override;
        void AddSignals(const std::vector<BarCalData>& signals) override;
        void Calibrate(Cal2HitPar& hit_par) override;
        void EndOfEvent(unsigned int /*event_num*/ = 0) override { tsync_engine_.end_of_event(); }
        void EventReset() override { tsync_engine_.reset(); }
        void EndOfTask() override {}
        auto SignalFilter(const std::vector<BarCalData>& signals) -> bool override;
        void HistInit(DataMonitor& histograms) override;
        void SetMinStat(int min) override
        {
            minimum_hit_ = min;
            R3BLOG(info, fmt::format("Minimum number of hits is set to {}", minimum_hit_));
        }

        // private non-virtual function
        void fill_hist(const BarCalData& signal);
    };
} // namespace R3B::Neuland::Calibration
