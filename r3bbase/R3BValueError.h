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
#include <Rtypes.h>
#include <TObject.h>
#include <cmath>
#include <nlohmann/json.hpp>
#include <type_traits>

namespace R3B
{
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

        template <typename OtherType, typename = std::enable_if_t<std::is_arithmetic_v<OtherType>>>
        auto operator+(OtherType other) const -> ValueError<DataType>
        {
            return { value + other, error };
        }

        template <typename OtherType, typename = std::enable_if_t<std::is_arithmetic_v<OtherType>>>
        auto operator*(OtherType other) const -> ValueError<DataType>
        {
            return { value * other, error * other };
        }

        template <typename OtherType, typename = std::enable_if_t<std::is_arithmetic_v<OtherType>>>
        auto operator/(OtherType other) const -> ValueError<DataType>
        {
            return { value / other, error / other };
        }

        template <typename OtherType, typename = std::enable_if_t<std::is_arithmetic_v<OtherType>>>
        auto operator-(OtherType val) const -> ValueError<DataType>
        {
            return { value - val, error };
        }

        template <typename OtherType, typename = std::enable_if_t<std::is_arithmetic_v<OtherType>>>
        void operator-=(OtherType other)
        {
            value -= other;
        }

      public:
        ClassDefNV(ValueError, 1);
    };

    using ValueErrorD = ValueError<double>;
    using ValueErrorF = ValueError<float>;
    using ValueErrorI = ValueError<int>;

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
