#pragma once

#include <cstdint>
#include <cstdlib>
#include <string>
#include <unordered_map>

namespace R3B
{
    class PDGConverter
    {
      public:
        enum class Type : uint8_t
        {
            basic,
            nuclear,
            invalid,
        };

        PDGConverter() = default;

        auto get_pid(const std::string& name) -> int32_t;

        auto get_name(int32_t pid) -> std::string;

        /**
         * @brief Get the type of the particle from its name
         *
         * To determine type from a name is a Type::nuclear, it has three criteria:
         *
         * - the name contains a dash "-"
         * - At least one letter after dash "-"
         * - At most two letters before dash "-"
         *
         * Otherwise the type is Type::basic
         *
         * @param pid Pid of the basic particle/nucleus
         * @return Type of the particle
         */
        static auto get_type(const std::string& name) -> Type;

        /**
         * @brief Get the type of the particle from its PDG ID
         *
         * The type of the particle can be determined by its PDG ID in the way of:
         *
         * - Type::basic if abs(pid) is larger than nuclear_id_max
         * - Type::basic if abs(pid) is smaller than nuclear_id_min
         * - Type::basic if abs(pid) is larger than nuclear_id_min
         *
         * @param name Name of the basic particle/nucleus
         * @return Type of the particle
         */
        static auto get_type(int32_t pid) -> Type;

        auto reset();
        static constexpr auto nuclear_id_min = int32_t{ 1000000000 };
        static constexpr auto nuclear_id_max = int32_t{ 1009999990 };

      private:
        std::unordered_map<int32_t, std::string> pid_to_name_;
        std::unordered_map<std::string, int32_t> name_to_pid_;

        void register_entry(int32_t pid);
        void register_entry(const std::string& name);

        auto convert_basic_pid_to_name(int32_t pid) -> std::string;
    };

} // namespace R3B
