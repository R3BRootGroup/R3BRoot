/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BActafCalData.h"
#include <fmt/core.h>

R3BActafCalData::R3BActafCalData(UInt_t pad,
                                 double energy,
                                 double maxampl,
                                 double leadingedge,
                                 double zpos,
                                 double syntime)
    : fPad(pad)
    , fEnergy(energy)
    , fEMaxAmpl(maxampl)
    , fLeadingEdge(leadingedge)
    , fZPos(zpos)
    , fSynTime(syntime)
{
}

std::string R3BActafCalData::toString() const
{
    return fmt::format("Pad: {}, Energy : {}, Max-ampl : {}, Leading-Time: {}, Z-Pos: {}, Syn-Time: {}",
                       GetPad(),
                       GetEnergy(),
                       GetEMaxAmpl(),
                       GetLeadingEdgeTime(),
                       GetZpos(),
                       GetSynTime());
}

void R3BActafCalData::Print(const Option_t*) const { std::cout << *this << std::endl; }

std::ostream& operator<<(std::ostream& os, const R3BActafCalData& data)
{
    os << data.toString();
    return os;
}

ClassImp(R3BActafCalData)
