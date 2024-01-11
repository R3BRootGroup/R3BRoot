/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2024 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -------------------------------------------------------------------------
// -----                  R3BTTTXHitData source file                   -----
// -------------------------------------------------------------------------

#include "R3BTTTXHitData.h"
#include <fmt/core.h>

R3BTTTXHitData::R3BTTTXHitData(double xpos, double energy, double angle, double charge, double time)
    : fXpos(xpos)
    , fEnergy(energy)
    , fAng(angle)
    , fCharge(charge)
    , fTime(time)
{
}

std::string R3BTTTXHitData::toString() const
{
    return fmt::format("Xpos(mm): {:.2f}, Energy: {:.2f}, Angle: {:.3f}, ChargeZ: {:.2f}, Time: {}",
                       GetX(),
                       GetEnergy(),
                       GetAngle(),
                       GetChargeZ(),
                       GetTime());
}

void R3BTTTXHitData::Print(const Option_t*) const { std::cout << *this << std::endl; }

std::ostream& operator<<(std::ostream& os, const R3BTTTXHitData& data)
{
    os << data.toString();
    return os;
}

ClassImp(R3BTTTXHitData)
