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

#include "R3BNeulandCalToHitParTask.h"
#include <R3BLogger.h>
#include <R3BNeulandLSQREngineAdaptor.h>
#include <R3BNeulandMillepede.h>

namespace R3B::Neuland
{
    Cal2HitParTask::Cal2HitParTask(std::string_view name, int iVerbose)
        : CalibrationTask(name, iVerbose)
    {
    }

    void Cal2HitParTask::HistogramInit(DataMonitor& histograms) {}

    void Cal2HitParTask::ExtraInit(FairRootManager* /*rootMan*/)
    {
        cal_data_.init();

        switch (method_)
        {
            case Cal2HitParMethod::LSQT:
                engine_ = std::make_unique<Calibration::LSQREngineAdaptor>();
                break;
            case Cal2HitParMethod::Millipede:
                engine_ = std::make_unique<Calibration::MillepedeEngine>();
                break;
        }

        const auto plane_num = base_par_->GetNumOfPlanes();
        if (plane_num == 0)
        {
            throw R3B::runtime_error("Plane number extracted from Map2CalPar is 0!");
        }
        engine_->SetModuleSize(plane_num * BarsPerPlane);
        engine_->SetMinStat(min_stat_);
        engine_->Init();
    }

    void Cal2HitParTask::SetExtraPar(FairRuntimeDb* rtdb) {}

    void Cal2HitParTask::TriggeredExec()
    {
        engine_->Reset();
        for (const auto& bar_signal : cal_data_)
        {
            engine_->AddSignal(bar_signal);
        }
        auto* eventHeader = GetEventHeader();
        engine_->EndOfEvent(eventHeader->GetEventno());
    }

    void Cal2HitParTask::EndOfTask()
    {
        R3BLOG(info, "Starting to do cosmic calibration calibration!");
        engine_->Calibrate();

        if (hit_par_ == nullptr)
        {
            throw R3B::runtime_error("Hit parameter is nullptr during the calibration!");
        }
        *hit_par_ = engine_->ExtractParameters();
    }

    auto Cal2HitParTask::CheckConditions() const -> bool { return engine_->SignalFilter(cal_data_.get()); }
} // namespace R3B::Neuland
