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
#include "PedeLauncher.h"
#include "R3BDataMonitor.h"
#include "R3BNeulandCalData2.h"
#include "R3BNeulandCalToHitPar.h"
#include "R3BNeulandMilleCalDataProcessor.h"
#include <R3BException.h>
#include <R3BNeulandCalToHitParTask.h>
#include <R3BNeulandCommon.h>
#include <R3BNeulandCommonFunc.h>
#include <SteerWriter.h>

#include <TGraphErrors.h>
#include <TH1.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fairlogger/Logger.h>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/format.h>
#include <functional>
#include <iterator>
#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <numeric>
#include <optional>
#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/algorithm/min_element.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/map.hpp>
#include <range/v3/view/repeat.hpp>
#include <range/v3/view/sliding.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/transform.hpp>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// #define TWO_PAR

namespace rng = ranges;

constexpr auto DEFAULT_RES_FILENAME = "millepede.res";
constexpr auto DEFAULT_RES_JSON_FILENAME = "millepede.res.json";
constexpr auto SCALE_FACTOR = 10.F;
// constexpr auto REFERENCE_BAR_NUM = 25;
constexpr auto MILLE_BUFFER_SIZE = std::size_t{ 100000 };
constexpr auto DEFAULT_T_ERROR = 2; // ns

namespace
{
    enum class FilterEvent : uint8_t
    {
        after_trigger,
        n_plane_filter,
    };
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

    template <typename T, typename U>
    void vector_zip_action(const T& data, U&& unitary_action)
    {
        namespace sr = ranges;
        namespace sv = ranges::views;
        const auto max_size = sr::max(data | sv::transform([](const auto& vec) -> auto { return vec.size(); }));
        static constexpr auto nth_element = [](auto& vec, const int n) -> auto
        { return sv::repeat(vec) | sv::join | sv::drop(n) | sv::take(1); };

        for (const auto index : sv::iota(std::size_t{ 0 }, max_size))
        {
            auto zip_view = data | sv::filter([](const auto& vec) -> auto { return not vec.empty(); }) |
                            sv::transform([index](auto&& vec) -> auto { return nth_element(vec, index); }) | sv::join;
            std::forward<U>(unitary_action)(zip_view);
        }
    }

    namespace sv = ranges::views;
} // namespace

namespace R3B::Neuland::Calibration
{
    void MillepedeEngine::set_options(const MillepedeOptions& options) { config_ = options; }

    void MillepedeEngine::Init()
    {
        set_working_dir();
        if (config_.enable_data_write)
        {
            output_mille_data_.init();
            output_tsync_mille_data_.init();
            output_mille_track_info_.init();
        }
        fs::create_directories(fs::path(working_dir_));
        cal_to_hit_par_ = GetTask()->GetCal2HitPar();

        par_result_.set_filename((fs::path(working_dir_) / DEFAULT_RES_FILENAME).string());
        pede_launcher_.set_steer_filename(pede_steer_filename_);
        pede_launcher_.set_parameter_filename(config_.pede_par_filename);
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
        binary_data_writer_ =
            std::make_unique<Mille>((fs::path{ working_dir_ } / config_.mille_data_filename).string());
        binary_data_writer_->set_buffer_size(MILLE_BUFFER_SIZE);
        data_preprocessor_ = std::make_unique<MilleDataProcessor>(GetModuleSize());
        data_preprocessor_->set_p_value_cut(config_.p_value_cut);

        init_steer_writer();
        init_parameter();
    }

    void MillepedeEngine::set_working_dir()
    {
        const auto filename = FairRun::Instance()->GetSink()->GetFileName();
        const auto output_dir = fs::path{ filename.View() }.parent_path();
        const auto time_now = std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
        const auto output_folder_name =
            config_.outdir_has_timestamp ? fmt::format("{:%Y-%m-%d_%H-%M-%S}", time_now) : "output";

        working_dir_ = (output_dir / DEFAULT_SUB_DIR / output_folder_name);
        LOGP(debug, "Working dir has set to be: {}", working_dir_);
    }

