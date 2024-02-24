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

#include "R3BCalifaHitData.h"
#include <TMath.h>
#include <fmt/core.h>

R3BCalifaHitData::R3BCalifaHitData(double ene, double theta, double phi, double pdi, uint8_t hittype, ULong64_t time)
    : fEnergy(ene)
    , fTheta(theta)
    , fPhi(phi)
    , fPid(pdi)
    , fHitType(hittype)
    , fTime(time)
{
}

R3BCalifaHitData::R3BCalifaHitData(const R3BCalifaHitData& right)
    : fEnergy(right.fEnergy)
    , fTheta(right.fTheta)
    , fPhi(right.fPhi)
    , fPid(right.fPid)
    , fHitType(right.fHitType)
    , fTime(right.fTime)
{
}

std::string R3BCalifaHitData::toString() const
{
    return fmt::format("Hit type: {}, Energy(MeV): {:.3f}, Theta(deg): {:.3f}, Phi(deg): {:.3f}, PID: {:.3f}, Time: {}",
                       GetType(),
                       GetEnergy(),
                       GetTheta() * TMath::RadToDeg(),
                       GetPhi() * TMath::RadToDeg(),
                       GetPid(),
                       GetTime());
}

void R3BCalifaHitData::Print(const Option_t*) const { std::cout << *this << std::endl; }

std::ostream& operator<<(std::ostream& os, const R3BCalifaHitData& data)
{
    os << data.toString();
    return os;
}

ClassImp(R3BCalifaHitData)
