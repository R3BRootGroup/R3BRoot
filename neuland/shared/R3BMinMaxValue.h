#pragma once

#include <fmt/core.h>
#include <nlohmann/json.hpp>

namespace R3B
{
    template <typename DataType>
    struct MinMaxValue
    {
        MinMaxValue() = default;
        constexpr MinMaxValue(DataType min_val, DataType max_val)
            : min{ min_val }
            , max{ max_val }
        {
        }

        DataType min{};
        DataType max{};
    };

    using MinMaxValueD = MinMaxValue<double>;
    using MinMaxValueF = MinMaxValue<float>;
    using MinMaxValueI = MinMaxValue<int>;

    template <typename DataType>
    void to_json(nlohmann::ordered_json& json_obj, const MinMaxValue<DataType>& value)
    {
        json_obj = nlohmann::ordered_json{ { "min", value.min }, { "max", value.max } };
    }

    template <typename DataType>
    void from_json(const nlohmann::ordered_json& json_obj, MinMaxValue<DataType>& value)
    {
        json_obj.at("min").get_to(value.min);
        json_obj.at("max").get_to(value.max);
    }

} // namespace R3B

template <typename DataType>
class fmt::formatter<R3B::MinMaxValue<DataType>>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::MinMaxValue<DataType>& value, FmtContent& ctn) const
    {
        return format_to(ctn.out(), "{{min: {}, max: {}}}", value.min, value.max);
    }
};
