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

#include "R3BNeulandCalToHitPar.h"
#include "R3BException.h"
#include "R3BNeulandParSet.h"
#include "R3BValueError.h"
#include <R3BNeulandCommon.h>
#include <R3BShared.h>
#include <Rtypes.h>
#include <RtypesCore.h>
#include <algorithm>
#include <cmath>
#include <fmt/format.h>
#include <range/v3/algorithm/max_element.hpp>
#include <range/v3/view/map.hpp>
#include <range/v3/view/transform.hpp>
#include <string_view>

namespace
{
    auto calculate_threshold(const R3B::Neuland::HitModulePar& modulePar, R3B::Side side)
    {
        const auto value = modulePar.pmt_threshold.get(side).value *
                           std::exp(R3B::Neuland::TotalBarLength / modulePar.light_attenuation_length.value);
        const auto err = 0.;
        return R3B::ValueError<double>{ value, err };
    }

} // namespace

namespace R3B::Neuland
{
    Cal2HitPar::Cal2HitPar(std::string_view name, std::string_view title, std::string_view context, bool own)
        : ParSet(name, title, context, own)
    {
    }

    void Cal2HitPar::CalculateEnergyCutoff()
    {
        auto threshold_views =
            ranges::views::transform(module_pars_ | ranges::views::values,
                                     [](const auto& ele)
                                     {
                                         return std::max(calculate_threshold(ele, R3B::Side::left).value,
                                                         calculate_threshold(ele, R3B::Side::right).value);
                                     });

        auto max_value = ranges::max_element(threshold_views);
        if (max_value != threshold_views.end())
        {
            energy_cut_ = *max_value;
        }
    }
    auto Cal2HitPar::GetModuleParAt(int module_num) const -> const ::R3B::Neuland::HitModulePar&
    {
#ifdef HAS_CPP_STANDARD_17
        if (module_pars_.find(module_num) == module_pars_.end())
#else
        if (!module_pars_.contains(module_num))
#endif
        {
            throw R3B::logic_error(fmt::format("module pars has no module num {}", module_num));
        }
        return module_pars_.at(module_num);
    }

} // namespace R3B::Neuland

ClassImp(R3B::Neuland::Cal2HitPar);
