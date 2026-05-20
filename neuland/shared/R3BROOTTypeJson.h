#pragma once

#include <Math/Vector3D.h> // NOLINT
#include <Math/Vector3Dfwd.h>
#include <nlohmann/json.hpp>

namespace ROOT::Math
{
    inline void to_json(nlohmann::ordered_json& json_obj, const XYZVectorD& value)
    {
        json_obj = nlohmann::ordered_json{ { "x", value.x() }, { "y", value.y() }, { "z", value.z() } };
    }

    inline void from_json(const nlohmann::ordered_json& json_obj, XYZVectorD& value)
    {
        auto val_x = 0.;
        auto val_y = 0.;
        auto val_z = 0.;
        json_obj.at("x").get_to(val_x);
        json_obj.at("y").get_to(val_y);
        json_obj.at("z").get_to(val_z);
        value.SetXYZ(val_x, val_y, val_z);
    }

} // namespace ROOT::Math
