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
#include <SteerWriter.h>
#include <optional>
#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>

namespace rng = ranges;

constexpr auto DEFAULT_MEAS_ERROR = 1.F;

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

        input_data_buffer_.measurement =
            static_cast<float>(t_sum.value / 100.F / 2.F - BarLength / 100.F / init_effective_c_);
        // input_data_buffer_.sigma = static_cast<float>(t_sum.error / 2.);
        input_data_buffer_.sigma = static_cast<float>(DEFAULT_MEAS_ERROR);
        const auto local_derivs_t = std::array{ 0.F, 0.F, pos_z / 100.F - minimum_pos_z_ / 100.F, 0.F, 0.F };
        std::copy(local_derivs_t.begin(), local_derivs_t.end(), std::back_inserter(input_data_buffer_.locals));
        input_data_buffer_.globals.emplace_back(get_global_label_id(module_num, module_size, Global::tsync), 1.F);
        input_data_buffer_.globals.emplace_back(get_global_label_id(module_num, module_size, Global::effective_c),
                                                -BarLength / 100.F / 2.F / init_effective_c_ / init_effective_c_);

        // fmt::print("tsum: {} Added entry: {}\n", t_sum.value / 2.F, input_data_buffer_);
        write_to_buffer();
        // R3BLOG(
        //     debug,
        //     fmt::format(
        //         "Writting Mille data to binary file with meas = {} and z = {}", input_data_buffer_.measurement,
        //         pos_z));
    }

    void MillepedeEngine::add_spacial_local_constraint(int module_num)
    {
        buffer_clear();
        const auto plane_id = GetPlaneID(module_num - 1);
        const auto pos_z = static_cast<float>(GetPlaneZPos(plane_id));
        const auto is_horizontal = IsPlaneHorizontal(plane_id);
        const auto pos_bar_vert_disp = GetBarVerticalDisplacement(module_num);
        const auto local_derivs = is_horizontal ? std::array{ 0.F, pos_z / 100.F, 0.F, 0.F, 1.F }
                                                : std::array{ pos_z / 100.F, 0.F, 0.F, 1.F, 0.F };

        input_data_buffer_.measurement = static_cast<float>(pos_bar_vert_disp / 100.F);
        input_data_buffer_.sigma = static_cast<float>(BarSize_XY / 100.F);

        std::copy(local_derivs.begin(), local_derivs.end(), std::back_inserter(input_data_buffer_.locals));
        // fmt::print("xy: Added entry: {}\n", input_data_buffer_);
        write_to_buffer();
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

        input_data_buffer_.measurement = 0.F;
        // input_data_buffer_.sigma = static_cast<float>(t_diff.error / 2.);
        // input_data_buffer_.sigma = static_cast<float>(DEFAULT_MEAS_ERROR * init_effective_c_ / 2);
        input_data_buffer_.sigma = static_cast<float>(DEFAULT_MEAS_ERROR / 2.F);
        const auto local_derivs = is_horizontal ? std::array{ pos_z / 100.F, 0.F, 0.F, 1.F, 0.F }
                                                : std::array{ 0.F, pos_z / 100.F, 0.F, 0.F, 1.F };
        std::copy(local_derivs.begin(), local_derivs.end(), std::back_inserter(input_data_buffer_.locals));
        input_data_buffer_.globals.emplace_back(
            get_global_label_id(module_num, module_size, Global::offset_effective_c), -0.5F);
        input_data_buffer_.globals.emplace_back(get_global_label_id(module_num, module_size, Global::effective_c),
                                                static_cast<float>(t_diff.value / 100. / 2.));
        // R3BLOG(info, fmt::format("module num: {}, time diff: {}", module_num, t_diff.value));
        // fmt::print("tdiff: Added entry: {}\n", input_data_buffer_);
        write_to_buffer();
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

    void MillepedeEngine::Init()
    {
        auto steer_writer = SteerWriter{};
        steer_writer.set_filepath(pede_steer_filename_);
        steer_writer.set_data_filepath(input_data_filename_);
        steer_writer.add_method(SteerWriter::Method::inversion, std::make_pair(3.F, 0.01F));

        const auto module_size = GetModuleSize();
        for (int module_num{ 1 }; module_num <= module_size; ++module_num)
        {
            steer_writer.add_parameter_default(get_global_label_id(module_num, module_size, Global::effective_c),
                                               std::make_pair(static_cast<float>(init_effective_c_), 0.F));
        }
        steer_writer.add_parameter_default(get_global_label_id(25, module_size, Global::tsync),
                                           std::make_pair(0.F, -1.F));
        steer_writer.write();
    }

    void MillepedeEngine::Calibrate() {}
    void MillepedeEngine::EndOfEvent(unsigned int /*event_num*/)
    {
        // TODO: could be an empty event
        binary_data_writer_.end();
        // if (has_rank_check_)
        // {
        //     rank_checker_.calculate();
        //     auto decomp = rank_checker_.get_decomp();
        //     decomp.setThreshold(matrix_epsilon_);
        //     R3BLOG(info, fmt::format("\nrank: {}", decomp.rank()));
        // }
        // std::cout << "Event ended.-----------------------------------\n";
    }

    auto MillepedeEngine::ExtractParameters() -> Cal2HitPar { return Cal2HitPar{}; }

    void MillepedeEngine::Reset()
    {
        // if (has_rank_check_)
        // {
        //     rank_checker_.reset();
        // }
    }

    void MillepedeEngine::buffer_clear()
    {
        input_data_buffer_.locals.clear();
        input_data_buffer_.globals.clear();
        input_data_buffer_.measurement = 0.F;
        input_data_buffer_.sigma = 0.F;
    }

    void MillepedeEngine::write_to_buffer()
    {
        binary_data_writer_.mille(input_data_buffer_);
        // if (has_rank_check_)
        // {
        //     rank_checker_.add_entry(input_data_buffer_);
        // }
    }

} // namespace R3B::Neuland::Calibration
