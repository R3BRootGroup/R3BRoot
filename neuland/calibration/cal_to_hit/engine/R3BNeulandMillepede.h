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

#include "R3BNeulandCosmicEngine.h"
#include <Mille.h>
#include <ParResultReader.h>
#include <PedeLauncher.h>
#include <R3BNeulandCommon.h>
#include <optional>
// #include <RankChecker.h>

class TGraphErrors;

namespace R3B::Neuland::Calibration
{
    enum class GlobalLabel
    {
        tsync,              // tsync
        offset_effective_c, // offset times effective_C
        effective_c         // effective speed of light
    };

    class MillepedeEngine : public CosmicEngineInterface
    {
      public:
        MillepedeEngine() = default;
        void SetMinStat(int min) override { minimum_hit_ = min; }
        void enable_rank_check(bool rank_check = true) { has_rank_check_ = rank_check; }

      private:
        bool has_rank_check_ = false;
        int minimum_hit_ = 1;
        // float minimum_pos_z_ = 0;
        // float smallest_time_sum_ = 0.;
        std::optional<float> average_t_sum_;
        float init_effective_c_ = -DEFAULT_EFFECTIVE_C;
        MilleDataPoint input_data_buffer_;
        std::string input_data_filename_ = "neuland_cosmic_mille.bin";
        std::string pede_steer_filename_ = "neuland_steer.txt";
        std::string parameter_filename_ = "neuland_pars.txt";
        Mille binary_data_writer_{ input_data_filename_ };
        Millepede::ResultReader par_result_;
        Millepede::Launcher pede_launcher_;

        // histograms:
        TGraphErrors* graph_time_offset_ = nullptr;
        TGraphErrors* graph_time_sync_ = nullptr;
        TGraphErrors* graph_effective_c_ = nullptr;

        // parameter:
        Cal2HitPar* cal_to_hit_par_ = nullptr;

        void Init() override;
        void AddSignal(const BarCalData& signal) override;
        void Calibrate(Cal2HitPar& hit_par) override;
        void EndOfEvent(unsigned int event_num = 0) override;
        void EventReset() override;
        auto SignalFilter(const std::vector<BarCalData>& signals) -> bool override;
        void EndOfTask() override;
        void HistInit(DataMonitor& histograms) override;

        void buffer_clear();
        void write_to_buffer();
        void add_signal_t_sum(const BarCalData& signal);
        void add_signal_t_diff(const BarCalData& signal);
        void add_spacial_local_constraint(int module_num);
        auto set_minimum_values(const std::vector<R3B::Neuland::BarCalData>& signals) -> bool;
        inline auto get_global_label_id(int module_num, GlobalLabel label) -> int;
        inline auto to_module_num_label(int par_num) -> std::pair<int, GlobalLabel>;
        void fill_module_parameters(const Millepede::ResultReader& result, Neuland::Cal2HitPar& cal_to_hit_par);
        void fill_data_to_figure(Cal2HitPar& hit_par);

        void init_parameter();
        void init_steer_writer();
    };

} // namespace R3B::Neuland::Calibration
