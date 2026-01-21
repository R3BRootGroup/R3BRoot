#include "R3BNeulandHitOnlineMonitorTask.h"
#include "R3BNeulandCalibrationTask.h"
#include "R3BNeulandCommon.h"
#include "R3BNeulandCommonFunc.h"
#include "R3BNeulandTriggerTypes.h"
#include <string_view>

namespace R3B::Neuland::Calibration
{
    HitOnlineMonitorTask::HitOnlineMonitorTask(const Config& config)
        : CalibrationTask(config.name, 1)
        , config_{ config }
        , hit_data_{ get_from_sep_string(0, config.read) }
    {
        SetTrigger(CalTrigger::onspill);
    }

    void HitOnlineMonitorTask::HistogramInit(DataMonitor& histograms)
    {
        const auto module_size = GetBasePar()->get_num_of_planes() * BarsPerPlane;
        static constexpr auto POSITION_BINS = 1000;

        auto set_x_title = [](auto* hist, std::string_view title) -> void { hist->GetXaxis()->SetTitle(title.data()); };
        auto set_y_title = [](auto* hist, std::string_view title) -> void { hist->GetYaxis()->SetTitle(title.data()); };

        hist_positions_along_bar_ = histograms.add_hist<TH2D>("position_along_bar",
                                                              "Positions along the bar directions",
                                                              module_size,
                                                              0.5,
                                                              0.5 + module_size,
                                                              POSITION_BINS,
                                                              -BarLength / 2.,
                                                              BarLength / 2.);
        set_x_title(hist_positions_along_bar_, "module number");
        set_y_title(hist_positions_along_bar_, "position (cm)");

        static constexpr auto TIME_MIN_NS = 120;
        static constexpr auto TIME_MAX_NS = 220;
        static constexpr auto TIME_VALUE_BINS = 1000;
        hist_times_ = histograms.add_hist<TH2D>("time_val",
                                                "Positions along the bar directions",
                                                module_size,
                                                0.5,
                                                0.5 + module_size,
                                                TIME_VALUE_BINS,
                                                TIME_MIN_NS,
                                                TIME_MAX_NS);
        set_x_title(hist_times_, "module number");
        set_y_title(hist_times_, "time (ns)");
    }

    void HitOnlineMonitorTask::TriggeredExec()
    {
        for (const auto& hit : hit_data_)
        {
            const auto& position = hit.position;
            const auto module_num = hit.module_id + 1;
            const auto is_horizontal = IsModuleNumHorizontal(module_num);

            const auto pos_along_bar = is_horizontal ? position.X() : position.Y();
            hist_positions_along_bar_->Fill(module_num, pos_along_bar.value);
            hist_times_->Fill(module_num, hit.time.value);
        }
    }

    void HitOnlineMonitorTask::ExtraInit([[maybe_unused]] FairRootManager* rootMan) { hit_data_.init(); }

    auto HitOnlineMonitorTask::CheckConditions([[maybe_unused]] TH1L* hist_condition) const -> bool
    {
        if (hit_data_.get().size() < config_.n_hit_min)
        {
            ConditionFillToHist(hist_condition, "undersized");
            return false;
        }
        return true;
    }
} // namespace R3B::Neuland::Calibration
