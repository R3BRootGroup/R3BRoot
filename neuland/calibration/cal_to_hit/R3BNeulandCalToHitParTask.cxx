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
#include "R3BDataMonitor.h"
#include "R3BException.h"
#include "R3BNeulandBasePar.h"
#include "R3BNeulandCalToHitPar.h"
#include "R3BNeulandCalibrationTask.h"
#include "R3BNeulandCommon.h"
#include "R3BNeulandCommonFunc.h"
#include <FairRootManager.h>
#include <FairRuntimeDb.h>
#include <R3BLogger.h>
#include <R3BNeulandCal2HitHistAnalysis.h>
#include <R3BNeulandMillepede.h>
#include <R3BNeulandMuonRecons.h>
#include <TH1.h>
#include <fairlogger/Logger.h>
#include <memory>
#include <utility>

namespace R3B::Neuland
{
    Cal2HitParTask::Cal2HitParTask(const Config& config)

        : CalibrationTask(config.name, 1)
        , config_{ config }
        , cal_data_{ get_from_sep_string(0, config.read) }
        , base_par_{ AddInputPar<CalibrationBasePar>(get_from_sep_string(1, config.read)) }
        , hit_par_(AddOutputPar<Cal2HitPar>(get_from_sep_string(0, config.write)))

    {
        if (config.method == Cal2HitParMethod::millepede)
        {
            auto millepede_engine = std::make_unique<Calibration::MillepedeEngine>();
            millepede_engine->set_options(config.millepede);
            SetMethod(std::move(millepede_engine));
        }
        else
        {
            SetMethod(config.method);
        }
        SetMinStat(config.min_stat);
        SetTrigger(config.mode);
    }

    void Cal2HitParTask::SetMinStat(int min)
    {
        if (engine_ == nullptr)
        {
            LOGP(warn, "Engine is nullptr!");
            return;
        }
        engine_->SetMinStat(min);
    }

    void Cal2HitParTask::SetErrorScale(float scale)
    {
        if (engine_ == nullptr)
        {
            LOGP(warn, "Engine is nullptr!");
            return;
        }
        engine_->SetErrorScale(scale);
    }

    void Cal2HitParTask::SetMethod(Cal2HitParMethod method)
    {
        switch (method)
        {
            case Cal2HitParMethod::recons:
                engine_ = std::make_unique<Calibration::MuonReconstruction>();
                break;
            case Cal2HitParMethod::millepede:
                engine_ = std::make_unique<Calibration::MillepedeEngine>();
                break;
            case Cal2HitParMethod::histogram:
                engine_ = std::make_unique<Calibration::HistAnalysis>();
                break;
        }
    }

    void Cal2HitParTask::HistogramInit(DataMonitor& histograms) { engine_->HistInit(histograms); }

    void Cal2HitParTask::ExtraInit(FairRootManager* /*rootMan*/)
    {
        cal_data_.init();

        const auto plane_num = base_par_->get_num_of_planes();
        if (plane_num == 0)
        {
            throw R3B::runtime_error("Plane number extracted from Map2CalPar is 0!");
        }
        if (engine_ == nullptr)
        {
            throw R3B::logic_error("Calibration engine is not set!");
        }
        engine_->SetTask(this);
        engine_->SetModuleSize(plane_num * BarsPerPlane);
        engine_->Init();
    }

    void Cal2HitParTask::SetExtraPar(FairRuntimeDb* rtdb) {}

    void Cal2HitParTask::TriggeredExec()
    {
        engine_->EventReset();
        engine_->AddSignals(cal_data_.get());
        // for (const auto& bar_signal : cal_data_)
        // {
        //     engine_->AddSignal(bar_signal);
        // }
        auto* eventHeader = GetEventHeader();
        if (eventHeader != nullptr)
        {
            engine_->EndOfEvent(eventHeader->GetEventno());
        }
        else
        {
            throw R3B::runtime_error("Event header is nullptr!");
        }
    }

    void Cal2HitParTask::EndOfTask()
    {
        if (hit_par_ == nullptr)
        {
            throw R3B::runtime_error("Hit parameter is nullptr during the calibration!");
        }

        R3BLOG(info, "Starting to do cosmic calibration calibration!");
        engine_->Calibrate(*hit_par_);
        engine_->EndOfTask();
    }
    void Cal2HitParTask::BeginOfEvent() { engine_->BeginOfEvent(); }

    auto Cal2HitParTask::CheckConditions([[maybe_unused]] TH1L* hist_condition) const -> bool
    {
        auto res = engine_->SignalFilter(cal_data_.get());
        return res;
    }
} // namespace R3B::Neuland
