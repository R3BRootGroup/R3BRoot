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
#pragma once

#include "SteerWriter.h"
#include <boost/asio/io_context.hpp>
#include <string>
#include <string_view>

namespace R3B::Millepede
{
    constexpr auto DEFAULT_RES_FILENAME = "millepede.res";

    class Launcher
    {
      public:
        Launcher() = default;
        void set_steer_filename(std::string_view filename) { steer_filename_ = filename; }
        void set_parameter_filename(std::string_view filename) { parameter_file_ = filename; }
        void set_binary_dir(std::string_view dir) { binary_directory_ = dir; }
        void set_working_dir(std::string_view dir) { working_directory_ = dir; }
        void launch();
        void end();

      private:
        boost::asio::io_context ios_;
        std::string steer_filename_ = DEFAULT_STEER_FILENAME;
        std::string parameter_file_;
        std::string binary_directory_ = ".";
        std::string working_directory_ = ".";
        std::string executable_ = "pede";
    };
} // namespace R3B::Millepede
