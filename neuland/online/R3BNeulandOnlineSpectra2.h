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

#include <R3BIOConnector.h>
#include <R3BNeulandCalCanvas.h>
#include <R3BNeulandCalData2.h>
#include <R3BNeulandCalibrationTask.h>
#include <R3BNeulandEventHeaderCanvas.h>
#include <R3BNeulandHit.h>
#include <R3BNeulandHitCanvas.h>
#include <R3BNeulandHitCosmicCanvas.h>
#include <R3BNeulandHitXYCanvas.h>
#include <R3BNeulandMappedCanvas.h>
#include <R3BNeulandOnlineCanvas.h>
#include <R3BNeulandTJumpCanvas.h>
#include <R3BNeulandTimingCanvas.h>
#include <R3BNeulandTriggerTypes.h>
#include <R3BPaddleTamexMappedData2.h>
#include <TRandom.h>

class FairRunOnline;

namespace R3B
{
    class UcesbSource;
}

namespace R3B::Neuland
{
    class OnlineSpectra : public CalibrationTask
    {
      public:
        explicit OnlineSpectra(std::string_view name = "R3BNeulandOnlineSpectra", int iVerbose = 1);

        void ResetHistos();
        void ResetHistosMapped();
        void SaveAllHists();
        void RestartUcesbServer();

        OnlineSpectra(const OnlineSpectra&) = delete;
        OnlineSpectra(OnlineSpectra&&) = delete;
        auto operator=(const OnlineSpectra&) -> OnlineSpectra& = delete;
        auto operator=(OnlineSpectra&&) -> OnlineSpectra& = delete;
        ~OnlineSpectra() override;

        auto GetRandomGenerator() -> TRandom* { return random_gen_; }
        [[nodiscard]] auto GetDistanceToTarget() const { return distance_to_target_; }

        void SetRandomGenerator(TRandom* rand)
        {
            if (rand != nullptr)
            {
                random_gen_ = rand;
            }
        }
        void SetDistanceToTarget(double distance) { distance_to_target_ = distance; }
        void SetUcesbSource(UcesbSource* source) { ucesb_source_ = source; }

        template <typename SpecType, typename = std::enable_if_t<std::is_base_of_v<OnlineCanvas, SpecType>>>
        auto AddCanvas(std::string_view name, CalTrigger trigger = CalTrigger::all) -> SpecType&
        {
            auto& spectrum = spectra_.emplace_back(std::make_unique<SpecType>(name));
            spectrum->SetOnlineSpectra(this);
            spectrum->SetTrigger(trigger);
            return *static_cast<SpecType*>(spectrum.get());
        }

        template <typename UnaryOp>
        inline void ActionToSpectra(UnaryOp&& optn)
        {
            for (auto& spectrum : spectra_)
            {
                optn(spectrum.get());
            }
        }

        // for backward compability
        [[deprecated("Please use SetTrigger()")]] void SetCosmicTpat(UInt_t CosmicTpat = 0) {}

      private:
        double distance_to_target_ = 0.;
        std::vector<std::unique_ptr<OnlineCanvas>> spectra_;
        UcesbSource* ucesb_source_ = nullptr;
        TRandom* random_gen_ = gRandom; // non-owning

        // virtual functions:
        void HistogramInit(DataMonitor& histograms) override;
        void ExtraInit(FairRootManager* rootMan) override;
        void SetExtraPar(FairRuntimeDb* rtdb) override;
        void TriggeredExec() override;
        // void EndOfTask() override;
        // [[nodiscard]] auto CheckConditions() const -> bool override;

        // non-virtual member functions:
        void init_http_server(FairRunOnline* run);

      public:
        ClassDefOverride(OnlineSpectra, 1);
    };

} // namespace R3B::Neuland

using R3BNeulandOnlineSpectra2 = R3B::Neuland::OnlineSpectra;
