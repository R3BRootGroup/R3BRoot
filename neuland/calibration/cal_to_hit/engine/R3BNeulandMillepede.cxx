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
#include <R3BNeulandCalToHitParTask.h>
#include <R3BNeulandCommon.h>
#include <SteerWriter.h>
#include <TGraphErrors.h>
#include <optional>
#include <range/v3/algorithm.hpp>
#include <range/v3/numeric.hpp>
#include <range/v3/view.hpp>

namespace rng = ranges;

constexpr auto DEFAULT_MEAS_ERROR = 1.F;
constexpr auto DEFAULT_RES_FILENAME = "millepede.res";
constexpr auto SCALE_FACTOR = 100.F;
constexpr auto ERROR_SCALE_FACTOR = 100.F;
constexpr auto REFERENCE_BAR_NUM = 25;
constexpr auto MILLE_BUFFER_SIZE = std::size_t{ 100000 };

namespace
{
    void calculate_time_offset(R3B::Neuland::Cal2HitPar& cal_to_hit_par)
    {
        auto& module_pars = cal_to_hit_par.GetListOfModuleParRef();
        for (auto& [module_num, module_par] : module_pars)
        {
            if (module_par.effectiveSpeed.value != 0)
            {
                module_par.tDiff = module_par.tDiff / module_par.effectiveSpeed;
            }
        }
    }
} // namespace

namespace R3B::Neuland::Calibration
{
    void MillepedeEngine::Init()
    {
        cal_to_hit_par_ = GetTask()->GetCal2HitPar();

        par_result_.set_filename(DEFAULT_RES_FILENAME);
        pede_launcher_.set_steer_filename(pede_steer_filename_);
        pede_launcher_.set_parameter_filename(parameter_filename_);
        binary_data_writer_.set_buffer_size(MILLE_BUFFER_SIZE);

        init_steer_writer();
        init_parameter();
    }

    // output: module_num & global label
    inline auto MillepedeEngine::to_module_num_label(int par_num) -> std::pair<int, GlobalLabel>
    {
        const auto num_of_module = GetModuleSize();
        auto res = std::pair<int, GlobalLabel>{};
        const auto factor = (par_num - 1) / num_of_module;
        res.first = (par_num - 1) % num_of_module + 1;

        switch (factor)
        {
            case 0:
                res.second = GlobalLabel::tsync;
                break;
            case 1:
                res.second = GlobalLabel::offset_effective_c;
                break;
            case 2:
                res.second = GlobalLabel::effective_c;
                break;
            default:
                throw R3B::logic_error(fmt::format("An error occured with unrecognized global par id: {}", par_num));
        }

        return res;
    }

    inline auto MillepedeEngine::get_global_label_id(int module_num, GlobalLabel label) -> int
    {
        const auto num_of_module = GetModuleSize();
        switch (label)
        {
            case GlobalLabel::tsync:
                return module_num;
            case GlobalLabel::offset_effective_c:
                return module_num + num_of_module;
            case GlobalLabel::effective_c:
                return module_num + 2 * num_of_module;
            default:
                throw std::runtime_error("An error occured with unrecognized global tag");
        }
    }

    void MillepedeEngine::fill_module_parameters(const Millepede::ResultReader& result,
                                                 Neuland::Cal2HitPar& cal_to_hit_par)
    {
        const auto& pars = result.get_pars();
        for (const auto& [par_id, par] : pars)
        {
            const auto [module_num, global_label] = to_module_num_label(par_id);
            auto& module_pars = cal_to_hit_par.GetListOfModuleParRef();

            auto& par_ref = module_pars.emplace(module_num, HitModulePar{}).first->second;
            switch (global_label)
            {
                case GlobalLabel::tsync:
                    par_ref.tSync.value = par.value * SCALE_FACTOR;
                    par_ref.tSync.error = par.error * SCALE_FACTOR;
                    break;
                case GlobalLabel::offset_effective_c:
                    // The value here is the product of tDiff and effectiveSped. Real tDiff will be calculated later
                    par_ref.tDiff.value = par.value * SCALE_FACTOR;
                    par_ref.tDiff.error = par.error * SCALE_FACTOR;
                    break;
                case GlobalLabel::effective_c:
                    par_ref.effectiveSpeed.value = par.value;
                    par_ref.effectiveSpeed.error = par.error;
                    break;
                default:
                    throw std::runtime_error("An error occured with unrecognized global tag");
            }
        }

        calculate_time_offset(cal_to_hit_par);
    }

