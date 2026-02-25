/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BActafClusterData.h"

#include <Rtypes.h>
#include <RtypesCore.h>
#include <TVector3.h>

#include <cstdint>
#include <fmt/core.h>
#include <iostream>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

R3BActafClusterData::R3BActafClusterData(UInt_t side,
                                         double xpos,
                                         double ypos,
                                         double zpos,
                                         double energy,
                                         const TVector3& track,
                                         std::vector<uint16_t> padlist,
                                         double chi2polarfit)
    : fSide(side)
    , fXPos(xpos)
    , fYPos(ypos)
    , fZPos(zpos)
    , fEnergy(energy)
    , fTrack(track)
    , fPadList(std::move(padlist))
    , fChi2PolarFit(chi2polarfit)
{
}

std::string R3BActafClusterData::toString() const
{
    return fmt::format("Side : {}, X-Pos: {}, Y-Pos: {}, Z-Pos: {}, Energy: {}, Pads: {}, Chi2PolarFit : {}",
                       GetSide(),
                       GetXpos(),
                       GetYpos(),
                       GetZpos(),
                       GetEnergy(),
                       GetNbOfPads(),
                       GetChi2PolarFit());
}

void R3BActafClusterData::Print(const Option_t* /*option*/) const { std::cout << *this << '\n'; }

std::ostream& operator<<(std::ostream& output, const R3BActafClusterData& data)
{
    output << data.toString();
    return output;
}

ClassImp(R3BActafClusterData)
