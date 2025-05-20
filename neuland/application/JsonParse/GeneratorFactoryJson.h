#pragma once

#include "R3BNeulandGeneratorFactory.h"
#include "R3BNeulandJsonHeader.h"
#include <R3BROOTTypeJson.h> // NOLINT
#include <magic_enum/magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>

namespace R3B::Neuland
{
    template <>
    inline void to_json(nlohmann::ordered_json& json_obj, const GeneratorFactory::Options& options)
    {
        json_obj = nlohmann::ordered_json{
            { "type", magic_enum::enum_name(options.generator_type) },
            { "particle", options.particle_type },
            { "energy", options.energy },
            { "theta", options.theta },
            { "phi", options.phi },
            { "position", options.position },
        };
    }

    template <>
    inline void from_json(const nlohmann::ordered_json& json_obj, GeneratorFactory::Options& options)
    {
        auto type_str = std::string{};
        json_obj.at("type").get_to(type_str);
        json_obj.at("particle").get_to(options.particle_type);
        json_obj.at("energy").get_to(options.energy);
        json_obj.at("theta").get_to(options.theta);
        json_obj.at("phi").get_to(options.phi);
        json_obj.at("position").get_to(options.position);
        auto type_str_val = magic_enum::enum_cast<GeneratorType>(type_str, magic_enum::case_insensitive);
        if (type_str_val.has_value())
        {
            options.generator_type = type_str_val.value();
        }
    }
} // namespace R3B::Neuland
