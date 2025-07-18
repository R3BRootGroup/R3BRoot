#pragma once

#include "R3BNeulandCommon.h"
#include <R3BException.h>
#include <Rtypes.h>
#include <algorithm>
#include <array>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <fairlogger/Logger.h>
#include <fmt/format.h>
#include <root/TH1.h>
#include <string>
#include <utility>
#include <vector>

namespace R3B::Neuland
{

    class Common
    {
      public:
        static constexpr auto GetBarVerticalDisplacement(int module_num) -> double
        {
            const auto bar_num = module_num % BarsPerPlane;
            return (2 * bar_num - 1 - BarsPerPlane) / 2. * BarSize_XY;
        }
        static constexpr auto IsPlaneIDHorizontal(int plane_id) -> bool
        {
            return (plane_id % 2 == FirstHorizontalPlane);
        }
        static constexpr auto IsPlaneIDVertical(int plane_id) -> bool { return !IsPlaneIDHorizontal(plane_id); }
        static constexpr auto ModuleID2PlaneID(int moduleID) -> int { return moduleID / BarsPerPlane; }
        static constexpr auto ModuleID2PlaneNum(int moduleID) -> int { return ModuleID2PlaneID(moduleID) + 1; }
        static constexpr auto IsModuleNumHorizontal(int module_num) -> bool
        {
            return IsPlaneIDHorizontal(ModuleID2PlaneID(module_num - 1));
        }
        // planeNum, barNum and ModuleNum is 1-based
        static constexpr auto Neuland_PlaneBar2ModuleNum(int planeNum, int barNum) -> int
        {
            return ((planeNum - 1) * BarsPerPlane) + barNum;
        }
        template <typename T = double>
        static constexpr auto PlaneID2ZPos(int plane_id) -> T
        {
            return static_cast<T>((plane_id + 0.5) * BarSize_Z);
        }
        template <typename T = double>
        static constexpr auto ModuleNum2ZPos(int module_num) -> T
        {
            return PlaneID2ZPos<T>(ModuleID2PlaneID(module_num - 1));
        }
        ClassDefNV(Common, 1);
    };

    template <typename Option>
    void parse_io_branch_names(const Option& option,
                               std::vector<std::string>& read,
                               int read_num,
                               std::vector<std::string>& write,
                               int write_num)
    {
        LOGP(info, "Task {:?} is enabled", option.name);
        auto resolve_branch_names = [](const std::string& input, std::vector<std::string>& output)
        {
            output.clear();
            boost::split(output, input, boost::is_any_of(";"));
            // trim the empty spaces
            std::for_each(output.begin(), output.end(), [](auto& name) { boost::trim(name); });
            // remove empty names
            output.erase(std::remove(output.begin(), output.end(), ""), output.end());
        };

        resolve_branch_names(option.read, read);
        if (read.size() != read_num)
        {
            throw R3B::logic_error(fmt::format(
                "Task {:?} requires {} read branch(es) but only received {} branch(es)! Parsed string: {:?}",
                option.name,
                read_num,
                read.size(),
                option.read));
        }
        resolve_branch_names(option.write, write);
        if (write.size() != write_num)
        {
            throw R3B::logic_error(fmt::format(
                "Task {:?} requires {} write branch(es) but only received {} branch(es)! Parsed string: {:?}",
                option.name,
                read_num,
                write.size(),
                option.write));
        }
    }

    inline auto calculate_cdf(TH1* histogram) -> TH1*
    {
        histogram->Scale(1 / histogram->GetEntries());
        auto* cumulative = histogram->GetCumulative();
        histogram->Scale(histogram->GetEntries());
        return cumulative;
    }

    inline auto calculate_hist_quantiles(TH1* histogram, double quantile_ratio) -> std::array<double, 2>
    {
        static constexpr auto QUANTILES_NUM = 2;
        const auto quantiles =
            std::array<double, QUANTILES_NUM>{ 0.5 - (quantile_ratio / 2.), 0.5 + (quantile_ratio / 2.) };
        auto x_pos = std::array<double, QUANTILES_NUM>{};
        histogram->GetQuantiles(QUANTILES_NUM, x_pos.data(), quantiles.data());
        return x_pos;
    }

    inline auto calculate_CDF_with_quantiles(TH1* histogram, double ratio) -> std::pair<TH1*, std::array<double, 2>>
    {
        if (histogram == nullptr)
        {
            return {};
        }
        return { calculate_cdf(histogram), calculate_hist_quantiles(histogram, ratio) };
    }

} // namespace R3B::Neuland
