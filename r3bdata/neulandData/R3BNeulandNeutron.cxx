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
#include <Rtypes.h>
#include <RtypesCore.h>
#include <cmath>
#include <iostream>
#include <ostream>

static const double massNeutron = 939.565379;   // MeV/c²
static const double c = 29.9792458;             // cm/ns
static const double c2 = 898.75517873681758374; // cm²/ns²

R3BNeulandNeutron::R3BNeulandNeutron(Int_t paddle,
                                     double time,
                                     const ROOT::Math::XYZVector& pos,
                                     const ROOT::Math::XYZVector& pix)
    : fPaddle(paddle)
    , fT(time)
    , fPosition(pos)
    , fPixel(pix)
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

double R3BNeulandNeutron::GetGamma() const
{
    const double v2 = GetPosition().Mag2() / std::pow(GetT(), 2); // cm²/ns²
    return 1. / std::sqrt(1. - (v2 / c2));
}

ROOT::Math::XYZVector R3BNeulandNeutron::GetP() const
{
    return GetPosition() * (GetGamma() * massNeutron / GetT() / c);
}

auto R3BNeulandNeutron::GetEtot() const -> double { return GetGamma() * massNeutron; }

auto R3BNeulandNeutron::GetEkin() const -> double { return (GetGamma() - 1.) * massNeutron; }

auto operator<<(std::ostream& os, const R3BNeulandNeutron& digi) -> std::ostream&
{
    os << "R3BNeulandNeutron: XYZTE    " << digi.GetPosition().X() << "    " << digi.GetPosition().Y() << "    "
       << digi.GetPosition().Z() << "    " << digi.GetT() << "    " << digi.GetEkin() << "\n";
    return os;
}

void R3BNeulandNeutron::Print(const Option_t*) const { std::cout << *this; }

ClassImp(R3BNeulandNeutron)
