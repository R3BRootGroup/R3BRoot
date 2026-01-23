#pragma once

#include "R3BValueError.h"
#include <Rtypes.h>
#include <fmt/base.h>
#include <root/Math/Vector3D.h> // IWYU pragma: keep
#include <root/Math/Vector3Dfwd.h>

namespace R3B::Neuland
{
    using XYZVectorValueErrorD =
        ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<ValueErrorD>, ROOT::Math::DefaultCoordinateSystemTag>;
    struct Hit
    {
        int module_id{};
        ValueErrorD tdc_left;
        ValueErrorD tdc_right;
        ValueErrorD time;
        ValueErrorD qdc_left;
        ValueErrorD qdc_right;
        ValueErrorD energy;
        XYZVectorValueErrorD position;
        ROOT::Math::XYZVectorD pixel;
        ClassDefNV(Hit, 1);
    };
} // namespace R3B::Neuland

using R3BNeulandHit2 = R3B::Neuland::Hit;

#ifndef __CLING__
template <>
class fmt::formatter<R3B::Neuland::Hit>
{
  public:
    static constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3B::Neuland::Hit& hit, FmtContent& ctn) const
    {
        return fmt::format_to(
            ctn.out(),
            "{{module_id: {}, left_tdc: {}, right_tdc: {}, time: {} ns, left_qdc: {}, right_qdc: {}, energy: "
            "{} MeV, position: {} cm, pixel: {}}}",
            hit.module_id,
            hit.tdc_left,
            hit.tdc_right,
            hit.time,
            hit.qdc_left,
            hit.qdc_right,
            hit.energy,
            hit.position,
            hit.pixel);
    }
};
#endif