    auto MillepedeEngine::set_minimum_values(const std::vector<R3B::Neuland::BarCalData>& signals) -> bool
    {
        // make sure only one hit exists in one bar
        auto filtered_signals = rng::filter_view(
            signals | rng::views::all,
            [](const auto& bar_signal) { return bar_signal.left.size() == 1 and bar_signal.right.size() == 1; });
        if (filtered_signals.empty())
        {
            return false;
        }

        if (not average_t_sum_.has_value())
        {
            auto t_sum_view = filtered_signals | rng::views::transform(
                                                     [](const auto& bar_signal)
                                                     {
                                                         const auto& left_signal = bar_signal.left.front();
                                                         const auto& right_signal = bar_signal.right.front();
                                                         return (left_signal.leading_time - left_signal.trigger_time +
                                                                 right_signal.leading_time - right_signal.trigger_time)
                                                             .value;
                                                     });
            auto sum = rng::accumulate(t_sum_view, 0.F);
            average_t_sum_ = sum / static_cast<float>(rng::distance(t_sum_view.begin(), t_sum_view.end()));
            R3BLOG(info, fmt::format("Average t_sum is calculated to be {}", average_t_sum_.value()));
        }
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
                                                  { return ModuleID2PlaneID(bar_signal.module_num - 1); }) |
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
        const auto module_num = static_cast<int>(signal.module_num);
        const auto pos_z = ModuleNum2ZPos<float>(static_cast<int>(module_num));

        auto init_effective_c = cal_to_hit_par_->GetModuleParAt(module_num).effectiveSpeed.value;

        const auto& left_signal = signal.left.front();
        const auto& right_signal = signal.right.front();
        const auto t_sum = (left_signal.leading_time - left_signal.trigger_time) +
                           (right_signal.leading_time - right_signal.trigger_time) - average_t_sum_.value_or(0.F);

        input_data_buffer_.measurement =
            static_cast<float>(t_sum.value / SCALE_FACTOR / 2.F - BarLength / SCALE_FACTOR / init_effective_c);
        input_data_buffer_.sigma = static_cast<float>(t_sum.error / SCALE_FACTOR / 2. * ERROR_SCALE_FACTOR);
        // input_data_buffer_.sigma = static_cast<float>(DEFAULT_MEAS_ERROR);
        const auto local_derivs_t = std::array{ 0.F, 0.F, pos_z / SCALE_FACTOR, 0.F, 0.F, 1.F };
        std::copy(local_derivs_t.begin(), local_derivs_t.end(), std::back_inserter(input_data_buffer_.locals));
        input_data_buffer_.globals.emplace_back(get_global_label_id(module_num, GlobalLabel::tsync), 1.F);
        input_data_buffer_.globals.emplace_back(get_global_label_id(module_num, GlobalLabel::effective_c),
                                                -BarLength / SCALE_FACTOR / 2.F / init_effective_c / init_effective_c);

