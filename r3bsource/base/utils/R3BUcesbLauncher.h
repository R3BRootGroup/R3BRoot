#pragma once

/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include <boost/asio/io_context.hpp>
#include <boost/asio/readable_pipe.hpp>
#include <boost/process/v2/process.hpp>
#include <ext_data_clnt.hh>
#include <memory>
#include <string>
#include <vector>

constexpr auto UCESB_NULL_STR_MSG = "Can't retrieve error message as last_error returns nullptr!";

namespace bpv2 = boost::process::v2;

namespace R3B
{
    class UcesbServerLauncher
    {
      public:
        struct ResolveResult
        {
            std::string executable;
            std::vector<std::string> options;
            std::vector<std::string> lmds;
            std::vector<std::string> others;
        };

        explicit UcesbServerLauncher(ext_data_clnt* client)
            : client_{ client }
        {
        }
        void Launch();
        // void Setup(ext_data_struct_info& struct_info, size_t event_struct_size);
        void SetLaunchCmd(const std::string& command_string);
        void Close();

      private:
        ext_data_clnt* client_ = nullptr;
        std::unique_ptr<bpv2::process> ucesb_server_;
        ResolveResult launch_strings_{};
        std::vector<std::string> launch_args;
        boost::asio::io_context ios_;
        boost::asio::readable_pipe server_pipe_{ ios_ };
    };

} // namespace R3B
