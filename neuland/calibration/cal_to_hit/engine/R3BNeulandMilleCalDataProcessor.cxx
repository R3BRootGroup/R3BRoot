#include "R3BNeulandMilleCalDataProcessor.h"
#include "R3BHuberRegression.h"
#include "R3BNeulandCalData2.h"
#include "R3BNeulandCalToHitPar.h"
#include "R3BNeulandCommonFunc.h"
#include <Fit/BinData.h>
#include <Math/WrappedMultiTF1.h>
#include <R3BNeulandCommon.h>
#include <TError.h>
#include <fairlogger/Logger.h>
#include <fmt/core.h>
#include <functional>
#include <numeric>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#ifdef HAS_CPP_STANDARD_23
#include <algorithm>
#include <ranges>
namespace stdrng = std::ranges;
#else
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/min_element.hpp>
using namespace stdrng = ranges
#endif

namespace R3B::Neuland::Calibration
{
    using DataBufferType = std::unordered_map<int, std::vector<MilleCalData>>;
    MilleDataProcessor::MilleDataProcessor(int num_of_modules)
    {
        init_data_registers(num_of_modules);
        auto& config = huber_regressor_.get_config_ref();
        config.weight.init = 1.;
        config.weight.learning_rate = 0.1;
        config.bias.init = 0.;
        config.bias.learning_rate = 10.;
    }

    void MilleDataProcessor::init_data_registers(int num_of_modules)
    {
        const auto num_of_planes = num_of_modules / BarsPerPlane;
        for (int plane_id{}; plane_id < num_of_planes; ++plane_id)
        {
            auto data_iter = data_buffers_.insert_or_assign(plane_id, std::vector<MilleCalData>{}).first;
            data_iter->second.reserve(BarsPerPlane);
        }
    }

    void MilleDataProcessor::reset_fit_pars() { huber_regressor_.reset_parameters(); }

    void MilleDataProcessor::reset()
    {
        for (auto& [plane_id, bar_data] : data_buffers_)
        {
            bar_data.clear();
        }
        track_info_ = TrackInfo{};
        track_fit_data_.clear();
        reset_fit_pars();
    }

    auto MilleDataProcessor::process(const std::vector<BarCalData>& signals, const Cal2HitPar& hit_par) -> bool
    {
        // fill only the bar_cal_data with only one pmt signal on both sides
        for (const auto& signal : signals)
        {
            if (signal.left.size() == 1 && signal.right.size() == 1)
            {
                const auto module_num = static_cast<int>(signal.module_num);
                const auto plane_id = ModuleID2PlaneID(module_num - 1);
                data_buffers_[plane_id].emplace_back(signal);
            }
        }

        remove_isolated_bar_signal();

        return fit_planes(hit_par);
    }

    void MilleDataProcessor::remove_isolated_bar_signal()
    {
        for (auto& [plane_id, bar_data] : data_buffers_)
        {
            if (bar_data.size() < 2)
            {
                continue;
            }

            const auto& bar_signals = bar_data;
            auto check_if_isolated = [&bar_signals](const auto& signal) -> bool
            {
                const auto module_num = static_cast<int>(signal.module_num);
                return (stdrng::find_if(bar_signals,
                                        [module_num](const auto& bar_signal) -> auto
                                        { return bar_signal.module_num == module_num - 1; }) == bar_signals.end()) and
                       (stdrng::find_if(bar_signals,
                                        [module_num](const auto& bar_signal) -> auto
                                        { return bar_signal.module_num == module_num + 1; }) == bar_signals.end());
            };

#if CPP_STANDARD < 20
            bar_data.erase(std::remove_if(bar_data.begin(), bar_data.end(), check_if_isolated), bar_data.end());
#else
            std::erase_if(bar_data, check_if_isolated);
#endif
        }
    }

    namespace
    {
        using TrackInfo = MilleDataProcessor::TrackInfo;

        // auto calculate_residual(const TrackInfo& track_info, double val, int module_num) -> float
        // {
        //     const auto z_val = ModuleNum2ZPos(module_num);
        //     const auto is_plane_horizontal = IsPlaneIDHorizontal(ModuleID2PlaneID(module_num - 1));
        //     const auto& bar_disp_info = track_info.bar_disp_data;
        //     const auto& fit_result = is_plane_horizontal ? bar_disp_info.x_z : bar_disp_info.y_z;
        //     const auto diff = val - (fit_result.slope * z_val) - fit_result.offset;
        //     return static_cast<float>(diff * diff);
        // }

        constexpr auto calculate_diff(const TrackFitPar& fit_result, double val, int module_num) -> float
        {
            const auto z_val = ModuleNum2ZPos(module_num);
            return static_cast<float>(val - (fit_result.slope * z_val) - fit_result.offset);
        }

        constexpr auto calculate_residual(const TrackFitPar& fit_result, double val, int module_num) -> float
        {
            const auto diff = calculate_diff(fit_result, val, module_num);
            return static_cast<float>(diff * diff);
        }

