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
#include "Mille.h"
#include "ParResultReader.h"
#include "R3BDataMonitor.h"
#include "R3BNeulandCalData2.h"
#include "R3BNeulandCalToHitPar.h"
#include "R3BNeulandMilleCalDataProcessor.h"
#include <R3BException.h>
#include <R3BNeulandCalToHitParTask.h>
#include <R3BNeulandCommon.h>
#include <SteerWriter.h>

#include <TGraphErrors.h>
#include <TH1.h>
#include <algorithm>
#include <array>
#include <cstdlib>
#include <fairlogger/Logger.h>
#include <filesystem>
#include <fmt/core.h>
#include <iterator>
#include <memory>
#include <numeric>
#include <optional>
#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/sliding.hpp>
#include <range/v3/view/transform.hpp>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace rng = ranges;

constexpr auto DEFAULT_RES_FILENAME = "millepede.res";
constexpr auto SCALE_FACTOR = 10.F;
// constexpr auto REFERENCE_BAR_NUM = 25;
constexpr auto MILLE_BUFFER_SIZE = std::size_t{ 100000 };
constexpr auto DEFAULT_T_ERROR = 2; // ns

namespace
{
    void calculate_time_offset(R3B::Neuland::Cal2HitPar& cal_to_hit_par)
    {
        auto& module_pars = cal_to_hit_par.GetListOfModuleParRef();
        for (auto& [module_num, module_par] : module_pars)
        {
            if (module_par.effective_speed.value != 0)
            {
                module_par.t_diff = module_par.t_diff / module_par.effective_speed;
            }
        }
    }

    void change_time_offset(R3B::Neuland::Cal2HitPar& cal_to_hit_par)
    {
        auto& module_pars = cal_to_hit_par.GetListOfModuleParRef();
        for (auto& [module_num, module_par] : module_pars)
        {
            module_par.t_diff = module_par.t_diff * module_par.effective_speed;
        }
    }
} // namespace

namespace R3B::Neuland::Calibration
{
    void MillepedeEngine::Init()
    {
        set_working_dir();
        fs::create_directories(fs::path(working_dir_));
        cal_to_hit_par_ = GetTask()->GetCal2HitPar();

        par_result_.set_filename((fs::path(working_dir_) / DEFAULT_RES_FILENAME).string());
        pede_launcher_.set_steer_filename(pede_steer_filename_);
        pede_launcher_.set_parameter_filename(parameter_filename_);
        if (const auto* r3b_dir = std::getenv("R3BROOTPATH"); r3b_dir != nullptr)
        {
            pede_launcher_.set_binary_dir(fmt::format("{}/bin", r3b_dir));
            // TODO: make subdir configurable
            pede_launcher_.set_working_dir(fs::absolute(fs::path{ working_dir_ }).string());
        }
        else
        {
            throw R3B::runtime_error(
                "Environment variable R3BROOTPATH is not defined! Did you forget to source the \"config.sh\" file?");
        }
        binary_data_writer_ = std::make_unique<Mille>((fs::path{ working_dir_ } / input_data_filename_).string());
        binary_data_writer_->set_buffer_size(MILLE_BUFFER_SIZE);
        data_preprocessor_ = std::make_unique<MilleDataProcessor>(GetModuleSize());
        data_preprocessor_->set_p_value_cut(p_value_cut_);

        init_steer_writer();
        init_parameter();
    }

