/******************************************************************************
 *   Copyright (C) 2022 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2022-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -------------------------------------------------------------------
// -----            R3BAlpideHitData source file                 -----
// -------------------------------------------------------------------

#include "R3BAlpideHitData.h"
#include <fmt/core.h>
// Needed for TVector3 fmt
template <>
struct fmt::formatter<TVector3>
{
    constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const TVector3& vec, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), "({}, {}, {})", vec.X(), vec.Y(), vec.Z());
    }
};
// Needed for std::vector<double>
template <>
struct fmt::formatter<std::vector<double>>
{
    constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const std::vector<double>& v, FormatContext& ctx)
    {
        std::string s = "[";
        for (size_t i = 0; i < v.size(); ++i)
        {
            s += fmt::format("{}", v[i]);
            if (i + 1 < v.size())
                s += ", ";
        }
        s += "]";
        return fmt::format_to(ctx.out(), "{}", s);
    }
};
R3BAlpideHitData::R3BAlpideHitData(uint16_t sensorId,
                                   uint16_t clustersize,
                                   uint16_t genus,
                                   double elong,
                                   TVector3 majorUnit,
                                   double holePerimeter,
                                   double totalPerimeter,
                                   std::vector<double> hu,
                                   double posx,
                                   double posy,
                                   double posz,
                                   double locposx,
                                   double locposy)
    : fSensorId(sensorId)
    , fClustersize(clustersize)
    , fGenus(genus)
    , fElong(elong)
    , fMajorUnit(majorUnit)
    , fHolePerimeter(holePerimeter)
    , fTotalPerimeter(totalPerimeter)
    , fHu(hu)
    , fX(posx)
    , fY(posy)
    , fZ(posz)
    , fXloc(locposx)
    , fYloc(locposy)
{
    fTrack.SetXYZ(posx, posy, posz);
    fTheta = fTrack.Theta();
    fPhi = fTrack.Phi();
}

std::string R3BAlpideHitData::toString() const
{
    return fmt::format("SensorID: {}, ClusterSize: {},Genus: {},Elong: {},MajorUnit:{},HolePerimeter: "
                       "{},TotalPerimeter:{},Hu:{}, Xpos: {}, Ypos: {}, Zpos: {}",
                       GetSensorId(),
                       GetClusterSize(),
                       GetGenus(),
                       GetElong(),
                       GetMajorUnit(),
                       GetHolePerimeter(),
                       GetTotalPerimeter(),
                       GetHu(),
                       GetX(),
                       GetY(),
                       GetZ());
}

void R3BAlpideHitData::Print(const Option_t*) const { std::cout << *this << std::endl; }

std::ostream& operator<<(std::ostream& os, const R3BAlpideHitData& data)
{
    os << data.toString();
    return os;
}

ClassImp(R3BAlpideHitData)
