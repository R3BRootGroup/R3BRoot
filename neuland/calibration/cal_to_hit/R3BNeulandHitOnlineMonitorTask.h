#pragma once

#include "R3BIOConnector.h"
#include "R3BNeulandCalibrationTask.h"
#include "R3BNeulandHit2.h"
#include <string>

class FairRootManager;
class FairRuntimeDb;
class TH2D;
class TH1D;
class TH1L;

namespace R3B
{
    class DataMonitor;
}

namespace R3B::Neuland::Calibration
{
    class HitOnlineMonitorTask;

    struct HitOnlineMonitorTaskConfig
    {
        using Task = HitOnlineMonitorTask;
        bool enable = false;
        int n_hit_min = 0;
        std::string name = "NeulandHitOnlineMonitorTask";
        std::string read = "NeulandHits";
    };

    class HitOnlineMonitorTask : public CalibrationTask
    {
      public:
        using Config = HitOnlineMonitorTaskConfig;
        explicit HitOnlineMonitorTask(const Config& config);

      private:
        Config config_;
        InputVectorConnector<Hit> hit_data_;

        // histograms:
        TH2D* hist_positions_along_bar_ = nullptr;
        TH2D* hist_times_ = nullptr;

        // overriden functions:
        void HistogramInit(DataMonitor& histograms) override;
        void TriggeredExec() override;
        void ExtraInit(FairRootManager* rootMan) override;
        void BeginOfEvent() override {}
        void SetExtraPar(FairRuntimeDb* rtdb) override {}
        void EndOfTask() override {}
        [[nodiscard]] auto CheckConditions([[maybe_unused]] TH1L* hist_condition) const -> bool override;
    };

} // namespace R3B::Neuland::Calibration
