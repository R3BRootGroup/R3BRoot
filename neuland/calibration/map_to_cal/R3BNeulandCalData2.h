#pragma once

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

#include "R3BValueError.h"
#include <Rtypes.h>
#include <vector>

#ifndef __CLING__
#include "R3BFormatters.h" // IWYU pragma: keep
#include <fmt/base.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#endif

namespace R3B::Neuland
{
    struct CalDataSignal
    {
        ValueError<double> leading_time;        // ns
        ValueError<double> time_over_threshold; // ns
        ValueError<double> trigger_time;        // ns
        ClassDefNV(CalDataSignal, 1)
    };

    struct BarCalData
    {
      public:
        BarCalData() = default;
        explicit BarCalData(int mod_num)
            : module_num{ mod_num }
        {
        }
        int module_num = 0; // 1 based bar num
        std::vector<CalDataSignal> left;
        std::vector<CalDataSignal> right;
        ClassDefNV(BarCalData, 1)
    };
} // namespace R3B::Neuland
  //
using R3BNeulandCalDataContainer = std::vector<R3B::Neuland::BarCalData>;

#ifndef __CLING__
template <>
class fmt::formatter<R3B::Neuland::CalDataSignal>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::Neuland::CalDataSignal& signal, FmtContent& ctn) const
    {
        return fmt::format_to(ctn.out(),
                              "{{leadT: {}, tot: {}, trigT: {} }}",
                              signal.leading_time,
                              signal.time_over_threshold,
                              signal.trigger_time);
    }
};

template <>
class fmt::formatter<R3B::Neuland::BarCalData>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::Neuland::BarCalData& signal, FmtContent& ctn) const
    {
        return fmt::format_to(ctn.out(),
                              "ModuleNum: {}, left bar: [{}], right bar: [{}]",
                              signal.module_num,
                              fmt::join(signal.left, ", "),
                              fmt::join(signal.right, ", "));
    }
};
#endif
