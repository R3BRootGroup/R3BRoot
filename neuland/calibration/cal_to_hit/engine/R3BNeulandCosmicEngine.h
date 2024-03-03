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

#include <R3BDataMonitor.h>
#include <R3BNeulandCalData2.h>
#include <R3BNeulandCalToHitPar.h>
#include <R3BShared.h>

namespace R3B::Neuland
{
    class Cal2HitParTask;
}

namespace R3B::Neuland::Calibration
{
    class CosmicEngineInterface
    {
      public:
        CosmicEngineInterface() = default;
        virtual ~CosmicEngineInterface() = default;

        CosmicEngineInterface(const CosmicEngineInterface&) = default;
        CosmicEngineInterface(CosmicEngineInterface&&) = delete;
        auto operator=(const CosmicEngineInterface&) -> CosmicEngineInterface& = default;
        auto operator=(CosmicEngineInterface&&) -> CosmicEngineInterface& = delete;

        void SetModuleSize(int module_size) { module_size_ = module_size; }
        void SetTask(Cal2HitParTask* task) { mother_task_ = task; };
        [[nodiscard]] auto GetModuleSize() const -> auto { return module_size_; }
        auto GetTask() -> Cal2HitParTask* { return mother_task_; }

        virtual void Init() {}
        virtual auto SignalFilter(const std::vector<BarCalData>& /*signals*/) -> bool { return true; }
        virtual void AddSignal(const BarCalData& signal) = 0;
        virtual void Calibrate(Cal2HitPar& hit_par) = 0;
        virtual void SetMinStat(int min) {}
        virtual void BeginOfEvent(unsigned int event_num){};
        virtual void EndOfEvent(unsigned int event_num){};
        virtual void Reset(){};
        virtual void EndOfTask(){};
        virtual void HistInit(DataMonitor& histograms){};

      private:
        int module_size_ = 0;
        Cal2HitParTask* mother_task_ = nullptr;
    };

} // namespace R3B::Neuland::Calibration
