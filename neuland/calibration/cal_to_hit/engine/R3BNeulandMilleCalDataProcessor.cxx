#include "R3BNeulandMilleCalDataProcessor.h"
#include "R3BLogger.h"
#include "R3BNeulandCalData2.h"
#include <Fit/BinData.h>
#include <Math/WrappedMultiTF1.h>
#include <R3BNeulandCommon.h>
#include <TError.h>
#include <algorithm>
#include <fmt/core.h>
#include <numeric>
#include <range/v3/algorithm/find_if.hpp>
#include <vector>

// NOLINTNEXTLINE
#include <fmt/ranges.h>

namespace R3B::Neuland::Calibration
{
    MilleDataProcessor::MilleDataProcessor(int num_of_modules)
    {
        init_data_registers(num_of_modules);
        fitter_.SetFunction(
            ROOT::Math::WrappedMultiTF1{ fit_function_, static_cast<unsigned int>(fit_function_.GetNdim()) }, true);
        fitter_.Config().SetMinimizer("Linear");
    }

    void MilleDataProcessor::init_data_registers(int num_of_modules)
    {
        const auto num_of_planes = num_of_modules / BarsPerPlane;
        for (int plane_id{}; plane_id < num_of_planes; ++plane_id)
        {
            auto data_iter = data_regsiters_.insert_or_assign(plane_id, std::vector<MilleCalData>{}).first;
            data_iter->second.reserve(BarsPerPlane);
        }
    }

    void MilleDataProcessor::reset_fitpars()
    {
        fitter_.Config().ParSettings(0).SetValue(0.);
        fitter_.Config().ParSettings(1).SetValue(0.);
    }

    void MilleDataProcessor::reset()
    {
        for (auto& [plane_id, bar_data] : data_regsiters_)
        {
            bar_data.clear();
        }
        fit_result_.x_z = FitPar{};
        fit_result_.y_z = FitPar{};
        x_z_vals_.clear();
        y_z_vals_.clear();
        reset_fitpars();
    }

    auto MilleDataProcessor::filter(const std::vector<BarCalData>& signals) -> bool
    {
        // fill only the bar_cal_data with only one pmt signal on both sides
        for (const auto& signal : signals)
        {
            if (signal.left.size() == 1 && signal.right.size() == 1)
            {
                const auto bar_num = static_cast<int>(signal.module_num);
                const auto plane_id = ModuleID2PlaneID(bar_num - 1);
                data_regsiters_[plane_id].emplace_back(signal);
            }
        }

        remove_isolated_bar_signal();

        return fit_planes();
    }

    void MilleDataProcessor::remove_isolated_bar_signal()
    {
        namespace rng = ranges;
        for (auto& [plane_id, bar_data] : data_regsiters_)
        {
            if (bar_data.size() < 2)
            {
                continue;
            }

            const auto& bar_signals = bar_data;
            auto check_if_isolated = [&bar_signals](const auto& signal) -> bool
            {
                const auto module_num = static_cast<int>(signal.module_num);
                return (rng::find_if(bar_signals,
                                     [module_num](const auto& bar_signal)
                                     { return bar_signal.module_num == module_num - 1; }) == bar_signals.end()) and
                       (rng::find_if(bar_signals,
                                     [module_num](const auto& bar_signal)
                                     { return bar_signal.module_num == module_num + 1; }) == bar_signals.end());
            };

            bar_data.erase(std::remove_if(bar_data.begin(), bar_data.end(), check_if_isolated), bar_data.end());
        }
    }

    auto MilleDataProcessor::fit_planes() -> bool
    {
        fill_fit_data();

        return fit_plane_data();
    }

    void MilleDataProcessor::fill_fit_data()
    {
        for (auto& [plane_id, bar_data] : data_regsiters_)
        {
            if (bar_data.empty())
            {
                continue;
            }
            const auto is_plane_horizontal = IsPlaneIDHorizontal(plane_id);
            auto& fit_data = is_plane_horizontal ? y_z_vals_ : x_z_vals_;
            const auto z_val = PlaneID2ZPos(plane_id);

            const auto displacement =
                std::accumulate(bar_data.begin(),
                                bar_data.end(),
                                0.,
                                [](double sum, const MilleCalData& signal)
                                { return sum + GetBarVerticalDisplacement(static_cast<int>(signal.module_num)); }) /
                static_cast<double>(bar_data.size());
            fit_data.z_vals.push_back(z_val);
            fit_data.z_errs.push_back(BarSize_Z / 2.);
            fit_data.errs.push_back(BarSize_XY / 2.);
            fit_data.vals.push_back(displacement);
        }
    }

    auto MilleDataProcessor::fit_plane_data() -> bool
    {
        return linear_fit(x_z_vals_, fit_result_.x_z) and linear_fit(y_z_vals_, fit_result_.y_z);
    }

    auto MilleDataProcessor::calculate_residual(double val, int module_num) const -> double
    {
        const auto z_val = ModuleNum2ZPos(module_num);
        const auto is_plane_horizontal = IsPlaneIDHorizontal(ModuleID2PlaneID(module_num - 1));
        const auto& fit_result = is_plane_horizontal ? fit_result_.x_z : fit_result_.y_z;
        const auto diff = val - (fit_result.slope * z_val) - fit_result.offset;
        return diff * diff;
    }

    auto MilleDataProcessor::linear_fit(const FitData& data, FitPar& fit_par) -> bool
    {

        const auto bin_data = ROOT::Fit::BinData{ static_cast<unsigned int>(data.size()),
                                                  data.z_vals.data(),
                                                  data.vals.data(),
                                                  data.z_errs.data(),
                                                  data.errs.data() };
        reset_fitpars();

        // disable annoying root printouts
        auto old_var = gErrorIgnoreLevel;
        gErrorIgnoreLevel = kFatal;
        auto res = fitter_.Fit(bin_data);
        gErrorIgnoreLevel = old_var;

        if (not res)
        {
            R3BLOG(debug, "Linear fitting on x_z data failed");
            return false;
        }
        fit_par.slope = fitter_.Result().Parameter(0);
        fit_par.offset = fitter_.Result().Parameter(1);
        if (fitter_.Result().Prob() < p_value_cut_)
        {
            R3BLOG(debug,
                   fmt::format("p-value ({}) is too small from the fit.\n\
                       x = np.array({}) \n\
                       x_err = np.array({}) \n\
                       y = np.array({}) \n\
                       y_err = np.array({})",
                               fitter_.Result().Prob(),
                               data.z_vals,
                               data.z_errs,
                               data.vals,
                               data.errs));
            return false;
        }
        return true;
    }
} // namespace R3B::Neuland::Calibration
