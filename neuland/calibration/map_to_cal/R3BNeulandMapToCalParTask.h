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

#include "R3BFTCalEngine.h"
#include "R3BNeulandMapToCalPar.h"
#include "R3BPaddleTamexMappedData2.h"
#include "R3BTrigIDMappingFinder.h"
#include <FairRootManager.h>
#include <R3BIOConnector.h>
#include <R3BNeulandCalibrationTask.h>
#include <R3BNeulandCommon.h>
#include <R3BParView.h>
#include <string_view>

namespace R3B::Neuland
{
    class Map2CalParTask : public CalibrationTask
    {
      public:
        using ErrorMethod = calibration::FTCalErrorMethod;
        // Map2CalParTask();
        explicit Map2CalParTask(std::string_view mapped_data_name = "NeulandMappedData",
                                std::string_view trig_mapped_data_name = "NeulandTrigMappedData",
                                std::string_view par_name = "LandTCalPar",
                                std::string_view trig_par_name = "LandTrigTCalPar");

        void SetTrigIDMapPrintFormat(TrigIDMappingPrintFormat format) { trig_id_IO_.SetFormat(format); }
        void SetTrigEnabled(bool is_enabled) { is_trig_enabled_ = is_enabled; }
        void SetCoarseTimeFreqMHz(float freq) { coarse_time_frequency_ = freq; }
        // void SetTrigIDMapAutoFind(bool is_auto = true) { is_trigID_auto_ = is_auto; }
        void SetTrigIDMapDir(std::string_view dirName) { trig_id_IO_.SetDir(dirName); }
        void SetTrigIDMapFileName(std::string_view fileName) { trig_id_IO_.SetFileName(fileName); }
        void SetErrorMethod(ErrorMethod method) { cal_strategy_.Set_error_method(method); }

      private:
        // bool is_trigID_auto_ = true;
        bool is_trig_enabled_ = true;
        unsigned int plane_num_ = 0;
        float coarse_time_frequency_ = COARSE_TIME_CLOCK_FREQUENCY_MHZ;
        calibration::FTCalStrategy cal_strategy_;
        TrigIDMappingFinder trig_id_mapping_finder_;
        TrigMappingIO trig_id_IO_;

        // IO data and paramters:
        InputMapConnector<int, PaddleTamexMappedData> map_data_;
        InputMapConnector<int, PaddleTamexTrigMappedData> trig_map_data_;
        OutputParView<Map2CalPar> map_to_cal_par_;
        OutputParView<Map2CalPar> map_to_cal_trig_par_;

        // calibration engines:
        calibration::FTEngine<calibration::PlaneCal> map_cal_engine_{ "MapCal" };
        calibration::FTEngine<calibration::ModuleCal> trig_map_cal_engine_{ "TrigMapCal" };

        // virtual functions:
        void TriggeredExec() override;
        void ExtraInit(FairRootManager* rootMan) override;
        void EndOfTask() override;

        // non-virtual functions:
        void write_parameter();
        // void RecordTrigMappingID();
        void FillMapData();
        void FillTrigMapData();
        void PrintData() const;
        void WriteHists();
        void PrintTrigID() const;
    };

} // namespace R3B::Neuland

using R3BNeulandMapped2CalPar2 = R3B::Neuland::Map2CalParTask;
