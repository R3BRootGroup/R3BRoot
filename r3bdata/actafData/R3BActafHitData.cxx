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

#include "R3BActafHitData.h"
#include <fmt/core.h>

R3BActafHitData::R3BActafHitData(UInt_t pad,
                                 UInt_t side,
                                 UInt_t ring,
                                 double xpos,
                                 double ypos,
                                 double zpos,
                                 double energy,
                                 TVector3 track,
                                 double maxAmpl)
    : fPad(pad)
    , fSide(side)
    , fRing(ring)
    , fXPos(xpos)
    , fYPos(ypos)
    , fZPos(zpos)
    , fEnergy(energy)
    , fTrack(track)
    , fMaxAmpl(maxAmpl)
{
}

std::string R3BActafHitData::toString() const
{
    return fmt::format("Pad: {}, Side : {}, Ring : {}, X-Pos: {}, Y-Pos: {}, Z-Pos: {}, Energy: {}",
                       GetPad(),
                       GetSide(),
                       GetRing(),
                       GetXpos(),
                       GetYpos(),
                       GetZpos(),
                       GetEnergy(),
                       GetMaxAmpl());
}

void R3BActafHitData::Print(const Option_t*) const { std::cout << *this << std::endl; }

std::ostream& operator<<(std::ostream& os, const R3BActafHitData& data)
{
    os << data.toString();
    return os;
}

ClassImp(R3BActafHitData)
