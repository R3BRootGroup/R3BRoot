#include "ParResultReader.h"
#include <fmt/base.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fstream>
#include <glaze/core/context.hpp>
#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
#include <ios>
#include <optional>
#include <range/v3/view/map.hpp>
#include <string>
#include <string_view>
#include <unordered_map>
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
            result.pre_sigma = std::stof(std::string{ splits_view[2] });
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

        if (mode_ == Mode::res)
        {
            read_pede_par_file();
        }
        else
        {
            read_json_file();
        }
    }

    void ResultReader::read_pede_par_file()
    {

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
        else
        {
            fmt::println(stderr, "ERROR: parameter file {:?} cannot be read", filename_);
        }
    }

    void ResultReader::read_json_file()
    {

        auto obj = std::unordered_map<int, JsonParResultEntry>{};
        auto buffer = std::string{};
        auto error_code = glz::read_file_json(obj, filename_, buffer);
        if (error_code == glz::error_code::file_open_failure)
        {
            fmt::println(stderr, "Parameter JSON file {:?} doesn't exist!", filename_);
        }
        else if (error_code)
        {
            fmt::println(stderr, "{}", glz::format_error(error_code, buffer));
        }

        for (const auto& [key, value] : obj)
        {
            par_results_.emplace(value.label,
                                 ParResultEntry{ .par_num = value.label,
                                                 .value = value.correction,
                                                 .pre_sigma = 0.,
                                                 .value_diff = value.correction,
                                                 .error = value.error });
        }
    }
    void ResultReader::print() { fmt::print("{}\n", fmt::join(par_results_ | rng::views::values, "\n")); }

} // namespace R3B::Millepede
