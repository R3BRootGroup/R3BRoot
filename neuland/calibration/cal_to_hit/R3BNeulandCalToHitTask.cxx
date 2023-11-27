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
#include <R3BLogger.h>
#include <R3BNeulandSignalMatcher.h>
#include <R3BShared.h>

namespace
{
    using R3B::ValueErrorD;
    using R3B::Neuland::CalibratedSignal;
    using R3B::Neuland::HitModulePar;

    inline auto get_hit_energy(double first_e, double second_e, const HitModulePar& par)
    {
        // const auto attenuation_value = std::exp(R3B::Neuland::TotalBarLength / par.lightAttenuationLength.value);
        return par.lightAttenuationFactor.value * std::sqrt(first_e * second_e);
    }

    inline auto get_hit_position(double tdc_left, double tdc_right, const HitModulePar& par)
    {
        const auto plane_id = ::R3B::Neuland::GetPlaneID(static_cast<int>(par.module_num - 1));
        const auto bar_num = par.module_num % ::R3B::Neuland::BarsPerPlane;
        R3BLOG(debug2,
               fmt::format("Calculating position with left tdc: {}, right tdc {}, effective speed: {}, tdc_diff: {}",
                           tdc_left,
                           tdc_right,
                           par.effectiveSpeed,
                           tdc_right - tdc_left));
        const auto pos_along_bar = par.effectiveSpeed.value * (tdc_right - tdc_left);
        const auto pos_perp_bar = (bar_num - 0.5 - ::R3B::Neuland::BarsPerPlane * 0.5) * ::R3B::Neuland::BarSize_XY;
        const auto pos_z = (plane_id + 0.5) * ::R3B::Neuland::BarSize_Z;
        R3BLOG(debug2,
               fmt::format(
                   "pos along the bar: {} cm, pos perp to bar {} cm, z: {} cm", pos_along_bar, pos_perp_bar, pos_z));

        auto is_horizontal = ::R3B::Neuland::IsPlaneHorizontal(plane_id);
        return is_horizontal ? TVector3{ pos_along_bar, pos_perp_bar, pos_z }
                             : TVector3{ pos_perp_bar, pos_along_bar, pos_z };
    }

    inline auto get_hit_pixel(const TVector3& position)
    {
        const auto pixel_x = std::floor(position.X() / ::R3B::Neuland::BarSize_XY) + ::R3B::Neuland::BarsPerPlane / 2.;
        const auto pixel_y = std::floor(position.Y() / ::R3B::Neuland::BarSize_XY) + ::R3B::Neuland::BarsPerPlane / 2.;
        const auto pixel_z = std::floor(position.Z() / ::R3B::Neuland::BarSize_Z);

        return TVector3{ pixel_x, pixel_y, pixel_z };
    }
} // namespace

namespace R3B::Neuland
{

    Cal2HitTask::Cal2HitTask(std::string_view name, int iVerbose)
        : CalibrationTask(name, iVerbose)
    {
    }

    void Cal2HitTask::HistogramInit(DataMonitor& histograms) {}

    void Cal2HitTask::ExtraInit(FairRootManager* /*rootMan*/)
    {
        cal_data_.init();
        hit_data_.init(not IsHistDisabled());
    }

    void Cal2HitTask::SetExtraPar(FairRuntimeDb* rtdb) {}

    void Cal2HitTask::TriggeredExec() { calibrate(); }

