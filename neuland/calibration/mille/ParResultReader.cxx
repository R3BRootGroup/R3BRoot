#include "ParResultReader.h"
#include <fmt/ranges.h>
#include <fstream>
#include <optional>
#include <range/v3/view.hpp>
#include <vector>

namespace rng = ranges;

namespace
{
    using R3B::Millepede::ParResultEntry;
    auto split_string_view(std::string_view str, std::string_view del) -> std::vector<std::string_view>
    {
        auto splits = std::vector<std::string_view>{};

        for (auto sub_begin = str.find_first_not_of(del); sub_begin != std::string_view::npos;
             sub_begin = str.find_first_not_of(del, sub_begin))
        {
            auto sub_end = str.find_first_of(del, sub_begin);
            sub_end = (sub_end == std::string_view::npos) ? str.size() : sub_end;
            auto sub_str = str.substr(sub_begin, sub_end - sub_begin);
            splits.push_back(sub_str);
            sub_begin = sub_end;
        }
        return splits;
    }

    auto parse_parameter(std::string_view line_string) -> std::optional<ParResultEntry>
    {
        auto splits_view = split_string_view(line_string, " ");
        auto result = ParResultEntry{};

        constexpr auto max_size = 5;
        constexpr auto min_size = 3;
        if (splits_view.size() >= min_size)
        {
            result.par_num = std::stoi(std::string{ splits_view.front() });
            result.value = std::stof(std::string{ splits_view[1] });
            result.sigma = std::stof(std::string{ splits_view[2] });
        }
        else
        {
            return {};
        }

        if (splits_view.size() == max_size)
        {
            result.value_diff = std::stof(std::string{ splits_view[3] });
            result.error = std::stof(std::string{ splits_view.back() });
        }
        else if (splits_view.size() != min_size)
        {
            return {};
        }

        return result;
    }

} // namespace

namespace R3B::Millepede
{

    void ResultReader::read()
    {
        par_results_.clear();
        auto ifile = std::ifstream(filename_, std::ios_base::in);
        auto buffer = std::string{};
        if (ifile.is_open())
        {
            std::getline(ifile, buffer);
            while (std::getline(ifile, buffer))
            {
                auto result = parse_parameter(buffer);
                if (result.has_value())
                {
                    par_results_.emplace(result->par_num, result.value());
                }
            }
        }
    }
    void ResultReader::print() { fmt::print("{}\n", fmt::join(par_results_ | rng::views::values, "\n")); }

} // namespace R3B::Millepede
