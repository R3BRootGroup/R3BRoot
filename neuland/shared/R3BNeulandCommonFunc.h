#pragma once

#include <R3BException.h>
#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <fmt/format.h>
#include <string>
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
} // namespace R3B::Neuland
