#pragma once
#include "R3BNeulandBasePar.h"
#include "R3BNeulandJsonHeader.h"
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <utility>

namespace R3B::Neuland
{
    template <>
    inline void to_json(nlohmann::ordered_json& json_obj, const CalibrationBasePar& options)
    {
        json_obj = nlohmann::ordered_json{ { "expIDs", options.get_exp_ids() },
                                           { "num_of_planes", options.get_num_of_planes() },
                                           { "offspill", options.get_offspill_tpat_pos() },
                                           { "trig_mapping", options.get_trig_id_map() } };
    }

    template <>
    inline void from_json(const nlohmann::ordered_json& json_obj, CalibrationBasePar& options)
    {

        auto offspill_tpat_pos = 0;
        auto num_of_planes = 0;
        auto exp_ids = std::string{};
        auto trigIDMap = CalibrationBasePar::TrigIDMap{};

        json_obj.at("expIDs").get_to(exp_ids);
        json_obj.at("num_of_planes").get_to(num_of_planes);
        json_obj.at("offspill").get_to(offspill_tpat_pos);

        options.set_num_of_planes(num_of_planes);
        options.set_offspill_tpat_pos(offspill_tpat_pos);
        options.set_exp_ids(exp_ids);

        for (const auto& jsonEntry : json_obj.at("trig_mapping"))
        {
            const auto moduleID = jsonEntry.at("barID").template get<int>();
            const auto trigID_left = jsonEntry.at("trigID_left").template get<int>();
            const auto trigID_right = jsonEntry.at("trigID_right").template get<int>();
            trigIDMap.insert({ moduleID, std::make_pair(trigID_left, trigID_right) });
        }
        options.set_trig_id_map(std::move(trigIDMap));
    }

} // namespace R3B::Neuland
