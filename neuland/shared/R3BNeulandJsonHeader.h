#pragma once
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

namespace R3B::Neuland
{
    template <typename Option>
    void to_json(nlohmann::ordered_json& json_obj, const Option& option)
    {
        json_obj =
            nlohmann::ordered_json{ { "enable", option.enable }, { "read", option.read }, { "write", option.write } };
    }

    template <typename Option>
    void from_json(const nlohmann::ordered_json& json_obj, Option& option)
    {
        json_obj.at("enable").get_to(option.enable);
        json_obj.at("read").get_to(option.read);
        json_obj.at("write").get_to(option.write);
    }

    // template
} // namespace R3B::Neuland
