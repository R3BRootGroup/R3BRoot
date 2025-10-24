/******************************************************************************
 *   Copyright (C) 2024 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2024-2026 Members of R3B Collaboration                     *
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
#include <Math/GenVector/Cartesian3D.h>
#include <Math/GenVector/CoordinateSystemTags.h>
#include <Math/GenVector/DisplacementVector3D.h>
#include <Rtypes.h>
#include <TObject.h>
#include <cmath>
#ifndef __CLING__
#include <fmt/base.h>
#include <fmt/core.h>
#endif
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#ifndef HAS_CPP_STANDARD_17
#else
#include <type_traits>
#endif

namespace R3B
{

#ifndef HAS_CPP_STANDARD_17
    template <typename T>
    concept Arithmetic = std::is_arithmetic_v<T>;
#endif

    template <typename DataType>
    struct ValueError
    {
        bool valid = true;
        DataType value{};
        DataType error{};

        constexpr ValueError(DataType val, DataType err)
            : value{ val }
            , error{ err }
        {
        }

        constexpr explicit ValueError(DataType val)
            : ValueError{ val, DataType{} }
        {
        }
        // ValueError(const DataType& val, const DataType& err)
        //     : value{ val }
        //     , error{ err }
        // {
        // }

        ValueError()
            : valid{ false }
        {
        }

        auto operator-() const -> ValueError<DataType> { return { -value, error }; }

#ifndef HAS_CPP_STANDARD_17
        auto operator+(Arithmetic auto other) const -> ValueError<DataType>
#else
        template <typename OtherType, typename = std::enable_if_t<std::is_arithmetic_v<OtherType>>>
        auto operator+(OtherType other) const -> ValueError<DataType>
#endif
        {
            return { value + other, error };
        }

#ifndef HAS_CPP_STANDARD_17
        auto operator*(Arithmetic auto other) const -> ValueError<DataType>
#else
        template <typename OtherType, typename = std::enable_if_t<std::is_arithmetic_v<OtherType>>>
        auto operator*(OtherType other) const -> ValueError<DataType>
#endif
        {
            return { value * other, error * other };
        }

#ifndef HAS_CPP_STANDARD_17
        auto operator/(Arithmetic auto other) const -> ValueError<DataType>
#else
        template <typename OtherType, typename = std::enable_if_t<std::is_arithmetic_v<OtherType>>>
        auto operator/(OtherType other) const -> ValueError<DataType>
#endif
        {
            return { value / other, error / other };
        }

#ifndef HAS_CPP_STANDARD_17
        auto operator-(Arithmetic auto val) const -> ValueError<DataType>
#else
        template <typename OtherType, typename = std::enable_if_t<std::is_arithmetic_v<OtherType>>>
        auto operator-(OtherType val) const -> ValueError<DataType>
#endif
        {
            return { value - val, error };
        }

#ifndef HAS_CPP_STANDARD_17
        void operator-=(Arithmetic auto val)
#else
        template <typename OtherType, typename = std::enable_if_t<std::is_arithmetic_v<OtherType>>>
        void operator-=(OtherType val)
#endif
        {
            value -= val;
        }

#ifndef HAS_CPP_STANDARD_17
        auto operator>(Arithmetic auto val) -> bool
#else
        template <typename OtherType, typename = std::enable_if_t<std::is_arithmetic_v<OtherType>>>
        auto operator>(OtherType val) -> bool
#endif
        {
            return value > val;
        }

#ifndef HAS_CPP_STANDARD_17
        auto operator<(Arithmetic auto val) -> bool
#else
        template <typename OtherType, typename = std::enable_if_t<std::is_arithmetic_v<OtherType>>>
        auto operator<(OtherType val) -> bool
#endif
        {
            return value < val;
        }

      public:
        ClassDefNV(ValueError, 1);
    };

    using ValueErrorD = ValueError<double>;
    using ValueErrorF = ValueError<float>;
    using ValueErrorI = ValueError<int>;
    using XYZVectorValueErrorD =
        ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<ValueErrorD>, ROOT::Math::DefaultCoordinateSystemTag>;

    template <typename DataType>
    auto operator*(ValueError<DataType> left, ValueError<DataType> right) -> ValueError<DataType>
    {
        auto new_value = left.value * right.value;
        auto new_error = std::sqrt(right.value * right.value * left.error * left.error +
                                   left.value * left.value * right.error * right.error);
        return { new_value, new_error };
    }

    template <typename DataType>
    auto operator/(ValueError<DataType> numerator, ValueError<DataType> denominator) -> ValueError<DataType>
    {
        auto new_value = numerator.value / denominator.value;
        auto new_error = std::sqrt(numerator.error * numerator.error +
                                   denominator.error * denominator.error * new_value * new_value) /
                         denominator.value;
        return { new_value, std::abs(new_error) };
    }

    template <typename DataType>
    auto operator+(ValueError<DataType> left, ValueError<DataType> right) -> ValueError<DataType>
    {
        auto new_value = left.value + right.value;
        // TODO: not the fastest way
        auto new_err = std::sqrt(left.error * left.error + right.error * right.error);
        return { new_value, new_err };
    }

    template <typename DataType>
    auto operator-(ValueError<DataType> left, ValueError<DataType> right) -> ValueError<DataType>
    {
        auto new_value = left.value - right.value;
        // TODO: not the fastest way
        auto new_err = std::sqrt(left.error * left.error + right.error * right.error);
        return { new_value, new_err };
    }

    template <typename DataType>
    auto operator-=(ValueError<DataType>& left, const ValueError<DataType>& right) -> ValueError<DataType>&
    {
        left = left - right;
        return left;
    }

    template <typename DataType>
    auto operator+=(ValueError<DataType>& left, const ValueError<DataType>& right) -> ValueError<DataType>&
    {
        left = left + right;
        return left;
    }

    template <typename DataType>
    auto operator>(ValueError<DataType>& left, const ValueError<DataType>& right) -> bool
    {
        return left.value > right.value;
    }

    template <typename DataType>
    auto operator<(ValueError<DataType>& left, const ValueError<DataType>& right) -> bool
    {
        return left.value < right.value;
    }

    template <typename DataType>
    void to_json(nlohmann::ordered_json& json_obj, const ValueError<DataType>& value)
    {
        json_obj = nlohmann::ordered_json{
            { "value", value.value },
            { "error", value.error },
        };
    }

    template <typename DataType>
    void from_json(const nlohmann::ordered_json& json_obj, ValueError<DataType>& value)
    {
        json_obj.at("value").get_to(value.value);
        json_obj.at("error").get_to(value.error);
    }
} // namespace R3B

#ifndef __CLING__
template <typename DataType>
class fmt::formatter<R3B::ValueError<DataType>>
{
  public:
    // TODO: add more options
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::ValueError<DataType>& value_error, FmtContent& ctn) const
    {
        return fmt::format_to(ctn.out(), "{}+/-{}", value_error.value, value_error.error);
    }
};

template <>
class fmt::formatter<R3B::XYZVectorValueErrorD>
{
  public:
    static constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::XYZVectorValueErrorD& hit, FmtContent& ctn) const
    {
        return fmt::format_to(ctn.out(), "[x: {}, y: {}, z: {}]", hit.X(), hit.Y(), hit.Z());
    }
};
#endif
