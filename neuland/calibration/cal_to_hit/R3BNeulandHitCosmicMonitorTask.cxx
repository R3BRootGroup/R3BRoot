#include "R3BNeulandHitCosmicMonitorTask.h"
#include "R3BDataMonitor.h"
#include "R3BNeulandCalibrationTask.h"
#include "R3BNeulandCommon.h"
#include "R3BNeulandCommonFunc.h"
#include "R3BNeulandMilleCalDataProcessor.h"
#include "R3BNeulandTriggerTypes.h"
#include <FairRootManager.h>
#include <FairRuntimeDb.h>
#include <TH1.h>
#include <TH2.h>
#include <algorithm>
#include <chrono>
#include <fmt/format.h>
#include <limits>
#include <string_view>
#include <tuple>

namespace R3B::Neuland::Calibration
{
    CosmicMonitorTask::CosmicMonitorTask(const Config& config)
        : CalibrationTask(config.name, 1)
        , config_{ config }
        , hit_data_{ get_from_sep_string(0, config.read) }
    {
        SetTrigger(CalTrigger::offspill);
    }

    void CosmicMonitorTask::HistogramInit(DataMonitor& histograms)
    {
        const auto module_size = GetBasePar()->get_num_of_planes() * BarsPerPlane;
        static constexpr auto RESIDUAL_BIN_NUM = 500;
        static constexpr auto PAR_BIN_NUM = 400;

        static constexpr auto MAX_TIME_SEC = 800;
        static constexpr auto TIME_BIN_NUM = 800;
        static constexpr auto TIME_US_MAX = 100.;
        static constexpr auto ITER_BIN_NUM = 200;
        static constexpr auto ITER_MAX = 200.;

        static const auto SLOPE_MAX = 10.;
        static const auto OFFSET_MAX = 200.;
        static constexpr auto residual_bin_num = 1000;
        static constexpr auto max_residual = 300.;
        static constexpr auto max_diff = 50;

        auto set_x_title = [](auto* hist, std::string_view title) -> void { hist->GetXaxis()->SetTitle(title.data()); };
        auto set_y_title = [](auto* hist, std::string_view title) -> void { hist->GetYaxis()->SetTitle(title.data()); };

        hist_fit_time_duration_us_ = histograms.add_hist<TH1D>(
            "fit_time_duration", "Time spent to fit the tracks", TIME_BIN_NUM, 0., TIME_US_MAX);
        set_x_title(hist_fit_time_duration_us_, "Fit time (us)");
        set_y_title(hist_fit_time_duration_us_, fmt::format("Entries per {} us", TIME_US_MAX / TIME_BIN_NUM));

        hist_fit_iterations_ = histograms.add_hist<TH1D>(
            "fit_iterations", "Iterations used to fit the tracks", ITER_BIN_NUM, 0., ITER_MAX);
        set_y_title(hist_fit_iterations_, fmt::format("Entries per {} iteration", ITER_MAX / ITER_BIN_NUM));
        set_x_title(hist_fit_iterations_, "Iterations");

        hist_a_xz_ = histograms.add_hist<TH1D>("a_xz", "slope of the xz plane", PAR_BIN_NUM, 0., SLOPE_MAX);
        set_x_title(hist_a_xz_, "abs(a_xz)");
        set_y_title(hist_a_xz_, fmt::format("counts per {}", SLOPE_MAX / PAR_BIN_NUM));

        hist_b_xz_ = histograms.add_hist<TH1D>("b_xz", "offset of the xz plane", PAR_BIN_NUM, 0., OFFSET_MAX);
        set_x_title(hist_b_xz_, "abs(b_xz)");
        set_y_title(hist_b_xz_, fmt::format("counts per {}", OFFSET_MAX / PAR_BIN_NUM));

        hist_a_yz_ = histograms.add_hist<TH1D>("a_yz", "slope of the yz plane", PAR_BIN_NUM, 0., SLOPE_MAX);
        set_x_title(hist_a_yz_, "abs(a_yz)");
        set_y_title(hist_a_yz_, fmt::format("counts per {}", SLOPE_MAX / PAR_BIN_NUM));

        hist_b_yz_ = histograms.add_hist<TH1D>("b_yz", "offset of the yz plane", PAR_BIN_NUM, 0., OFFSET_MAX);
        set_x_title(hist_b_yz_, "abs(b_yz)");
        set_y_title(hist_b_yz_, fmt::format("counts per {}", OFFSET_MAX / PAR_BIN_NUM));

        hist_t_diff_module_counts_ = histograms.add_hist<TH1D>("t_diff_module_counts",
                                                               "counts of each module used for t_diff relation",
                                                               module_size,
                                                               0.5,
                                                               0.5 + module_size);
        set_x_title(hist_t_diff_module_counts_, "Module number");
        set_y_title(hist_t_diff_module_counts_, "Counts");

        hist_fit_diff_time_ = histograms.add_hist<TH2D>("fit_diff_time",
                                                        "difference between the fitted line and time values",
                                                        module_size,
                                                        0.5,
                                                        0.5 + module_size,
                                                        residual_bin_num,
                                                        -max_diff,
                                                        max_diff);
        set_x_title(hist_fit_diff_time_, "Module number");
        set_y_title(hist_fit_diff_time_, "diff value (cm)");

        hist_fit_diff_bar_pos_ = histograms.add_hist<TH2D>("fit_diff_bar_pos",
                                                           "difference between the fitted line and bar positions",
                                                           module_size,
                                                           0.5,
                                                           0.5 + module_size,
                                                           residual_bin_num,
                                                           -max_diff,
                                                           max_diff);
        set_x_title(hist_fit_diff_bar_pos_, "Module number");
        set_y_title(hist_fit_diff_bar_pos_, "diff value (cm)");
    }

