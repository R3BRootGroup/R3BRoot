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

#include "R3BDataMonitor.h"
#include "R3BNeulandCalData2.h"
#include "R3BNeulandMapToCalPar.h"
#include "R3BPaddleTamexMappedData2.h"
#include "R3BParView.h"
#include "R3BShared.h"
#include "R3BValueError.h"
#include <FairRootManager.h>
#include <R3BIOConnector.h>
#include <R3BNeulandBasePar.h>
#include <R3BNeulandCalibrationTask.h>
#include <R3BNeulandCommon.h>
#include <string_view>
#include <vector>

class R3BEventHeader;
class FairRuntimeDb;
namespace R3B::Neuland
{
    class Map2CalTask : public CalibrationTask
    {
      public:
        explicit Map2CalTask(std::string_view map_data_name = "NeulandMapData",
                             std::string_view trig_map_data_name = "NeulandTrigMapData",
                             std::string_view par_name = "LandTCalPar",
                             std::string_view trig_par_name = "LandTrigTCalPar",
                             std::string_view cal_data_name = "NeulandCalData");
        void SetPulserMode(bool pulser_mode = true) { is_pulse_mode_ = pulser_mode; }
        void SetNhitmin(unsigned int size) { signal_min_size_ = size; }
        void EnableWalk(bool is_walk_enabled = true) { is_walk_enabled_ = is_walk_enabled; }

      private:
        bool is_pulse_mode_ = false;
        bool is_walk_enabled_ = true;
        float coarse_time_frequency_ = 0.; // MHz
        unsigned int coarse_time_max_num_ = MAXCTValue;
        double max_coarse_time_ = R3B::Neuland::MaxCalTime;
        unsigned int total_pmt_nums_ = 0;
        unsigned int signal_min_size_ = 1;
        unsigned int plane_num_ = 0;

        // IO data and paramters:
        InputMapConnector<int, PaddleTamexMappedData> map_data_;
        InputMapConnector<int, PaddleTamexTrigMappedData> trig_map_data_;
        OutputVectorConnector<BarCalData> cal_data_;

        InputParView<Map2CalPar> calibration_par_;
        InputParView<Map2CalPar> calibration_trig_par_;

        void ExtraInit(FairRootManager* rootMan) override;
        void HistogramInit(DataMonitor& histograms) override;
        void BeginOfEvent() override { cal_data_.clear(); };
        void TriggeredExec() override;
        void FinishEvent() override;
        void SetExtraPar(FairRuntimeDb* rtdb) override;
        [[nodiscard]] auto CheckConditions() const -> bool override;

        void set_pmt_num();
        void set_ct_freq();
        void sample_parameters();
        void calibrate();
        void histogram_monitor(const BarCalData& cal, Side side);
        void fill_cal_data(BarCalData& cal, const MapBarSignal& signals);
        [[nodiscard]] auto doubleEdgeSignal_to_calSignal(const DoubleEdgeSignal& double_edge_signal,
                                                         R3B::Side side,
                                                         int module_num) const -> CalDataSignal;
        [[nodiscard]] auto mapBarSignal_to_calSignals(const MapBarSignal& map_bar_signals,
                                                      int module_num,
                                                      R3B::Side side) const -> std::vector<CalDataSignal>;
        [[nodiscard]] auto convert_to_real_time(R3BTCalPar2* calPar,
                                                SingleEdgeSignal signal,
                                                FTType ftType,
                                                int module_num) const -> ValueError<double>;
        [[nodiscard]] auto get_tot(const DoubleEdgeSignal& pmtSignal,
                                   int module_num,
                                   R3B::Side module_side) const -> ValueError<double>;
        [[nodiscard]] auto get_trigger_time(int module_num, Side side) const -> ValueError<double>;
        void overflow_correct(R3B::Neuland::CalDataSignal& calSignal) const;
    };
} // namespace R3B::Neuland

using R3BNeulandMapped2Cal2 = R3B::Neuland::Map2CalTask;