        write_to_buffer();
    }

    void MillepedeEngine::add_spacial_local_constraint(int module_num)
    {
        buffer_clear();
        const auto plane_id = ModuleID2PlaneID(module_num - 1);
        const auto pos_z = PlaneID2ZPos<float>(plane_id);
        const auto is_horizontal = IsPlaneIDHorizontal(plane_id);
        const auto pos_bar_vert_disp = GetBarVerticalDisplacement(module_num);
        const auto local_derivs = is_horizontal ? std::array{ 0.F, pos_z / SCALE_FACTOR, 0.F, 0.F, 1.F, 0.F }
                                                : std::array{ pos_z / SCALE_FACTOR, 0.F, 0.F, 1.F, 0.F, 0.F };

        input_data_buffer_.measurement = static_cast<float>(pos_bar_vert_disp / SCALE_FACTOR);
        input_data_buffer_.sigma = static_cast<float>(BarSize_XY / SQRT_12 / SCALE_FACTOR * ERROR_SCALE_FACTOR);

        std::copy(local_derivs.begin(), local_derivs.end(), std::back_inserter(input_data_buffer_.locals));
        write_to_buffer();
    }

    void MillepedeEngine::add_signal_t_diff(const BarCalData& signal)
    {
        buffer_clear();
        const auto module_num = static_cast<int>(signal.module_num);
        const auto plane_id = ModuleID2PlaneID(static_cast<int>(module_num) - 1);
        const auto is_horizontal = IsPlaneIDHorizontal(plane_id);
        auto init_effective_c = cal_to_hit_par_->GetModuleParAt(module_num).effectiveSpeed.value;
        const auto pos_z = static_cast<float>(PlaneID2ZPos(plane_id));

        const auto& left_signal = signal.left.front();
        const auto& right_signal = signal.right.front();
        const auto t_diff = (right_signal.leading_time - right_signal.trigger_time) -
                            (left_signal.leading_time - left_signal.trigger_time);

        input_data_buffer_.measurement = 0.F;
        // input_data_buffer_.sigma = static_cast<float>(t_diff.error / 2.);
        // input_data_buffer_.sigma = static_cast<float>(DEFAULT_MEAS_ERROR * init_effective_c_ / 2);
        // input_data_buffer_.sigma = static_cast<float>(DEFAULT_MEAS_ERROR / 2.F);
        input_data_buffer_.sigma =
            static_cast<float>(t_diff.error / SCALE_FACTOR / 2. * std::abs(init_effective_c) * ERROR_SCALE_FACTOR);
        const auto local_derivs = is_horizontal ? std::array{ pos_z / SCALE_FACTOR, 0.F, 0.F, 1.F, 0.F, 0.F }
                                                : std::array{ 0.F, pos_z / SCALE_FACTOR, 0.F, 0.F, 1.F, 0.F };
        std::copy(local_derivs.begin(), local_derivs.end(), std::back_inserter(input_data_buffer_.locals));
        input_data_buffer_.globals.emplace_back(get_global_label_id(module_num, GlobalLabel::offset_effective_c),
                                                -0.5F);
        input_data_buffer_.globals.emplace_back(get_global_label_id(module_num, GlobalLabel::effective_c),
                                                static_cast<float>(-t_diff.value / SCALE_FACTOR / 2.));
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

    void MillepedeEngine::Calibrate(Cal2HitPar& hit_par)
    {
        hit_par.Reset();

        R3BLOG(info, "Launching pede algorithm..");
        pede_launcher_.sync_launch();
        pede_launcher_.end();

        par_result_.read();
        fill_module_parameters(par_result_, hit_par);
        fill_data_to_figure(hit_par);
    }

    void MillepedeEngine::fill_data_to_figure(Cal2HitPar& hit_par)
    {
        const auto& pars = hit_par.GetListOfModulePar();
        for (const auto& [module_num, par] : pars)
        {
            graph_time_offset_->SetPoint(static_cast<int>(module_num), module_num, par.tDiff.value);
            graph_time_offset_->SetPointError(static_cast<int>(module_num), 0., par.tDiff.error);
            graph_time_sync_->SetPoint(static_cast<int>(module_num), module_num, par.tSync.value);
            graph_time_sync_->SetPointError(static_cast<int>(module_num), 0., par.tSync.error);
            graph_effective_c_->SetPoint(static_cast<int>(module_num), module_num, par.effectiveSpeed.value);
            graph_effective_c_->SetPointError(static_cast<int>(module_num), 0., par.effectiveSpeed.error);
        }
    }

    void MillepedeEngine::EndOfEvent(unsigned int /*event_num*/)
    {
        // TODO: could be an empty event
        binary_data_writer_.end();
    }

    void MillepedeEngine::EventReset() {}

    void MillepedeEngine::HistInit(DataMonitor& histograms)
    {
        const auto module_size = GetModuleSize();

        graph_time_offset_ = histograms.add_graph("time_offset", std::make_unique<TGraphErrors>(module_size));
        graph_time_offset_->SetTitle("Time offset vs BarNum");

        graph_time_sync_ = histograms.add_graph("time_sync", std::make_unique<TGraphErrors>(module_size));
        graph_time_sync_->SetTitle("Time sync vs BarNum");

        graph_effective_c_ = histograms.add_graph("effective_c", std::make_unique<TGraphErrors>(module_size));
        graph_effective_c_->SetTitle("Effective c vs BarNum");
    }

    void MillepedeEngine::buffer_clear()
    {
        input_data_buffer_.locals.clear();
        input_data_buffer_.globals.clear();
        input_data_buffer_.measurement = 0.F;
        input_data_buffer_.sigma = 0.F;
    }

    void MillepedeEngine::write_to_buffer() { binary_data_writer_.mille(input_data_buffer_); }

    void MillepedeEngine::EndOfTask()
    {
        average_t_sum_.reset();
        buffer_clear();
    }

    void MillepedeEngine::init_parameter()
    {
        auto num_of_modules = GetModuleSize();

        if (cal_to_hit_par_ == nullptr)
        {
            throw R3B::runtime_error("Pointer to cal_to_hit_par is nullptr!");
        }

        auto& module_pars = cal_to_hit_par_->GetListOfModuleParRef();
        module_pars.clear();

        for (unsigned int module_num{ 1 }; module_num <= num_of_modules; ++module_num)
        {
            auto module_par_iter = module_pars.try_emplace(module_num).first;
            module_par_iter->second.effectiveSpeed.value = init_effective_c_;
        }
    }

    void MillepedeEngine::init_steer_writer()
    {
        auto steer_writer = SteerWriter{};
        steer_writer.set_filepath(pede_steer_filename_);
        steer_writer.set_parameter_file(parameter_filename_);
        steer_writer.set_data_filepath(input_data_filename_);
        steer_writer.add_method(SteerWriter::Method::inversion, std::make_pair(3.F, 0.001F));

        const auto module_size = GetModuleSize();
        for (int module_num{ 1 }; module_num <= module_size; ++module_num)
        {
            steer_writer.add_parameter_default(get_global_label_id(module_num, GlobalLabel::effective_c),
                                               std::make_pair(static_cast<float>(init_effective_c_), 0.F));
        }
        steer_writer.add_parameter_default(get_global_label_id(REFERENCE_BAR_NUM, GlobalLabel::tsync),
                                           std::make_pair(0.F, -1.F));
        steer_writer.write();
    }
} // namespace R3B::Neuland::Calibration
