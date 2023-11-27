/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/
#pragma once

#include "TObject.h"
#include "TVector3.h"
#include <R3BFormatters.h>
#include <Rtypes.h>
#include <iostream>

constexpr auto NEUTRON_MASS_MEV = 939.565;

struct R3BNeulandHit : public TObject
{
    int module_id = 0;
    double tdc_left = 0.;
    double tdc_right = 0.;
    double time = 0.;
    double qdc_left = 0.;
    double qdc_right = 0.;
    double energy = 0.;
    TVector3 position{};
    TVector3 pixel{};

    R3BNeulandHit() = default;
    R3BNeulandHit(Int_t paddle,
                  double TdcL,
                  double TdcR,
                  double time,
                  double QdcL,
                  double QdcR,
                  double energy,
                  const TVector3& pos,
                  const TVector3& pix);

    void Print(const Option_t* /*option*/) const override;
    auto operator==(const R3BNeulandHit& other) const -> bool
    {
        // TODO: Change this if multi-hit capability is introduced
        return this->GetPaddle() == other.GetPaddle();
    }

    // NOTE: for backward compabitlity
    [[nodiscard]] auto GetPaddle() const -> int { return module_id; }
    [[nodiscard]] auto GetTdcR() const -> double { return tdc_right; }
    [[nodiscard]] auto GetTdcL() const -> double { return tdc_left; }
    [[nodiscard]] auto GetT() const -> double { return time; }
    [[nodiscard]] auto GetQdcR() const -> double { return qdc_right; }
    [[nodiscard]] auto GetQdcL() const -> double { return qdc_left; }
    [[nodiscard]] auto GetE() const -> double { return energy; }
    [[nodiscard]] auto GetPosition() const -> TVector3 { return position; }
    [[nodiscard]] auto GetPixel() const -> TVector3 { return pixel; }

    [[nodiscard]] auto GetBeta() const -> double;
    [[nodiscard]] auto GetEToF(double mass = NEUTRON_MASS_MEV) const -> double;

    ClassDefOverride(R3BNeulandHit, 2);
};

auto operator<<(std::ostream&, const R3BNeulandHit&) -> std::ostream&; // Support easy printing
                                                                       //
template <>
class fmt::formatter<R3BNeulandHit>
{
  public:
    static constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }
    template <typename FmtContent>
    constexpr auto format(const R3BNeulandHit& hit, FmtContent& ctn) const
    {
        return format_to(
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
