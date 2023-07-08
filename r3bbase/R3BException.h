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
