#include "R3BNeulandT0Checker.h"
#include "R3BException.h"
#include <R3BLogger.h>
#include <cmath>
#include <fmt/format.h>

constexpr auto COARSE_TIME_FULL_CYCLE = 1U << 11U; // TAMEX period
// constexpr auto T0_PERIOD_CYCLE = 1U << 14U;

namespace R3B::Neuland
{
    T0Entry::T0Entry(int t0_value)
        : t0_ref_{ t0_value }
    {
    }

    void T0Entry::SetInitial(int t0_value)
    {
        if (t0_ref_.has_value())
        {
            throw R3B::runtime_error(fmt::format("T0 reference has already been set!"));
        }
        t0_ref_ = t0_value;
    }

    auto T0Entry::GetCorrection(int coarse_time) const -> int
    {
        if (not t0_ref_.has_value())
        {
            throw R3B::runtime_error(fmt::format("Can't get correction without T0 reference!"));
        }
        auto diff = coarse_time - t0_ref_.value();
        return static_cast<int>(std::remainder(diff, COARSE_TIME_FULL_CYCLE));
    }

    void T0Checker::Add(int module_num, int t0_value)
    {
        if (auto entry = entries_.find(module_num); entry == entries_.end())
        {
            entries_.insert(std::make_pair(module_num, T0Entry{ t0_value }));
        }
    }

    auto T0Checker::GetCorrection(int module_num, int t0_value) const -> int
    {
        auto entry = entries_.find(module_num);
        if (entry == entries_.end())
        {
            R3BLOG(error,
                   fmt::format("Can't get T0 correction value as T0 value hasn't be set for module {}.", module_num));
            return 0;
        }
        return entry->second.GetCorrection(t0_value);
    }
} // namespace R3B::Neuland
