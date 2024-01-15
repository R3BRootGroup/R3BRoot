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

#include "R3BNeulandCalToHitPar.h"
#include <R3BIOConnector.h>
#include <R3BNeulandCalData2.h>
#include <R3BNeulandCalibrationTask.h>
#include <R3BNeulandHit.h>
#include <R3BShared.h>

namespace R3B::Neuland
{
    struct CalibratedSignal
    {
        ValueErrorD energy{};
        ValueErrorD time{};
    };

    class Cal2HitTask : public CalibrationTask
    {
      public:
        explicit Cal2HitTask(std::string_view name = "R3BNeulandCal2Hit", int iVerbose = 1);
        void SetGlobalTimeOffset(double offset) { global_time_offset_ = offset; }
        void SetDistanceToTarget(double distance) { distance_to_target_ = distance; }

      private:
        double global_time_offset_ = 0.;
        double distance_to_target_ = 0.;
        InputVectorConnector<BarCalData> cal_data_{ "NeulandCalData" };
        OutputVectorConnector<R3BNeulandHit> hit_data_{ "NeulandHits" };
        Cal2HitPar* cal_to_hit_par_ = InputPar<Cal2HitPar>("NeulandHitPar");

        // temporary variables to reduce dynamic allocations
        std::vector<CalibratedSignal> temp_left_signals_;
        std::vector<CalibratedSignal> temp_right_signals_;

        // overriden functions:
        void HistogramInit(DataMonitor& histograms) override;
        void ExtraInit(FairRootManager* rootMan) override;
        void SetExtraPar(FairRuntimeDb* rtdb) override;
        void BeginOfEvent() override { hit_data_.clear(); };
        void TriggeredExec() override;
        void EndOfTask() override;
        // [[nodiscard]] auto CheckConditions() const -> bool override {};

        // non-virtual private functions:
        void calibrate();
        void calculate_calibrated_signals(const BarCalData& calBar,
                                          /* inout */ std::vector<CalibratedSignal>& signals,
                                          Side side);
        void construct_hits(const std::vector<CalibratedSignal>& left_signals,
                            const std::vector<CalibratedSignal>& right_signals,
                            const HitModulePar& par,
                            /* inout */ std::vector<R3BNeulandHit>& hits);
        [[nodiscard]] auto construct_hit(const LRPair<CalibratedSignal>& signalPair, const HitModulePar& par) const
            -> R3BNeulandHit;
        static auto get_calibrated_energy(const CalDataSignal& calSignal, const HitModulePar& par, R3B::Side side)
            -> ValueErrorD;
        static auto get_calibrated_time(const CalDataSignal& calSignal, const HitModulePar& par, R3B::Side side)
            -> ValueErrorD;
        static auto to_calibrated_signal(const CalDataSignal& calSignal, const HitModulePar& par, R3B::Side side)
            -> CalibratedSignal;
        [[nodiscard]] auto signal_match_checking(const CalibratedSignal& first_signal,
                                                 const CalibratedSignal& second_signal,
                                                 const HitModulePar& par) -> bool;
        [[nodiscard]] inline auto get_hit_time(double first_t, double second_t) const -> double;
    };

} // namespace R3B::Neuland
template <>
class fmt::formatter<R3B::Neuland::CalibratedSignal>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::Neuland::CalibratedSignal& signal, FmtContent& ctn) const
    {
        return format_to(ctn.out(), "{{time: {}, energy: {}}}", signal.time, signal.energy);
    }
};
