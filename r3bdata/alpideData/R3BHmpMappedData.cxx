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

// --------------------------------------------------------------------
// -----            R3BHmpMappedData source file                  -----
// --------------------------------------------------------------------

#include "R3BHmpMappedData.h"
#include <fmt/core.h>

R3BHmpMappedData::R3BHmpMappedData(uint32_t TSYS_LO,
                                   uint32_t TSYS_HI,
                                   uint32_t volt1,
                                   uint32_t curr1,
                                   uint32_t volt2,
                                   uint32_t curr2,
                                   uint32_t volt3,
                                   uint32_t curr3,
                                   uint32_t volt4,
                                   uint32_t curr4)
    : fTSLO(TSYS_LO)
    , fTSHI(TSYS_HI)
    , fVolt1(volt1)
    , fCurr1(curr1)
    , fVolt2(volt2)
    , fCurr2(curr2)
    , fVolt3(volt3)
    , fCurr3(curr3)
    , fVolt4(volt4)
    , fCurr4(curr4)
{
}

std::string R3BHmpMappedData::toString() const
{
    return fmt::format("TSLO: {}, Region: {}, TSHI: {}, Volt1: {}, Curr1: {}, Volt2: {}, Curr2: {}, Volt3: {}, Curr3: "
                       "{}, Volt4: {}, Curr4: {}",
                       GetTSLO(),
                       GetTSHI(),
                       GetVolt1(),
                       GetCurr1(),
                       GetVolt2(),
                       GetCurr2(),
                       GetVolt3(),
                       GetCurr3(),
                       GetVolt4(),
                       GetCurr4());
}

void R3BHmpMappedData::Print(const Option_t*) const { std::cout << *this << std::endl; }

std::ostream& operator<<(std::ostream& os, const R3BHmpMappedData& data)
{
    os << data.toString();
    return os;
}

ClassImp(R3BHmpMappedData)
