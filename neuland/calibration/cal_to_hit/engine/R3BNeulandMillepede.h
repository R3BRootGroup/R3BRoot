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
#include <R3BNeulandCommon.h>
// #include <RankChecker.h>

namespace R3B::Neuland::Calibration
{
    class MillepedeEngine : public CosmicEngineInterface
    {
      public:
        MillepedeEngine() = default;
        void SetMinStat(int min) override { minimum_hit_ = min; }
        void enable_rank_check(bool rank_check = true) { has_rank_check_ = rank_check; }

      private:
        bool has_rank_check_ = false;
        int minimum_hit_ = 1;
        float matrix_epsilon_ = 0.001;
        float minimum_pos_z_ = 0;
        float smallest_time_sum_ = 0.;
        float init_effective_c_ = - DEFAULT_EFFECTIVE_C;
        MilleDataPoint input_data_buffer_;
        std::string input_data_filename_ = "neuland_cosmic_mille.bin";
        std::string pede_steer_filename_ = "neuland_steer.txt";
        Mille binary_data_writer_{ input_data_filename_ };
        // RankChecker<3900, 5> rank_checker_;

        void Init() override;
        void AddSignal(const BarCalData& signal) override;
        void Calibrate() override;
        void EndOfEvent(unsigned int event_num = 0) override;
        [[nodiscard]] auto ExtractParameters() -> Cal2HitPar override;
        void Reset() override;
        auto SignalFilter(const std::vector<BarCalData>& signals) -> bool override;

        void buffer_clear();
        void write_to_buffer();
        void add_signal_t_sum(const BarCalData& signal);
        void add_signal_t_diff(const BarCalData& signal);
        void add_spacial_local_constraint(int module_num);
        auto set_minimum_values(const std::vector<R3B::Neuland::BarCalData>& signals) -> bool;
    };

} // namespace R3B::Neuland::Calibration
