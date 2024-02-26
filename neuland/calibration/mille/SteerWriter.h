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

#include <fmt/format.h>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

constexpr auto DEFAULT_MILLEPEDE_PARAMETER_FILE = "parameters.txt";

namespace R3B
{
    class SteerWriter
    {
      public:
        enum class Method
        {
            inversion,
            diagonalization,
            fullGMRES,
            sparseGMRES,
            cholesky,
            bandcholesky,
            HIP
        };

        SteerWriter() = default;
        void set_filepath(std::string_view filepath) { filepath_ = filepath; }
        void set_data_filepath(std::string_view filepath) { data_filepath_ = filepath; }
        void add_parameter_default(int par_num, const std::pair<float, float>& values);
        void add_method(Method method, const std::pair<float, float>& values);
        void add_other_options(std::vector<std::string> options);

        void write();

      private:
        std::map<Method, std::pair<float, float>> methods_;
        std::string filepath_ = "steer.txt";
        std::string data_filepath_ = "data.bin";
        std::unordered_map<int, std::pair<float, float>> parameter_defaults_;
        std::vector<std::vector<std::string>> other_options_;

        void write_parameter_defaults();
        void write_data_file(std::ofstream& ofile);
        void write_methods(std::ofstream& ofile);
        void write_others(std::ofstream& ofile);
    };

} // namespace R3B

template <>
class fmt::formatter<R3B::SteerWriter::Method>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::SteerWriter::Method& method, FmtContent& ctn) const
    {
        switch (method)
        {
            case R3B::SteerWriter::Method::inversion:
                return format_to(ctn.out(), "{}", "inversion");
                break;
            case R3B::SteerWriter::Method::diagonalization:
                return format_to(ctn.out(), "{}", "diagonalization");
                break;
            case R3B::SteerWriter::Method::fullGMRES:
                return format_to(ctn.out(), "{}", "fullGMRES");
                break;
            case R3B::SteerWriter::Method::sparseGMRES:
                return format_to(ctn.out(), "{}", "sparseGMRES");
                break;
            case R3B::SteerWriter::Method::cholesky:
                return format_to(ctn.out(), "{}", "cholesky");
                break;
            case R3B::SteerWriter::Method::bandcholesky:
                return format_to(ctn.out(), "{}", "bandcholesky");
                break;
            case R3B::SteerWriter::Method::HIP:
                return format_to(ctn.out(), "{}", "HIP");
                break;
            default:
                return format_to(ctn.out(), "{}", "unrecognized");
                break;
        }
    }
};
