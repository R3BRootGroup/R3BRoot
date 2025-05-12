/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BUcesbLauncher.h"
#include <R3BException.h>
#include <R3BLogger.h>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/process/v2/process.hpp>
#include <boost/process/v2/stdio.hpp>
#include <chrono>
#include <cstddef>
#include <fairlogger/Logger.h>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/os.h>
#include <memory>
#include <regex>

#include <ext_data_clnt.hh>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

constexpr auto CHILD_CLOSE_WAITING_TIME = std::chrono::seconds(5);

namespace fs = std::filesystem;
namespace
{
    struct ResolveResult
    {
        std::string executable;
        std::vector<std::string> options;
        std::vector<std::string> lmds;
        std::vector<std::string> others;
    };

    bool Check_exist(std::string_view exe)
    {
        auto exe_path = fs::path{ exe };
        return fs::exists(exe_path) || fs::is_symlink(exe_path);
    }

    // TODO: C++23 insert range
    template <typename T>
    void Append_elements(std::vector<T>& base, std::vector<T> to_append)
    {
        base.reserve(base.size() + to_append.size());
        for (auto& ele : to_append)
        {
            base.emplace_back(std::move(ele));
        }
    }

    auto get_regex_filelist(std::string filename_regex) -> std::vector<std::string>
    {
        auto wildcard_path = fs::path{ filename_regex };
        auto parent_folder = wildcard_path.parent_path();
        if (not fs::exists(parent_folder))
        {
            R3BLOG(error, fmt::format("Cannot get the parent folder of the regex path {}", filename_regex));
            return {};
        }
        // const auto regex_string = std::regex_replace(wildcard_path.filename().string(), std::regex{ "\\*" }, ".*");
        const auto regex_string = wildcard_path.filename().string();
        auto filelist = std::vector<std::string>{};
        for (const auto& dir_entry : fs::directory_iterator(parent_folder))
        {
            if (std::regex_match(dir_entry.path().filename().string(), std::regex{ regex_string }))
            {
                filelist.emplace_back(fs::absolute(dir_entry.path()));
            }
        }
        if (filelist.empty())
        {
            R3BLOG(error, fmt::format(R"(Cannot find any files with regex "{}")", regex_string));
        }
        return filelist;
    }

    void Append_lmds(std::vector<std::string>& lmds, std::string filename_regex)
    {
        // expand filenames on regex
        Append_elements(lmds, get_regex_filelist(std::move(filename_regex)));
    }

    auto parse_splits(std::vector<std::string> splits) -> ResolveResult
    {
        auto result = ResolveResult{};

        auto option_regex = std::regex{ "^--[0-9A-z,=\\-]+" };
        auto lmd_regex = std::regex{ "^.*\\.lmd$" };

        for (auto& split_item : splits)
        {
            if (std::regex_match(split_item, option_regex))
            {
                result.options.emplace_back(std::move(split_item));
            }
            else if (std::regex_match(split_item, lmd_regex))
            {
                Append_lmds(result.lmds, std::move(split_item));
            }
            else if (Check_exist(split_item))
            {
                // it must be an executable then
                if (not result.executable.empty())
                {
                    R3BLOG(info, fmt::format("Ucesb Executable has been set to \"{}\" ", result.executable));
                    R3BLOG(error, fmt::format("Found another executable \"{}\" but only one is allowed!", split_item));
                    continue;
                }
                result.executable = std::move(split_item);
            }
            else
            {
                // In other cases, let ucesb deal with this
                result.others.emplace_back(std::move(split_item));
            }
        }
        return result;
    }

    void lmd_filenames_sorting(std::vector<std::string>& filenames)
    {
        // simple lexicographically sorting
        std::sort(filenames.begin(), filenames.end());
    }

    auto resolve_exe_options_lmd(std::string cmd) -> ResolveResult
    {
        if (cmd.empty())
        {
            throw R3B::logic_error("Ucesb command string is empty!");
        }
        R3BLOG(debug, fmt::format("Resolving string command: {}", cmd));
        boost::trim(cmd);
        auto splits = std::vector<std::string>{};
        boost::split(splits, cmd, boost::is_any_of(" "), boost::token_compress_on);
        if (splits.empty())
        {
            throw R3B::runtime_error(fmt::format("Get 0 element from splitting string {}", cmd));
        }
        auto results = parse_splits(std::move(splits));
        lmd_filenames_sorting(results.lmds);

        return results;
    }
} // namespace

namespace R3B
{
    void UcesbServerLauncher::Launch(std::string command_string)
    {
        auto launch_strings = resolve_exe_options_lmd(std::move(command_string));
        auto launch_args = std::vector<std::string>{};
        Append_elements(launch_args, std::move(launch_strings.options));
        Append_elements(launch_args, std::move(launch_strings.lmds));
        Append_elements(launch_args, std::move(launch_strings.others));

        R3BLOG(debug,
               fmt::format("Ucesb command after resolving wildcard filename: \n {} {}",
                           launch_strings.executable,
                           fmt::join(launch_args, " ")));

        ucesb_server_ =
            std::make_unique<bpv2::process>(ios_,
                                            launch_strings.executable,
                                            launch_args,
                                            bpv2::process_stdio{ .in = nullptr, .out = server_pipe_, .err = stdout });
        if (auto is_status_ok = client_->connect(server_pipe_.native_handle()); not is_status_ok)
        {
            R3BLOG(error, "ext_data_clnt::connect() failed");
            const auto* msg = (client_->last_error() == nullptr) ? UCESB_NULL_STR_MSG : client_->last_error();
            throw R3B::runtime_error(fmt::format("UCESB error: {}", msg));
        }
    }

    void UcesbServerLauncher::Setup(ext_data_struct_info& struct_info, size_t event_struct_size) {}

    void UcesbServerLauncher::Close()
    {
        if (auto ret_val = client_->close(); ret_val != 0)
        {
            throw R3B::runtime_error("ext_data_clnt::close() failed");
        }
        auto err_code = std::error_code{};
        ucesb_server_->async_wait(
            [](const std::error_code& err, int ret)
            {
                if (err)
                {
                    LOGP(error, "Error occured from ucesb server. Error message: {}", err.message());
                }
                LOGP(info, "Ucesb server is closed successfully with the return value: {}", ret);
            });
        ios_.run_for(CHILD_CLOSE_WAITING_TIME);
    }
} // namespace R3B
