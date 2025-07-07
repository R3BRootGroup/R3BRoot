#pragma once

#include <R3BException.h>
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
