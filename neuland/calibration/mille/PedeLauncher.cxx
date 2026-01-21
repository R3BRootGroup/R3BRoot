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

#include "PedeLauncher.h"
#include <boost/process/search_path.hpp>
#include <boost/process/v2/process.hpp>
#include <boost/process/v2/start_dir.hpp>
#include <boost/process/v2/stdio.hpp>
#include <exception>
#include <filesystem>
#include <fmt/base.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h> // NOLINT
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

namespace bp = boost::process;
namespace fs = std::filesystem;
namespace bpv2 = boost::process::v2;

namespace R3B::Millepede
{
    void Launcher::launch()
    {
#ifndef WITH_MILLEPEDE
        throw std::runtime_error(
            "Program is not compiled with with millepede2. Please enable \"WITH_MILLEPEDE=ON\" with CMake.");
#endif
        fs::create_directories(fs::path{ working_directory_ });
        const auto exe_string = fmt::format("{}/{}", binary_directory_, executable_);
        const auto steer_filepath = fs::absolute(fs::path{ working_directory_ } / fs::path{ steer_filename_ });
        auto launch_args = std::vector<std::string>{ "-i", steer_filepath.string() };
        auto exe_path = std::filesystem::path(exe_string);
        if (not std::filesystem::exists(exe_path))
        {
            throw std::runtime_error(fmt::format("Command {:?} cannot be found or executed!", exe_string));
        }

        try
        {
            auto pede_program = bpv2::process{ ios_,
                                               exe_path,
                                               launch_args,
                                               bpv2::process_start_dir{ working_directory_ },
                                               bpv2::process_stdio{ .in = nullptr, .out = stdout, .err = stderr } };
            pede_program.async_wait(
                [](const std::error_code& err, int ret)
                {
                    if (err)
                    {
                        fmt::println("Error occurred from the pede program. Error message: {}", err.message());
                    }
                    fmt::println("The pede program is closed successfully with the return value: {}", ret);
                });
            ios_.run();
            ios_.reset();
        }
        catch (std::exception& ex)
        {
            throw std::runtime_error(fmt::format("Error occurred when launching pede with \"{} {}\": \n {}",
                                                 bp::search_path(executable_).string(),
                                                 fmt::join(launch_args, " "),
                                                 ex.what()));
        }
    }

    void Launcher::end()
    {
        auto old_result_path = fs::path{ working_directory_ } / fs::path{ DEFAULT_RES_FILENAME };
        // auto new_result_path = fs::path{ working_directory_ } / fs::path{ parameter_file_ };

        if (fs::exists(old_result_path))
        {
            // fmt::println(
            //     "Info: copy the file {:?} to the file {:?}!", old_result_path.string(), new_result_path.string());
            // fs::copy(old_result_path, new_result_path, fs::copy_options::overwrite_existing);
        }
        else
        {
            fmt::println("Error: millepede.res doesn't exist!");
        }
    }

} // namespace R3B::Millepede