        constexpr auto check_outlier_exist(const DataBufferType& data_buffer) -> bool
        {
            for (const auto& [plane_num, bars_data] : data_buffer)
            {
                if (std::ranges::any_of(bars_data, &MilleCalData::is_outlier))
                {
                    return true;
                }
            }
            return false;
        }

        constexpr void remove_outliers(DataBufferType& data_buffer)
        {
            for (auto& [plane_num, bars_data] : data_buffer)
            {
                std::erase_if(bars_data, [](const auto& bar_data) -> bool { return bar_data.is_outlier; });
            }
        }

    } // namespace

    auto MilleDataProcessor::fit_planes(const Cal2HitPar& hit_par) -> bool
    {
        auto is_ok = true;
        fill_bar_disp_dataset();
        LOGP(debug, "Fitting the data of bar displacements ... ");
        while (not std::ranges::all_of(data_buffers_ | std::views::values,
                                       [](const auto& bars) -> bool { return bars.empty(); }))
        {
            remove_outliers(data_buffers_);
            is_ok &=
                (linear_fit(track_fit_data_.bar_x_z, track_info_.bar_disp_data.x_z, huber_regressor_, p_value_cut_) and
                 linear_fit(track_fit_data_.bar_y_z, track_info_.bar_disp_data.y_z, huber_regressor_, p_value_cut_));
            if (not is_ok)
            {
                return false;
            }
            set_data_buffers(data_buffers_, track_info_, hit_par);
            if (not check_outlier_exist(data_buffers_))
            {
                break;
            }
        }
        fill_time_dataset();
        LOGP(debug, "Fitting the data of time-derived positions ... ");
        if (fair::Logger::GetConsoleSeverity() <= fair::Severity::debug)
        {
            check_fit_result();
        }
        return is_ok;
    }

    auto MilleDataProcessor::check_is_outlier(int module_num) const -> bool
    {
        const auto plane_id = ModuleID2PlaneID(module_num - 1);
        const auto is_horizontal = IsPlaneIDHorizontal(plane_id);

        const auto& fit_par = is_horizontal ? track_info_.bar_disp_data.y_z : track_info_.bar_disp_data.x_z;
        const auto pos_z = PlaneID2ZPos(plane_id);
        const auto pos_disp = GetBarVerticalDisplacement(module_num);
        return huber_regressor_.check_outlier(pos_z, pos_disp, std::make_pair(fit_par.slope, fit_par.offset)).first;
    }

    void MilleDataProcessor::check_fit_result()
    {
        LOGP(debug,
             "Bar data size: {{x_z: {}, y_z: {}}}, track data size: {{x_z: {}, y_z: {}}}",
             track_fit_data_.bar_x_z.size(),
             track_fit_data_.bar_y_z.size(),
             track_fit_data_.time_data_x_z.size(),
             track_fit_data_.time_data_y_z.size());

        LOGP(debug,
             "Fit result:\n"
             "bar displacements: {}\n"
             "positions: slope: {}",
             track_info_.bar_disp_data,
             track_info_.time_data);

        if (track_info_.bar_disp_data.x_z != track_info_.time_data.x_z)
        {
            LOGP(debug, "x_z plane fit result doesn't match!");
            LOGP(debug, "x_z plane bar fit data:\n{}", track_fit_data_.bar_x_z);
            LOGP(debug, "x_z plane time fit data:\n{}", track_fit_data_.time_data_x_z);
        }

        if (track_info_.bar_disp_data.y_z != track_info_.time_data.y_z)
        {
            LOGP(debug, "y_z plane fit result doesn't match!");
            LOGP(debug, "y_z plane bar fit data:\n{}", track_fit_data_.bar_y_z);
            LOGP(debug, "y_z plane time fit data:\n{}", track_fit_data_.time_data_y_z);
        }
    }

    void MilleDataProcessor::fill_bar_disp_dataset()
    {
        for (auto& [plane_id, plane_data] : data_buffers_)
        {
            if (plane_data.empty())
            {
                continue;
            }
            const auto is_plane_horizontal = IsPlaneIDHorizontal(plane_id);
            auto& bar_disp_fit_data = is_plane_horizontal ? track_fit_data_.bar_y_z : track_fit_data_.bar_x_z;
            const auto bar_z_val = PlaneID2ZPos(plane_id);

            const auto bar_disp =
                std::accumulate(plane_data.begin(),
                                plane_data.end(),
                                0.,
                                [](double sum, const MilleCalData& signal) -> double
                                { return sum + GetBarVerticalDisplacement(static_cast<int>(signal.module_num)); }) /
                static_cast<double>(plane_data.size());
            bar_disp_fit_data.z_vals.push_back(bar_z_val);
            bar_disp_fit_data.z_errs.push_back(BarSize_Z / 2.);
            bar_disp_fit_data.errs.push_back(BarSize_XY / 2.);
            bar_disp_fit_data.vals.push_back(bar_disp);
        }
    }

