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

#include "SteerWriter.h"
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fstream>

namespace R3B
{
    void SteerWriter::add_parameter_default(int par_num, const std::pair<float, float>& values)
    {
        if (auto iter = parameter_defaults_.find(par_num); iter != parameter_defaults_.end())
        {
            throw std::logic_error(fmt::format(
                "Default value of parameter {} has been already set to {}!", iter->second.first, iter->second.second));
        }
        parameter_defaults_.emplace(par_num, values);
    }

    void SteerWriter::add_method(Method method, const std::pair<float, float>& values)
    {
        if (auto iter = methods_.find(method); iter != methods_.end())
        {
            throw std::logic_error(fmt::format("Method {} has been already set to {}!", iter->first, iter->second));
        }
        methods_.emplace(method, values);
    }

    void SteerWriter::add_other_options(std::vector<std::string> options)
    {
        other_options_.push_back(std::move(options));
    }

    void SteerWriter::write()
    {
        auto ofile = std::ofstream{ filepath_, std::ios_base::out | std::ios_base::trunc };

        if (not ofile.is_open())
        {
            throw std::runtime_error(fmt::format("Failed to open file {}", filepath_));
        }

        write_data_file(ofile);
        write_parameter_defaults(ofile);
        write_methods(ofile);
        write_others(ofile);
        ofile << "end\n";
    }

    void SteerWriter::write_data_file(std::ofstream& ofile)
    {
        ofile << "CFiles\n";
        ofile << data_filepath_ << "\n";
        ofile << "\n";
    }

    void SteerWriter::write_parameter_defaults(std::ofstream& ofile)
    {
        ofile << "Parameter\n";
        for (const auto& [par_id, values] : parameter_defaults_)
        {
            ofile << fmt::format("{}\t{:.1f}\t{:.1f}\n", par_id, values.first, values.second);
        }
        ofile << "\n";
    }

    void SteerWriter::write_methods(std::ofstream& ofile)
    {
        for (const auto& [method, values] : methods_)
        {
            ofile << fmt::format("method\t{}\t{}\t{}\n", method, values.first, values.second);
        }
        ofile << "\n";
    }

    void SteerWriter::write_others(std::ofstream& ofile)
    {
        for (const auto& other : other_options_)
        {
            ofile << fmt::format("{}\n", fmt::join(other, "\t"));
        }
    }
} // namespace R3B