    void Cal2HitTask::calibrate()
    {
        for (const auto& calBar : cal_data_)
        {
            temp_left_signals_.clear();
            temp_right_signals_.clear();

            R3BLOG(debug1, fmt::format("Input calBar: {}", calBar));
            if (calBar.module_num == 0)
            {
                throw R3B::runtime_error("cal-level bar signal has invalid moudule number 0!");
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

    auto Cal2HitTask::construct_hit(const LRPair<CalibratedSignal>& signalPair, const HitModulePar& par) const
        -> R3BNeulandHit
    {
        auto hit = R3BNeulandHit{};

        R3BLOG(debug2,
               fmt::format("Input left calibrated signal: {} and right calibrated signal: {}",
                           signalPair.left(),
                           signalPair.right()));
        // hit module id is 0-based
        hit.module_id = static_cast<int>(par.module_num - 1);
        hit.tdc_left = signalPair.left().time.value;
        hit.tdc_right = signalPair.right().time.value;
        hit.time = get_hit_time(hit.tdc_left, hit.tdc_right);
        hit.qdc_left = signalPair.left().energy.value;
        hit.qdc_right = signalPair.right().energy.value;
        hit.energy = get_hit_energy(hit.qdc_left, hit.qdc_right, par);
        hit.position = get_hit_position(hit.tdc_left, hit.tdc_right, par);
        hit.pixel = get_hit_pixel(hit.position);
        R3BLOG(debug, fmt::format("Adding a new NeulandHit: {}\n", hit));
        return hit;
    }

    void Cal2HitTask::construct_hits(const std::vector<CalibratedSignal>& left_signals,
                                 const std::vector<CalibratedSignal>& right_signals,
                                 const HitModulePar& par,
                                 /* inout */ std::vector<R3BNeulandHit>& hits)
    {
        // TODO: Multi-hits needs to be implemented here
        if (left_signals.size() == 1 and right_signals.size() == 1)
        {
            const auto& left_signal = left_signals.front();
            const auto& right_signal = right_signals.front();
            // signal_match_checking(left_signal, right_signal, par);
            hits.push_back(construct_hit(R3B::LRPair<CalibratedSignal>{ left_signal, right_signal }, par));
        }
    }

    auto Cal2HitTask::signal_match_checking(const CalibratedSignal& first_signal,
                                        const CalibratedSignal& second_signal,
                                        const HitModulePar& par) -> bool
    {
        const auto first_input = SignalMatcher::Input{ first_signal.time.value, first_signal.energy.value };
        const auto second_input = SignalMatcher::Input{ second_signal.time.value, second_signal.energy.value };

        const auto match_par =
            SignalMatcher::Par{ BarLength / par.lightAttenuationLength.value, par.effectiveSpeed.value };
        const auto match_goodness = SignalMatcher::GetGoodnessOfMatch(first_input, second_input, match_par);
        const auto match_result = std::log10(match_goodness);
        // FIXME: BAD comparison values. They should be 0. Why? Need fixing
        GetHistMonitor().get("match_values")->Fill(match_result);
        return true;
    }

    void Cal2HitTask::EndOfTask() {}

    // auto Cal2Hit::CheckConditions() const -> bool
    // {
    //     const auto t_start = GetEventHeader()->GetTStart();
    //     const auto is_beam_on = not std::isnan(t_start);
    //     return is_beam_on;
    // }

    auto Cal2HitTask::get_hit_time(double first_t, double second_t) const -> double
    {
        auto larger_t = (first_t > second_t) ? first_t : second_t;
        auto smaller_t = (first_t < second_t) ? first_t : second_t;

        // check if the smaller value is overflowed
        // TODO: overflow should be checked in cal level for better time calibration. But the cal level calibration
        // doesn't have singal paring. Solution: Do it in cal level only for one hit bar signal and calibration should
        // only use those with one hit.
        if (larger_t - smaller_t > 0.5 * R3B::Neuland::MaxCalTime)
        {
            larger_t -= R3B::Neuland::MaxCalTime;
        }
        return std::remainder((larger_t + smaller_t) / 2. - global_time_offset_ - GetEventHeader()->GetTStart(),
                              R3B::Neuland::MaxCalTime);
    }

    auto Cal2HitTask::get_calibrated_energy(const CalDataSignal& calSignal, const HitModulePar& par, R3B::Side side)
        -> ValueErrorD
    {
        const auto tot_no_offset = calSignal.time_over_threshold - par.pedestal.get(side);

        // apply minimum 1 ns:
        return (tot_no_offset.value < 1)
                   ? ValueErrorD{}
                   : tot_no_offset / (par.energyGain.get(side) - par.PMTSaturation.get(side) * tot_no_offset);
    }

    auto Cal2HitTask::get_calibrated_time(const CalDataSignal& calSignal, const HitModulePar& par, R3B::Side side)
        -> ValueErrorD
    {
        // TODO: why positive for left?
        const auto time_offset = (side == R3B::Side::left) ? (par.tSync - par.tDiff / 2) : (par.tSync + par.tDiff / 2);
        return calSignal.leading_time - calSignal.trigger_time - time_offset;
    }

    auto Cal2HitTask::to_calibrated_signal(const CalDataSignal& calSignal, const HitModulePar& par, R3B::Side side)
        -> CalibratedSignal
    {
        const auto energy = get_calibrated_energy(calSignal, par, side);
        const auto time = get_calibrated_time(calSignal, par, side);
        return CalibratedSignal{ energy, time };
    }
} // namespace R3B::Neuland
