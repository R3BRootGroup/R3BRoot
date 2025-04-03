/******************************************************************************
 *   Copyright (C) 2024 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2024-2026 Members of R3B Collaboration                     *
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

#include "R3BShared.h"
#include "R3BValueError.h"
#include <TVector3.h>
#include <fmt/core.h>
#include <fmt/format.h>

// NOLINTNEXTLINE(misc-include-cleaner)
#include <Math/Vector3D.h>

template <>
class fmt::formatter<TVector3>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const TVector3& vec, FmtContent& ctn) const
    {
        return fmt::format_to(ctn.out(), "[x: {}, y: {}, z: {}]", vec.X(), vec.Y(), vec.Z());
    }
};

template <typename DataType>
class fmt::formatter<R3B::ValueError<DataType>>
{
  public:
    // TODO: add more options
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::ValueError<DataType>& value_error, FmtContent& ctn) const
    {
        return fmt::format_to(ctn.out(), "{}+/-{}", value_error.value, value_error.error);
    }
};

template <>
class fmt::formatter<ROOT::Math::XYZVector>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const ROOT::Math::XYZVector& vec, FmtContent& ctn) const
    {
        return fmt::format_to(ctn.out(), "[x: {}, y: {}, z: {}]", vec.X(), vec.Y(), vec.Z());
    }
};

template <typename DataType>
class fmt::formatter<R3B::LRPair<DataType>>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::LRPair<DataType>& data_pair, FmtContent& ctn) const
    {
        return fmt::format_to(ctn.out(), "[left: {}, right: {}]", data_pair.left(), data_pair.right());
    }
};

template <>
class fmt::formatter<R3B::Side>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::Side& side, FmtContent& ctn) const
    {
        if (side == R3B::Side::left)
        {
            return fmt::format_to(ctn.out(), "{}", "left");
        }
        return fmt::format_to(ctn.out(), "{}", "right");
    }
};

template <typename DataType>
class fmt::formatter<R3B::LRPair<DataType>>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::LRPair<DataType>& data_pair, FmtContent& ctn) const
    {
        return format_to(ctn.out(), "[left: {}, right: {}]", data_pair.left(), data_pair.right());
    }
};

template <>
class fmt::formatter<R3B::Side>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::Side& side, FmtContent& ctn) const
    {
        if (side == R3B::Side::left)
        {
            return format_to(ctn.out(), "{}", "left");
        }
        return format_to(ctn.out(), "{}", "right");
    }
};
