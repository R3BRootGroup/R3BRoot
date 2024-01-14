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

// -----------------------------------------------------------------------
// -----            R3BAlpideMappedData source file                  -----
// -----------------------------------------------------------------------

#include "R3BAlpideMappedData.h"
#include <fmt/core.h>

R3BAlpideMappedData::R3BAlpideMappedData(uint16_t senId,
                                         uint16_t reg,
                                         uint16_t ads,
                                         uint16_t chip,
                                         uint16_t row,
                                         uint16_t col)
    : fSenId(senId)
    , fReg(reg)
    , fAds(ads)
    , fChip(chip)
    , fRow(row)
    , fCol(col)
{
}

std::string R3BAlpideMappedData::toString() const
{
    return fmt::format("SensorID: {}, Region: {}, Address: {}, Chip: {}, Row: {}, Col: {}",
                       GetSensorId(),
                       GetReg(),
                       GetAds(),
                       GetChip(),
                       GetRow(),
                       GetCol());
}

void R3BAlpideMappedData::Print(const Option_t*) const { std::cout << *this << std::endl; }

std::ostream& operator<<(std::ostream& os, const R3BAlpideMappedData& data)
{
    os << data.toString();
    return os;
}

ClassImp(R3BAlpideMappedData)
