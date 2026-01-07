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

#include "MilleEntry.h"
#include "R3BDataMonitor.h"
#include "R3BIOConnector.h"
#include "R3BLogger.h"
#include "R3BNeulandCalData2.h"
#include "R3BNeulandCalToHitPar.h"
#include "R3BNeulandCosmicEngine.h"
#include "R3BNeulandMilleCalDataProcessor.h"
#include <Mille.h>
#include <ParResultReader.h>
#include <PedeLauncher.h>
#include <R3BNeulandCommon.h>
#include <TH1.h>
#include <cstdint>
#include <fmt/core.h>
#include <fmt/format.h>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
// #include <RankChecker.h>

class TGraphErrors;

class TH2D;

namespace R3B::Neuland::Calibration
{
    enum class GlobalLabel : int8_t
    {
        tsync,              // tsync
        offset_effective_c, // offset times effective_C
        effective_c         // effective speed of light
    };

    struct MillepedeOptions
    {
        bool outdir_has_timestamp = true;
        int min_plane_num = DEFAULT_MINIMUM_PLANE_NUM;
        int num_of_threads = 0;
        float scale_factor = 1000.F;
        float max_abs_a_xz = DEFAULT_MAX_SLOPE_VALUE;
        float max_abs_a_yz = DEFAULT_MAX_SLOPE_VALUE;
        double t_diff_residual_cut = DEFAULT_T_DIFF_RESIDUAL_CUT;
        double p_value_cut = DEFAULT_CALIBRATION_P_VALUE_CUT;
        std::string mille_data_filename = "neuland_cosmic_mille.bin";
        std::string pede_par_filename = "neuland_pars.txt";
        std::string mille_log_filename;
    };

    class MillepedeEngine : public CosmicEngineInterface
    {
      public:
        MillepedeEngine() = default;
        void enable_rank_check(bool rank_check = true) { has_rank_check_ = rank_check; }
        void set_t_diff_residual_cut(double val) { config_.t_diff_residual_cut = val; }
        void set_p_value_cut(double val) { config_.p_value_cut = val; }
        void set_options(const MillepedeOptions& options);

      private:
        bool has_rank_check_ = false;
        int minimum_hit_ = 1;

        MillepedeOptions config_;
        // float minimum_pos_z_ = 0;
        // float smallest_time_sum_ = 0.;
        std::optional<float> average_t_sum_;
        constexpr static std::string_view DEFAULT_SUB_DIR = "millepede";

        MilleDataPoint input_data_buffer_;
        R3B::OutputVectorConnector<MilleCalData> output_mille_data_{ "MilleData" };
        R3B::OutputConnector<MilleTrackInfo> output_mille_track_info_{ "MilleTrackInfo" };
        // R3B::OutputVectorConnector<MilleDataPoint> output_mille_data_point_{ "MilleDataPoint" };
        std::string pede_steer_filename_ = "neuland_steer.txt";
        std::string input_parameter_filename_ = "neuland_pars.txt";
        std::string working_dir_;

        std::vector<int> plane_counter_;
        std::unique_ptr<Mille> binary_data_writer_;
        Millepede::ResultReader par_result_;
        Millepede::Launcher pede_launcher_;

        // histograms:
        TGraphErrors* graph_time_offset_ = nullptr;
        TGraphErrors* graph_time_sync_ = nullptr;
        TGraphErrors* graph_effective_c_ = nullptr;
        TH1D* hist_t_offset_residual_ = nullptr;
        TH1D* hist_p_value_xz_ = nullptr;
        TH1D* hist_p_value_yz_ = nullptr;
        TH1D* hist_a_xz_ = nullptr;
        TH1D* hist_b_xz_ = nullptr;
        TH1D* hist_a_yz_ = nullptr;
        TH1D* hist_b_yz_ = nullptr;
        TH1D* hist_t_diff_module_counts_ = nullptr;
        TH1D* hist_plane_hit_num_ = nullptr;
        TH2D* hist_module_residuals_ = nullptr;
        TH2D* hist_module_residuals_bar_pos_ = nullptr;
        TH2D* hist_fit_diff_time_ = nullptr;
        TH1L* barplot_filter_counts_ = nullptr;

        // parameter:
        Cal2HitPar* cal_to_hit_par_ = nullptr;

        std::unique_ptr<MilleDataProcessor> data_preprocessor_;

        void Init() override;
        void AddSignals(const std::vector<BarCalData>& signals) override;
        void Calibrate(Cal2HitPar& hit_par) override;
        void EndOfEvent(unsigned int event_num = 0) override;
        void EventReset() override;
        auto SignalFilter(const std::vector<BarCalData>& signals) -> bool override;
        void BeginOfEvent() override
        {
            output_mille_data_.clear();
            output_mille_track_info_.clear();
            // output_mille_data_point_.clear();
        };
        void EndOfTask() override;
        void HistInit(DataMonitor& histograms) override;
        void SetMinStat(int min) override
        {
            minimum_hit_ = min;
            R3BLOG(info, fmt::format("Minimum number of hits is set to {}", minimum_hit_));
        }
        void SetErrorScale(float scale) override { config_.scale_factor = scale; }

        void buffer_clear();
        void write_to_buffer();
        void add_signal_t_sum(const MilleCalData& signal, double a_t);
        void add_signal_t_diff(const MilleCalData& signal);
        void add_spacial_local_constraint(int plane_id, const std::vector<MilleCalData>& plane_signals);
        auto set_minimum_values(const std::vector<R3B::Neuland::BarCalData>& signals) -> bool;
        inline auto get_global_label_id(int module_num, GlobalLabel label) -> int;
        inline auto to_module_num_label(int par_num) -> std::pair<int, GlobalLabel>;
        void fill_module_parameters(const Millepede::ResultReader& result, Neuland::Cal2HitPar& cal_to_hit_par);
        void fill_data_to_figure(Cal2HitPar& hit_par);

        void init_parameter();
        void init_steer_writer();
        void set_working_dir();
        void add_fit_result_hist(const MilleDataProcessor::FitResult& fit_result);

        auto select_t_diff_signal(const std::vector<MilleCalData>& plane_data);
    };

} // namespace R3B::Neuland::Calibration
