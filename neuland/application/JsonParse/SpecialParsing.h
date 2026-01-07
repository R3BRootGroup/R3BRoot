#pragma once

#include "R3BDigitizingTamex.h"
#include "R3BMinMaxValue.h"
#include "R3BValueError.h"
#include <glaze/core/common.hpp>
#include <glaze/core/meta.hpp>
#include <glaze/core/wrappers.hpp>
#include <glaze/glaze.hpp>
#include <magic_enum/magic_enum.hpp>
#include <root/Math/Vector3Dfwd.h>

template <>
struct glz::meta<R3B::Digitizing::Neuland::Tamex::Params>
{
    using T = R3B::Digitizing::Neuland::Tamex::Params;
    static constexpr auto value = glz::object(&T::experimental_data_is_corrected_for_saturation,
                                              &T::pmt_thresh,
                                              &T::saturation_coefficient,
                                              &T::time_res,
                                              &T::energy_res_rel,
                                              &T::energy_gain,
                                              &T::pedestal,
                                              &T::max_time,
                                              &T::min_time,
                                              &T::pileup_time_window,
                                              &T::pileup_distance,
                                              &T::min_energy);
};

template <>
struct glz::meta<ROOT::Math::XYZVectorD>
{
    using T = ROOT::Math::XYZVectorD;
    static constexpr auto read_x = [](T& vec, double input) -> void { vec.SetX(input); };
    static constexpr auto read_y = [](T& vec, double input) -> void { vec.SetY(input); };
    static constexpr auto read_z = [](T& vec, double input) -> void { vec.SetZ(input); };
    static constexpr auto write_x = [](const T& vec) -> double { return vec.X(); };
    static constexpr auto write_y = [](const T& vec) -> double { return vec.Y(); };
    static constexpr auto write_z = [](const T& vec) -> double { return vec.Z(); };
    static constexpr auto value = glz::object("x",
                                              glz::custom<read_x, write_x>,
                                              "y",
                                              glz::custom<read_y, write_y>,
                                              "z",
                                              glz::custom<read_z, write_z>);
};

template <typename DataType>
struct glz::meta<R3B::MinMaxValue<DataType>>
{
    using T = R3B::MinMaxValue<DataType>;
    static constexpr auto value = glz::object("min", &T::min, "max", &T::max);
};

template <typename DataType>
struct glz::meta<R3B::ValueError<DataType>>
{
    using T = R3B::ValueError<DataType>;
    static constexpr auto value = glz::object("value", &T::value, "error", &T::error);
};

template <typename EnumType>
    requires std::is_enum_v<EnumType>
struct glz::meta<EnumType>
{
    constexpr static auto value = magic_enum::enum_values<EnumType>();
    constexpr static auto keys = magic_enum::enum_names<EnumType>();
};