    void MilleDataProcessor::fill_time_dataset()
    {
        for (auto& [plane_id, plane_data] : data_buffers_)
        {
            if (plane_data.empty())
            {
                continue;
            }
            const auto is_plane_horizontal = IsPlaneIDHorizontal(plane_id);
            auto& bar_time_data = is_plane_horizontal ? track_fit_data_.time_data_x_z : track_fit_data_.time_data_y_z;
            const auto bar_z_val = PlaneID2ZPos(plane_id);

            auto iter = stdrng::min_element(plane_data, std::less{}, &MilleCalData::residual);
            if (iter == plane_data.end())
            {
                continue;
            }

            bar_time_data.z_vals.push_back(bar_z_val);
            bar_time_data.z_errs.push_back(BarSize_Z / 2.);
            bar_time_data.errs.push_back(iter->position.error);
            bar_time_data.vals.push_back(iter->position.value);
        }
    }

    void MilleDataProcessor::set_data_buffers(DataBufferType& data_buffers,
                                              const TrackInfo& track_info,
                                              const Cal2HitPar& hit_par) const
    {
        for (auto& [plane_id, plane_data] : data_buffers)
        {
            for (auto& signal : plane_data)
            {
                const auto t_diff = (signal.right.leading_time - signal.right.trigger_time) -
                                    (signal.left.leading_time - signal.left.trigger_time);
                const auto& module_par = hit_par.GetModuleParAt(signal.module_num);
                const auto position_along_bar = (-t_diff + module_par.t_diff) / 2 * module_par.effective_speed;
                const auto position_vert_bar = Common::GetBarVerticalDisplacement(signal.module_num);
                signal.position = position_along_bar;
                const auto& bar_disp_info = track_info.bar_disp_data;

                const auto& [fit_result, fit_result_bar] = IsPlaneIDHorizontal(ModuleID2PlaneID(signal.module_num - 1))
                                                               ? std::tie(bar_disp_info.x_z, bar_disp_info.y_z)
                                                               : std::tie(bar_disp_info.y_z, bar_disp_info.x_z);
                signal.fit_diff = calculate_diff(fit_result, position_along_bar.value, signal.module_num);
                signal.residual = calculate_residual(fit_result, position_along_bar.value, signal.module_num);
                signal.residual_bar_pos = calculate_residual(fit_result_bar, position_vert_bar, signal.module_num);
                signal.is_outlier = check_is_outlier(signal.module_num);
            }
        }
    }

    auto MilleDataProcessor::linear_fit(const NeulandTrackDataSet& data,
                                        FitPar& fit_par,
                                        HuberRegressor& huber_regressor,
                                        double p_value_cut) -> bool
    {

        // // NOTE: ROOT::FIT::BinData only contains the references to the raw data.
        // const auto bin_data = ROOT::Fit::BinData{ static_cast<unsigned int>(data.size()),
        //                                           data.z_vals.data(),
        //                                           data.vals.data(),
        //                                           data.z_errs.data(),
        //                                           data.errs.data() };

        huber_regressor.reset_parameters();

        LOGP(debug, "fitting the data with x: {} and y: {}", data.z_vals, data.vals);
        auto is_ok = huber_regressor.train_from_data(data.z_vals, data.vals);

        const auto& result = huber_regressor.get_result();
        LOGP(debug, "Fitting result: {}", result);

        if (not is_ok)
        {
            LOGP(debug, "Huber regression minimization failed!");
            return false;
        }

        // INFO: scale the error by 2 such that p_value isn't too small.
        fit_par.p_value = huber_regressor.calculate_p_value(data.errs, 2.);
        fit_par.slope = result.weight.value;
        fit_par.offset = result.bias.value;
        if (fit_par.p_value < p_value_cut)
        {
            LOGP(
                debug, "p-value ({}) is too small from the fit. Must be larger than {}.", fit_par.p_value, p_value_cut);
            LOGP(debug, "fit data: \n {}", data);
            return false;
        }
        return true;

        // // disable annoying root printouts
        // auto old_var = gErrorIgnoreLevel;
        // gErrorIgnoreLevel = kFatal;
        // auto res = fitter_.Fit(bin_data);
        // gErrorIgnoreLevel = old_var;

        // if (not res)
        // {
        //     LOGP(debug, "Linear fitting on x_z data failed");
        //     return false;
        // }
        // fit_par.slope = fitter_.Result().Parameter(0);
        // fit_par.offset = fitter_.Result().Parameter(1);
        // fit_par.p_value = fitter_.Result().Prob();
        // if (fit_par.p_value < p_value_cut_)
        // {
        //     LOGP(debug,
        //          "p-value ({}) is too small from the fit. Must be larger than {}.",
        //          fitter_.Result().Prob(),
        //          p_value_cut_);
        //     LOGP(debug, "fit data: \n {}", data);
        //     return false;
        // }
        // return true;
    }
} // namespace R3B::Neuland::Calibration
