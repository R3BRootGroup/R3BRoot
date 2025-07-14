/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BNeulandNeutron.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandHit.h"
#include <Math/Vector3Dfwd.h>
#include <RtypesCore.h>
#include <cmath>
#include <iostream>
#include <ostream>
#include <utility>

static const double MASS_OF_NEUTRON = 939.565379;              // MeV/c²
static const double SPEED_OF_LIGHT = 29.9792458;               // cm/ns
static const double SPEED_OF_LIGHT_SQ = 898.75517873681758374; // cm²/ns²

R3BNeulandNeutron::R3BNeulandNeutron(Int_t paddle, double time, ROOT::Math::XYZVector pos, ROOT::Math::XYZVector pix)
    : fPaddle(paddle)
    , fT(time)
    , fPosition(std::move(pos))
    , fPixel(std::move(pix))
{
}

R3BNeulandNeutron::R3BNeulandNeutron(const R3BNeulandHit& hit)
    : fPaddle(hit.GetPaddle())
    , fT(hit.GetT())
    , fPosition(hit.GetPosition())
    , fPixel(hit.GetPixel())
{
}

R3BNeulandNeutron::R3BNeulandNeutron(const R3BNeulandCluster& cluster)
    : R3BNeulandNeutron(cluster.GetFirstHit())
{
}

auto R3BNeulandNeutron::GetGamma() const -> double
{
    const double velocity_sq = GetPosition().Mag2() / std::pow(GetT(), 2); // cm²/ns²
    return 1. / std::sqrt(1. - (velocity_sq / SPEED_OF_LIGHT_SQ));
}

auto R3BNeulandNeutron::GetP() const -> ROOT::Math::XYZVector
{
    return GetPosition() * (GetGamma() * MASS_OF_NEUTRON / GetT() / SPEED_OF_LIGHT);
}

auto R3BNeulandNeutron::GetEtot() const -> double { return GetGamma() * MASS_OF_NEUTRON; }

auto R3BNeulandNeutron::GetEkin() const -> double { return (GetGamma() - 1.) * MASS_OF_NEUTRON; }

auto operator<<(std::ostream& ostream, const R3BNeulandNeutron& digi) -> std::ostream&
{
    ostream << "R3BNeulandNeutron: XYZTE    " << digi.GetPosition().X() << "    " << digi.GetPosition().Y() << "    "
            << digi.GetPosition().Z() << "    " << digi.GetT() << "    " << digi.GetEkin() << "\n";
    return ostream;
}

void R3BNeulandNeutron::Print(const Option_t* /*option*/) const { std::cout << *this; }
