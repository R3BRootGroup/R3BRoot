#include "R3BPDGConverter.h"
#include "R3BException.h"
#include <G4NistManager.hh>
#include <TDatabasePDG.h>
#include <charconv>
#include <cstdint>
#include <cstdlib>
#include <fmt/core.h>
#include <string>
#include <string_view>
#include <system_error>

namespace R3B
{
    constexpr auto MASS_POS = 10;
    constexpr auto ATOMIC_POS = 10000;
    constexpr auto PDG_FULL_POS = 10000000;

    namespace
    {
        using Type = PDGConverter::Type;

        auto convert_to_int(std::string_view str, int& val)
        {
            auto res = std::from_chars(str.begin(), str.end(), val);
            if (res.ec != std::errc())
            {
                fmt::println(
                    "conversion failed with the input: {}. Reason: {}", str, std::make_error_code(res.ec).message());
            }
        }

        auto convert_nuclear_pid_to_name(int32_t pid) -> std::string
        {
            const auto isomeric_number = pid % MASS_POS;
            const auto mass_number = ((pid % ATOMIC_POS) - isomeric_number) / MASS_POS;
            const auto atomic_number = ((pid % PDG_FULL_POS) - (mass_number * MASS_POS) - isomeric_number) / ATOMIC_POS;

            auto* nist_manager = G4NistManager::Instance();
            auto* element = nist_manager->FindOrBuildElement(atomic_number);

            if (isomeric_number == 0)
            {
                return fmt::format("{}-{}", element->GetName(), mass_number);
            }
            return fmt::format("{}-{}s", element->GetName(), mass_number, isomeric_number);
        }

        inline auto convert_basic_pid_to_name(int32_t pid) -> std::string
        {
            auto* particle = TDatabasePDG::Instance()->GetParticle(pid);
            if (particle == nullptr)
            {
                throw R3B::logic_error(fmt::format("Failed to query a basic particle name from a pid: {}", pid));
            }
            return particle->GetName();
        }

        inline auto convert_basic_name_to_pid(const std::string& name) -> int32_t
        {
            auto* particle = TDatabasePDG::Instance()->GetParticle(name.c_str());
            if (particle == nullptr)
            {
                throw R3B::logic_error(fmt::format("Failed to query a basic particle pid from the name: {:?}", name));
            }
            return particle->PdgCode();
        }

        inline auto convert_nuclear_name_to_pid(const std::string_view name) -> int32_t
        {
            const auto dash_pos = name.find('-');
            auto element_name = name.substr(0, dash_pos);
            const auto s_pos = name.find('s', dash_pos);
            auto mass_num = 0;
            auto isomeric_num = 0;
            if (s_pos != std::string_view::npos)
            {
                convert_to_int(name.substr(dash_pos + 1, s_pos - dash_pos - 1), mass_num);
                convert_to_int(name.substr(s_pos + 1, std::string_view::npos), isomeric_num);
            }
            else
            {
                convert_to_int(name.substr(dash_pos + 1, std::string_view::npos), mass_num);
            }
            auto* nist_manager = G4NistManager::Instance();
            auto* element = nist_manager->FindOrBuildElement(std::string{ element_name });
            if (element == nullptr)
            {
                throw R3B::logic_error(fmt::format("Cannot find any element with the name {:?}", element_name));
            }
            auto atomic_number = static_cast<int32_t>(element->GetZ());
            return R3B::PDGConverter::nuclear_id_min + isomeric_num + (mass_num * MASS_POS) +
                   (atomic_number * ATOMIC_POS);
        }

        auto convert_pid_to_name(int32_t pid) -> std::string
        {
            switch (PDGConverter::get_type(pid))
            {
                case Type::nuclear:
                    return convert_nuclear_pid_to_name(pid);
                case Type::basic:
                    return convert_basic_pid_to_name(pid);
                case Type::invalid:
                    return "invalid";
            }
            return "invalid";
        }

        auto convert_name_to_pid(const std::string& name) -> int32_t
        {
            switch (PDGConverter::get_type(name))
            {
                case Type::nuclear:
                    return convert_nuclear_name_to_pid(name);
                case Type::basic:
                    return convert_basic_name_to_pid(name);
                case Type::invalid:
                    return 0;
            }
            return 0;
        }

    } // namespace

    auto PDGConverter::get_pid(const std::string& name) -> int32_t
    {
        register_entry(name);
        return name_to_pid_.at(name);
    }

    auto PDGConverter::get_name(int32_t pid) -> std::string
    {
        register_entry(pid);
        return pid_to_name_.at(pid);
    }

    auto PDGConverter::get_type(int32_t pid) -> Type
    {
        if (std::abs(pid) > nuclear_id_max)
        {
            return Type::invalid;
        }
        if (std::abs(pid) > nuclear_id_min)
        {
            return Type::nuclear;
        }
        return Type::basic;
    }

    auto PDGConverter::get_type(const std::string& name) -> Type
    {
        const auto dash_pos = name.find('-');
        if (dash_pos == std::string::npos)
        {
            return Type::basic;
        }

        if (name.size() == dash_pos + 1)
        {
            return Type::basic;
        }

        if (dash_pos > 2)
        {
            return Type::invalid;
        }
        return Type::nuclear;
    }

    auto PDGConverter::reset()
    {
        pid_to_name_.clear();
        name_to_pid_.clear();
    }

    void PDGConverter::register_entry(int32_t pid)
    {
        if (pid_to_name_.find(pid) == pid_to_name_.end())
        {
            const auto name = convert_pid_to_name(pid);
            pid_to_name_.emplace(pid, name);
            name_to_pid_.emplace(name, pid);
        }
    }

    void PDGConverter::register_entry(const std::string& name)
    {
        if (name_to_pid_.find(name) == name_to_pid_.end())
        {
            const auto pid = convert_name_to_pid(name);
            pid_to_name_.emplace(pid, name);
            name_to_pid_.emplace(name, pid);
        }
    }

} // namespace R3B
