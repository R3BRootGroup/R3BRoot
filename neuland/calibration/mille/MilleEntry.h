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

#include <Rtypes.h>
#include <fmt/base.h>
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <utility>
#include <vector>

namespace R3B
{
    struct MilleDataPoint
    {
        std::vector<float> locals;                  // local derivatives
        std::vector<std::pair<int, float>> globals; // global label and derivatives pair
        float measurement = 0.;                     // measurement corresponding to the error value
        float sigma = 1.;                           // error value
        void clear()
        {
            locals.clear();
            globals.clear();
            measurement = 0.;
            sigma = 1.;
        }
        ClassDefNV(MilleDataPoint, 1);
    };

    inline void to_json(nlohmann::json& json_obj, const MilleDataPoint& point)
    {
        json_obj = nlohmann::ordered_json{
            { "measurement", point.measurement },
            { "sigma", point.sigma },
            { "locals", point.locals },
            { "globals", point.globals },
        };
    }
} // namespace R3B

#ifndef __CLING__
template <>
class fmt::formatter<R3B::MilleDataPoint>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::MilleDataPoint& point, FmtContent& ctn) const
    {
        return fmt::format_to(ctn.out(),
                              "measurement: {}, sigma: {}\nlocals: {}\nglobals: {}",
                              point.measurement,
                              point.sigma,
                              point.locals,
                              point.globals);
    }
};
#endif
