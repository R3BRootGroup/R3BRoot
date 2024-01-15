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

#include "R3BNeulandHit.h"

constexpr double light_speed = 29.97924580000000105; // cm/ns
// constexpr double light_speed_square = 898.75517873681758374; // cm²/ns²

R3BNeulandHit::R3BNeulandHit(int paddle,
                             double TdcL,
                             double TdcR,
                             double time_val,
                             double QdcL,
                             double QdcR,
                             double energy_val,
                             const TVector3& pos,
                             const TVector3& pix)
    : module_id(paddle)
    , tdc_left(TdcL)
    , tdc_right(TdcR)
    , time(time_val)
    , qdc_left(QdcL)
    , qdc_right(QdcR)
    , energy(energy_val)
    , position(pos)
    , pixel(pix)
{
}

auto R3BNeulandHit::GetBeta() const -> double { return position.Mag() / (time * light_speed); }

auto R3BNeulandHit::GetEToF(const double mass) const -> double
{
    const auto beta = GetBeta();
    const auto gamma = 1. / std::sqrt(1. - beta * beta);
    const auto etof = (gamma - 1.) * mass;
    return 1.81522 + 0.984612 * etof; // TODO: EToF is ever so slightly off. Maybe some rounding/mass error?
}

auto operator<<(std::ostream& os_stream, const R3BNeulandHit& hit) -> std::ostream&
{
    os_stream << "R3BNeulandHit: NeuLAND Hit in Paddle " << hit.module_id << std::endl
              << "    TdcL: " << hit.tdc_left << "    TdcR: " << hit.tdc_right << "    Time: " << hit.time << std::endl
              << "    QdcL: " << hit.qdc_left << "    QdcR: " << hit.qdc_right << "    Energy: " << hit.energy
              << std::endl
              << "    Position XYZ: " << hit.position.X() << "    " << hit.position.Y() << "    "
              << hit.GetPosition().Z() << std::endl;
    return os_stream;
}

void R3BNeulandHit::Print(const Option_t* /*unused*/) const { std::cout << *this; }

ClassImp(R3BNeulandHit)
