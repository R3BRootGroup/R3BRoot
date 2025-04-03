#pragma once

#include <cstdint>
#include <fmt/core.h>
#include <string_view>

namespace R3B::Particle
{
    enum class Type : uint8_t
    {
        proton,
        neutron,
        alpha,
        muon,
        electron,
        invalid,
    };

    constexpr auto type_to_uint64(Type type) -> uint64_t
    {
        // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
        switch (type)
        {
            case Type::proton:
                return 2212;
                break;
            case Type::neutron:
                return 2112;
                break;
            case Type::alpha:
                return 1000020040;
                break;
            case Type::muon:
                return 13;
                break;
            case Type::electron:
                return 11;
                break;
            case Type::invalid:
                return 0;
                break;
        }
        return 0;
        // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
    }

    constexpr auto uint64_to_type(uint64_t code) -> Type
    {
        // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
        if (code == 2212)
        {
            return Type::proton;
        }
        if (code == 2112)
        {
            return Type::neutron;
        }
        if (code == 1000020040)
        {
            return Type::alpha;
        }
        if (code == 13)
        {
            return Type::muon;
        }
        if (code == 11)
        {
            return Type::electron;
        }
        return Type::invalid;
        // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
    }
    constexpr auto type_to_string_view(Type type) -> std::string_view
    {
        using R3B::Particle::Type;
        switch (type)
        {
            case Type::proton:
                return "proton";
                break;
            case Type::neutron:
                return "neutron";
                break;
            case Type::alpha:
                return "alpha";
                break;
            case Type::muon:
                return "muon";
                break;
            case Type::electron:
                return "electron";
                break;
            case Type::invalid:
                return "invalid";
                break;
        }
    }
} // namespace R3B::Particle

template <>
class fmt::formatter<R3B::Particle::Type>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(R3B::Particle::Type type, FmtContent& ctn) const
    {
        return format_to(ctn.out(), "{}", R3B::Particle::type_to_string_view(type));
    }
};
