#pragma once

#include "NeulandParticleFilter.h"
#include "R3BException.h"
#include "R3BNeulandJsonHeader.h"
#include <fmt/core.h>
#include <magic_enum/magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <vector>

namespace R3B::Neuland
{
    template <>
    inline void to_json(nlohmann::ordered_json& json_obj, const ParticleFilter::Options& options)
    {
        json_obj = nlohmann::ordered_json{
            { "mode", magic_enum::enum_name(options.mode) },    { "global-min-energy", options.global_min_energy },
            { "global-max-energy", options.global_max_energy }, { "particle-types", options.particle_names },
            { "energy-range-mins", options.energy_range_mins }, { "energy-range-maxs", options.energy_range_maxs },
        };
    }

    template <>
    inline void from_json(const nlohmann::ordered_json& json_obj, ParticleFilter::Options& option)
    {
        json_obj.at("particle-types").get_to(option.particle_names);
        json_obj.at("global-min-energy").get_to(option.global_min_energy);
        json_obj.at("global-max-energy").get_to(option.global_max_energy);
        json_obj.at("energy-range-mins").get_to(option.energy_range_mins);
        json_obj.at("energy-range-maxs").get_to(option.energy_range_maxs);

        // parse mode
        auto mode_name = std::string{};
        json_obj.at("mode").get_to(mode_name);
        auto mode = magic_enum::enum_cast<R3B::Neuland::ParticleFilter::Mode>(mode_name, magic_enum::case_insensitive);
        if (mode.has_value())
        {
            option.mode = mode.value();
        }
        else
        {
            throw R3B::logic_error(fmt::format("Cannot parse the mode string {:?} to the corresponding enum class. "
                                               "Please check if the enum string is correct!",
                                               mode_name));
        }

        // append zeros if size unmatched
        const auto particle_size = option.particle_names.size();
        option.energy_range_maxs.resize(particle_size, 0.);
        option.energy_range_mins.resize(particle_size, 0.);
    }

} // namespace R3B::Neuland
