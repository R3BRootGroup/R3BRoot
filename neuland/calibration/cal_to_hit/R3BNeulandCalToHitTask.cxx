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

#include "R3BNeulandCalToHitTask.h"
#include "R3BDataMonitor.h"
#include "R3BException.h"
#include "R3BNeulandCalData2.h"
#include "R3BNeulandCalToHitPar.h"
#include "R3BNeulandCalibrationTask.h"
#include "R3BNeulandCommon.h"
#include "R3BNeulandHit2.h"
#include "R3BNeulandTriggerTypes.h"
#include "R3BValueError.h"
#include <FairRootManager.h>
#include <FairRuntimeDb.h>
#include <Math/Vector3Dfwd.h>
#include <R3BNeulandSignalMatcher.h>
#include <R3BShared.h>
#include <TH1.h>
#include <cmath>
#include <fairlogger/Logger.h>
#include <fmt/core.h>
#include <string_view>
#include <vector>

namespace R3B::Neuland
{
    namespace
    {
        using R3B::ValueErrorD;
        using R3B::Neuland::CalibratedSignal;
        using R3B::Neuland::HitModulePar;
        using ROOT::Math::XYZVector;

        template <typename T>
        inline auto sqrt_value_error(const ValueError<T>& val_err)
        {
            // NOTE: Error evaluation of the sqrt function only uses the first order of Taylor expansion
            const auto value = std::sqrt(val_err.value);
            const auto error = 1. / 2 / std::sqrt(val_err.value) * val_err.error;
            return ValueErrorD{ value, error };
        }

        inline auto get_hit_energy(const ValueErrorD& first_e, const ValueErrorD& second_e, const HitModulePar& par)
        {
            return par.light_attenuation_factor * sqrt_value_error(first_e) * sqrt_value_error(second_e);
        }

        inline auto get_hit_position(const ValueErrorD& tdc_left, const ValueErrorD& tdc_right, const HitModulePar& par)
        {
            const auto plane_id = ModuleID2PlaneID(static_cast<int>(par.module_num - 1));
            LOGP(debug2,
                 "Calculating position with left tdc: {}, right tdc {}, effective speed: {}, tdc_diff: {}",
                 tdc_left,
                 tdc_right,
                 par.effective_speed,
                 tdc_right - tdc_left);
            const auto pos_along_bar = -par.effective_speed * (tdc_right - tdc_left) / 2.;
            const auto pos_perp_bar = ValueErrorD{ GetBarVerticalDisplacement(par.module_num), BarSize_XY / SQRT_12 };
            const auto pos_z = ValueErrorD{ (plane_id + 0.5) * BarSize_Z, BarSize_Z / SQRT_12 };
            LOGP(debug2,
                 "pos along the bar: {} cm, pos perp to bar {} cm, z: {} cm",
                 pos_along_bar,
                 pos_perp_bar,
                 pos_z);

            auto is_horizontal = IsPlaneIDHorizontal(plane_id);
            return is_horizontal ? XYZVectorValueErrorD{ pos_along_bar, pos_perp_bar, pos_z }
                                 : XYZVectorValueErrorD{ pos_perp_bar, pos_along_bar, pos_z };
        }

        inline auto get_hit_pixel(const XYZVectorValueErrorD& position)
        {
            const auto pixel_x =
                std::floor(position.X().value / ::R3B::Neuland::BarSize_XY) + (::R3B::Neuland::BarsPerPlane / 2.);
            const auto pixel_y =
                std::floor(position.Y().value / ::R3B::Neuland::BarSize_XY) + (::R3B::Neuland::BarsPerPlane / 2.);
            const auto pixel_z = std::floor(position.Z().value / ::R3B::Neuland::BarSize_Z);

            return XYZVector{ pixel_x, pixel_y, pixel_z };
        }
    } // namespace

