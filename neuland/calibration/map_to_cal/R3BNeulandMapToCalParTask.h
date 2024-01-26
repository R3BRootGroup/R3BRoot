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
#include "R3BPaddleTamexMappedData2.h"
#include "R3BTrigIDMappingFinder.h"
#include <R3BIOConnector.h>
#include <R3BNeulandBasePar.h>
#include <R3BNeulandCalibrationTask.h>
#include <R3BNeulandCommon.h>

namespace R3B::Neuland
{
    class Map2CalParTask : public CalibrationTask
    {
      public:
        Map2CalParTask();
        Map2CalParTask(std::string_view name, int iVerbose);

        void SetPlaneNum(unsigned int num) { plane_num_ = num; }
        void SetTrigIDMapPrintFormat(TrigIDMappingPrintFormat format) { trigIDIO.SetFormat(format); }
        void SetTrigEnabled(bool is_enabled) { is_trig_enabled_ = is_enabled; }
        void SetCoarseTimeFreqMHz(float freq) { coarse_time_frequency_ = freq; }
        void SetTrigIDMapAutoFind(bool is_auto = true) { is_trigID_auto_ = is_auto; }
        void SetTrigIDMapDir(std::string_view dirName) { trigIDIO.SetDir(dirName); }
        void SetTrigIDMapFileName(std::string_view fileName) { trigIDIO.SetFileName(fileName); }

      private:
        bool is_trigID_auto_ = true;
        bool is_trig_enabled_ = true;
        unsigned int plane_num_ = 0;
        float coarse_time_frequency_ = COARSE_TIME_CLOCK_FREQUENCY_MHZ;
        calibration::FTCalStrategy cal_strategy_;
        TrigIDMappingFinder trigIDMappingFinder_;
        TrigMappingIO trigIDIO;

        // IO data and paramters:
        InputVectorConnector<PaddleTamexMappedData> mappedData_{ "NeulandMappedData" };
        InputMapConnector<unsigned int, PaddleTamexTrigMappedData> trigMappedData_{ "NeulandTrigMappedData" };
        CalibrationBasePar* base_par_ = InputPar<CalibrationBasePar>("NeulandCalibrationBasePar");
        Map2CalPar* calibrationPar_ = OutputPar<Map2CalPar>("LandTCalPar");
        Map2CalPar* calibrationTrigPar_ = OutputPar<Map2CalPar>("LandTrigTCalPar");

        // calibration engines:
        calibration::FTEngine<calibration::PlaneCal> mapCalEngine_{ "MapCal" };
        calibration::FTEngine<calibration::ModuleCal> trigMapCalEngine_{ "TrigMapCal" };

        // virtual functions:
        void TriggeredExec() override;
        void ExtraInit(FairRootManager* rootMan) override;
        void EndOfTask() override;

        // non-virtual functions:
        void write_parameter();
        void RecordTrigMappingID();
        void FillMapData();
        void FillTrigMapData();
        void PrintData() const;
        void WriteHists();
        void PrintTrigID() const;
    };

} // namespace R3B::Neuland

using R3BNeulandMapped2CalPar2 = R3B::Neuland::Map2CalParTask;
