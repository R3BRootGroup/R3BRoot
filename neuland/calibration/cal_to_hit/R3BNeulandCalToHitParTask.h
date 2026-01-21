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
#include "R3BNeulandCalToHitPar.h"
#include "R3BNeulandMillepede.h"
#include "R3BNeulandTriggerTypes.h"
#include <FairRootManager.h>
#include <FairRuntimeDb.h>
#include <R3BIOConnector.h>
#include <R3BNeulandBasePar.h>
#include <R3BNeulandCalData2.h>
#include <R3BNeulandCalibrationTask.h>
#include <R3BNeulandCosmicEngine.h>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

#ifdef HAS_CPP_STANDARD_17
#include <type_traits>
#else
#include <concepts>
#endif

class TH1L;

namespace R3B::Neuland
{
    enum class Cal2HitParMethod : uint8_t
    {
        recons,
        histogram,
        millepede
    };
    constexpr auto DEFAULT_MIN_STAT = 10;

    class Cal2HitParTask;

    struct Cal2HitParTaskConfig
    {
        using Task = Cal2HitParTask;
        bool enable = false;
        int min_stat = DEFAULT_MIN_STAT;
        CalTrigger mode = CalTrigger::offspill;
        Cal2HitParMethod method = Cal2HitParMethod::recons;
        Calibration::MillepedeOptions millepede;
        std::string name = "NeulandCal2HitParTask";
        std::string read = "NeulandCalData;NeulandCalibrationBasePar";
        std::string write = "NeulandHitPar";
    };

    class Cal2HitParTask : public CalibrationTask
    {
      public:
        using Config = Cal2HitParTaskConfig;
        explicit Cal2HitParTask(const Config& config);
        void SetMinStat(int min);
        void SetErrorScale(float scale);
        auto GetCal2HitPar() -> auto* { return hit_par_; }
        void SetMethod(Cal2HitParMethod method);

#ifdef HAS_CPP_STANDARD_17
        template <typename Engine,
                  typename = std::enable_if_t<std::is_base_of_v<Calibration::CosmicEngineInterface, Engine>>>
#else
        template <std::derived_from<Calibration::CosmicEngineInterface> Engine>
#endif
        void SetMethod(std::unique_ptr<Engine> engine)
        {
            engine_ = std::move(engine);
        }

      private:
        Config config_;
        InputVectorConnector<BarCalData> cal_data_{ "NeulandCalData" };

        CalibrationBasePar* base_par_ = nullptr; // input par
        Cal2HitPar* hit_par_ = nullptr;          // output par

        std::unique_ptr<Calibration::CosmicEngineInterface> engine_;

        // overriden functions:
        void HistogramInit(DataMonitor& histograms) override;
        void ExtraInit(FairRootManager* rootMan) override;
        void BeginOfEvent() override;
        void SetExtraPar(FairRuntimeDb* rtdb) override;
        void TriggeredExec() override;
        void EndOfTask() override;
        [[nodiscard]] auto CheckConditions([[maybe_unused]] TH1L* hist_condition) const -> bool override;

        // private non virtual functions:
    };

} // namespace R3B::Neuland
