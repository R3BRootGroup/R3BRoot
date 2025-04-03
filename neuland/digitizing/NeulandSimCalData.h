#pragma once

#include "R3BShared.h"
#include "TObject.h"
#include <Rtypes.h>
#include <fmt/core.h>

namespace R3B::Neuland
{
    struct SimCalData : public TObject
    {
        int module_id{}; // ns
        LRPair<double> leading_time;
        LRPair<double> time_over_thresh;
        SimCalData() = default;
        SimCalData(int paddle_id, double totL, double totR, double tleL, double tleR)
            : module_id{ paddle_id }
            , leading_time{ tleL, tleR }
            , time_over_thresh{ totL, totR }
        {
        }
        ClassDefNV(SimCalData, 3);
    };
} // namespace R3B::Neuland

template <>
class fmt::formatter<R3B::Neuland::SimCalData>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::Neuland::SimCalData& cal_data, FmtContent& ctn) const
    {
        return format_to(ctn.out(),
                         "{{module_id: {}, leading_time: {}, time_over_thresh: {}}}",
                         cal_data.module_id,
                         cal_data.leading_time,
                         cal_data.time_over_thresh);
    }
};
