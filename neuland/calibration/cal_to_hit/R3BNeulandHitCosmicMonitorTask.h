#pragma once

#include "R3BHuberRegression.h"
#include "R3BIOConnector.h"
#include "R3BNeulandCalibrationTask.h"
#include "R3BNeulandHit2.h"
#include "R3BNeulandMilleCalDataProcessor.h"
#include <TH1.h>
#include <TH2.h>
#include <chrono>
#include <string>
#include <vector>

class FairRootManager;
class FairRuntimeDb;

namespace R3B
{
    class DataMonitor;
}

namespace R3B::Neuland::Calibration
{
    class CosmicMonitorTask;

    struct CosmicMonitorTaskConfig
    {
        using Task = CosmicMonitorTask;
        bool enable = false;
        int n_plane_min = 0;
        int n_hit_min = 0;
        double max_abs_a_xz = -1.;
        double max_abs_a_yz = -1.;
        std::string name = "NeulandHitCosmicMonitorTask";
        std::string read = "NeulandHits";
        std::string write;
    };

    class CosmicMonitorTask : public CalibrationTask
    {
      public:
        using Config = CosmicMonitorTaskConfig;

        explicit CosmicMonitorTask(const Config& config);

      private:
        Config config_;
        InputVectorConnector<Hit> hit_data_;
        TrackFitResult track_info_;
        NeulandTrackDataSet track_dataset_xz_;
        NeulandTrackDataSet track_dataset_yz_;
        HuberRegressor huber_regressor_;
        std::vector<int> plane_counter_;
        std::chrono::microseconds fit_duration_ms_{};

        TH1D* hist_fit_time_duration_us_ = nullptr;
        TH1D* hist_fit_iterations_ = nullptr;
        TH1D* hist_a_xz_ = nullptr;
        TH1D* hist_b_xz_ = nullptr;
        TH1D* hist_a_yz_ = nullptr;
        TH1D* hist_b_yz_ = nullptr;
        TH1D* hist_t_diff_module_counts_ = nullptr;
        TH2D* hist_fit_diff_time_ = nullptr;
        TH2D* hist_fit_diff_bar_pos_ = nullptr;

        // overriden functions:
        void HistogramInit(DataMonitor& histograms) override;
        void TriggeredExec() override;
        void ExtraInit(FairRootManager* rootMan) override;
        void SetExtraPar(FairRuntimeDb* rtdb) override {}
        void BeginOfEvent() override;
        void EndOfTask() override {}
        [[nodiscard]] auto CheckConditions([[maybe_unused]] TH1L* hist_condition) const -> bool override;

        // non-virtual functions:
        void fill_histograms();
    };
} // namespace R3B::Neuland::Calibration
