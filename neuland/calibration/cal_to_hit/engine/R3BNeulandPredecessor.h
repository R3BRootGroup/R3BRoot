#pragma once

#include "R3BNeulandCosmicEngine.h"

namespace R3B::Neuland::Calibration
{
    class Predecessor : public CosmicEngineInterface
    {
      public:
        Predecessor() = default;

      private:
        Cal2HitPar* cal_to_hit_par_ = nullptr;
        int minimum_hit_ = 1;

        // histograms:
        TH2D* hist_time_diff_ = nullptr;
        // TH2D* hist_time_sum_ = nullptr;

        // private virtual functions:
        void Init() override;
        void AddSignal(const BarCalData& signal) override;
        void Calibrate(Cal2HitPar& hit_par) override;
        void EndOfEvent(unsigned int event_num = 0) override {}
        void EventReset() override {}
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
