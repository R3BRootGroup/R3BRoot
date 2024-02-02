#include "R3BNeulandCalibrationTask.h"
#include "R3BNeulandTriggerTypes.h"
#include <FairParSet.h>
#include <FairRootManager.h>
#include <FairRuntimeDb.h>
#include <R3BEventHeader.h>
#include <R3BException.h>
#include <R3BLogger.h>
#include <R3BNeulandCommon.h>
#include <range/v3/all.hpp>

namespace R3B::Neuland
{

    CalibrationTask::CalibrationTask(std::string_view name, int iVerbose)
        : FairTask(name.data(), iVerbose)
    {
        SetDisableHistWrite();
    }

    CalibrationTask::CalibrationTask()
        : CalibrationTask("NeulandCalibrationReader", 1)
    {
    }

    void CalibrationTask::SetParContainers()
    {
        if (auto* rtdb = FairRuntimeDb::instance(); rtdb != nullptr)
        {
            SetExtraPar(rtdb);
        }
        else
        {
            throw R3B::runtime_error("Fail to get a non-nullptr FairRuntimeDb!");
        }
    }

    auto CalibrationTask::Init() -> InitStatus
    {
        if (auto* rootMan = FairRootManager::Instance(); rootMan != nullptr)
        {
            if (eventHeader_ = dynamic_cast<R3BEventHeader*>(rootMan->GetObject("EventHeader."));
                eventHeader_ == nullptr)
            {
                throw R3B::logic_error("R3BEventHeader is nullptr!");
            }

            check_input_par();
            offspill_tpat_bit_ = base_par_->GetOffSpillTpatPos();

            ExtraInit(rootMan);

            if (not is_hist_disabled_)
            {
                init_histogram();
            }

            R3BLOG(info,
                   fmt::format("Neuland calibration task \"{}\" has event trigger type: {}",
                               GetName(),
                               CalTrigger2Str(trig_type_)));
            return kSUCCESS;
        }
        return kFATAL;
    }

    void CalibrationTask::Exec(Option_t* /*option*/)
    {
        BeginOfEvent();
        (is_hist_disabled_) ? execute_no_hist() : execute_with_hist();
    }

    void CalibrationTask::execute_with_hist()
    {
        if (!check_trigger())
        {
            histograms_.get("trig_check")->Fill(fmt::format("{:016b}", eventHeader_->GetTpat()).c_str(), 1);
            return;
        }
        histograms_.get("trig_check")->Fill("triggered", 1);
        if (!CheckConditions())
        {
            histograms_.get("condition_check")->Fill("failure", 1);
            return;
        }
        histograms_.get("condition_check")->Fill("success", 1);
        passed_num_of_events++;
        TriggeredExec();
    }

    void CalibrationTask::execute_no_hist()
    {
        if (check_trigger() and CheckConditions())
        {
            passed_num_of_events++;
            TriggeredExec();
        }
    }

    void CalibrationTask::FinishTask()
    {
        EndOfTask();
        R3BLOG(info,
               fmt::format(R"(Passed events with the trigger type "{}" in task "{}": {})",
                           CalTrigger2Str(trig_type_),
                           GetName(),
                           passed_num_of_events));
        if (not is_hist_disabled_ and not is_write_hist_disabled_)
        {
            histograms_.save(GetName());
        }
        ranges::for_each(output_pars_, [](FairParSet* par) { par->setChanged(); });
    }

    auto CalibrationTask::check_trigger() const -> bool
    {
        R3BLOG(debug2,
               fmt::format(R"(From task "{}": tpat {}. Trig type {})",
                           GetName(),
                           eventHeader_->GetTpat(),
                           CalTrigger2Tpat(trig_type_, offspill_tpat_bit_).to_string()));
        return CheckTriggerWithTpat(trig_type_, eventHeader_->GetTpat(), offspill_tpat_bit_);
    }

    void CalibrationTask::init_histogram()
    {
        histograms_.add_hist<TH1I>("trig_check", "check the triggered or passed events", 1, 0., 0.);
        histograms_.add_hist<TH1I>("condition_check", "check the condition", 1, 0., 0.);
        HistogramInit(histograms_);
    }

    void CalibrationTask::check_input_par()
    {
        auto par_not_changed = ranges::find_if(input_pars_, [](auto* par) { return !par->hasChanged(); });
        if (par_not_changed != input_pars_.end())
        {
            throw R3B::runtime_error(fmt::format(R"(Calibration parameter "{}" is not initiated from the root file!)",
                                                 (*par_not_changed)->GetName()));
        }
    }
} // namespace R3B::Neuland
