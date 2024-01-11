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
// -----                  R3BTTTXCalData source file                   -----
// -------------------------------------------------------------------------

#include "R3BTTTXCalData.h"
#include <fmt/core.h>

R3BTTTXCalData::R3BTTTXCalData(uint8_t DetID, uint8_t StripID, double time, double energy)
    : fDetID(DetID)
    , fStripID(StripID)
    , fTime(time)
    , fEnergy(energy)
{
}

std::string R3BTTTXCalData::toString() const
{
    return fmt::format(
        "DetID: {}, StripID: {}, Time: {}, Energy: {}", GetDetID(), GetStripID(), GetTime(), GetEnergy());
}

void R3BTTTXCalData::Print(const Option_t*) const { std::cout << *this << std::endl; }

std::ostream& operator<<(std::ostream& os, const R3BTTTXCalData& data)
{
    os << data.toString();
    return os;
}

ClassImp(R3BTTTXCalData)