    void MillepedeEngine::set_working_dir()
    {
        auto filename = FairRun::Instance()->GetSink()->GetFileName();
        auto output_dir_str = filename.View();
        auto output_path = fs::path{ output_dir_str };
        auto output_dir = output_path.parent_path();
        auto output_filename = output_path.filename();
        working_dir_ = (output_dir / DEFAULT_SUB_DIR / output_filename);
        LOGP(debug, "Working dir has set to be: {}", working_dir_);
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
            // case 0:
            //     res.second = GlobalLabel::tsync;
            //     break;
            // case 1:
            //     res.second = GlobalLabel::offset_effective_c;
            //     break;
            // case 2:
            //     res.second = GlobalLabel::effective_c;
            //     break;
            case 0:
                res.second = GlobalLabel::offset_effective_c;
                break;
            case 1:
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
            // case GlobalLabel::tsync:
            //     return module_num;
            // case GlobalLabel::offset_effective_c:
            //     return module_num + num_of_module;
            // case GlobalLabel::effective_c:
            //     return module_num + (2 * num_of_module);
            case GlobalLabel::offset_effective_c:
                return module_num;
            case GlobalLabel::effective_c:
                return module_num + num_of_module;
            default:
                throw R3B::logic_error("An error occured with unrecognized global tag");
        }
    }

    void MillepedeEngine::fill_module_parameters(const Millepede::ResultReader& result,
                                                 Neuland::Cal2HitPar& cal_to_hit_par)
    {
        change_time_offset(cal_to_hit_par);
        const auto& pars = result.get_pars();
        for (const auto& [par_id, par] : pars)
        {
            const auto [module_num, global_label] = to_module_num_label(par_id);
            auto& module_pars = cal_to_hit_par.GetListOfModuleParRef();

            auto& par_ref = module_pars.emplace(module_num, HitModulePar{}).first->second;
            switch (global_label)
            {
                case GlobalLabel::tsync:
                    par_ref.t_sync.value = par.value * SCALE_FACTOR;
                    par_ref.t_sync.error = par.error * SCALE_FACTOR;
                    break;
                case GlobalLabel::offset_effective_c:
                    // The value here is the product of tDiff and effectiveSped. Real tDiff will be calculated later
                    par_ref.t_diff.value += par.value * SCALE_FACTOR;
                    par_ref.t_diff.error += par.error * SCALE_FACTOR;
                    break;
                case GlobalLabel::effective_c:
                    par_ref.effective_speed.value += par.value;
                    par_ref.effective_speed.error += par.error;
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
            LOGP(info, "Average t_sum is calculated to be {}", average_t_sum_.value());
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

    void MillepedeEngine::add_signal_t_sum(const MilleCalData& signal)
    {
        buffer_clear();
        const auto module_num = static_cast<int>(signal.module_num);
        const auto pos_z = ModuleNum2ZPos<float>(static_cast<int>(module_num));

        auto init_effective_c = cal_to_hit_par_->GetModuleParAt(module_num).effective_speed.value;

        const auto& left_signal = signal.left;
        const auto& right_signal = signal.right;
        const auto t_sum = (left_signal.leading_time - left_signal.trigger_time) +
                           (right_signal.leading_time - right_signal.trigger_time) - average_t_sum_.value_or(0.F);

        input_data_buffer_.measurement =
            static_cast<float>((t_sum.value / SCALE_FACTOR / 2.F) - (BarLength / SCALE_FACTOR / init_effective_c));
        input_data_buffer_.sigma = static_cast<float>(t_sum.error / SCALE_FACTOR / 2. * error_scale_factor_);
        // input_data_buffer_.sigma = static_cast<float>(DEFAULT_MEAS_ERROR);
        const auto local_derivs_t = std::array{ 0.F, 0.F, pos_z / SCALE_FACTOR, 0.F, 0.F, 1.F };
        std::copy(local_derivs_t.begin(), local_derivs_t.end(), std::back_inserter(input_data_buffer_.locals));
        input_data_buffer_.globals.emplace_back(get_global_label_id(module_num, GlobalLabel::tsync), 1.F);
        input_data_buffer_.globals.emplace_back(get_global_label_id(module_num, GlobalLabel::effective_c),
                                                -BarLength / SCALE_FACTOR / 2.F / init_effective_c / init_effective_c);

        write_to_buffer();
    }

    void MillepedeEngine::add_spacial_local_constraint(int plane_id, const std::vector<MilleCalData>& plane_signals)
    {
        buffer_clear();
        const auto pos_z = PlaneID2ZPos<float>(plane_id);
        const auto is_horizontal = IsPlaneIDHorizontal(plane_id);
        // const auto pos_bar_vert_disp = GetBarVerticalDisplacement(module_num);
        const auto pos_bar_vert_disp = std::accumulate(plane_signals.begin(),
                                                       plane_signals.end(),
                                                       0.,
                                                       [](double sum, const auto& signal) {
                                                           return sum + GetBarVerticalDisplacement(signal.module_num);
                                                       }) /
                                       static_cast<double>(plane_signals.size());
        const auto local_derivs = is_horizontal ? std::array{ 0.F, pos_z / SCALE_FACTOR, 0.F, 1.F }
                                                : std::array{ pos_z / SCALE_FACTOR, 0.F, 1.F, 0.F };
        // const auto local_derivs = is_horizontal ? std::array{ 0.F, pos_z / SCALE_FACTOR, 0.F, 0.F, 1.F, 0.F }
        //                                         : std::array{ pos_z / SCALE_FACTOR, 0.F, 0.F, 1.F, 0.F, 0.F };

        input_data_buffer_.measurement = static_cast<float>(pos_bar_vert_disp / SCALE_FACTOR);
        input_data_buffer_.sigma = static_cast<float>(BarSize_XY / SQRT_12 / SCALE_FACTOR * error_scale_factor_);

        // if (not is_horizontal)
        // {
        //     fmt::println("c_value_1.append({})\na_value_1.append({})\nb_value_1.append({})",
        //                  pos_bar_vert_disp / SCALE_FACTOR,
        //                  pos_z / SCALE_FACTOR,
        //                  1.);
        // }
        std::copy(local_derivs.begin(), local_derivs.end(), std::back_inserter(input_data_buffer_.locals));
        write_to_buffer();
    }

    void MillepedeEngine::add_signal_t_diff(const MilleCalData& signal)
    {
        buffer_clear();
        const auto module_num = static_cast<int>(signal.module_num);
        const auto plane_id = ModuleID2PlaneID(static_cast<int>(module_num) - 1);
        const auto is_horizontal = IsPlaneIDHorizontal(plane_id);
        const auto& module_par = cal_to_hit_par_->GetModuleParAt(module_num);
        auto init_effective_c = module_par.effective_speed.value;
        auto init_t_offset = module_par.t_diff.value;
        const auto pos_z = static_cast<float>(PlaneID2ZPos(plane_id));

        const auto& left_signal = signal.left;
        const auto& right_signal = signal.right;
        const auto t_diff = (right_signal.leading_time - right_signal.trigger_time) -
                            (left_signal.leading_time - left_signal.trigger_time);

        const auto t_error = t_diff.error == 0 ? DEFAULT_T_ERROR : t_diff.error;
        input_data_buffer_.measurement = static_cast<float>((init_effective_c * init_t_offset / 2. / SCALE_FACTOR) -
                                                            (init_effective_c * t_diff.value / SCALE_FACTOR / 2.));
        input_data_buffer_.sigma =
            static_cast<float>(t_error / SCALE_FACTOR / 2. * std::abs(init_effective_c) * error_scale_factor_);
        const auto local_derivs = is_horizontal ? std::array{ pos_z / SCALE_FACTOR, 0.F, 1.F, 0.F }
                                                : std::array{ 0.F, pos_z / SCALE_FACTOR, 0.F, 1.F };
        // const auto local_derivs = is_horizontal ? std::array{ pos_z / SCALE_FACTOR, 0.F, 0.F, 1.F, 0.F, 0.F }
        //                                         : std::array{ 0.F, pos_z / SCALE_FACTOR, 0.F, 0.F, 1.F, 0.F };
        std::copy(local_derivs.begin(), local_derivs.end(), std::back_inserter(input_data_buffer_.locals));
        // fmt::println("Adding global: {}", get_global_label_id(module_num, GlobalLabel::offset_effective_c));
        input_data_buffer_.globals.emplace_back(get_global_label_id(module_num, GlobalLabel::offset_effective_c),
                                                -0.5F);
        input_data_buffer_.globals.emplace_back(get_global_label_id(module_num, GlobalLabel::effective_c),
                                                static_cast<float>(t_diff.value / SCALE_FACTOR / 2.));

        // if (is_horizontal)
        // {
        //     const auto c_val = (0.5 * (module_par.t_diff * module_par.effective_speed).value / SCALE_FACTOR) -
        //                        (module_par.effective_speed.value * t_diff.value / SCALE_FACTOR / 2.);
        //     fmt::println(
        //         "c_value_2.append({})\na_value_2.append({})\nb_value_2.append({})", c_val, pos_z / SCALE_FACTOR, 1.);
        // }
        write_to_buffer();
        LOGP(debug,
             "Writting Mille data to binary file with meas = {} and z = {}",
             input_data_buffer_.measurement,
             pos_z);
    }

    auto MillepedeEngine::select_t_diff_signal(const std::vector<MilleCalData>& plane_data)
    {
        if (plane_data.empty())
        {
            return plane_data.end();
        }
        auto calculate_residual = [this](const MilleCalData& signal) -> double
        {
            const auto t_diff = (signal.right.leading_time - signal.right.trigger_time) -
                                (signal.left.leading_time - signal.left.trigger_time);
            const auto& module_par = cal_to_hit_par_->GetModuleParAt(signal.module_num);
            const auto position = (-t_diff + module_par.t_diff) / 2 * module_par.effective_speed;
            const auto res =
                data_preprocessor_->calculate_residual(position.value, static_cast<int>(signal.module_num));
            return res;
        };
        auto iter = std::min_element(plane_data.begin(),
                                     plane_data.end(),
                                     [calculate_residual](const auto& first, const auto& second)
                                     { return calculate_residual(first) < calculate_residual(second); });
        if (iter != plane_data.end())
        {
            auto residual = calculate_residual(*iter);
            hist_t_offset_residual_->Fill(residual);
            if (residual > t_diff_residual_cut_)
            {
                return plane_data.end();
            }
            // fmt::println("selected signal residual: {}", residual);
        }
        return iter;
    }

    void MillepedeEngine::AddSignals(const std::vector<BarCalData>& signals)
    {

        // fmt::println("==================new event===============\n");
        if (not data_preprocessor_->filter(signals))
        {
            return;
        }
        const auto& processed_data = data_preprocessor_->get_data();
        for (const auto& [plane_id, plane_signals] :
             processed_data |
                 rng::views::filter([](const auto& planeid_signals) { return not planeid_signals.second.empty(); }))
        {
            // fmt::println("\n--------------------\n");
            auto iter = select_t_diff_signal(plane_signals);
            if (iter == plane_signals.end())
            {
                continue;
            }
            // fmt::println("selected signal: {}", *iter);
            // for (const auto& signal : plane_signals)
            // {
            //     // add_signal_t_sum(signal);
            add_signal_t_diff(*iter);
            add_spacial_local_constraint(plane_id, plane_signals);
            // }
        }
    }

    void MillepedeEngine::Calibrate(Cal2HitPar& hit_par)
    {
        binary_data_writer_->close();
        LOGP(info, "Launching pede algorithm..");
        pede_launcher_.launch();
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
            graph_time_offset_->SetPoint(static_cast<int>(module_num), module_num, par.t_diff.value);
            graph_time_offset_->SetPointError(static_cast<int>(module_num), 0., par.t_diff.error);
            graph_time_sync_->SetPoint(static_cast<int>(module_num), module_num, par.t_sync.value);
            graph_time_sync_->SetPointError(static_cast<int>(module_num), 0., par.t_sync.error);
            graph_effective_c_->SetPoint(static_cast<int>(module_num), module_num, par.effective_speed.value);
            graph_effective_c_->SetPointError(static_cast<int>(module_num), 0., par.effective_speed.error);
        }
    }

    void MillepedeEngine::EndOfEvent(unsigned int /*event_num*/)
    {
        // TODO: could be an empty event
        binary_data_writer_->end();
        data_preprocessor_->reset();
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

        static constexpr auto RESIDUAL_BIN_NUM = 500;
        hist_t_offset_residual_ = histograms.add_hist<TH1D>(
            "t_diff_residual", "Residual values of the positios calculated from t_dff", RESIDUAL_BIN_NUM, 0., 1000.);
    }

    void MillepedeEngine::buffer_clear()
    {
        input_data_buffer_.locals.clear();
        input_data_buffer_.globals.clear();
        input_data_buffer_.measurement = 0.F;
        input_data_buffer_.sigma = 0.F;
    }

    void MillepedeEngine::write_to_buffer() { binary_data_writer_->mille(input_data_buffer_); }

    void MillepedeEngine::EndOfTask()
    {
        average_t_sum_.reset();
        buffer_clear();
    }

    void MillepedeEngine::init_parameter()
    {
        if (cal_to_hit_par_ == nullptr)
        {
            throw R3B::runtime_error("Pointer to cal_to_hit_par is nullptr!");
        }
    }

    void MillepedeEngine::init_steer_writer()
    {
        auto steer_writer = SteerWriter{};
        steer_writer.set_working_dir(working_dir_);
        steer_writer.set_filepath(pede_steer_filename_);
        steer_writer.set_parameter_file(parameter_filename_);
        steer_writer.set_data_filepath(input_data_filename_);
        static constexpr auto NUMBER_OF_ITERARTION = 3.F;
        static constexpr auto CONVERGENCE_RECOGNITION = 0.001F;
        steer_writer.add_method(SteerWriter::Method::inversion,
                                std::make_pair(NUMBER_OF_ITERARTION, CONVERGENCE_RECOGNITION));
        steer_writer.add_other_options(std::vector<std::string>{ "hugecut", "50000" });
        steer_writer.add_other_options(std::vector<std::string>{ "outlierdownweighting", "4" });

        // const auto module_size = GetModuleSize();
        // for (int module_num{ 1 }; module_num <= module_size; ++module_num)
        // {
        //     const auto& module_par = cal_to_hit_par_->GetModuleParAt(module_num);
        //     steer_writer.add_parameter_default(
        //         get_global_label_id(module_num, GlobalLabel::effective_c),
        //         // std::make_pair(module_par.effective_speed.value, module_par.effective_speed.error));
        //         std::make_pair(DEFAULT_EFFECTIVE_C, module_par.effective_speed.error));

        //     const auto offset_effective_c = module_par.t_diff * module_par.effective_speed;
        //     steer_writer.add_parameter_default(
        //         get_global_label_id(module_num, GlobalLabel::offset_effective_c),
        //         std::make_pair(offset_effective_c.value / SCALE_FACTOR, offset_effective_c.error / SCALE_FACTOR));
        // }
        // steer_writer.add_parameter_default(get_global_label_id(REFERENCE_BAR_NUM, GlobalLabel::tsync),
        //                                    std::make_pair(0.F, -1.F));
        steer_writer.write();
    }
} // namespace R3B::Neuland::Calibration
