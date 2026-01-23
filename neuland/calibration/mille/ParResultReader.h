#pragma once

#include <cstdint>
#include <fmt/base.h>
#include <fmt/core.h>
#include <string>
#include <string_view>
#include <unordered_map>

namespace R3B::Millepede
{
    struct ParResultEntry
    {
        int par_num = 0;
        float value = 0.F;
        float pre_sigma = 0.F;
        float value_diff = 0.F;
        float error = 0.F;
    };

    struct JsonParResultEntry
    {
        int label = 0;
        int entry = 0;
        float correction = 0.;
        float error = 0.;
    };

    class ResultReader
    {
      public:
        enum class Mode : uint8_t
        {
            res,
            json
        };
        ResultReader() = default;
        void set_filename(std::string_view filename) { filename_ = filename; }
        void set_mode(Mode mode) { mode_ = mode; }

        void read();
        void print();
        [[nodiscard]] auto get_pars() const -> const auto& { return par_results_; }

      private:
        Mode mode_ = Mode::res;
        std::string filename_;
        void read_pede_par_file();
        void read_json_file();
        std::unordered_map<int, ParResultEntry> par_results_;
    };

} // namespace R3B::Millepede

template <>
class fmt::formatter<R3B::Millepede::ParResultEntry>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::Millepede::ParResultEntry& entry, FmtContent& ctn) const
    {
        return fmt::format_to(ctn.out(),
                              "par id: {}, value: {}, sigma: {}, value_diff: {}, error: {}",
                              entry.par_num,
                              entry.value,
                              entry.pre_sigma,
                              entry.value_diff,
                              entry.error);
    }
};
