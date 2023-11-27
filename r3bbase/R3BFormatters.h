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

#include "R3BShared.h"
#include <TVector3.h>
#include <fmt/format.h>

template <>
class fmt::formatter<TVector3>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const TVector3& vec, FmtContent& ctn) const
    {
        return format_to(ctn.out(), "[x: {}, y: {}, z: {}]", vec.X(), vec.Y(), vec.Z());
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
        return format_to(ctn.out(), "{}+/-{}", value_error.value, value_error.error);
    }
};
