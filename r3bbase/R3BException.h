/******************************************************************************
 *   Copyright (C) 2023 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2023-2026 Members of R3B Collaboration                     *
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

// TODO: C++20
// use std::source_location
#include <boost/assert/source_location.hpp>
#include <stdexcept>
#include <string>

namespace R3B
{

    class runtime_error : public std::runtime_error
    {
      public:
        explicit runtime_error(const std::string& err, const boost::source_location& loc = BOOST_CURRENT_LOCATION);
    };

    class logic_error : public std::logic_error
    {
      public:
        explicit logic_error(const std::string& err, const boost::source_location& loc = BOOST_CURRENT_LOCATION);
    };
} // namespace R3B
