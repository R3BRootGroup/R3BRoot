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

#include "R3BNeulandBasePar.h"

namespace R3B::Neuland
{
    CalibrationBasePar::CalibrationBasePar(std::string_view name,
                                           std::string_view title,
                                           std::string_view context,
                                           bool own)
        : Neuland::ParSet(name.data(), title.data(), context.data(), own)
    {
    }

    void to_json(nlohmann::json& jsn, const CalibrationBasePar& par)
    {
        jsn = nlohmann::json{ { "expIDs", par.exp_ids_ },
                              { "num_of_planes", par.num_of_planes_ },
                              { "offspill", par.offspill_tpat_pos_ },
                              { "trig_mapping", par.trigIDMap_ } };
    }
    void from_json(const nlohmann::json& jsn, CalibrationBasePar& par)
    {
        jsn.at("expIDs").get_to(par.exp_ids_);
        jsn.at("num_of_planes").get_to(par.num_of_planes_);
        jsn.at("offspill").get_to(par.offspill_tpat_pos_);

        par.trigIDMap_.clear();
        for (const auto& jsonEntry : jsn.at("trig_mapping"))
        {
            const auto moduleID = jsonEntry.at("barID").template get<int>();
            const auto trigID_left = jsonEntry.at("trigID_left").template get<int>();
            const auto trigID_right = jsonEntry.at("trigID_right").template get<int>();
            par.trigIDMap_.insert({ moduleID, std::make_pair(trigID_left, trigID_right) });
        }
    }
} // namespace R3B::Neuland

ClassImp(R3B::Neuland::CalibrationBasePar);
