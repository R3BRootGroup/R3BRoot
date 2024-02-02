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

#include "R3BNeulandMillepede.h"
#include <R3BNeulandCommon.h>
#include <optional>
#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>

namespace rng = ranges;

constexpr auto DEFAULT_MEAS_ERROR = 0.01F;

namespace
{
    enum class Global
    {
        tsync,              // tsync
        offset_effective_c, // offset times effective_C
        effective_c         // effective speed of light
    };

    inline auto get_global_label_id(int module_num, int num_of_module, Global opn) -> int
    {
        switch (opn)
        {
            case Global::tsync:
                return module_num;
            case Global::offset_effective_c:
                return module_num + num_of_module;
            case Global::effective_c:
                return module_num + 2 * num_of_module;
            default:
                throw std::runtime_error("An error occured with unrecognized global tag");
        }
    }

} // namespace

namespace R3B::Neuland::Calibration
{
    auto MillepedeEngine::set_minimum_values(const std::vector<R3B::Neuland::BarCalData>& signals) -> bool
    {
        auto filtered_signals = rng::filter_view(
            signals | rng::views::all,
            [](const auto& bar_signal) { return bar_signal.left.size() == 1 and bar_signal.right.size() == 1; });
        if (filtered_signals.empty())
        {
            return false;
        }
        const auto sum_min =
            rng::min(filtered_signals | rng::views::transform(
                                            [](const auto& bar_signal)
                                            {
                                                const auto& left_signal = bar_signal.left.front();
                                                const auto& right_signal = bar_signal.right.front();
                                                return (left_signal.leading_time - left_signal.trigger_time +
                                                        right_signal.leading_time - right_signal.trigger_time)
                                                    .value;
                                            }));
        smallest_time_sum_ = static_cast<float>(sum_min);

        const auto& min_module = rng::min(filtered_signals, rng::less{}, &R3B::Neuland::BarCalData::module_num);
        minimum_pos_z_ = GetModuleZPos<float>(static_cast<int>(min_module.module_num));
        return true;
    }

    auto MillepedeEngine::SignalFilter(const std::vector<BarCalData>& signals) -> bool
    {
        // select out rays with few hits
        if (signals.size() < minimum_hit_)
        {
            return false;
        }

        // select out vertical cosmic rays
        if (rng::all_of(signals |
                            rng::views::transform([](const auto& bar_signal)
                                                  { return GetPlaneID(bar_signal.module_num - 1); }) |
                            rng::views::sliding(2),
                        [](const auto& pair) { return pair.front() == pair.back(); }))
        {
            return false;
        }

        if (not set_minimum_values(signals))
        {
            return false;
        }

        return true;
    }

    void MillepedeEngine::add_signal_t_sum(const BarCalData& signal)
    {
        buffer_clear();
        const auto module_size = GetModuleSize();
        const auto module_num = static_cast<int>(signal.module_num);
        const auto pos_z = GetModuleZPos<float>(static_cast<int>(module_num));

        const auto& left_signal = signal.left.front();
        const auto& right_signal = signal.right.front();
        const auto t_sum = (left_signal.leading_time - left_signal.trigger_time) +
                           (right_signal.leading_time - right_signal.trigger_time) - smallest_time_sum_;

        input_data_buffer_.measurement = static_cast<float>(t_sum.value);
        // input_data_buffer_.sigma = static_cast<float>(t_sum.error / 2.);
        input_data_buffer_.sigma = static_cast<float>(DEFAULT_MEAS_ERROR);
        const auto local_derivs_t = std::array{ 0.F, 0.F, pos_z - minimum_pos_z_, 0.F, 0.F };
        std::copy(local_derivs_t.begin(), local_derivs_t.end(), std::back_inserter(input_data_buffer_.locals));
        input_data_buffer_.globals.emplace_back(get_global_label_id(module_num, module_size, Global::tsync), 1.F);
        input_data_buffer_.globals.emplace_back(get_global_label_id(module_num, module_size, Global::effective_c),
                                                -BarLength / 2. * InvCLight * InvCLight);
        binary_data_writer_.mille(input_data_buffer_);
        R3BLOG(
            debug,
            fmt::format(
                "Writting Mille data to binary file with meas = {} and z = {}", input_data_buffer_.measurement, pos_z));
    }

