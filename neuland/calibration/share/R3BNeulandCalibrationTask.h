#pragma once
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

#include "R3BDataMonitor.h"
#include "R3BNeulandTriggerTypes.h"
#include <FairRuntimeDb.h>
#include <FairTask.h>
#include <R3BEventHeader.h>

class FairParSet;

namespace R3B::Neuland
{
    class CalibrationTask : public FairTask
    {
      public:
        CalibrationTask();
        explicit CalibrationTask(std::string_view name, int iVerbose = 1);

        // Getters:
        [[nodiscard]] auto GetEventHeader() const -> auto* { return eventHeader_; }
        [[nodiscard]] auto GetHistMonitor() -> DataMonitor& { return histograms_; }
        [[nodiscard]] auto IsHistDisabled() const -> bool { return is_hist_disabled_; }

        void SetTrigger(CalTrigger trigger) { trig_type_ = trigger; }
        void SetDisableHist(bool is_hist_disabled = true) { is_hist_disabled_ = is_hist_disabled; }
        void SetDisableHistWrite(bool is_write_hist_disabled = true)
        {
            is_write_hist_disabled_ = is_write_hist_disabled;
        }
        // void SetOnline()

      protected:
        template <typename ParType>
        auto InputPar(std::string_view par_name, FairRuntimeDb* rtdb = FairRuntimeDb::instance()) -> ParType*
        {
            return add_par<ParType>(par_name, input_pars_, rtdb);
        }

        template <typename ParType>
        auto OutputPar(std::string_view par_name, FairRuntimeDb* rtdb = FairRuntimeDb::instance()) -> ParType*
        {
            return add_par<ParType>(par_name, output_pars_, rtdb);
        }

      private:
        bool is_hist_disabled_ = false;
        bool is_write_hist_disabled_ = false;
        CalTrigger trig_type_ = CalTrigger::all;
        R3BEventHeader* eventHeader_ = nullptr;
        std::vector<FairParSet*> input_pars_;
        std::vector<FairParSet*> output_pars_;

        // helpers:
        DataMonitor histograms_;

        // virtual functions:
        virtual void HistogramInit(DataMonitor& histograms) {}
        virtual void ExtraInit(FairRootManager* rootMan) {}
        virtual void SetExtraPar(FairRuntimeDb* rtdb) {}
        virtual void BeginOfEvent(){};
        virtual void TriggeredExec() = 0;
        virtual void EndOfTask(){};
        [[nodiscard]] virtual auto CheckConditions() const -> bool { return true; }

        // overriden functions:
        auto Init() -> InitStatus override;
        void SetParContainers() override;
        void Exec(Option_t* option) override;
        void FinishTask() override;

        // private functions:
        void init_histogram();
        void check_input_par();
        void execute_no_hist();
        void execute_with_hist();

        [[nodiscard]] auto check_trigger() const -> bool;
        template <typename ParType>
        [[nodiscard]] auto add_par(std::string_view par_name, std::vector<FairParSet*>& pars, FairRuntimeDb* rtdb)
            -> ParType*;
    };

    template <typename ParType>
    auto CalibrationTask::add_par(std::string_view par_name, std::vector<FairParSet*>& pars, FairRuntimeDb* rtdb)
        -> ParType*
    {
        auto* par = dynamic_cast<ParType*>(rtdb->findContainer(par_name.data()));
        if (par == nullptr)
        {
            par = std::make_unique<ParType>(par_name.data()).release();
            if (rtdb->addContainer(par); par == nullptr)
            {
                throw R3B::runtime_error("Calibration parameter becomes nullptr!");
            }
        }
        pars.push_back(par);
        return par;
    }

}; // namespace R3B::Neuland
