#pragma once

#include <fmt/format.h>
#include <string>
#include <unordered_map>

namespace R3B::Millepede
{
    struct ParResultEntry
    {
        int par_num = 0;
        float value = 0.F;
        float sigma = 0.F;
        float value_diff = 0.F;
        float error = 0.F;
    };

    class ResultReader
    {
      public:
        ResultReader() = default;
        void set_filename(std::string_view filename) { filename_ = filename; }

        void read();
        void print();
        [[nodiscard]] auto get_pars() const -> const auto& { return par_results_; }

      private:
        std::string filename_;
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
        return format_to(ctn.out(),
                         "par id: {}, value: {}, sigma: {}, value_diff: {}, error: {}",
                         entry.par_num,
                         entry.value,
                         entry.sigma,
                         entry.value_diff,
                         entry.error);
    }
};