    Cal2HitTask::Cal2HitTask(std::string_view input_cal_data_name,
                             std::string_view input_cal_2_hit_par_name,
                             std::string_view output_hit_data_name)
        : CalibrationTask("R3BNeulandCal2Hit", 1)
        , cal_data_{ input_cal_data_name }
        , hit_data_{ output_hit_data_name }
        , cal_to_hit_par_{ input_cal_2_hit_par_name }
    {
    }

    void Cal2HitTask::HistogramInit(DataMonitor& histograms) {}

    void Cal2HitTask::ExtraInit(FairRootManager* /*rootMan*/)
    {
        cal_data_.init();
        hit_data_.init(not IsHistDisabled());
        if (cal_to_hit_par_->GetModulePars().empty())
        {
            throw R3B::logic_error("Cal2HitPar has no modules!");
        }
    }

    void Cal2HitTask::SetExtraPar(FairRuntimeDb* /*rtdb*/) { cal_to_hit_par_.init(this); }

    void Cal2HitTask::TriggeredExec() { calibrate(); }

    void Cal2HitTask::calibrate()
    {
        for (const auto& calBar : cal_data_)
        {
            temp_left_signals_.clear();
            temp_right_signals_.clear();

            LOGP(debug1, "Input calBar: {}", calBar);
            if (calBar.module_num == 0)
            {
                throw R3B::runtime_error("cal-level bar signal has invalid module number 0!");
            }

            calculate_calibrated_signals(calBar, temp_left_signals_, Side::left);
            calculate_calibrated_signals(calBar, temp_right_signals_, Side::right);

            const auto& module_par = cal_to_hit_par_->GetModuleParAt(calBar.module_num);
            construct_hits(temp_left_signals_, temp_right_signals_, module_par, hit_data_.get());
        }
    }

    void Cal2HitTask::calculate_calibrated_signals(const BarCalData& calBar,
                                                   /* inout */ std::vector<CalibratedSignal>& signals,
                                                   Side side)
    {
        const auto calBar_signals = (side == Side::left) ? calBar.left : calBar.right;

        const auto& module_par = cal_to_hit_par_->GetModuleParAt(calBar.module_num);

        for (const auto& cal_signal : calBar_signals)
        {
            signals.push_back(to_calibrated_signal(cal_signal, module_par, side));
        }
    }

    auto Cal2HitTask::construct_hit(const LRPair<CalibratedSignal>& signalPair, const HitModulePar& par) const -> Hit
    {
        auto hit = Hit{};

        LOGP(debug2,
             "Input left calibrated signal: {} and right calibrated signal: {}",
             signalPair.left(),
             signalPair.right());
        // hit module id is 0-based
        hit.module_id = static_cast<int>(par.module_num - 1);
        hit.tdc_left = signalPair.left().time;
        hit.tdc_right = signalPair.right().time;
        hit.time = get_hit_time(hit.tdc_left, hit.tdc_right);
        hit.qdc_left = signalPair.left().energy;
        hit.qdc_right = signalPair.right().energy;
        hit.energy = get_hit_energy(hit.qdc_left, hit.qdc_right, par);
        hit.position = get_hit_position(hit.tdc_left, hit.tdc_right, par);
        hit.pixel = get_hit_pixel(hit.position);
        LOGP(debug, "Adding a new NeulandHit: {}\n", hit);
        return hit;
    }

    void Cal2HitTask::construct_hits(const std::vector<CalibratedSignal>& left_signals,
                                     const std::vector<CalibratedSignal>& right_signals,
                                     const HitModulePar& par,
                                     /* inout */ std::vector<Hit>& hits)
    {
        // TODO: Multi-hits needs to be implemented here
        if (left_signals.size() == 1 and right_signals.size() == 1)
        {
            const auto& left_signal = left_signals.front();
            const auto& right_signal = right_signals.front();
            // signal_match_checking(left_signal, right_signal, par);
            const auto new_hit = construct_hit(R3B::LRPair<CalibratedSignal>{ left_signal, right_signal }, par);
            LOGP(debug, "Adding a new hit with: {}", new_hit);
            hits.push_back(new_hit);
        }
    }