    void MillepedeEngine::add_spacial_local_constraint(int module_num)
    {
        buffer_clear();
        const auto plane_id = GetPlaneID(module_num - 1);
        const auto pos_z = static_cast<float>(GetPlaneZPos(plane_id));
        const auto is_horizontal = IsPlaneHorizontal(plane_id);
        const auto pos_bar_vert_disp = GetBarVerticalDisplacement(module_num);
        const auto local_derivs =
            is_horizontal ? std::array{ 0.F, pos_z, 0.F, 0.F, 1.F } : std::array{ pos_z, 0.F, 0.F, 1.F, 0.F };
        input_data_buffer_.measurement = static_cast<float>(pos_bar_vert_disp);
        input_data_buffer_.sigma = static_cast<float>(DEFAULT_MEAS_ERROR);
        std::copy(local_derivs.begin(), local_derivs.end(), std::back_inserter(input_data_buffer_.locals));
        binary_data_writer_.mille(input_data_buffer_);
    }

    void MillepedeEngine::add_signal_t_diff(const BarCalData& signal)
    {
        buffer_clear();
        const auto module_size = GetModuleSize();
        const auto module_num = static_cast<int>(signal.module_num);
        const auto plane_id = GetPlaneID(static_cast<int>(module_num) - 1);
        const auto is_horizontal = IsPlaneHorizontal(plane_id);
        const auto pos_z = static_cast<float>(GetPlaneZPos(plane_id));

        const auto& left_signal = signal.left.front();
        const auto& right_signal = signal.right.front();
        const auto t_diff = (left_signal.leading_time - left_signal.trigger_time) -
                            (right_signal.leading_time - right_signal.trigger_time);

        input_data_buffer_.measurement = static_cast<float>(t_diff.value);
        // input_data_buffer_.sigma = static_cast<float>(t_diff.error / 2.);
        input_data_buffer_.sigma = static_cast<float>(DEFAULT_MEAS_ERROR);
        const auto local_derivs =
            is_horizontal ? std::array{ pos_z, 0.F, 0.F, 1.F, 0.F } : std::array{ 0.F, pos_z, 0.F, 0.F, 1.F };
        std::copy(local_derivs.begin(), local_derivs.end(), std::back_inserter(input_data_buffer_.locals));
        input_data_buffer_.globals.emplace_back(
            get_global_label_id(module_num, module_size, Global::offset_effective_c), -0.5F);
        input_data_buffer_.globals.emplace_back(get_global_label_id(module_num, module_size, Global::effective_c),
                                                static_cast<float>(t_diff.value / 2.));
        binary_data_writer_.mille(input_data_buffer_);
        R3BLOG(
            debug,
            fmt::format(
                "Writting Mille data to binary file with meas = {} and z = {}", input_data_buffer_.measurement, pos_z));
    }

    void MillepedeEngine::AddSignal(const BarCalData& signal)
    {
        // all bar signal must have one signal on both sides
        if (signal.left.size() != 1 or signal.right.size() != 1)
        {
            return;
        }

        add_signal_t_sum(signal);
        add_signal_t_diff(signal);
        add_spacial_local_constraint(static_cast<int>(signal.module_num));
    }

    void MillepedeEngine::Calibrate() {}
    void MillepedeEngine::EndOfEvent(unsigned int /*event_num*/) { binary_data_writer_.end(); }
    auto MillepedeEngine::ExtractParameters() -> Cal2HitPar { return Cal2HitPar{}; }
    void MillepedeEngine::Reset() {}
    void MillepedeEngine::buffer_clear()
    {
        input_data_buffer_.locals.clear();
        input_data_buffer_.globals.clear();
        input_data_buffer_.measurement = 0.F;
        input_data_buffer_.sigma = 0.F;
    }

} // namespace R3B::Neuland::Calibration
