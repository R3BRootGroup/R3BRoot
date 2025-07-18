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
#include <FairRootManager.h>
#include <FairRuntimeDb.h>
#include <R3BIOConnector.h>
#include <R3BNeulandBasePar.h>
#include <R3BNeulandCalData2.h>
#include <R3BNeulandCalibrationTask.h>
#include <R3BNeulandCosmicEngine.h>
#include <cstdint>
#include <memory>
#include <string_view>

#ifdef HAS_CPP_STANDARD_17
#include <type_traits>
#else
#include <concepts>
#endif

namespace R3B::Neuland
{
    enum class Cal2HitParMethod : uint8_t
    {
        recons,
        histogram,
        millepede
    };

    class Cal2HitParTask : public CalibrationTask
    {
      public:
        explicit Cal2HitParTask(Cal2HitParMethod method = Cal2HitParMethod::recons,
                                std::string_view cal_data_name = "NeulandCalData",
                                std::string_view base_par_name = "NeulandCalibrationBasePar",
                                std::string_view hit_par_name = "NeulandHitPar",
                                std::string_view name = "NeulandCal2HitParTask",
                                int iVerbose = 1);
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
        [[nodiscard]] auto CheckConditions() const -> bool override;

        // private non virtual functions:
    };

} // namespace R3B::Neuland