    auto Cal2HitTask::signal_match_checking(const CalibratedSignal& first_signal,
                                            const CalibratedSignal& second_signal,
                                            const HitModulePar& par) -> bool
    {
        const auto first_input =
            SignalMatcher::Input{ .time = first_signal.time.value, .energy = first_signal.energy.value };
        const auto second_input =
            SignalMatcher::Input{ .time = second_signal.time.value, .energy = second_signal.energy.value };

        const auto match_par = SignalMatcher::Par{ .attenuation = BarLength / par.light_attenuation_length.value,
                                                   .c_medium = par.effective_speed.value };
        const auto match_goodness = SignalMatcher::GetGoodnessOfMatch(first_input, second_input, match_par);
        const auto match_result = std::log10(match_goodness);
        // FIXME: BAD comparison values. They should be 0. Why? Need fixing
        GetHistMonitor().get("match_values")->Fill(match_result);
        return true;
    }

    void Cal2HitTask::EndOfTask() {}

    auto Cal2HitTask::CheckConditions([[maybe_unused]] TH1L* hist_condition) const -> bool
    {
        if (GetTrigger() == CalTrigger::onspill)
        {
            const auto t_start = GetEventHeader()->GetTStart();
            const auto is_beam_on = not std::isnan(t_start);
            return is_beam_on;
        }
        return true;
    }

    auto Cal2HitTask::get_hit_time(ValueErrorD first_t, ValueErrorD second_t) const -> ValueErrorD
    {
        auto larger_t = (first_t > second_t) ? first_t : second_t;
        auto smaller_t = (first_t < second_t) ? first_t : second_t;

        // check if the smaller value is overflowed
        // TODO: overflow should be checked in cal level for better time calibration. But the cal level calibration
        // doesn't have signal pairing. Solution: Do it in cal level only for one hit bar signal and calibration should
        // only use those with one hit.
        if (larger_t - smaller_t > 0.5 * R3B::Neuland::MaxCalTime)
        {
            larger_t -= R3B::Neuland::MaxCalTime;
        }
        const auto t_start = [this]() -> double
        {
            const auto los_time = GetEventHeader()->GetTStart();
            if (std::isnan(los_time))
            {
                return 0.;
            }
            return los_time;
        }();
        auto time_val = ((larger_t + smaller_t) / 2.) - global_time_offset_ - t_start;
        time_val.value = std::remainder(time_val.value, R3B::Neuland::MaxCalTime);
        return time_val;
    }

    auto Cal2HitTask::get_calibrated_energy(const CalDataSignal& calSignal, const HitModulePar& par, R3B::Side side)
        -> ValueErrorD
    {
        const auto tot_no_offset = calSignal.time_over_threshold - par.pedestal.get(side);

        const auto denominator = par.energy_gain.get(side) - par.pmt_saturation.get(side) * tot_no_offset;

        if (denominator.value == 0.)
        {
            return ValueErrorD{};
        }

        // apply minimum 1 ns:
        return (tot_no_offset.value < 1) ? ValueErrorD{} : tot_no_offset / denominator;
    }

    auto Cal2HitTask::get_calibrated_time(const CalDataSignal& calSignal, const HitModulePar& par, R3B::Side side)
        -> ValueErrorD
    {
        // TODO: why positive for left?
        const auto time_offset =
            (side == R3B::Side::left) ? (par.t_sync - par.t_diff / 2) : (par.t_sync + par.t_diff / 2);
        return calSignal.leading_time - calSignal.trigger_time - time_offset;
    }

    auto Cal2HitTask::to_calibrated_signal(const CalDataSignal& calSignal, const HitModulePar& par, R3B::Side side)
        -> CalibratedSignal
    {
        const auto energy = get_calibrated_energy(calSignal, par, side);
        const auto time = get_calibrated_time(calSignal, par, side);
        return CalibratedSignal{ .energy = energy, .time = time };
    }
} // namespace R3B::Neuland