    void CosmicMonitorTask::ExtraInit([[maybe_unused]] FairRootManager* rootMan)
    {
        hit_data_.init();
        plane_counter_.resize(GetBasePar()->get_num_of_planes());
    }

    void CosmicMonitorTask::BeginOfEvent()
    {
        track_info_.clear();
        track_dataset_xz_.clear();
        track_dataset_yz_.clear();
        std::ranges::fill(plane_counter_, 0);
        for (const auto& hit : hit_data_)
        {
            const auto plane_id = ModuleID2PlaneID(hit.module_id);
            ++(plane_counter_.at(plane_id));
        }
        fit_duration_ms_ = std::chrono::microseconds{ 0 };
    }

    void CosmicMonitorTask::TriggeredExec()
    {
        for (const auto& hit : hit_data_)
        {
            track_dataset_xz_.z_vals.push_back(hit.position.Z().value);
            track_dataset_xz_.z_errs.push_back(hit.position.Z().error);
            track_dataset_xz_.vals.push_back(hit.position.X().value);

            track_dataset_yz_.z_vals.push_back(hit.position.Z().value);
            track_dataset_yz_.z_errs.push_back(hit.position.Z().error);
            track_dataset_yz_.vals.push_back(hit.position.Y().value);
        }
        auto before_fit = std::chrono::steady_clock::now();
        MilleDataProcessor::linear_fit(
            track_dataset_xz_, track_info_.x_z, huber_regressor_, std::numeric_limits<double>::max());
        hist_fit_iterations_->Fill(huber_regressor_.get_result().iteration);
        MilleDataProcessor::linear_fit(
            track_dataset_yz_, track_info_.y_z, huber_regressor_, std::numeric_limits<double>::max());
        hist_fit_iterations_->Fill(huber_regressor_.get_result().iteration);

        auto is_ok = true;
        if (config_.max_abs_a_xz > 0 and config_.max_abs_a_xz < track_info_.x_z.slope)
        {
            ConditionFillToHist("xz_slope_too_large");
            is_ok = false;
        }
        if (config_.max_abs_a_yz > 0 and config_.max_abs_a_yz < track_info_.y_z.slope)
        {
            ConditionFillToHist("yz_slope_too_large");
            is_ok = false;
        }
        if (not is_ok)
        {
            return;
        }

        fit_duration_ms_ =
            std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - before_fit);
        hist_fit_time_duration_us_->Fill(static_cast<double>(fit_duration_ms_.count()));
        fill_histograms();

        ConditionFillToHist("passed");
    }

    void CosmicMonitorTask::fill_histograms()
    {
        if (track_info_.x_z.slope != 0 and track_info_.x_z.offset != 0)
        {
            hist_a_xz_->Fill(track_info_.x_z.slope);
            hist_b_xz_->Fill(track_info_.x_z.offset);
        }
        if (track_info_.y_z.slope != 0 and track_info_.y_z.offset != 0)
        {
            hist_a_yz_->Fill(track_info_.y_z.slope);
            hist_b_yz_->Fill(track_info_.y_z.offset);
        }

        auto predict = [](const TrackFitPar& par, double val) -> double { return (par.slope * val) + par.offset; };

        for (const auto& hit : hit_data_)
        {
            const auto z_pos = hit.position.Z().value;
            const auto x_pos = hit.position.X().value;
            const auto y_pos = hit.position.Y().value;
            const auto module_num = hit.module_id + 1;
            const auto is_horizontal = IsModuleNumHorizontal(module_num);

            const auto [time_diff_pos, bar_diff_pos] =
                is_horizontal ? std::tuple{ x_pos, y_pos } : std::tuple{ y_pos, x_pos };
            const auto [time_diff_par, bar_diff_par] =
                is_horizontal ? std::tie(track_info_.x_z, track_info_.y_z) : std::tie(track_info_.y_z, track_info_.x_z);
            const auto time_diff_val = predict(time_diff_par, z_pos) - time_diff_pos;
            const auto bar_diff_val = predict(bar_diff_par, z_pos) - bar_diff_pos;
            hist_fit_diff_time_->Fill(module_num, time_diff_val);
            hist_fit_diff_bar_pos_->Fill(module_num, bar_diff_val);
        }
    }

    auto CosmicMonitorTask::CheckConditions([[maybe_unused]] TH1L* hist_condition) const -> bool
    {
        auto res = true;
        if (hit_data_.get().size() < config_.n_hit_min)
        {
            ConditionFillToHist(hist_condition, "undersized");
            res = false;
        }
        auto n_plane = std::ranges::count_if(plane_counter_, [](auto val) -> bool { return val > 0; });
        if (n_plane <= config_.n_plane_min)
        {
            ConditionFillToHist(hist_condition, "too_few_planes");
            res = false;
        }
        return res;
    }
} // namespace R3B::Neuland::Calibration