    // output: module_num & global label
    inline auto MillepedeEngine::to_module_num_label(int par_num) -> std::pair<int, GlobalLabel>
    {
        const auto num_of_module = GetModuleSize();
        auto res = std::pair<int, GlobalLabel>{};
        res.first = ((par_num - 1) % num_of_module) + 1;

        if (config_.cal_mode == MillepedeCalibrationMode::tsync)
        {
            res.second = GlobalLabel::tsync;
        }
        else if (config_.cal_mode == MillepedeCalibrationMode::offset_effective_c)
        {
            const auto factor = (par_num - 1) / num_of_module;
            switch (factor)
            {
                case 0:
                    res.second = GlobalLabel::offset_effective_c;
                    break;
                case 1:
                    res.second = GlobalLabel::effective_c;
                    break;
                default:
                    throw R3B::logic_error(
                        fmt::format("An error occurred with unrecognized global par id: {}", par_num));
            }
        }
        else
        {
            throw R3B::logic_error(
                fmt::format("cal mode {} is not implemented!", magic_enum::enum_name(config_.cal_mode)));
        }

        return res;
    }

    inline auto MillepedeEngine::get_global_label_id(int module_num, GlobalLabel label) -> int
    {
        const auto num_of_module = GetModuleSize();

        if (config_.cal_mode == MillepedeCalibrationMode::tsync)
        {
            return module_num;
        }
        if (config_.cal_mode == MillepedeCalibrationMode::offset_effective_c)
        {
            switch (label)
            {
                case GlobalLabel::offset_effective_c:
                    return module_num;
                case GlobalLabel::effective_c:
                    return module_num + num_of_module;
                default:
            }
        }
        throw R3B::logic_error(fmt::format("cal mode {} is not implemented!", magic_enum::enum_name(config_.cal_mode)));
    }

    void MillepedeEngine::fill_module_parameters(const Millepede::ResultReader& result,
                                                 Neuland::Cal2HitPar& cal_to_hit_par)
    {
        // this changes the t_diff value to the product of t_diff and speed.
        if (config_.cal_mode == MillepedeCalibrationMode::offset_effective_c)
        {
            change_time_offset(cal_to_hit_par);
        }

        const auto& pars = result.get_pars();
        for (const auto& [par_id, par] : pars)
        {
            const auto [module_num, global_label] = to_module_num_label(par_id);
            auto& module_pars = cal_to_hit_par.GetListOfModuleParRef();

            auto& par_ref = module_pars.emplace(module_num, HitModulePar{}).first->second;
            if (config_.cal_mode == MillepedeCalibrationMode::tsync)
            {
                if (module_num == DEFAULT_TSYNC_REFERENCE_BAR_NUM)
                {
                    par_ref.t_sync.value = DEFAULT_TSYNC_REFERENCE_BAR_VALUE;
                    par_ref.t_sync.error = 0.;
                }
                else
                {
                    par_ref.t_sync.value += par.value * SCALE_FACTOR;
                    par_ref.t_sync.error = par.error * SCALE_FACTOR;
                }
            }
            else if (config_.cal_mode == MillepedeCalibrationMode::offset_effective_c)
            {
                switch (global_label)
                {
                    case GlobalLabel::offset_effective_c:
                        // NOTE: The value here is the product of tDiff and effectiveSped. Real tDiff will be calculated
                        // later
                        par_ref.t_diff.value += par.value * SCALE_FACTOR;
                        par_ref.t_diff.error = par.error * SCALE_FACTOR;
                        break;
                    case GlobalLabel::effective_c:
                        par_ref.effective_speed.value += par.value;
                        par_ref.effective_speed.error = par.error;
                        break;
                    default:
                        throw std::runtime_error("An error occurred with unrecognized global tag");
                }
            }
            else
            {
                throw R3B::logic_error(
                    fmt::format("cal mode {} is not implemented!", magic_enum::enum_name(config_.cal_mode)));
            }
        }

        if (config_.cal_mode == MillepedeCalibrationMode::offset_effective_c)
        {
            calculate_time_offset(cal_to_hit_par);
        }
    }

