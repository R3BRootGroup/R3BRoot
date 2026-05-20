#include "R3BNeulandCalibrationTask.h"
#include "R3BNeulandTriggerTypes.h"
#include <FairParSet.h>
#include <FairRootManager.h>
#include <FairRuntimeDb.h>
#include <FairTask.h>
#include <R3BEventHeader.h>
#include <R3BException.h>
#include <R3BLogger.h>
#include <RtypesCore.h>
#include <TH1.h>
#include <fairlogger/Logger.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <string_view>

namespace R3B::Neuland
{

    CalibrationTask::CalibrationTask(std::string_view name, int iVerbose)
        : FairTask(name.data(), iVerbose)
    {
        LOGP(info, "Calibration task {:?} is enable!", name);
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
                throw R3B::logic_error("R3BEventHeader is required. But it's nullptr!");
            }

            check_input_par();
            offspill_tpat_bit_ = base_par_->get_offspill_tpat_pos();

            ExtraInit(rootMan);

            if (not is_hist_disabled_)
            {
                init_histogram();
            }

            LOGP(info,
                 "Neuland calibration task \"{}\" has event trigger type: {}",
                 GetName(),
                 CalTrigger2Str(trig_type_));
            return kSUCCESS;
        }
        return kFATAL;
    }

    void CalibrationTask::Exec(Option_t* /*option*/)
    {
        BeginOfEvent();
        LOGP(debug, "Event number: {}, tpat: {:016b}", eventHeader_->GetEventno(), eventHeader_->GetTpat());
        (is_hist_disabled_) ? execute_no_hist() : execute_with_hist();
    }

    void CalibrationTask::execute_with_hist()
    {
        hist_condition_check_->Fill("total", 1);
        if (!check_offspill_trigger())
        {
            hist_trig_check_->Fill(fmt::format("{:016b}", eventHeader_->GetTpat()).c_str(), 1);
            return;
        }
        hist_condition_check_->Fill("triggered", 1);
        hist_trig_check_->Fill(fmt::format("*{:016b}", eventHeader_->GetTpat()).c_str(), 1);
        if (!CheckConditions(hist_condition_check_))
        {
            hist_condition_check_->Fill("failure", 1);
            return;
        }
        hist_condition_check_->Fill("success", 1);
        passed_num_of_events++;
        TriggeredExec();
    }

    void CalibrationTask::ConditionFillToHist(TH1L* hist_condition, std::string_view condition)
    {
        if (condition == "failure" or condition == "success" or condition == "triggered" or condition == "total")
        {
            LOGP(warn, R"("failure", "success", "triggered" and "total" are reserved conditions!)");
        }
        hist_condition->Fill(condition.data(), 1);
    }

    void CalibrationTask::ConditionFillToHist(std::string_view condition)
    {

        ConditionFillToHist(hist_condition_check_, condition);
    }

    void CalibrationTask::execute_no_hist()
    {
        if (check_offspill_trigger() and CheckConditions(hist_condition_check_))
        {
            passed_num_of_events++;
            TriggeredExec();
        }
    }

    void CalibrationTask::FinishTask()
    {
        EndOfTask();
        LOGP(info,
             R"(Passed events with the trigger type "{}" in task "{}": {})",
             CalTrigger2Str(trig_type_),
             GetName(),
             passed_num_of_events);
        if (not is_hist_disabled_ and not is_write_hist_disabled_)
        {
            histograms_.save_to_sink(GetName());
        }
        else
        {
            LOGP(info,

                 "Figures not saved due to current configuration: hist_disabled = {}, write_hist_disabled = {}",
                 is_hist_disabled_,
                 is_write_hist_disabled_);
        }
        ranges::for_each(output_pars_, [](FairParSet* par) -> void { par->setChanged(); });
        reset();
    }

    auto CalibrationTask::check_offspill_trigger() const -> bool
    {
        LOGP(debug2,
             R"(From task "{}": tpat {}. Trig type {})",
             GetName(),
             eventHeader_->GetTpat(),
             CalTrigger2Tpat(trig_type_, offspill_tpat_bit_).to_string());
        return CheckTriggerWithTpat(trig_type_, eventHeader_->GetTpat(), offspill_tpat_bit_);
    }

    void CalibrationTask::init_histogram()
    {
        hist_trig_check_ = histograms_.add_hist<TH1I>("trig_check", "check the triggered or passed events", 1, 0., 0.);
        hist_trig_check_->GetYaxis()->SetTitle("Entries");
        hist_trig_check_->GetXaxis()->SetTitle("TPAT");
        hist_condition_check_ = histograms_.add_hist<TH1L>("condition_check", "check the condition", 1, 0., 0.);
        hist_condition_check_->GetYaxis()->SetTitle("Entries");
        hist_condition_check_->GetXaxis()->SetTitle("Condition");
        HistogramInit(histograms_);
    }

    void CalibrationTask::check_input_par()
    {
        auto par_not_changed = ranges::find_if(input_pars_, [](auto* par) -> bool { return !par->hasChanged(); });
        if (par_not_changed != input_pars_.end())
        {
            auto par_name = std::string_view{ (*par_not_changed)->GetName() };
            throw R3B::runtime_error(
                fmt::format(R"(Calibration parameter "{}" is not initiated from the root file!)", par_name));
        }
    }
    void CalibrationTask::reset()
    {
        passed_num_of_events = 0;
        histograms_.reset_all_hists();
    }
} // namespace R3B::Neuland
