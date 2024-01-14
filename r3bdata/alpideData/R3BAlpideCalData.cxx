/******************************************************************************
 *   Copyright (C) 2022 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2022-2024 Members of R3B Collaboration                     *
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
// -----            R3BAlpideCalData source file                 -----
// -------------------------------------------------------------------

#include "R3BAlpideCalData.h"
#include <fmt/core.h>

R3BAlpideCalData::R3BAlpideCalData(uint16_t senId, uint16_t row, uint16_t col)
    : fSenId(senId)
    , fRow(row)
    , fCol(col)
{
}

std::string R3BAlpideCalData::toString() const
{
    return fmt::format("SensorID: {}, Row: {}, Col: {}", GetSensorId(), GetRow(), GetCol());
}

void R3BAlpideCalData::Print(const Option_t*) const { std::cout << *this << std::endl; }

std::ostream& operator<<(std::ostream& os, const R3BAlpideCalData& data)
{
    os << data.toString();
    return os;
}

ClassImp(R3BAlpideCalData)