    auto MillepedeEngine::set_minimum_values(const std::vector<R3B::Neuland::BarCalData>& signals) -> bool
    {
        // make sure only one hit exists in one bar
        auto filtered_signals =
            rng::filter_view(signals | rng::views::all,
                             [](const auto& bar_signal) -> auto
                             { return bar_signal.left.size() == 1 and bar_signal.right.size() == 1; });
        if (filtered_signals.empty())
        {
            return false;
        }

        if (not average_t_sum_.has_value())
        {
            auto t_sum_view = filtered_signals | rng::views::transform(
                                                     [](const auto& bar_signal) -> auto
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
        const auto n_plane = GetTask()->GetBasePar()->get_num_of_planes();
        plane_counter_.clear();
        plane_counter_.resize(n_plane, 0);

        // select out events with less plane hits
        for (const auto& bar_data : signals)
        {
            if (bar_data.left.size() != bar_data.right.size())
            {
                continue;
            }
            if (bar_data.left.size() != 1)
            {
                continue;
            }
            const auto module_num = bar_data.module_num;
            const auto plane_id = Common::ModuleID2PlaneID(module_num - 1);
            ++plane_counter_.at(plane_id);
        }
        const auto n_hit_plane = std::ranges::count_if(plane_counter_, [](auto val) -> bool { return val != 0; });
        hist_plane_hit_num_->Fill(static_cast<double>(n_hit_plane));
        if (n_hit_plane < config_.min_plane_num)
        {
            return false;
        }

        if (not set_minimum_values(signals))
        {
            return false;
        }

        return true;
    }

    auto MillepedeEngine::add_signal_t_sum(const MilleCalData& signal, const TrackFitResult& fit_coeff) -> float
    {
        buffer_clear();

        const auto x_z_slope = fit_coeff.x_z.slope;
        const auto y_z_slope = fit_coeff.y_z.slope;
        const auto a_z = std::sqrt(1 + (x_z_slope * x_z_slope) + (y_z_slope * y_z_slope)) / CLight;
        const auto sign_factor = (y_z_slope > 0.) ? 1. : -1.;

        const auto module_num = static_cast<int>(signal.module_num);
        const auto pos_z = ModuleNum2ZPos<float>(static_cast<int>(module_num));

        const auto init_effective_c = cal_to_hit_par_->GetModuleParAt(module_num).effective_speed.value;
        const auto init_t_sync = (module_num == DEFAULT_TSYNC_REFERENCE_BAR_NUM)
                                     ? DEFAULT_TSYNC_REFERENCE_BAR_VALUE / SCALE_FACTOR
                                     : cal_to_hit_par_->GetModuleParAt(module_num).t_sync.value;

        const auto& left_signal = signal.left;
        const auto& right_signal = signal.right;
        const auto t_sum = (left_signal.leading_time - left_signal.trigger_time) +
                           (right_signal.leading_time - right_signal.trigger_time) - average_t_sum_.value_or(0.F);

        auto meas = static_cast<float>((t_sum.value / 2.F) + (sign_factor * pos_z * a_z) -
                                       (BarLength / init_effective_c / 2.F) - init_t_sync) /
                    SCALE_FACTOR;
        // auto meas = static_cast<float>((t_sum.value / 2.F) + (sign_factor * pos_z * a_z) +
        //                                (BarLength / init_effective_c) - init_t_sync) /
        //             SCALE_FACTOR;
        input_data_buffer_.measurement = meas;
        input_data_buffer_.sigma = static_cast<float>(t_sum.error / SCALE_FACTOR / 2. * config_.scale_factor);

        const auto local_derivs_t = std::array{ 1.F };
#ifdef HAS_CPP_STANDARD_17
        std::copy(local_derivs_t.begin(), local_derivs_t.end(), std::back_inserter(input_data_buffer_.locals));
#else
        ranges::copy(local_derivs_t, std::back_inserter(input_data_buffer_.locals));
#endif
        input_data_buffer_.globals.emplace_back(get_global_label_id(module_num, GlobalLabel::tsync), 1.F);

        write_to_buffer();
        return meas;
    }

    void MillepedeEngine::add_spacial_local_constraint(int plane_id, const std::vector<MilleCalData>& plane_signals)
    {
        buffer_clear();
        const auto pos_z = PlaneID2ZPos<float>(plane_id);
        const auto is_horizontal = IsPlaneIDHorizontal(plane_id);
        // const auto pos_bar_vert_disp = GetBarVerticalDisplacement(module_num);
        auto n_not_outlier = 0;
        auto pos_bar_vert_disp = std::accumulate(plane_signals.begin(),
                                                 plane_signals.end(),
                                                 0.,
                                                 [this, &n_not_outlier](double sum, const auto& signal) -> double
                                                 {
                                                     if (data_preprocessor_->check_is_outlier(signal.module_num))
                                                     {
                                                         return 0.;
                                                     }
                                                     ++n_not_outlier;
                                                     return sum + GetBarVerticalDisplacement(signal.module_num);
                                                 });
        if (n_not_outlier == 0)
        {
            LOGP(debug, "no valid constraint found in event id: {}", GetTask()->GetEventHeader()->GetEventno());
            return;
        }
        pos_bar_vert_disp /= n_not_outlier;
        const auto local_derivs = std::array{ pos_z / SCALE_FACTOR, 1.F };

        input_data_buffer_.measurement = static_cast<float>(pos_bar_vert_disp / SCALE_FACTOR);
        input_data_buffer_.sigma = static_cast<float>(BarSize_XY / SQRT_12 / SCALE_FACTOR * n_not_outlier);

#ifdef HAS_CPP_STANDARD_17
        std::copy(local_derivs.begin(), local_derivs.end(), std::back_inserter(input_data_buffer_.locals));
#else
        ranges::copy(local_derivs, std::back_inserter(input_data_buffer_.locals));
#endif
        write_to_buffer();
    }

    void MillepedeEngine::add_signal_t_diff(const MilleCalData& signal)
    {
        buffer_clear();
        const auto module_num = static_cast<int>(signal.module_num);
        const auto plane_id = ModuleID2PlaneID(static_cast<int>(module_num) - 1);
        const auto is_horizontal = IsPlaneIDHorizontal(plane_id);
        const auto& module_par = cal_to_hit_par_->GetModuleParAt(module_num);
        const auto init_effective_c = module_par.effective_speed.value;
        const auto init_t_offset = module_par.t_diff.value;
        const auto pos_z = static_cast<float>(PlaneID2ZPos(plane_id));

        const auto& left_signal = signal.left;
        const auto& right_signal = signal.right;
        const auto t_diff = (right_signal.leading_time - right_signal.trigger_time) -
                            (left_signal.leading_time - left_signal.trigger_time);

        const auto t_error = t_diff.error == 0 ? DEFAULT_T_ERROR : t_diff.error;
        input_data_buffer_.measurement = static_cast<float>((init_effective_c * init_t_offset / 2. / SCALE_FACTOR) -
                                                            (init_effective_c * t_diff.value / SCALE_FACTOR / 2.));
        input_data_buffer_.sigma =
            static_cast<float>(t_error / SCALE_FACTOR / 2. * std::abs(init_effective_c) * config_.scale_factor);
        const auto local_derivs = std::array{ pos_z / SCALE_FACTOR, 1.F };
        ranges::copy(local_derivs, std::back_inserter(input_data_buffer_.locals));
        input_data_buffer_.globals.emplace_back(get_global_label_id(module_num, GlobalLabel::offset_effective_c),
                                                -0.5F);
        input_data_buffer_.globals.emplace_back(get_global_label_id(module_num, GlobalLabel::effective_c),
                                                static_cast<float>(t_diff.value / SCALE_FACTOR / 2.));

        write_to_buffer();
        LOGP(debug,
             "Writing Mille data to binary file with meas = {} and z = {}",
             input_data_buffer_.measurement,
             pos_z);
    }

    auto MillepedeEngine::select_t_sync_signal(const std::vector<MilleCalData>& plane_data)
    {
        auto iter = ranges::min_element(plane_data, std::less{}, &MilleCalData::residual);
        if (iter != plane_data.end())
        {
            auto residual = iter->residual;
            hist_t_offset_residual_->Fill(residual);
            if (residual > config_.t_diff_residual_cut)
            {
                return plane_data.end();
            }
            // fmt::println("selected signal residual: {}", residual);
        }
        return iter;
    }

    template <>
    void MillepedeEngine::add_signals<MillepedeCalibrationMode::offset_effective_c>(
        const DataBufferType& processed_data,
        const TrackFitResult& fit_coeff)
    {
        const auto is_xz_flat = std::abs(fit_coeff.x_z.slope) < std::abs(config_.max_abs_a_xz);
        const auto is_yz_flat = std::abs(fit_coeff.y_z.slope) < std::abs(config_.max_abs_a_yz);
        auto add_to_constraints = [this, &processed_data](bool is_x_z) -> void
        {
            for (const auto& [plane_id, plane_signals] :
                 processed_data | rng::views::filter(
                                      [is_x_z](const auto& planeid_signals) -> auto
                                      {
                                          return (not planeid_signals.second.empty()) and
                                                 (IsPlaneIDHorizontal(planeid_signals.first) != is_x_z);
                                      }))
            {
                add_spacial_local_constraint(plane_id, plane_signals);
            }
        };
        for (const auto& [plane_id, plane_signals] :
             processed_data | rng::views::filter([](const auto& planeid_signals) -> auto
                                                 { return not planeid_signals.second.empty(); }))
        {
            const auto is_horizontal = IsPlaneIDHorizontal(plane_id);
            if (not(is_horizontal ? is_xz_flat : is_yz_flat))
            {
                continue;
            }
            for (const auto& signal : plane_signals)
            {
                add_signal_t_diff(signal);
                add_to_constraints(is_horizontal);
                binary_data_writer_->end();
            }
        }
        if (config_.enable_data_write)
        {
            for (const auto& bar_signal : processed_data | sv::values | sv::join)
            {
                output_mille_data_.get().push_back(bar_signal);
            }
        }
    }

    template <>
    void MillepedeEngine::add_signals<MillepedeCalibrationMode::tsync>(const DataBufferType& processed_data,
                                                                       const TrackFitResult& fit_coeff)
    {
        for (const auto& [plane_id, plane_signals] :
             processed_data | rng::views::filter([](const auto& planeid_signals) -> auto
                                                 { return not planeid_signals.second.empty(); }))
        {
            auto iter = select_t_sync_signal(plane_signals);
            if (iter == plane_signals.end())
            {
                continue;
            }
            auto mille_data = *iter;
            mille_data.tsync_meas = add_signal_t_sum(mille_data, fit_coeff);
            if (config_.enable_data_write)
            {
                output_tsync_mille_data_.get().push_back(mille_data);
            }
        }

        binary_data_writer_->end();
    }

    void MillepedeEngine::AddSignals(const std::vector<BarCalData>& signals)
    {
        auto* task = GetTask();
        if (not data_preprocessor_->process(signals, *cal_to_hit_par_))
        {
            task->ConditionFillToHist("preprocess_fail");
            return;
        }
        task->ConditionFillToHist("preprocess_success");
        const auto& processed_data = data_preprocessor_->get_data();
        if (config_.enable_data_write)
        {
            output_mille_track_info_ = data_preprocessor_->get_track_info();
        }
        const auto& fit_coeff = data_preprocessor_->get_track_info().bar_disp_data;

        const auto is_xz_flat = std::abs(fit_coeff.x_z.slope) < std::abs(config_.max_abs_a_xz);
        const auto is_yz_flat = std::abs(fit_coeff.y_z.slope) < std::abs(config_.max_abs_a_yz);

        if (not(is_xz_flat or is_yz_flat))
        {
            return;
        }

        switch (config_.cal_mode)
        {
            case MillepedeCalibrationMode::offset_effective_c:
                add_signals<MillepedeCalibrationMode::offset_effective_c>(processed_data, fit_coeff);
                break;
            case MillepedeCalibrationMode::tsync:
                add_signals<MillepedeCalibrationMode::tsync>(processed_data, fit_coeff);
                break;
            default:
                throw R3B::logic_error(
                    fmt::format("Unrecognized calibration mode: {}", magic_enum::enum_name(config_.cal_mode)));
        }

        add_fit_result_hist(fit_coeff);
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

    void MillepedeEngine::EndOfEvent([[maybe_unused]] unsigned int event_num)
    {
        // TODO: could be an empty event
        binary_data_writer_->end();
        data_preprocessor_->reset();
        // fmt::println("<<<<<<<<<<<<<<<< End of the event {}", event_num);
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
        static constexpr auto PAR_BIN_NUM = 400;
        static const auto SLOPE_MAX = 10.;
        static const auto OFFSET_MAX = 200.;

        auto set_x_title = [](auto* hist, std::string_view title) -> void { hist->GetXaxis()->SetTitle(title.data()); };
        auto set_y_title = [](auto* hist, std::string_view title) -> void { hist->GetYaxis()->SetTitle(title.data()); };

        hist_t_offset_residual_ = histograms.add_hist<TH1D>(
            "t_diff_residual", "Residual values of the positions calculated from t_dff", RESIDUAL_BIN_NUM, 0., 1000.);

        hist_p_value_xz_ = histograms.add_hist<TH1D>(
            "p_value_xz", "p values from the fitting of the muon track in xz plane", RESIDUAL_BIN_NUM, 0., 1.);
        hist_p_value_yz_ = histograms.add_hist<TH1D>(
            "p_value_yz", "p values from the fitting of the muon track in yz plane", RESIDUAL_BIN_NUM, 0., 1.);
        hist_a_xz_ = histograms.add_hist<TH1D>("a_xz", "slope of the xz plane", PAR_BIN_NUM, 0., SLOPE_MAX);
        set_x_title(hist_a_xz_, "abs(a_xz)");
        set_y_title(hist_a_xz_, fmt::format("counts per {}", SLOPE_MAX / PAR_BIN_NUM));

        hist_b_xz_ = histograms.add_hist<TH1D>("b_xz", "offset of the xz plane", PAR_BIN_NUM, 0., OFFSET_MAX);
        set_x_title(hist_b_xz_, "abs(b_xz)");
        set_y_title(hist_b_xz_, fmt::format("counts per {}", OFFSET_MAX / PAR_BIN_NUM));

        hist_a_yz_ = histograms.add_hist<TH1D>("a_yz", "slope of the yz plane", PAR_BIN_NUM, 0., SLOPE_MAX);
        set_x_title(hist_a_yz_, "abs(a_yz)");
        set_y_title(hist_a_yz_, fmt::format("counts per {}", SLOPE_MAX / PAR_BIN_NUM));

        hist_b_yz_ = histograms.add_hist<TH1D>("b_yz", "offset of the yz plane", PAR_BIN_NUM, 0., OFFSET_MAX);
        set_x_title(hist_b_yz_, "abs(b_yz)");
        set_y_title(hist_b_yz_, fmt::format("counts per {}", OFFSET_MAX / PAR_BIN_NUM));

        barplot_filter_counts_ = histograms.add_hist<TH1L>("filter_counts", "Counts after filters", 1, 0., 0.);

        const auto n_plane = module_size / R3B::Neuland::BarsPerPlane;
        hist_plane_hit_num_ =
            histograms.add_hist<TH1D>("plane_hit_num", "Number of hit planes", n_plane + 1, -0.5, 0.5 + n_plane);
        hist_plane_hit_num_->GetXaxis()->SetTitle("Number of planes hit");
        hist_plane_hit_num_->GetYaxis()->SetTitle("Counts");

        static constexpr auto residual_bin_num = 1000;
        static constexpr auto max_residual = 300.;
        static constexpr auto max_diff = 50;
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
        binary_data_writer_->mille(input_data_buffer_);
        // output_mille_data_point_->push_back(input_data_buffer_);
    }

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

        if (config_.cal_mode == MillepedeCalibrationMode::tsync)
        {
            auto& module_pars = cal_to_hit_par_->GetListOfModuleParRef();
            auto reference_bar_value = module_pars.at(DEFAULT_TSYNC_REFERENCE_BAR_NUM).t_sync.value;
            if (reference_bar_value != DEFAULT_TSYNC_REFERENCE_BAR_VALUE)
            {
                LOGP(info,
                     "The reference bar (bar_num {}) from the input tsync parameter has the wrong value ({}, should be "
                     "{}). Offsetting the input parameters ...",
                     DEFAULT_TSYNC_REFERENCE_BAR_NUM,
                     reference_bar_value,
                     DEFAULT_TSYNC_REFERENCE_BAR_VALUE);
                for (auto& [module_num, module_par] : module_pars)
                {
                    module_par.t_sync.value -= reference_bar_value;
                }
            }
        }
    }

    void MillepedeEngine::add_fit_result_hist(const MilleDataProcessor::FitResult& fit_result)
    {
        hist_p_value_xz_->Fill(fit_result.x_z.p_value);
        hist_p_value_yz_->Fill(fit_result.y_z.p_value);
        hist_a_xz_->Fill(std::abs(fit_result.x_z.slope));
        hist_b_xz_->Fill(std::abs(fit_result.x_z.offset));
        hist_a_yz_->Fill(std::abs(fit_result.y_z.slope));
        hist_b_yz_->Fill(std::abs(fit_result.y_z.offset));
    }

    void MillepedeEngine::init_steer_writer()
    {
        auto steer_writer = SteerWriter{};
        steer_writer.set_working_dir(working_dir_);
        steer_writer.set_filepath(pede_steer_filename_);
        steer_writer.set_parameter_file(input_parameter_filename_);
        steer_writer.set_data_filepath(config_.mille_data_filename);
        static constexpr auto NUMBER_OF_ITERATION = 3.F;
        static constexpr auto CONVERGENCE_RECOGNITION = 0.001F;
        steer_writer.add_method(SteerWriter::Method::inversion,
                                std::make_pair(NUMBER_OF_ITERATION, CONVERGENCE_RECOGNITION));
        steer_writer.add_other_options(std::vector<std::string>{ "hugecut", "50000" });

        if (config_.cal_mode == MillepedeCalibrationMode::tsync)
        {

            steer_writer.add_parameter_default(get_global_label_id(DEFAULT_TSYNC_REFERENCE_BAR_NUM, GlobalLabel::tsync),
                                               std::make_pair(DEFAULT_TSYNC_REFERENCE_BAR_VALUE / SCALE_FACTOR, -1.F));
        }
        else
        {
            steer_writer.add_other_options(std::vector<std::string>{ "outlierdownweighting", "2" });
        }

        if (config_.num_of_threads > 0)
        {
            steer_writer.add_other_options(std::vector<std::string>{
                "threads", fmt::format("{}", config_.num_of_threads), fmt::format("{}", config_.num_of_threads) });
        }
        steer_writer.write();
    }
} // namespace R3B::Neuland::Calibration
