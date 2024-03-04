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

#include "TObject.h"
#include <R3BShared.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

namespace R3B
{
    struct SingleEdgeSignal
    {
        unsigned int fine = 0;   // fine time value
        unsigned int coarse = 0; // coarse time value
        ClassDefNV(SingleEdgeSignal, 1)
    };

    struct DoubleEdgeSignal
    {
        SingleEdgeSignal leading{};  // leading edge
        SingleEdgeSignal trailing{}; // trailing edge
                                     //
        ClassDefNV(DoubleEdgeSignal, 1)
    };

    struct MapBarSignal
    {
        MapBarSignal() = default;
        MapBarSignal(R3B::Side side, const DoubleEdgeSignal& signal);
        inline void push_back(R3B::Side side, const DoubleEdgeSignal& signal)
        {
            (side == R3B::Side::left ? left : right).push_back(signal);
        }
        [[nodiscard]] auto get(R3B::Side side) const -> const auto& { return side == R3B::Side::left ? left : right; }

        std::vector<DoubleEdgeSignal> left;
        std::vector<DoubleEdgeSignal> right;
        ClassDefNV(MapBarSignal, 1)
    };

    struct PaddleTamexMappedData
    {
        unsigned int plane_num = 0;                               //... number of plane 1..n
        std::unordered_map<unsigned int, R3B::MapBarSignal> bars; //... number of bar   1..n

        PaddleTamexMappedData() = default;
        explicit PaddleTamexMappedData(unsigned int pID);
        void push_back(R3B::Side side, unsigned int barID, const R3B::DoubleEdgeSignal& signal);
        inline auto empty() const -> bool { return bars.empty(); };

        ClassDefNV(PaddleTamexMappedData, 1)
    };

    struct PaddleTamexTrigMappedData
    {
        unsigned int module_num = 0; //... number of trigID 1..n
        R3B::SingleEdgeSignal signal;
        ClassDefNV(PaddleTamexTrigMappedData, 1)
    };
} // namespace R3B

template <>
class fmt::formatter<R3B::SingleEdgeSignal>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::SingleEdgeSignal& signal, FmtContent& ctn) const
    {
        return format_to(ctn.out(), "{{fine: {}, coarse: {} }}", signal.fine, signal.coarse);
    }
};

template <>
class fmt::formatter<R3B::DoubleEdgeSignal>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::DoubleEdgeSignal& signal, FmtContent& ctn) const
    {
        return format_to(ctn.out(), "{{leading: {}, trailing: {} }}", signal.leading, signal.trailing);
    }
};

template <>
class fmt::formatter<R3B::MapBarSignal>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::MapBarSignal& signal, FmtContent& ctn) const
    {
        return format_to(
            ctn.out(), "{{left: [{}], right: [{}] }}", fmt::join(signal.left, ", "), fmt::join(signal.left, ", "));
    }
};

using R3BPaddleTamexMappedDataContainer = std::vector<R3B::PaddleTamexMappedData>;
using R3BPaddleTamexTrigDataContainer = std::map<unsigned int, R3B::PaddleTamexTrigMappedData>;

using R3BPaddleTamexMappedData2 = R3B::PaddleTamexMappedData;
using R3BPaddleTamexTrigMappedData = R3B::PaddleTamexTrigMappedData;
